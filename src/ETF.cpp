#include "ETF.h"
#include "CSVParser.h"
#include <cmath>
#include <iomanip>
#include <iostream>

ETF::ETF(const string& ticker, const string& name, const string& sector, double expenseRatio)
    : Stock(ticker, name, sector), expenseRatio(expenseRatio) {}

ETF::~ETF() {}

double ETF::getExpenseRatio() const {
    return expenseRatio;
}

void ETF::setExpenseRatio(double ratio) {
    expenseRatio = ratio;
}

double ETF::calculate10YearCAGR() const {
    if (history == nullptr || history->getSize() == 0) {
        return 0.0;
    }

    PriceNode* endNode = history->getTail();
    if (endNode == nullptr) {
        return 0.0;
    }

    int endYear = CSVParser::extractYear(endNode->date);
    int startYear = endYear - 10;
    double startValue = getYearStartPrice(startYear);
    double endValue = getYearEndPrice(endYear);

    if (startValue <= 0.0 || endValue <= 0.0) {
        return 0.0;
    }

    return (pow(endValue / startValue, 1.0 / 10.0) - 1.0) * 100.0;
}

void ETF::printSummary() const {
    Stock::printSummary();

    cout << fixed << setprecision(2);
    cout << "Expense Ratio: " << expenseRatio * 100.0 << "%" << endl;
    cout << "10-Year CAGR: " << calculate10YearCAGR() << "%" << endl;
}

string ETF::getType() const {
    return "ETF";
}
