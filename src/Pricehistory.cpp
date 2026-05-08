#include "PriceHistory.h"
#include "CSVParser.h"
#include <iomanip>
#include <iostream>

PriceHistory::PriceHistory()
    : head(nullptr), tail(nullptr), size(0) {}

PriceHistory::~PriceHistory() {
    PriceNode* current = head;

    while (current != nullptr) {
        PriceNode* next = current->next;
        delete current;
        current = next;
    }

    head = nullptr;
    tail = nullptr;
    size = 0;
}

void PriceHistory::append(const string& date, double open, double high,
                          double low, double close, long volume) {
    PriceNode* node = new PriceNode(date, open, high, low, close, volume);

    if (head == nullptr) {
        head = node;
        tail = node;
    } else {
        tail->next = node;
        node->prev = tail;
        tail = node;
    }

    ++size;
}

PriceNode* PriceHistory::findByDate(const string& date) const {
    PriceNode* current = head;

    while (current != nullptr) {
        if (current->date == date) {
            return current;
        }

        current = current->next;
    }

    return nullptr;
}

void PriceHistory::printRange(const string& startDate, const string& endDate) const {
    PriceNode* current = head;

    cout << fixed << setprecision(2);

    while (current != nullptr) {
        if (current->date >= startDate && current->date <= endDate) {
            cout << current->date
                 << "  Open: " << current->open
                 << "  High: " << current->high
                 << "  Low: " << current->low
                 << "  Close: " << current->close
                 << "  Volume: " << current->volume
                 << endl;
        }

        current = current->next;
    }
}

PriceNode* PriceHistory::getHead() const {
    return head;
}

PriceNode* PriceHistory::getTail() const {
    return tail;
}

int PriceHistory::getSize() const {
    return size;
}

PriceHistory::Iterator::Iterator(PriceNode* node)
    : current(node) {}

PriceHistory::Iterator& PriceHistory::Iterator::operator++() {
    if (current != nullptr) {
        current = current->next;
    }

    return *this;
}

PriceNode& PriceHistory::Iterator::operator*() {
    return *current;
}

bool PriceHistory::Iterator::operator!=(const Iterator& other) const {
    return current != other.current;
}

PriceHistory::Iterator PriceHistory::begin() const {
    return Iterator(head);
}

PriceHistory::Iterator PriceHistory::end() const {
    return Iterator(nullptr);
}

PriceHistory::ReverseIterator::ReverseIterator(PriceNode* node)
    : current(node) {}

PriceHistory::ReverseIterator& PriceHistory::ReverseIterator::operator++() {
    if (current != nullptr) {
        current = current->prev;
    }

    return *this;
}

PriceNode& PriceHistory::ReverseIterator::operator*() {
    return *current;
}

bool PriceHistory::ReverseIterator::operator!=(const ReverseIterator& other) const {
    return current != other.current;
}

PriceHistory::ReverseIterator PriceHistory::rbegin() const {
    return ReverseIterator(tail);
}

PriceHistory::ReverseIterator PriceHistory::rend() const {
    return ReverseIterator(nullptr);
}
