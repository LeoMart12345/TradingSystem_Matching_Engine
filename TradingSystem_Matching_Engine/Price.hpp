#pragma once
#include <iostream>

class Price{

    public:

    uint mPriceValueInCent;
    
    // Default constructor
    Price(): mPriceValueInCent(0) {};
    // parameterised constructor
    Price(long long value): mPriceValueInCent(value) {};
    
    // copy constructor
    Price(const Price& other) : mPriceValueInCent(other.mPriceValueInCent){};
    // copy assignment operator
    Price& operator=(const Price& other){
        if(this != &other){
            mPriceValueInCent = other.mPriceValueInCent;
        }
        return *this;
    }

    int getPriceInTicks() const {return mPriceValueInCent; } 

    //operator overloading
    Price operator<(const Price& rhs){ return mPriceValueInCent < rhs.mPriceValueInCent; };
    Price operator>(const Price& rhs){ return mPriceValueInCent > rhs.mPriceValueInCent; };
    Price operator==(const Price& rhs){ return mPriceValueInCent == rhs.mPriceValueInCent; };
    Price operator<=(const Price& rhs){ return mPriceValueInCent <= rhs.mPriceValueInCent; };
    Price operator>=(const Price& rhs){ return mPriceValueInCent >= rhs.mPriceValueInCent; }; 
    Price operator!=(const Price& rhs){ return mPriceValueInCent != rhs.mPriceValueInCent; };

    void Print(){ std::cout << "€" << mPriceValueInCent/100.00 << std::endl; }

};