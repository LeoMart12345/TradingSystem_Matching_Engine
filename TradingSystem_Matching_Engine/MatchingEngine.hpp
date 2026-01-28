#include "order.hpp"
#include <map>
#include <unordered_map>
#include <deque>
#include <queue>
#include "OrderBook.hpp"

class MatchingEngine {

    private:


    public:
        explicit MatchingEngine(OrderBook orderBook);

        void matchLimitOrders(OrderBook orderBook);


};