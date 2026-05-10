#include "GoldenCrossStrategy.h"
#include "CSVParser.h"
#include <sstream>
#include <vector>
using namespace std;

GoldenCrossStrategy::GoldenCrossStrategy(int shortWindow, int longWindow) {
    this->shortWindow = shortWindow;
    this->longWindow = longWindow;
}

SimResult GoldenCrossStrategy::backtest(PriceHistory* history,
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

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear || year > endYear) {
            continue;
        }

        lastClose = node.close;

        // Add monthly capital on the first trading day of each month
        if (year != lastYear || month != lastMonth) {
            cash += monthlyCapital;
            result.totalInvested += monthlyCapital;

            lastYear = year;
            lastMonth = month;
        }

        shortMA.enqueue(node.close);
        longMA.enqueue(node.close);

        if (shortMA.isFull() && longMA.isFull()) {
            double currentShortAvg = shortMA.getAverage();
            double currentLongAvg = longMA.getAverage();

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

        double currentValue = cash + shares * node.close;
        portfolioValues.push_back(currentValue);
    }

    result.finalValue = cash + shares * lastClose;

    if (result.totalInvested > 0.0) {
        result.totalReturn =
            (result.finalValue - result.totalInvested) / result.totalInvested * 100.0;
    }

    result.cagr = calculateCAGR(result.totalInvested,
                                result.finalValue,
                                endYear - startYear);

    result.maxDrawdown = calculateMaxDrawdown(portfolioValues);

    return result;
}

string GoldenCrossStrategy::getName() const {
    stringstream ss;
    ss << "Golden Cross (" << shortWindow << "/" << longWindow << " MA)";
    return ss.str();
}