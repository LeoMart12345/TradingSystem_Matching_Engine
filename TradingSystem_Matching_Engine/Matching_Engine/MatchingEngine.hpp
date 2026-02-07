#include "order.hpp"
#include <map>
#include <unordered_map>
#include <deque>
#include <queue>
#include "OrderBook.hpp"
#include "Trade.hpp"

class MatchingEngine {

    private:
        OrderBook orderBook;

    public:
        explicit MatchingEngine(OrderBook& orderBook);
        
        std::optional<Trade> matchLimitOrders();
};