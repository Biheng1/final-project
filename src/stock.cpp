#include "Stock.h"
#include "CSVParser.h"
#include <iomanip>
#include <iostream>

Stock::Stock(const string& ticker, const string& name, const string& sector)
    : FinancialAsset(ticker, name, sector), history(nullptr) {}

Stock::~Stock() {
    delete history;
    history = nullptr;
}

bool Stock::loadFromCSV(const string& filename) {
    PriceHistory* loadedHistory = CSVParser::loadHistory(filename);

    if (loadedHistory == nullptr) {
        cout << "Failed to load data for " << ticker << " from " << filename << endl;
        return false;
    }

    delete history;
    history = loadedHistory;

    cout << ticker << ": loaded " << history->getSize() << " trading days." << endl;
    return true;
}

PriceHistory* Stock::getHistory() const {
    return history;
}

double Stock::getPriceOnDate(const string& date) const {
    if (history == nullptr) {
        return -1.0;
    }

    PriceNode* node = history->findByDate(date);
    if (node == nullptr) {
        return -1.0;
    }

    return node->close;
}

double Stock::getYearStartPrice(int year) const {
    if (history == nullptr) {
        return -1.0;
    }

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        if (CSVParser::extractYear(node.date) == year) {
            return node.close;
        }
    }

    return -1.0;
}

double Stock::getYearEndPrice(int year) const {
    if (history == nullptr) {
        return -1.0;
    }

    for (PriceHistory::ReverseIterator it = history->rbegin(); it != history->rend(); ++it) {
        PriceNode& node = *it;
        if (CSVParser::extractYear(node.date) == year) {
            return node.close;
        }
    }

    return -1.0;
}

double Stock::calculateAnnualReturn(int year) const {
    double startPrice = getYearStartPrice(year);
    double endPrice = getYearEndPrice(year);

    if (startPrice <= 0.0 || endPrice < 0.0) {
        return 0.0;
    }

    return (endPrice - startPrice) / startPrice * 100.0;
}

void Stock::printSummary() const {
    cout << fixed << setprecision(2);
    cout << "Ticker: " << ticker << endl;
    cout << "Name: " << name << endl;
    cout << "Sector: " << sector << endl;
    cout << "Type: " << getType() << endl;

    if (history == nullptr || history->getSize() == 0) {
        cout << "No price history loaded." << endl;
        return;
    }

    PriceNode* first = history->getHead();
    PriceNode* last = history->getTail();

    cout << "Date Range: " << first->date << " to " << last->date << endl;
    cout << "Trading Days: " << history->getSize() << endl;
    cout << "First Close: $" << first->close << endl;
    cout << "Last Close: $" << last->close << endl;

    if (first->close > 0.0) {
        double totalReturn = (last->close - first->close) / first->close * 100.0;
        cout << "Overall Return: " << totalReturn << "%" << endl;
    } else {
        cout << "Overall Return: 0.00%" << endl;
    }
}

string Stock::getType() const {
    return "Stock";
}