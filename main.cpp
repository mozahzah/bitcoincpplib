#include <iostream>
#include "ecc.cpp"
#include "helper.cpp"

int main ()
{

    //std::cout << Helper::Decode_Base_58("n4TrCtCVPQaorW2WdHvJfH2bWh3RvL1StM");
    //std::cout << Helper::Hash256("6ffbb48feaea1944cd5498d012a6a72890f88604e5");
    std::cout << Helper::little_endian_to_int("43fb");
    return 0;
}
