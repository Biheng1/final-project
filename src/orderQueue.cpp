#include "OrderQueue.h"
#include <iostream>
using namespace std;

OrderQueue::OrderQueue() {
    front = nullptr;
    back = nullptr;
    size = 0;
}

OrderQueue::~OrderQueue() {
    while (!isEmpty()) {
        dequeue();
    }
}

void OrderQueue::enqueue(const Order& order) {
    QueueNode* newNode = new QueueNode;
    newNode->data = order;
    newNode->next = nullptr;

    if (isEmpty()) {
        front = newNode;
        back = newNode;
    }
    else {
        back->next = newNode;
        back = newNode;
    }

    size++;
}

Order OrderQueue::dequeue() {
    if (isEmpty()) {
        cout << "Order queue is empty." << endl;
        return Order();
    }

    QueueNode* nodeToDelete = front;
    Order order = front->data;

    front = front->next;
    delete nodeToDelete;
    size--;

    if (front == nullptr) {
        back = nullptr;
    }

    return order;
}

Order OrderQueue::peek() const {
    if (isEmpty()) {
        cout << "Order queue is empty." << endl;
        return Order();
    }

    return front->data;
}

bool OrderQueue::isEmpty() const {
    return front == nullptr;
}

int OrderQueue::getSize() const {
    return size;
}

void OrderQueue::printAll() const {
    if (isEmpty()) {
        cout << "No pending orders." << endl;
        return;
    }

    QueueNode* current = front;

    cout << "Pending Orders (oldest first):" << endl;
    while (current != nullptr) {
        const Order& order = current->data;

        cout << order.submittedDate << " | "
             << order.type << " "
             << order.side << " "
             << order.shares << " shares of "
             << order.ticker;

        if (order.type == "LIMIT") {
            cout << " at target price $" << order.targetPrice;
        }

        cout << endl;

        current = current->next;
    }
}