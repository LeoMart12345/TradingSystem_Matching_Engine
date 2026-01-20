#pragma once
#include "order.hpp"
#include <map>
#include <unordered_map>
#include <deque>
#include <queue>

class MatchingEngine {



private:



};



//Buy and sell side as maps
// std::unordered_map<int, std::deque<Order>> buyOrders; // maps an int price to a queue on the buy heap
// std::unordered_map<int, std::deque<Order>> sellOrders; // maps an int price to a queue on the sell heap
// Heaps for both buy and sell price levels