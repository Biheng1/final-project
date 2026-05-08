#include "StockBST.h"
#include <iostream>
using namespace std;

StockBST::StockBST() {
    root = nullptr;
}

StockBST::~StockBST() {
    clear();
}

StockBST::BSTNode* StockBST::insertHelper(BSTNode* node,
                                          const string& ticker,
                                          double key,
                                          int year) {
    if (node == nullptr) {
        return new BSTNode(ticker, key, year);
    }

    if (key < node->key) {
        node->left = insertHelper(node->left, ticker, key, year);
    }
    else {
        node->right = insertHelper(node->right, ticker, key, year);
    }

    return node;
}

StockBST::BSTNode* StockBST::searchHelper(BSTNode* node, double key) const {
    if (node == nullptr || node->key == key) {
        return node;
    }

    if (key < node->key) {
        return searchHelper(node->left, key);
    }

    return searchHelper(node->right, key);
}

void StockBST::rangeSearchHelper(BSTNode* node,
                                 double low,
                                 double high,
                                 vector<BSTNode*>& results) const {
    if (node == nullptr) {
        return;
    }

    if (low < node->key) {
        rangeSearchHelper(node->left, low, high, results);
    }

    if (low <= node->key && node->key <= high) {
        results.push_back(node);
    }

    if (node->key <= high) {
        rangeSearchHelper(node->right, low, high, results);
    }
}

void StockBST::inorderHelper(BSTNode* node) const {
    if (node == nullptr) {
        return;
    }

    inorderHelper(node->left);
    cout << node->ticker << " (" << node->key << ")";
    if (node->year != 0) {
        cout << " [" << node->year << "]";
    }
    cout << endl;
    inorderHelper(node->right);
}

void StockBST::preorderHelper(BSTNode* node) const {
    if (node == nullptr) {
        return;
    }

    cout << node->ticker << " (" << node->key << ")";
    if (node->year != 0) {
        cout << " [" << node->year << "]";
    }
    cout << endl;
    preorderHelper(node->left);
    preorderHelper(node->right);
}

void StockBST::postorderHelper(BSTNode* node) const {
    if (node == nullptr) {
        return;
    }

    postorderHelper(node->left);
    postorderHelper(node->right);
    cout << node->ticker << " (" << node->key << ")";
    if (node->year != 0) {
        cout << " [" << node->year << "]";
    }
    cout << endl;
}

int StockBST::heightHelper(BSTNode* node) const {
    if (node == nullptr) {
        return -1;
    }

    int leftHeight = heightHelper(node->left);
    int rightHeight = heightHelper(node->right);

    if (leftHeight > rightHeight) {
        return leftHeight + 1;
    }

    return rightHeight + 1;
}

void StockBST::clearHelper(BSTNode* node) {
    if (node == nullptr) {
        return;
    }

    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
}

void StockBST::insert(const string& ticker, double key, int year) {
    root = insertHelper(root, ticker, key, year);
}

StockBST::BSTNode* StockBST::search(double key) const {
    return searchHelper(root, key);
}

void StockBST::rangeSearch(double low,
                           double high,
                           vector<BSTNode*>& results) const {
    rangeSearchHelper(root, low, high, results);
}

void StockBST::inorder() const {
    if (root == nullptr) {
        cout << "BST is empty." << endl;
        return;
    }

    inorderHelper(root);
}

void StockBST::preorder() const {
    if (root == nullptr) {
        cout << "BST is empty." << endl;
        return;
    }

    preorderHelper(root);
}

void StockBST::postorder() const {
    if (root == nullptr) {
        cout << "BST is empty." << endl;
        return;
    }

    postorderHelper(root);
}

int StockBST::getHeight() const {
    return heightHelper(root);
}

StockBST::BSTNode* StockBST::findMax() const {
    if (root == nullptr) {
        return nullptr;
    }

    BSTNode* current = root;
    while (current->right != nullptr) {
        current = current->right;
    }

    return current;
}

void StockBST::clear() {
    clearHelper(root);
    root = nullptr;
}
