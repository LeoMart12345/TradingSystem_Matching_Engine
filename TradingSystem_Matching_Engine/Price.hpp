#pragma once
#include <iostream>

class Price{

    public:

    long long mPriceValueInCent;
    
    //constructors
    Price(): mPriceValueInCent(0) {};
    Price(long long value): mPriceValueInCent(value) {};
    int getPriceInTicks() const {return mPriceValueInCent; } 
    // Destructor

    //operator overloading
    Price operator<(const Price& rhs){ return mPriceValueInCent < rhs.mPriceValueInCent; };
    Price operator>(const Price& rhs){ return mPriceValueInCent > rhs.mPriceValueInCent; };
    Price operator==(const Price& rhs){ return mPriceValueInCent == rhs.mPriceValueInCent; };
    Price operator<=(const Price& rhs){ return mPriceValueInCent <= rhs.mPriceValueInCent; };
    Price operator>=(const Price& rhs){ return mPriceValueInCent >= rhs.mPriceValueInCent; }; 
    Price operator!=(const Price& rhs){ return mPriceValueInCent != rhs.mPriceValueInCent; };

    void Print(){ std::cout << "€" << mPriceValueInCent/100.00 << std::endl; }

};