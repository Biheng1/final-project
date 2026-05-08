#include "TradeStack.h"
#include <iostream>
using namespace std;

TradeStack::TradeStack() {
    top = nullptr;
    size = 0;
}

TradeStack::~TradeStack() {
    while (!isEmpty()) {
        pop();
    }
}

void TradeStack::push(const TradeRecord& record) {
    StackNode* newNode = new StackNode;
    newNode->data = record;
    newNode->next = top;

    top = newNode;
    size++;
}

TradeRecord TradeStack::pop() {
    if (isEmpty()) {
        cout << "Trade stack is empty." << endl;
        return TradeRecord();
    }

    StackNode* nodeToDelete = top;
    TradeRecord record = top->data;

    top = top->next;
    delete nodeToDelete;
    size--;

    return record;
}

TradeRecord TradeStack::peek() const {
    if (isEmpty()) {
        cout << "Trade stack is empty." << endl;
        return TradeRecord();
    }

    return top->data;
}

bool TradeStack::isEmpty() const {
    return top == nullptr;
}

int TradeStack::getSize() const {
    return size;
}

void TradeStack::printAll() const {
    if (isEmpty()) {
        cout << "No trade history." << endl;
        return;
    }

    StackNode* current = top;

    cout << "Trade History (most recent first):" << endl;
    while (current != nullptr) {
        const TradeRecord& record = current->data;

        cout << record.date << " | "
             << record.action << " "
             << record.shares << " shares of "
             << record.ticker << " at $"
             << record.price << " per share | Total: $"
             << record.totalCost << endl;

        current = current->next;
    }
}
