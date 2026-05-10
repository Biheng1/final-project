#include "DynamicSIPStrategy.h"
#include "CSVParser.h"
#include <sstream>
#include <vector>
using namespace std;

DynamicSIPStrategy::DynamicSIPStrategy(double dipThreshold, // constructor to initialize the strategy parameters
                                       double rallyThreshold,
                                       double multiplier) {
    this->dipThreshold = dipThreshold;
    this->rallyThreshold = rallyThreshold;
    this->multiplier = multiplier;
}

SimResult DynamicSIPStrategy::backtest(PriceHistory* history, // backtest implementation for the Dynamic SIP strategy, following the rules outlined in the header comments
                                       double monthlyCapital,
                                       int startYear,
                                       int endYear) {
    SimResult result;
    result.strategyName = getName();
    result.finalValue = 0.0;
    result.totalInvested = 0.0;
    result.totalReturn = 0.0;
    result.cagr = 0.0;
    result.maxDrawdown = 0.0;
    result.totalTrades = 0;

    if (history == nullptr || history->getSize() == 0) {
        return result;
    }

    double shares = 0.0;
    double lastClose = 0.0;

    int lastMonth = -1;
    int lastYear = -1;

    vector<double> portfolioValues;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) { // iterate through price history
        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear || year > endYear) { // skip dates outside the backtest range
            continue;
        }

        lastClose = node.close;

        // Only invest once per month
        if (year != lastYear || month != lastMonth) {

            double highestPrice = node.close;
            double lowestPrice = node.close;

            PriceNode* lookback = &node;
            int count = 0;

            // Scan backward about 12 months (~252 trading days)
            while (lookback != nullptr && count < 252) { // look back up to 252 trading days (approx. 12 months)
                if (lookback->close > highestPrice) {
                    highestPrice = lookback->close;
                }

                if (lookback->close < lowestPrice) {
                    lowestPrice = lookback->close;
                }

                lookback = lookback->prev;
                count++;
            }

            double investmentAmount = monthlyCapital;

            // % drop from 12-month high
            double dropFromHigh =
                (highestPrice - node.close) / highestPrice * 100.0;

            // % rise from 12-month low
            double riseFromLow =
                (node.close - lowestPrice) / lowestPrice * 100.0;

            // Buy bigger during dips
            if (dropFromHigh >= dipThreshold) {
                investmentAmount = monthlyCapital * multiplier;
            }

            // Reduce investment during rallies
            else if (riseFromLow >= rallyThreshold) {
                investmentAmount = monthlyCapital * 0.5;
            }

            double boughtShares = investmentAmount / node.close;

            shares += boughtShares;

            result.totalInvested += investmentAmount;
            result.totalTrades++;

            lastYear = year;
            lastMonth = month;
        }

        double currentValue = shares * node.close; // track portfolio value over time for drawdown calculation
        portfolioValues.push_back(currentValue);
    }

    result.finalValue = shares * lastClose;

    if (result.totalInvested > 0.0) { // calculate total return based on total invested and final value
        result.totalReturn =
            (result.finalValue - result.totalInvested) / result.totalInvested * 100.0; 
    }

    result.cagr = calculateCAGR(result.totalInvested, // calculate CAGR based on total invested and final value
                                result.finalValue,
                                endYear - startYear);

    result.maxDrawdown = calculateMaxDrawdown(portfolioValues);

    return result;
}

string DynamicSIPStrategy::getName() const { // return a human-readable name for the strategy including its parameters
    stringstream ss;

    ss << "Dynamic SIP (";
    ss << "Dip " << dipThreshold << "%, ";
    ss << "Rally " << rallyThreshold << "%, ";
    ss << "x" << multiplier << ")";

    return ss.str();
}

double DynamicSIPStrategy::getDipThreshold() const { // accessor for dipThreshold, used by the parameter sweep bonus
    return dipThreshold;
}

double DynamicSIPStrategy::getRallyThreshold() const { // accessor for rallyThreshold, used by the parameter sweep bonus
    return rallyThreshold;
}

double DynamicSIPStrategy::getMultiplier() const { // accessor for multiplier, used by the parameter sweep bonus
    return multiplier;
}