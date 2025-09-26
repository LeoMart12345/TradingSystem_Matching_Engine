#pragma once
#include "order.hpp"
#include <map>
#include <unordered_map>
#include <deque>


class MatchingEngine {



private:
	// The queue represents the individual orders at that price level for FIFO. maps a price level -> queue of orders
	// using doubly ended queue to have 0(1) access to add a new order on a price level we need a deque also allowd for best prices to match being o(1) also
	std::unordered_map<float, std::deque<order>> buyOrders; // max heap
	std::unordered_map<float, std::deque<order>> sellOrders; // max heap
	
	
};

