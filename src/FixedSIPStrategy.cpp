#include "FixedSIPStrategy.h"
#include "CSVParser.h"
#include <vector>
#include <string>
using namespace std;

SimResult FixedSIPStrategy::backtest(PriceHistory* history, // backtest implementation for the Fixed SIP strategy, following the rules outlined in the header comments
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

    string lastMonthKey = "";

    vector<double> portfolioValues;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) { // iterate through price history
        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);

        if (year < startYear || year > endYear) {
            continue;
        }

        lastClose = node.close;

        // Month key example: "2000-01"
        string currentMonthKey = node.date.substr(0, 7);

        // First trading day of a new month
        if (currentMonthKey != lastMonthKey) {
            shares += monthlyCapital / node.close;

            result.totalInvested += monthlyCapital;
            result.totalTrades++;

            lastMonthKey = currentMonthKey;
        }

        double currentValue = shares * node.close;
        portfolioValues.push_back(currentValue);
    }

    result.finalValue = shares * lastClose;

    if (result.totalInvested > 0.0) {
        result.totalReturn =
            (result.finalValue - result.totalInvested) / result.totalInvested * 100.0;
    }

    result.cagr = calculateCAGR(result.totalInvested, // calculate CAGR based on total invested and final value
                                result.finalValue,
                                endYear - startYear);

    result.maxDrawdown = calculateMaxDrawdown(portfolioValues); // calculate max drawdown from the portfolio value time series

    return result;
}

string FixedSIPStrategy::getName() const {
    return "Fixed SIP";
}