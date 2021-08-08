#ifndef HELPER_H
#define HELPER_H

#include "sha.h"
#include "ripemd.h"
#include "multiprecision/cpp_int.hpp"
#include "endian/conversion.hpp"
#include <iomanip>
#include <sstream>
#include <string>

using namespace boost::multiprecision;
using namespace std;

namespace Helper
{   
    /* Hashing Algorithms */
    string Hash256(string s, bool is_string);
    string Hash160(string s);

    /* Base 58 Encoding and Decoding */
    string Encode_Base_58(string s);
    string Encode_Base_58_Checksum(string s);
    string Decode_Base_58(string s);

    /* Integer encoding and decoding to Hex bytes */
    string int_to_little_endian(cpp_int n, int byte_size);
    string int_to_little_endian(string n, int byte_size);
    string int_to_big_endian(cpp_int n, int byte_size);
    cpp_int little_endian_to_int(string s);

    /* Variable Integers */
    string encode_varint(cpp_int n);
    cpp_int read_varint(string s);

    /* Reading from a string source */
    string Extract(string& s, int bytes);
}

using namespace Helper;
#endif