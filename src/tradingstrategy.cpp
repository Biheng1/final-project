#include "TradingStrategy.h"
#include <cmath>
#include <iomanip>
#include <iostream>
using namespace std;

void TradingStrategy::printResult(const SimResult& result) const {
    cout << fixed << setprecision(2);
    cout << "Strategy: " << result.strategyName << endl;
    cout << "Final value: $" << result.finalValue << endl;
    cout << "Total invested: $" << result.totalInvested << endl;
    cout << "Total return: " << result.totalReturn << "%" << endl;
    cout << "CAGR: " << result.cagr << "%" << endl;
    cout << "Max drawdown: " << result.maxDrawdown << "%" << endl;
    cout << "Total trades: " << result.totalTrades << endl;
}

double TradingStrategy::calculateCAGR(double startVal,
                                      double endVal,
                                      int years) const {
    if (years <= 0 || startVal <= 0.0 || endVal <= 0.0) {
        return 0.0;
    }

    return (pow(endVal / startVal, 1.0 / years) - 1.0) * 100.0;
}

double TradingStrategy::calculateMaxDrawdown(
    const vector<double>& portfolioValues) const {
    if (portfolioValues.empty()) {
        return 0.0;
    }

    double peak = portfolioValues[0];
    double maxDrawdown = 0.0;

    for (size_t i = 1; i < portfolioValues.size(); i++) {
        if (portfolioValues[i] > peak) {
            peak = portfolioValues[i];
        }

        if (peak > 0.0) {
            double drawdown = (peak - portfolioValues[i]) / peak * 100.0;

            if (drawdown > maxDrawdown) {
                maxDrawdown = drawdown;
            }
        }
    }

    return maxDrawdown;
}