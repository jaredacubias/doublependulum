#include <iostream>
#include <string>
#include "state.h"

int main() 
{
    state mystate = state(1.0f, 1.0f, 1.0f, 1.0f);
    std::string input;
    std::cin >> input;
    while(input == "Y")
    {
        mystate = rk4(mystate, 1.0, 1.0, 1.0, 1.0, 1.0/144.0);
        std::cin >> input;
    }
    return 0;
}