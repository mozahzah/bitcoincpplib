#include "op.h"

/*--------------------BITCOIN OP CODES--------------------*/

bool OP::OP_0(std::vector<std::string> stack){
    stack.push_back("0");
    return true;
}
bool OP::OP_1_NEG(std::vector<std::string> stack){
    stack.push_back("-1");
    return true;
}
bool OP::OP_1(std::vector<std::string> stack){
    stack.push_back("1");
    return true;
}
bool OP::OP_2(std::vector<std::string> stack){
    stack.push_back("2");
    return true;
}
bool OP::OP_3(std::vector<std::string> stack){
    stack.push_back("3");
    return true;
}
bool OP::OP_4(std::vector<std::string> stack){
    stack.push_back("4");
    return true;
}
bool OP::OP_5(std::vector<std::string> stack){
    stack.push_back("5");
    return true;
}
bool OP::OP_EQUAL (std::vector<std::string> stack){
    if (stack.size() < 2) return false;
    auto el1 = stack.back();
    stack.pop_back();
    auto el2 = stack.back();
    stack.pop_back();

    if (el1 == el2){stack.push_back("1");}
    else stack.push_back("0");
    return true;
}
bool OP::OP_HASH256(std::vector<std::string> stack)
{
    return true;
}
bool OP::OP_CHECKSIG(std::vector<std::string> stack, cpp_int z)
{
    if (stack.size() < 2) return false;
    auto sec_pubkey = stack.back();
    stack.pop_back();
    auto der_signature = stack.back();
    stack.pop_back();

    auto Point = ECC::Secp256Point::Parse(sec_pubkey);
    auto sig = ECC::Signature::Parse(der_signature);
    if (Point.Verify(z,sig)){stack.push_back("1");}
    else stack.push_back("0");
    return true;
}
