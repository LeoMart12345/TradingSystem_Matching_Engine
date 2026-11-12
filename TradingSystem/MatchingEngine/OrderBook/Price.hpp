// Price.hpp
#pragma once
#include <iostream>

class Price{

    public:

    u_int64_t mPriceValueInCent;
    
    // Constructors
    Price(): mPriceValueInCent(0) {};
    explicit Price(long long value): mPriceValueInCent(value) {};
    
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
    bool operator<(const Price& rhs){ return mPriceValueInCent < rhs.mPriceValueInCent; };
    bool operator>(const Price& rhs){ return mPriceValueInCent > rhs.mPriceValueInCent; };
    bool operator==(const Price& rhs){ return mPriceValueInCent == rhs.mPriceValueInCent; };
    bool operator<=(const Price& rhs){ return mPriceValueInCent <= rhs.mPriceValueInCent; };
    bool operator>=(const Price& rhs){ return mPriceValueInCent >= rhs.mPriceValueInCent; }; 
    bool operator!=(const Price& rhs){ return mPriceValueInCent != rhs.mPriceValueInCent; };
    
    void Print(){ std::cout << "€" << mPriceValueInCent/100.00 << std::endl; }

};