#include <vector>
#include <stdexcept>
#include <iostream>

class MinHeap {
private:
    std::vector<float> vctr = { -1.0f };  // 1-based indexing (placeholder at index 0)

    // Helper functions
    int getParent(int index) { return index >> 1; }      // i / 2
    int getLeftChild(int index) { return index << 1; }   // i * 2
    int getRightChild(int index) { return (index << 1) + 1; }  // i * 2 + 1

    // Restores the heap property by moving smaller values up
    void heapify(int index) {
        int smallest = index;
        int leftChild = getLeftChild(index);
        int rightChild = getRightChild(index);

        if (leftChild < vctr.size() && vctr[leftChild] < vctr[smallest]) {
            smallest = leftChild;
        }
        if (rightChild < vctr.size() && vctr[rightChild] < vctr[smallest]) {
            smallest = rightChild;
        }

        if (index != smallest) {
            std::swap(vctr[index], vctr[smallest]);
            heapify(smallest);  // Recursively fix the affected subtree
        }
    }

public:
    // Inserts a new value into the heap
    void insert(float value) {
        vctr.push_back(value);
        int i = vctr.size() - 1;

        // Bubble up the new value if it's smaller than its parent
        while (i != 1 && vctr[i] < vctr[getParent(i)]) {
            std::swap(vctr[i], vctr[getParent(i)]);
            i = getParent(i);
        }
    }

    // Removes and returns the minimum value (root)
    float Pop() {
        if (isEmpty()) {
            throw std::runtime_error("Heap is empty!");
        }

        float minValue = vctr[1];
        std::swap(vctr[1], vctr.back());
        vctr.pop_back();
        heapify(1);  // Restore heap property
        return minValue;
    }

    // Checks if the heap is empty
    bool isEmpty() const {
        return vctr.size() <= 1;
    }

    // Returns the minimum value without removal
    float Top() const {
        if (isEmpty()) {
            throw std::runtime_error("Heap is empty!");
        }
        return vctr[1];
    }

    // Optional: Print for debugging (matches MaxHeap style)
    void print() const {
        std::cout << "MinHeap elements (excluding placeholder):" << std::endl;
        for (size_t i = 1; i < vctr.size(); ++i) {
            std::cout << vctr[i] << std::endl;
        }
    }
};