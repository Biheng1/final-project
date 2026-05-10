#include "FixedSIPStrategy.h"
#include "CSVParser.h"
#include <vector>
using namespace std;

SimResult FixedSIPStrategy::backtest(PriceHistory* history,
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

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear || year > endYear) {
            continue;
        }

        lastClose = node.close;

        // First trading day of a new month
        if (year != lastYear || month != lastMonth) {
            double boughtShares = monthlyCapital / node.close;
            shares += boughtShares;

            result.totalInvested += monthlyCapital;
            result.totalTrades++;

            lastYear = year;
            lastMonth = month;
        }

        double currentValue = shares * node.close;
        portfolioValues.push_back(currentValue);
    }

    result.finalValue = shares * lastClose;

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

string FixedSIPStrategy::getName() const {
    return "Fixed SIP";
}