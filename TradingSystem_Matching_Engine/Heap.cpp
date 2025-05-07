#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include <map>

//Eventually needs to be a vector of queues, 
// the heap represents the value of each price level, and the queue represents the individual price level for FIFO

class MaxHeap {

private:
	int size;

	std::vector<float> vctr = { -1.0f };

	int getParent(int index) // i / 2
	{
		return index >> 1;
	}
	int getLeftChild(int index) // i * 2
	{
		return index << 1;
	}
	int getRightChild(int index)
	{
		return (index << 1) + 1; //index * 2 +1
	}

	void heapify(int index);
	void insert(float value);
	void deleteValue(float value);
	void print();

public:

	MaxHeap(int size) {
		this->size = 1;
	}

	bool isEmpty() const
	{
		return size == 0;
	}
	auto Top() const { return vctr[1]; }
	float Pop();
};

void MaxHeap::heapify(int index) {

	//Psudocode for the Heapify algorithm/function
	/*
		1. Find the left child and right child using the 2*i+1 and 2*i+2 formula.
		2. Identify the largest among the root, left child, and right child.
		3. If the root node is not the largest, swap it with the child that has the largest value.
		4. Recursively apply heapify to the affected subtree.
		5. Repeat until the heap property is restored.
	*/

	int largest = index;
	int leftChild = getLeftChild(index);
	int rightChild = getRightChild(index);

	if (vctr[leftChild] > vctr[largest]) {
		largest = leftChild;
	}
	if (vctr[rightChild] > vctr[largest]) {
		largest = rightChild;
	}

	if (index != index) {
		//if the parent is not then swap them and run heapify again on the sub tree
		std::swap(vctr[index], vctr[largest]);
		heapify(largest);
	}
}

void MaxHeap::insert(float value) {
	//Psudocode for the Heapify algorithm/function
	/*
		1. insert the value at the end of the vector
		2. make i the index of the newly added value
		3. bubble to the top(if needed). could make a function for this
	*/
	vctr.push_back(value);
	int i = vctr.back();

	// while the index of the added value is not the root && the value of it is larger than its parent swap element with its parent
	while (i != 1 && vctr[i] > vctr[getParent(i)]) {

		std::swap(vctr[i], vctr[getParent(i)]);
		int parentIndex = getParent(i);
		i = parentIndex;
	}
}

float MaxHeap::Pop() {
	float valueToReturn = vctr[1];
	//remove the value from the vector IMPLEMENT THAT FUNCTION
	deleteValue(vctr[1]);
	heapify(1);
	return valueToReturn;
}

// O(n) as it needs to search for the value to delete.. //maybe dont need to delete the price level in the final implementation
void MaxHeap::deleteValue(float value) {
	int index = -1;

	for (int i = 0; i < vctr.size(); i++) {
		if (vctr[i] == value) {
			index = i;
		}
	}

	if (index == -1) {
		std::cout << "that value was not found in the heap!" << std::endl;
		return;
	}
}

void MaxHeap::print() {

	std::cout << "Printing Maxheap" << std::endl;
	for (int i = 0; i < vctr.size(); i++) {
		std::cout << vctr[i] << "" << std::endl;
	}
}

//test the heap here.

int main() {

	MaxHeap heap(10);

	// Test isEmpty on new heap
	std::cout << "Is empty (should be 1): " << heap.isEmpty() << std::endl;

}