#include <iostream>
#include <array>
#include <vector>
#include <memory>

class Dog{

    public:
        void bark(){
            std::cout << "ehhh bark bark!" << std::endl;
        }

};

class Husky: public Dog{

};

int main(){
  
    Husky huzz;

    huzz.bark();

    return 0;
}