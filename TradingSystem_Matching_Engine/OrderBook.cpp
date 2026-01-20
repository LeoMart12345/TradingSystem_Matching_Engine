#include <iostream> 
#include <array> 
#include <vector>


class OrderBook{

private:
    std::array<int> PriceLevelBitmap;
public:
    OrderBook(): PriceLevelBitmap(100) {}

};

int main()
{
    std::cout << "hello" << std::endl;
    
}