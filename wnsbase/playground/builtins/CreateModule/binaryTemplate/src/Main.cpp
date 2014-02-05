#include <WNS/SmartPtr.hpp>

#include <iostream>

class A
{
};

int main(int argc, char** argv)
{
    wns::SmartPtr<A> a();
    std::cout << a << std::endl;
}

