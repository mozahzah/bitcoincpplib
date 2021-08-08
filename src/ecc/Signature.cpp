#include "ecc.h"

/*--------------------Signature--------------------*/

/* Constructor */
Signature::Signature(cpp_int r, cpp_int s)
{
    this->r = r;
    this->s = s;
}

/* Parsing and serialization */
Signature Signature::Parse(string der_signature)
{
    return Signature(1,1); //TODO
}

string Signature::Der()
{   
    string result;
    string rbin = Helper::int_to_big_endian(this->r, 32);
    for (string::iterator i = rbin.begin(); i < rbin.end(); i+=2)
    {
        if (*i == '0' && *(i+1) == '0')
        {
            continue;
        }
        else
        {
            rbin.erase(remove(rbin.begin(), i, '0'), i);
            break;
        }
    }
    string firstbyte;
    firstbyte[0] = rbin.at(0);
    firstbyte[1] = rbin.at(1);

    if (cpp_int("0x" + firstbyte) & 0x80)
    {
        rbin = "00" + rbin;
    }
    result = Helper::int_to_little_endian(2, 1) + Helper::int_to_little_endian(rbin.size()/2, 1) + rbin;
    auto sbin = Helper::int_to_big_endian(this->s, 32);
    for (string::iterator i = sbin.begin(); i <= sbin.end(); i+=2)
    {
        if (*i == '0' && * (i+1) == '0')
        {
            continue;
        }
        else
        {
            sbin.erase(remove(sbin.begin(), i, '0'), i);
            break;
        }
    }
    firstbyte.clear();
    firstbyte[0] = rbin.at(0);
    firstbyte[1] = rbin.at(1);

    if (cpp_int("0x" + firstbyte) & 0x80)
    {
        sbin = "00" + sbin; 
    }
    result += Helper::int_to_little_endian(2, 1) + Helper::int_to_little_endian(sbin.size()/2, 1) + sbin;
    result = "30" + Helper::int_to_little_endian(result.size()/2, 1) + result;
    return result;
}