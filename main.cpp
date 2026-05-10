/*
 * ESE 224 – Final Project: StockSim
 * Historical Market Analyzer & Trading Strategy Simulator
 *
 * Student Name : Irfan and Biheng
 * Student ID   : ___________________________
 *
 * Instructions:
 *   1. Implement all classes listed in the header files under include/.
 *   2. Create corresponding .cpp files in src/ for each header.
 *   3. Place your Yahoo Finance CSV files in data/ (SPY.csv, AAPL.csv, TSLA.csv).
 *   4. Complete the menu handlers below — each case should call the relevant
 *      class methods you implemented.
 *   5. Do NOT use std::queue, std::stack, std::list, std::map, std::unordered_map,
 *      or any external library. std::vector, std::string, std::sort are allowed.
 *
 * Compile with C++11 or later:
 *   g++ -std=c++11 -Iinclude src*.cpp main.cpp -o stocksim
 */

#include <iostream> // for std::cout, std::cin
#include <string>   // for std::string, std::getline
#include <limits> // for std::numeric_limits used in input validation
#include <iomanip> // for std::fixed and std::setprecision
#include <vector> // for std::vector used in StockManager and BST range search results

// Include all your headers here once implemented
#include "include/FinancialAsset.h"
#include "include/PriceNode.h"
#include "include/PriceHistory.h"
#include "include/CSVParser.h"
#include "include/Stock.h"
#include "include/ETF.h"
#include "include/CircularQueue.h"
#include "include/TradeStack.h"
#include "include/OrderQueue.h"
#include "include/StockBST.h"
#include "include/Portfolio.h"
#include "include/TradingStrategy.h"
#include "include/FixedSIPStrategy.h"
#include "include/DynamicSIPStrategy.h"
#include "include/GoldenCrossStrategy.h"
#include "include/MomentumStrategy.h"
#include "include/StockManager.h"

using namespace std;

// ---------------------------------------------------------------
// Forward declarations for menu handler functions
// ---------------------------------------------------------------
void menuLoadData(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager);
void menuDisplayHistory(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager);
void menuSearchByDate(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager);
void menuBSTRangeSearch(StockBST& bst);
void menuInsertIntoBST(StockBST& bst, StockManager<Stock>& stockManager);
void menuDisplayBST(StockBST& bst);
void menuAddToPortfolio(Portfolio& portfolio);
void menuRemoveFromPortfolio(Portfolio& portfolio);
void menuQueueOrder(Portfolio& portfolio);
void menuExecuteOrder(Portfolio& portfolio);
void menuUndoTrade(Portfolio& portfolio);
void menuRunStrategy(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager);
void menuCompareStrategies(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager);
void menuPortfolioSummary(Portfolio& portfolio);
void menuTradeHistory(Portfolio& portfolio);

// Bonus
//void parameterSweep(ETF* spy, double monthlyCapital, int startYear, int endYear, StockBST& bst);

// ---------------------------------------------------------------
// Utility: print the main menu
// ---------------------------------------------------------------
void printMenu(const string& studentName, const string& studentID) { // displays the main menu options
    cout << "\n===== StockSim: Historical Market Analyzer =====\n";
    cout << "Student: " << studentName << "  |  ID: " << studentID << "\n";
    cout << "-------------------------------------------------\n";
    cout << " [1]  Load stock data from CSV\n";
    cout << " [2]  Display price history (linked list traversal)\n";
    cout << " [3]  Search by date range\n";
    cout << " [4]  Find stocks by return range (BST range query)\n";
    cout << " [5]  Insert stock into performance BST\n";
    cout << " [6]  Display BST (inorder / preorder / postorder)\n";
    cout << " [7]  Add stock to portfolio\n";
    cout << " [8]  Remove stock from portfolio\n";
    cout << " [9]  Queue a pending order\n";
    cout << "[10]  Execute next pending order\n";
    cout << "[11]  Undo last trade\n";
    cout << "[12]  Run strategy simulation\n";
    cout << "[13]  Compare all strategies head-to-head\n";
    cout << "[14]  Display portfolio summary\n";
    cout << "[15]  Display full trade history\n";
    cout << " [0]  Exit\n";
    cout << "-------------------------------------------------\n";
    cout << "Enter choice: ";
}

// ---------------------------------------------------------------
// main
// ---------------------------------------------------------------
int main() { // main menu loop and calls menu handlers based on user input
    // --- Student login ---
    string studentName, studentID;
    cout << "========================================\n";
    cout << "  ESE 224 StockSim - Student Login\n";
    cout << "========================================\n";
    cout << "Enter your full name: ";
    getline(cin, studentName);
    cout << "Enter your student ID: ";
    getline(cin, studentID);
    cout << "\nWelcome, " << studentName << "!\n";

    // --- Initialize shared objects ---
    StockManager<ETF>   etfManager;
    StockManager<Stock> stockManager;
    StockBST            performanceBST;
    Portfolio           portfolio(studentName, 10000.0);  // start with $10,000 cash

    // --- Main menu loop ---
    int choice = -1;
    while (choice != 0) {
        printMenu(studentName, studentID);
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case  1: menuLoadData(etfManager, stockManager);                        break;
            case  2: menuDisplayHistory(stockManager, etfManager);                  break;
            case  3: menuSearchByDate(stockManager, etfManager);                    break;
            case  4: menuBSTRangeSearch(performanceBST);                            break;
            case  5: menuInsertIntoBST(performanceBST, stockManager);               break;
            case  6: menuDisplayBST(performanceBST);                                break;
            case  7: menuAddToPortfolio(portfolio);                                 break;
            case  8: menuRemoveFromPortfolio(portfolio);                            break;
            case  9: menuQueueOrder(portfolio);                                     break;
            case 10: menuExecuteOrder(portfolio);                                   break;
            case 11: menuUndoTrade(portfolio);                                      break;
            case 12: menuRunStrategy(etfManager, stockManager);                     break;
            case 13: menuCompareStrategies(etfManager, stockManager);                            break;
            case 14: menuPortfolioSummary(portfolio);                               break;
            case 15: menuTradeHistory(portfolio);                                   break;
            case  0: cout << "Goodbye, " << studentName << "!\n";                  break;
            default: cout << "Invalid choice. Please enter 0–15.\n";               break;
        }
    }

    return 0;
}

// ---------------------------------------------------------------
// Menu handler stubs — implement each one below
// ---------------------------------------------------------------

// ---------------------------------------------------------------
// Menu handler implementations
// ---------------------------------------------------------------

void menuLoadData(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager) { // prompts user for which CSV files to load, then creates Stock/ETF objects, loads their price history, and adds them to the respective StockManager
    string choice;

    cout << "\nChoose data to load:" << endl;
    cout << "1. SPX" << endl;
    cout << "2. AMZN" << endl;
    cout << "3. NVDA" << endl;
    cout << "4. All" << endl;
    cout << "Enter choice: ";

    getline(cin, choice);

    if (choice == "1" || choice == "4") {
        ETF* spx = new ETF("SPX", "S&P 500 Index", "Index", 0.0000);

        if (spx->loadFromCSV("data/SPX.csv")) {
            etfManager.addAsset(spx);
        }
        else {
            delete spx;
        }
    }

    if (choice == "2" || choice == "4") {
        Stock* amzn = new Stock("AMZN", "Amazon Inc.", "Consumer");

        if (amzn->loadFromCSV("data/AMZN.csv")) {
            stockManager.addAsset(amzn);
        }
        else {
            delete amzn;
        }
    }

    if (choice == "3" || choice == "4") {
        Stock* nvda = new Stock("NVDA", "NVIDIA Corporation", "Technology");

        if (nvda->loadFromCSV("data/NVidia_stock_history.csv")) {
            stockManager.addAsset(nvda);
        }
        else {
            delete nvda;
        }
    }

    cout << "Data loading complete." << endl;
}

void menuDisplayHistory(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager) {

    string ticker;
    string startDate;
    string endDate;

    cout << "Enter ticker: ";
    getline(cin, ticker);

    cout << "Enter start date (YYYY-MM-DD): ";
    getline(cin, startDate);

    cout << "Enter end date (YYYY-MM-DD): ";
    getline(cin, endDate);

    ETF* etf = etfManager.findByTicker(ticker);

    if (etf != nullptr) {
        etf->getHistory()->printRange(startDate, endDate);
        return;
    }

    Stock* stock = stockManager.findByTicker(ticker);

    if (stock != nullptr) {
        stock->getHistory()->printRange(startDate, endDate);
        return;
    }

    cout << "Ticker not found." << endl;
}

void menuSearchByDate(StockManager<Stock>& stockManager, StockManager<ETF>& etfManager) {

    menuDisplayHistory(stockManager, etfManager);
}

void menuBSTRangeSearch(StockBST& bst) { // prompts user for a return % range, performs a BST range search, and displays all matching tickers and their returns
    double low;
    double high;

    cout << "Enter minimum return: ";
    cin >> low;

    cout << "Enter maximum return: ";
    cin >> high;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<StockBST::BSTNode*> results;

    bst.rangeSearch(low, high, results);

    if (results.empty()) {
        cout << "No matching results." << endl;
        return;
    }

    for (size_t i = 0; i < results.size(); i++) {
        cout << results[i]->ticker
             << " | Return: "
             << results[i]->key
             << "%" << endl;
    }
}

void menuInsertIntoBST(StockBST& bst, StockManager<Stock>& stockManager) {

    string ticker;
    int year;

    cout << "Enter ticker: ";
    getline(cin, ticker);

    cout << "Enter year: ";
    cin >> year;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Stock* stock = stockManager.findByTicker(ticker);

    if (stock == nullptr) {
        cout << "Ticker not found." << endl;
        return;
    }

    double annualReturn = stock->calculateAnnualReturn(year);

    bst.insert(ticker, annualReturn, year);

    cout << "Inserted into BST." << endl;
}

void menuDisplayBST(StockBST& bst) { // prompts user for traversal type, then displays the BST accordingly
    int choice;

    cout << "\n1. Inorder" << endl;
    cout << "2. Preorder" << endl;
    cout << "3. Postorder" << endl;
    cout << "Enter choice: ";

    cin >> choice;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) {
        bst.inorder();
    }
    else if (choice == 2) {
        bst.preorder();
    }
    else if (choice == 3) {
        bst.postorder();
    }
    else {
        cout << "Invalid choice." << endl;
    }
}

void menuAddToPortfolio(Portfolio& portfolio) { // prompts user for ticker, shares, price, and date, then adds a position to the portfolio and records the trade
    string ticker;
    string date;
    int shares;
    double price;

    cout << "Enter ticker: ";
    getline(cin, ticker);

    cout << "Enter shares: ";
    cin >> shares;

    cout << "Enter price: ";
    cin >> price;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter date: ";
    getline(cin, date);

    portfolio.buyShares(ticker, shares, price, date);
}

void menuRemoveFromPortfolio(Portfolio& portfolio) { // prompts user for ticker, shares, price, and date, then sells the specified shares of the ticker from the portfolio
    string ticker;
    string date;
    int shares;
    double price;

    cout << "Enter ticker: ";
    getline(cin, ticker);

    cout << "Enter shares: ";
    cin >> shares;

    cout << "Enter price: ";
    cin >> price;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter date: ";
    getline(cin, date);

    portfolio.sellShares(ticker, shares, price, date);
}

void menuQueueOrder(Portfolio& portfolio) { // prompts user for order details and adds a pending order to the OrderQueue
    Order order;

    cout << "Enter ticker: ";
    getline(cin, order.ticker);

    cout << "Enter order type (MARKET/LIMIT): ";
    getline(cin, order.type);

    cout << "Enter side (BUY/SELL): ";
    getline(cin, order.side);

    cout << "Enter shares: ";
    cin >> order.shares;

    if (order.type == "LIMIT") {
        cout << "Enter target price: ";
        cin >> order.targetPrice;
    }
    else {
        order.targetPrice = 0.0;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter submitted date: ";
    getline(cin, order.submittedDate);

    portfolio.queueOrder(order);
}

void menuExecuteOrder(Portfolio& portfolio) { // dequeues the front Order and executes it if conditions are met: MARKET order → execute immediately at currentPrice; LIMIT BUY → execute if currentPrice <= order.targetPrice; LIMIT SELL → execute if currentPrice >= order.targetPrice. Prints "Order skipped" if LIMIT conditions are not met (order is discarded).
    double currentPrice;
    string date;

    cout << "Enter current market price: ";
    cin >> currentPrice;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter today's date: ";
    getline(cin, date);

    portfolio.executeNextOrder(currentPrice, date);
}

void menuUndoTrade(Portfolio& portfolio) { // undoes the most recent trade by popping the TradeStack and performing the opposite transaction (buy→sell, sell→buy). Restores cashBalance to its pre-trade state.
    portfolio.undoLastTrade();
}

void menuRunStrategy(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager) {

    string ticker;

    double monthlyCapital;
    int startYear;
    int endYear;

    int strategyChoice;

    cout << "Enter ticker (SPX, AMZN, NVDA): ";
    getline(cin, ticker);

    cout << "Enter monthly capital: ";
    cin >> monthlyCapital;

    cout << "Enter start year: ";
    cin >> startYear;

    cout << "Enter end year: ";
    cin >> endYear;

    cout << "\nChoose strategy:" << endl;
    cout << "1. Fixed SIP" << endl;
    cout << "2. Dynamic SIP" << endl;
    cout << "3. Golden Cross" << endl;
    cout << "4. Momentum" << endl;
    cout << "Enter choice: ";

    cin >> strategyChoice;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    PriceHistory* history = nullptr;

    ETF* etf = etfManager.findByTicker(ticker);

    if (etf != nullptr) {
        history = etf->getHistory();
    }
    else {
        Stock* stock = stockManager.findByTicker(ticker);

        if (stock != nullptr) {
            history = stock->getHistory();
        }
    }

    if (history == nullptr) {
        cout << "Ticker not loaded." << endl;
        return;
    }

    TradingStrategy* strategy = nullptr;

    if (strategyChoice == 1) {
        strategy = new FixedSIPStrategy();
    }
    else if (strategyChoice == 2) {
        double dipThreshold;
        double rallyThreshold;
        double multiplier;

        cout << "Enter dip threshold (%): ";
        cin >> dipThreshold;

        cout << "Enter rally threshold (%): ";
        cin >> rallyThreshold;

        cout << "Enter multiplier: ";
        cin >> multiplier;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        strategy = new DynamicSIPStrategy(
            dipThreshold,
            rallyThreshold,
            multiplier
        );
    }
    else if (strategyChoice == 3) {
        strategy = new GoldenCrossStrategy();
    }
    else if (strategyChoice == 4) {
        double threshold;

        cout << "Enter momentum threshold (%): ";
        cin >> threshold;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        strategy = new MomentumStrategy(threshold);
    }
    else {
        cout << "Invalid strategy." << endl;
        return;
    }
    
    SimResult result =
        strategy->backtest(
            history,
            monthlyCapital,
            startYear,
            endYear
    );

    strategy->printResult(result);

    delete strategy;
}

void menuCompareStrategies(StockManager<ETF>& etfManager, StockManager<Stock>& stockManager) {

    string ticker;

    cout << "Enter ticker (SPX, AMZN, NVDA): ";
    getline(cin, ticker);

    PriceHistory* history = nullptr;

    ETF* etf = etfManager.findByTicker(ticker);

    if (etf != nullptr) {
        history = etf->getHistory();
    }
    else {
        Stock* stock = stockManager.findByTicker(ticker);

        if (stock != nullptr) {
            history = stock->getHistory();
        }
    }

    if (history == nullptr) {
        cout << "Ticker not loaded." << endl;
        return;
    }

    double monthlyCapital;
    int startYear;
    int endYear;

    cout << "Enter monthly capital: ";
    cin >> monthlyCapital;

    cout << "Enter start year: ";
    cin >> startYear;

    cout << "Enter end year: ";
    cin >> endYear;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    FixedSIPStrategy fixedStrategy;
    DynamicSIPStrategy dynamicStrategy(10.0, 20.0, 2.0);
    GoldenCrossStrategy goldenStrategy;
    MomentumStrategy momentumStrategy(5.0);

    SimResult fixedResult =
        fixedStrategy.backtest(history, monthlyCapital, startYear, endYear);

    SimResult dynamicResult =
        dynamicStrategy.backtest(history, monthlyCapital, startYear, endYear);

    SimResult goldenResult =
        goldenStrategy.backtest(history, monthlyCapital, startYear, endYear);

    SimResult momentumResult =
        momentumStrategy.backtest(history, monthlyCapital, startYear, endYear);

    cout << std::fixed << setprecision(2);

    cout << "\n===== Strategy Comparison for " << ticker << " =====" << endl;

    fixedStrategy.printResult(fixedResult);
    cout << endl;

    dynamicStrategy.printResult(dynamicResult);
    cout << endl;

    goldenStrategy.printResult(goldenResult);
    cout << endl;

    momentumStrategy.printResult(momentumResult);
    cout << endl;
}
void menuPortfolioSummary(Portfolio& portfolio) { // displays current holdings and their unrealized returns
    portfolio.printHoldings();
}

void menuTradeHistory(Portfolio& portfolio) { // displays all executed trades in chronological order
    portfolio.printTradeHistory();
}

// ---------------------------------------------------------------
// BONUS: Parameter Sweep for DynamicSIPStrategy
// ---------------------------------------------------------------
//void parameterSweep(ETF* spy, double monthlyCapital, int startYear, int endYear, StockBST& bst) {
    // TODO:
    //  For dipThreshold from 3.0 to 20.0 (step 1.0):
    //    Create DynamicSIPStrategy(dipThreshold, 10.0, 2.0)
    //    Run backtest on spy->getHistory()
    //    Insert into bst keyed by result.finalValue, with year=0
    //      and ticker = "dip=" + to_string((int)dipThreshold) + "%"
    //  After all insertions:
    //    Call bst.inorder() to print results ranked worst to best
    //    Call bst.findMax() to identify the optimal threshold
   // cout << "(TODO: implement parameterSweep)" << endl;
//}