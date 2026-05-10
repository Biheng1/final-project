#include "GoldenCrossStrategy.h"
#include "CSVParser.h"
#include <sstream>
#include <vector>
using namespace std;

GoldenCrossStrategy::GoldenCrossStrategy(int shortWindow, int longWindow) { // constructor to initialize the strategy parameters
    this->shortWindow = shortWindow;
    this->longWindow = longWindow;
}

SimResult GoldenCrossStrategy::backtest(PriceHistory* history, // backtest implementation for the Golden Cross strategy, following the rules outlined in the header comments
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

    CircularQueue shortMA(shortWindow);
    CircularQueue longMA(longWindow);

    double cash = 0.0;
    double shares = 0.0;
    double lastClose = 0.0;

    double prevShortAvg = 0.0;
    double prevLongAvg = 0.0;
    bool hasPreviousAverage = false;

    int lastMonth = -1;
    int lastYear = -1;

    vector<double> portfolioValues;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) { // iterate through price history
        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear || year > endYear) {
            continue;
        }

        lastClose = node.close;

        // Add monthly capital on the first trading day of each month
        if (year != lastYear || month != lastMonth) { // only invest once per month
            cash += monthlyCapital;
            result.totalInvested += monthlyCapital;

            lastYear = year;
            lastMonth = month;
        }

        shortMA.enqueue(node.close);
        longMA.enqueue(node.close);

        if (shortMA.isFull() && longMA.isFull()) { // only generate signals if both moving averages are full (enough data)
            double currentShortAvg = shortMA.getAverage(); // calculate current short-term moving average
            double currentLongAvg = longMA.getAverage(); // calculate current long-term moving average

            if (hasPreviousAverage) {
                // Golden Cross: short average crosses above long average
                if (prevShortAvg <= prevLongAvg && currentShortAvg > currentLongAvg) {
                    if (cash > 0.0) {
                        shares += cash / node.close;
                        cash = 0.0;
                        result.totalTrades++;
                    }
                }

                // Death Cross: short average crosses below long average
                else if (prevShortAvg >= prevLongAvg && currentShortAvg < currentLongAvg) {
                    if (shares > 0.0) {
                        cash += shares * node.close;
                        shares = 0.0;
                        result.totalTrades++;
                    }
                }
            }

            prevShortAvg = currentShortAvg;
            prevLongAvg = currentLongAvg;
            hasPreviousAverage = true;
        }
 
        double currentValue = cash + shares * node.close; // track portfolio value over time for drawdown calculation
        portfolioValues.push_back(currentValue);
    }

    result.finalValue = cash + shares * lastClose;

    if (result.totalInvested > 0.0) { // calculate total return based on total invested and final value
        result.totalReturn =
            (result.finalValue - result.totalInvested) / result.totalInvested * 100.0;
    }

    result.cagr = calculateCAGR(result.totalInvested, // calculate CAGR based on total invested and final value
                                result.finalValue,
                                endYear - startYear);

    result.maxDrawdown = calculateMaxDrawdown(portfolioValues); // calculate max drawdown from the portfolio value time series

    return result;
}

string GoldenCrossStrategy::getName() const { 
    stringstream ss;
    ss << "Golden Cross (" << shortWindow << "/" << longWindow << " MA)"; // include the window sizes in the strategy name for clarity
    return ss.str();
}