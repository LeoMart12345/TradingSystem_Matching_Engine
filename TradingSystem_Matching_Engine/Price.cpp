#include "Price.hpp"


int main(){
    Price myPrice = Price();

    std::cout << sizeof(myPrice) << " Bytes" << std::endl;

    myPrice.Print();
    
    return 0;
}