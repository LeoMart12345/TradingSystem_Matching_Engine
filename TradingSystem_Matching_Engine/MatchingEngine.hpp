#include "order.hpp"
#include <map>
#include <unordered_map>
#include <deque>
#include <queue>
#include "OrderBook.hpp"

class MatchingEngine {

    private:
        OrderBook orderBook;

    public:
        explicit MatchingEngine(OrderBook& orderBook);
        

        Trade matchLimitOrders();


};