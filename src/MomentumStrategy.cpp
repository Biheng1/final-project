#include "MomentumStrategy.h"
#include "CSVParser.h"
#include <sstream>
#include <vector>
using namespace std;

MomentumStrategy::MomentumStrategy(double momentumThreshold, int lookbackDays) {
    this->momentumThreshold = momentumThreshold;
    this->lookbackDays = lookbackDays;
}

SimResult MomentumStrategy::backtest(PriceHistory* history,
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

    double cash = 0.0;
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

        // Add monthly capital on the first trading day of each month
        if (year != lastYear || month != lastMonth) {
            cash += monthlyCapital;
            result.totalInvested += monthlyCapital;

            PriceNode* lookbackNode = history->findByDate(node.date);
            int count = 0;

            while (lookbackNode != nullptr && count < lookbackDays) {
                lookbackNode = lookbackNode->prev;
                count++;
            }

            if (lookbackNode != nullptr && lookbackNode->close > 0.0) {
                double momentum =
                    (node.close - lookbackNode->close) / lookbackNode->close * 100.0;

                // Positive momentum above threshold: invest available cash
                if (momentum > momentumThreshold) {
                    if (cash > 0.0) {
                        shares += cash / node.close;
                        cash = 0.0;
                        result.totalTrades++;
                    }
                }

                // Negative momentum: sell everything and hold cash
                else if (momentum < 0.0) {
                    if (shares > 0.0) {
                        cash += shares * node.close;
                        shares = 0.0;
                        result.totalTrades++;
                    }
                }
            }

            lastYear = year;
            lastMonth = month;
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

string MomentumStrategy::getName() const {
    stringstream ss;
    ss << "6-Month Momentum (" << momentumThreshold << "%)";
    return ss.str();
}