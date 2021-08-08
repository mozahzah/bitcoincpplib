#ifndef OP_H
#define OP_H

#include <vector>
#include <string>
#include "ecc.h"

enum OP_CODES
{
    OP_0 = 0,
    OP_1_NEG = 79,
    OP_1 = 81,
    OP_2 = 82,
    OP_3 = 83,
    OP_4 = 84,
    OP_5 = 85,
    OP_6 = 86,
    OP_7 = 87,
    OP_8 = 88,
    OP_9 = 89,
    OP_10 = 90,
    OP_11 = 91,
    OP_12 = 92,
    OP_13 = 93,
    OP_14 = 94,
    OP_15 = 95,
    OP_16 = 96,
    OP_NOP = 97,
    OP_IF = 99,
    OP_NOTIF = 100,
    OP_VERIFY = 105,
    OP_RETURN = 106,
    OP_2DUP = 110,
    OP_3DUP = 111,
    OP_SWAP = 114,
    OP_EQUAL = 135,
    OP_EQUALVERIFY = 136,
    OP_NOT = 145,
    OP_ADD = 147,
    OP_RIPEMD160 = 166,
    OP_SHA256 = 168,
    OP_HASH160 = 169,
    OP_HASH256 = 170,
    OP_CHECKSIG = 172,
    OP_CHECKSIGVERIFY = 173,
    OP_DUP = 118,	
};

class OP
{
    public:
        bool OP_0(std::vector<std::string> stack);
        bool OP_1_NEG(std::vector<std::string> stack);
        bool OP_1(std::vector<std::string> stack);
        bool OP_2(std::vector<std::string> stack);
        bool OP_3(std::vector<std::string> stack);
        bool OP_4(std::vector<std::string> stack);
        bool OP_5(std::vector<std::string> stack);
        bool OP_EQUAL (std::vector<std::string> stack);
        bool OP_HASH256(std::vector<std::string> stack);
        bool OP_CHECKSIG(std::vector<std::string> stack, cpp_int z);
    


    // Not used YET

    //bool OP_CHECKSIGVERIFY(std::vector<std::string> stack);
    // bool OP_6();
    // bool OP_7();
    // bool OP_8();
    // bool OP_9();
    // bool OP_10();
    // bool OP_11();
    // bool OP_12();
    // bool OP_13();
    // bool OP_14();
    // bool OP_15();
    // bool OP_16();
    // bool OP_NOP();
    // bool OP_IF();
    // bool OP_NOTIF ();
    // bool OP_VERIFY ();
    // bool OP_RETURN ();
    // bool OP_2DUP ();
    // bool OP_3DUP ();
    // bool OP_SWAP ();
    // bool OP_EQUALVERIFY ();
    // bool OP_NOT();
    // bool OP_ADD ();
    // bool OP_RIPEMD160();
    // bool OP_SHA256();
    // bool OP_HASH160();
};

#endif