#include <iostream>
#include <utility>
#include <cstring>
#include <list>
using namespace std;

// implementing a hash table with seperate chaining

class HashTable {
private:
	static const int hashGroups = 10;

	list < pair<int, string>> table[hashGroups];

public:
		bool isEmpty() const;
		int hashFunction(int key);
		void insertItem(int key, string value);
		void removeItem(int key);
		string searchTable(int key);
		void printTable();
};

// Needs to be implemented
string HashTable::searchTable(int key) {
	std::cout << "This funtion needs to be implemented" << std::endl;

	return "";
}

bool HashTable::isEmpty() const {
	for (int i = 0; i < hashGroups; i++) {
		if (!table[i].empty()) {
			return false;
		}
	}
	return true;
}

int HashTable::hashFunction(int key) {
	return key % hashGroups; //key 905 - > 5
}

void HashTable::insertItem(int key, string value) {
    int hashValue = hashFunction(key);
    auto& cell = table[hashValue];

    for (auto& item : cell) {
        if (item.first == key) {
            item.second = value;
            cout << "[WARNING] key exists";
            return;
        }
    }
    cell.emplace_back(key, value);
}

void HashTable::removeItem(int key)
{
	int hashValue = hashFunction(key);

	auto& cell = table[hashValue];

	auto bItr = begin(cell);

	bool keyExists = false;

	//checking if the value already exists

	for (; bItr != end(cell); bItr++) {

		if (bItr->first == key) {
			keyExists = true;
			bItr = cell.erase(bItr);
			cout << "[INFO] Item removed" << endl;
			break;
		}

	}

	if (!keyExists) {
		cout << "[WARNING] item not found" << endl;
	}
	return;
}

void HashTable::printTable() {
	for (int i{}; i < hashGroups; i++) {
		if (table[i].size() == 0) continue;
		
		auto Itr = table[i].begin();

		for (; Itr != table[i].end(); Itr++) {
			cout << "[INFO] key:" << Itr->first << " Value:" << Itr->second << endl;
		}
	}
	return;
}





//int main() {
//
//	HashTable HT;
//
//	if (HT.isEmpty()) {
//		cout << "Correct answer, good job." << endl;
//	}
//	else {
//		cout << " [ERROR] check the code";
//	}
//
//	HT.insertItem(3, "lisa simpson");
//	HT.insertItem(13, "john doe");
//	HT.insertItem(5, "marge simpson");
//	HT.insertItem(4, "Homer simpson");
//	HT.insertItem(143, "maggie simpson");
//	HT.insertItem(12, "Bart simpson");
//	HT.insertItem(12, "Rick sanches"); //test replace
//
//	HT.printTable();
//
//	HT.removeItem(143);
//	HT.removeItem(100);
//
//	if (HT.isEmpty()) {
//		cout << "check the code ERROR HT is empty";
//	}
//	else {
//		cout << "Correct. Good job" << endl;
//	}
//}