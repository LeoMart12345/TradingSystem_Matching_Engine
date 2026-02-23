#include <iostream>
#include <memory>
#include <cstring>
//template programming practise:
// makes the compiker generate the code so its not explicitly written


template<typename T> T
myMax(T x, T y){ return (x > y) ? x : y;} 

int main(){
    
    std::cout << "max of 3 and 6 is: " << myMax(3, 6) << std::endl;
    std::cout << "max of 3.4 and 4.1 is: " << myMax(3.4, 4.1) << std::endl;
    return 0;
}