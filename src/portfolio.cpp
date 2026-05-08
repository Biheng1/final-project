#include "Portfolio.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
using namespace std;

Portfolio::Portfolio(const string& ownerName, double initialCash) {
    this->ownerName = ownerName;
    cashBalance = initialCash;
}

void Portfolio::buyShares(const string& ticker,
                          int shares,
                          double price,
                          const string& date) {
    if (shares <= 0 || price < 0.0) {
        cout << "Invalid buy order." << endl;
        return;
    }

    double totalCost = price * shares;
    if (totalCost > cashBalance) {
        cout << "Not enough cash to buy shares." << endl;
        return;
    }

    for (size_t i = 0; i < holdings.size(); i++) {
        if (holdings[i].ticker == ticker) {
            double oldTotalCost = holdings[i].avgCostBasis * holdings[i].shares;
            double newTotalCost = oldTotalCost + totalCost;

            holdings[i].shares += shares;
            holdings[i].avgCostBasis = newTotalCost / holdings[i].shares;
            holdings[i].currentPrice = price;
            cashBalance -= totalCost;

            TradeRecord record;
            record.ticker = ticker;
            record.date = date;
            record.price = price;
            record.shares = shares;
            record.action = "BUY";
            record.totalCost = totalCost;
            tradeHistory.push(record);
            return;
        }
    }

    Position position;
    position.ticker = ticker;
    position.shares = shares;
    position.avgCostBasis = price;
    position.currentPrice = price;
    holdings.push_back(position);

    cashBalance -= totalCost;

    TradeRecord record;
    record.ticker = ticker;
    record.date = date;
    record.price = price;
    record.shares = shares;
    record.action = "BUY";
    record.totalCost = totalCost;
    tradeHistory.push(record);
}

void Portfolio::sellShares(const string& ticker,
                           int shares,
                           double price,
                           const string& date) {
    if (shares <= 0 || price < 0.0) {
        cout << "Invalid sell order." << endl;
        return;
    }

    for (size_t i = 0; i < holdings.size(); i++) {
        if (holdings[i].ticker == ticker) {
            if (shares > holdings[i].shares) {
                cout << "Not enough shares to sell." << endl;
                return;
            }

            double proceeds = price * shares;
            holdings[i].shares -= shares;
            holdings[i].currentPrice = price;
            cashBalance += proceeds;

            if (holdings[i].shares == 0) {
                holdings.erase(holdings.begin() + i);
            }

            TradeRecord record;
            record.ticker = ticker;
            record.date = date;
            record.price = price;
            record.shares = shares;
            record.action = "SELL";
            record.totalCost = -proceeds;
            tradeHistory.push(record);
            return;
        }
    }

    cout << "Position not found." << endl;
}

void Portfolio::undoLastTrade() {
    if (tradeHistory.isEmpty()) {
        cout << "No trade to undo." << endl;
        return;
    }

    TradeRecord record = tradeHistory.pop();

    if (record.action == "BUY") {
        for (size_t i = 0; i < holdings.size(); i++) {
            if (holdings[i].ticker == record.ticker) {
                if (record.shares >= holdings[i].shares) {
                    holdings.erase(holdings.begin() + i);
                }
                else {
                    holdings[i].shares -= record.shares;
                    holdings[i].currentPrice = record.price;
                }
                cashBalance += record.price * record.shares;
                cout << "Undid BUY trade." << endl;
                return;
            }
        }
    }
    else if (record.action == "SELL") {
        double cost = record.price * record.shares;

        for (size_t i = 0; i < holdings.size(); i++) {
            if (holdings[i].ticker == record.ticker) {
                double oldTotalCost = holdings[i].avgCostBasis * holdings[i].shares;
                double restoredCost = record.price * record.shares;

                holdings[i].shares += record.shares;
                holdings[i].avgCostBasis =
                    (oldTotalCost + restoredCost) / holdings[i].shares;
                holdings[i].currentPrice = record.price;
                cashBalance -= cost;
                cout << "Undid SELL trade." << endl;
                return;
            }
        }

        Position position;
        position.ticker = record.ticker;
        position.shares = record.shares;
        position.avgCostBasis = record.price;
        position.currentPrice = record.price;
        holdings.push_back(position);
        cashBalance -= cost;
        cout << "Undid SELL trade." << endl;
    }
}

void Portfolio::queueOrder(const Order& order) {
    pendingOrders.enqueue(order);
}

void Portfolio::executeNextOrder(double currentPrice, const string& date) {
    if (pendingOrders.isEmpty()) {
        cout << "No pending orders." << endl;
        return;
    }

    Order order = pendingOrders.dequeue();
    bool shouldExecute = false;

    if (order.type == "MARKET") {
        shouldExecute = true;
    }
    else if (order.type == "LIMIT" &&
             order.side == "BUY" &&
             currentPrice <= order.targetPrice) {
        shouldExecute = true;
    }
    else if (order.type == "LIMIT" &&
             order.side == "SELL" &&
             currentPrice >= order.targetPrice) {
        shouldExecute = true;
    }

    if (!shouldExecute) {
        cout << "Order skipped" << endl;
        return;
    }

    if (order.side == "BUY") {
        buyShares(order.ticker, order.shares, currentPrice, date);
    }
    else if (order.side == "SELL") {
        sellShares(order.ticker, order.shares, currentPrice, date);
    }
    else {
        cout << "Invalid order side." << endl;
    }
}

double Portfolio::getTotalMarketValue() const {
    double total = 0.0;

    for (size_t i = 0; i < holdings.size(); i++) {
        total += holdings[i].shares * holdings[i].currentPrice;
    }

    return total;
}

double Portfolio::getTotalValue() const {
    return getTotalMarketValue() + cashBalance;
}

double Portfolio::getTotalUnrealizedReturn() const {
    double totalMarketValue = 0.0;
    double totalCostBasis = 0.0;

    for (size_t i = 0; i < holdings.size(); i++) {
        totalMarketValue += holdings[i].shares * holdings[i].currentPrice;
        totalCostBasis += holdings[i].shares * holdings[i].avgCostBasis;
    }

    if (totalCostBasis == 0.0) {
        return 0.0;
    }

    return (totalMarketValue - totalCostBasis) / totalCostBasis * 100.0;
}

double Portfolio::getCashBalance() const {
    return cashBalance;
}

void Portfolio::updatePrice(const string& ticker, double newPrice) {
    for (size_t i = 0; i < holdings.size(); i++) {
        if (holdings[i].ticker == ticker) {
            holdings[i].currentPrice = newPrice;
            return;
        }
    }

    cout << "Position not found." << endl;
}

void Portfolio::sortHoldingsByUnrealizedReturn() {
    sort(holdings.begin(), holdings.end(),
         [](const Position& a, const Position& b) {
             double aReturn = 0.0;
             double bReturn = 0.0;

             if (a.avgCostBasis != 0.0) {
                 aReturn = (a.currentPrice - a.avgCostBasis) / a.avgCostBasis;
             }
             if (b.avgCostBasis != 0.0) {
                 bReturn = (b.currentPrice - b.avgCostBasis) / b.avgCostBasis;
             }

             return aReturn > bReturn;
         });
}

void Portfolio::sortHoldingsByTicker() {
    sort(holdings.begin(), holdings.end(),
         [](const Position& a, const Position& b) {
             return a.ticker < b.ticker;
         });
}

void Portfolio::printHoldings() const {
    cout << fixed << setprecision(2);
    cout << "Portfolio for " << ownerName << endl;
    cout << "Cash balance: $" << cashBalance << endl;

    if (holdings.empty()) {
        cout << "No holdings." << endl;
        return;
    }

    cout << "Holdings:" << endl;
    for (size_t i = 0; i < holdings.size(); i++) {
        double marketValue = holdings[i].shares * holdings[i].currentPrice;
        double unrealizedReturn = 0.0;

        if (holdings[i].avgCostBasis != 0.0) {
            unrealizedReturn =
                (holdings[i].currentPrice - holdings[i].avgCostBasis) /
                holdings[i].avgCostBasis * 100.0;
        }

        cout << holdings[i].ticker
             << " | Shares: " << holdings[i].shares
             << " | Avg Cost: $" << holdings[i].avgCostBasis
             << " | Current: $" << holdings[i].currentPrice
             << " | Market Value: $" << marketValue
             << " | Return: " << unrealizedReturn << "%"
             << endl;
    }

    cout << "Total market value: $" << getTotalMarketValue() << endl;
    cout << "Total portfolio value: $" << getTotalValue() << endl;
    cout << "Total unrealized return: "
         << getTotalUnrealizedReturn() << "%" << endl;
}

void Portfolio::printTradeHistory() const {
    tradeHistory.printAll();
}

void Portfolio::printPendingOrders() const {
    pendingOrders.printAll();
}
