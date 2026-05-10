#ifndef PRICE_NODE_H
#define PRICE_NODE_H

#include <string>
using namespace std;

// Node for the doubly-linked price history list.
// Each node represents one trading day from the Yahoo Finance CSV.
// Note: Yahoo data skips weekends and market holidays — nodes are NOT
//       guaranteed to be consecutive calendar days.
struct PriceNode {
    string date;    // "YYYY-MM-DD"
    double open;
    double high;
    double low;
    double close;
    long long volume;
    PriceNode* next;  // toward more recent dates (chronological forward)
    PriceNode* prev;  // toward older dates (chronological backward)

        PriceNode(const string& d, double open_, double high_,
          double low_, double close_, long long volume_)
        : date(d), open(open_), high(high_), low(low_),
          close(close_), volume(volume_), next(nullptr), prev(nullptr) {}
};

#endif // PRICE_NODE_H