//#include "hex.h"
//#include "files.h"
#include "sha.h"
#include "ripemd.h"
#include "boost/multiprecision/cpp_int.hpp"
#include "boost/endian/conversion.hpp"
#include <iomanip>
#include <sstream>
#include <string>

#ifndef HELPER_CPP
#define HELPER_CPP

using namespace boost::multiprecision;
std::string BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

namespace Helper
{   
    std::string Hash256(std::string s)
    {   
        cpp_int i("0x"+ s);
        char outputBuffer[65];
        unsigned char bytes[s.length()/2];
        export_bits(i, bytes, 8);
        std::string result;
        unsigned char hash[SHA256_DIGEST_LENGTH];
        unsigned char hash2[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_CTX sha256_2;
        SHA256_Init(&sha256_2);
        SHA256_Update(&sha256, bytes, sizeof(bytes));
        SHA256_Final(hash, &sha256);
        SHA256_Update(&sha256_2, hash, SHA256_DIGEST_LENGTH);
        SHA256_Final(hash2, &sha256_2);
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            sprintf(outputBuffer + (i * 2),"%02x", hash2[i]);
        }
        return outputBuffer;
    }

    std::string Hash160(std::string s)
    {   
        char outputBuffer[65];
        std::string result;
        unsigned char hash[SHA256_DIGEST_LENGTH];
        unsigned char hash2[RIPEMD160_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        RIPEMD160_CTX ripemd160;
        RIPEMD160_Init(&ripemd160);
        SHA256_Update(&sha256, s.c_str(), s.length());
        SHA256_Final(hash, &sha256);
        RIPEMD160_Update(&ripemd160, hash, SHA256_DIGEST_LENGTH);
        RIPEMD160_Final(hash2, &ripemd160);
        for (int i = 0; i < RIPEMD160_DIGEST_LENGTH; i++)
        {
            sprintf(outputBuffer + (i * 2),"%02x", hash2[i]);
        }
        return outputBuffer;
    }

    std::string Encode_Base_58(std::string s)
    {   
        int c = 0;
        for (int j = 0; j < s.size(); j++)
        {
            if (s[j] == '0')
            {
                c += 1;
            }
            else 
            {
                break;
            }
        }
        s = "0x" + s;
        cpp_int i(s);
        std::string prefix = std::string(c, '1');
        std::string result;
        while (i > 0)
        {
            cpp_int mod = i % 58;
            i = i / 58;
            result = BASE58_ALPHABET[mod.convert_to<int>()] + result;
        }
        return prefix + result;
    }

    std::string Encode_Base_58_Checksum(std::string s)
    {
        auto postsFix = Hash256(s).substr(0,8);
        return Encode_Base_58(s + postsFix);
    }

    std::string int_to_little_endian(cpp_int n, int byte_size)
    {
        std::stringstream stream;
        stream << std::hex << n;
        std::string s(stream.str());
        if (byte_size == 1) return s;
        int l = strlen(s.c_str());
        for (int i = 0,j = l; i < l/2; i += 2, j-=2) 
        {   
            auto temp = s[j-2];
            s[j-2] = s[i];
            s[i] = temp;

            auto temp2 = s[j-1];
            s[j-1] = s[i+1];
            s[i+1] = temp2;
        }
        return s;
    }

    std::string int_to_little_endian(std::string n, int byte_size)
    {
        cpp_int i ("0x" + n);
        std::stringstream stream;
        stream << std::hex << i;
        std::string s(stream.str());
        int l = strlen(s.c_str());
        for (int i = 0,j = l; i < l/2; i += 2, j-=2) 
        {   
            auto temp = s[j-2];
            s[j-2] = s[i];
            s[i] = temp;

            auto temp2 = s[j-1];
            s[j-1] = s[i+1];
            s[i+1] = temp2;
        }
        return s;
    }

    std::string int_to_big_endian(cpp_int n, int byte_size)
    {
        //if (byte_size % 2 != 0 || byte_size > 32) throw std::invalid_argument("Byte size Error");
        std::stringstream stream;
        stream << std::hex << n;
        std::string s(stream.str());
        return s;
    }

    std::string Decode_Base_58(std::string s)
    {
        cpp_int num = 0;
        for (char c : s)
        {
            num *= 58;
            num += BASE58_ALPHABET.find(c);
        }
        std::string result = int_to_big_endian(num, 25);
        std::string checksum = result.substr(result.length() - 8);
        if (Hash256(result.substr(0 , result.length() - 8)).substr(0, 8) != checksum)
        {
            throw std::invalid_argument("Bad Address");
        }
        return result.substr(2,result.length() - 10);
    }

    std::string encode_varint(cpp_int n)
    {
        std::stringstream stream;
        if ( n < 0xfd)
        {
            stream << int_to_little_endian(n,1);
            return stream.str();
        }

        else if (n < 0x10000)
        {
            stream << "fd" << int_to_little_endian(n,2);
            return stream.str();
        }

        else if (n < 0x100000000)
        {
            stream << "fe" << int_to_little_endian(n,4); 
            return stream.str();
        }

        else if (n < 0x1000000000000000)
        {
            stream << "ff" << int_to_little_endian(n,8); 
            return stream.str();
        }
        else 
        {
            throw std::invalid_argument("Number too big Error");
        } 
    }

    std::string Extract(std::string& s, int i)
    {
        std::string ss;
        for (int j = 0; j < i*2; j++)
        {   
            ss += s.at(j);
        }
        s.erase(0,i*2);
        return ss;
    }

    cpp_int little_endian_to_int(std::string s)
    {
        int l = strlen(s.c_str());
        for (int i = 0,j = l; i < l/2; i += 2, j-=2) 
        {   
            auto temp = s[j-2];
            s[j-2] = s[i];
            s[i] = temp;

            auto temp2 = s[j-1];
            s[j-1] = s[i+1];
            s[i+1] = temp2;
        }
        std::string sh = "0x" + s;
        return cpp_int(sh.c_str());
    }

    cpp_int read_varint(std::string s)
    {
        if (s == "fd")
        {
            return little_endian_to_int(Extract(s,2));
        }
        if (s == "fe")
        {
            return little_endian_to_int(Extract(s,4));
        }
        if (s == "ff")
        {
            return little_endian_to_int(Extract(s,8));
        }
        else 
        {
            std::string ss = s + 'h';
            cpp_int i = cpp_int(ss.c_str());
            return i;
        }
    }
}
    
#endif