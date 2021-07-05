#include "hex.h"
#include "files.h"
#include "sha.h"
#include "cryptlib.h"
#include <iomanip>
#include <sstream>
#include <string>


#ifndef HELPER_CPP
#define HELPER_CPP



namespace HashLib
{
    std::string Hash256(std::string s)
    {
        CryptoPP::SHA256 hasher;
        CryptoPP::byte hash1[CryptoPP::SHA256::DIGESTSIZE];
        hasher.CalculateDigest(hash1, (CryptoPP::byte*)s.data(), s.size());
    
        CryptoPP::byte hash2[CryptoPP::SHA256::DIGESTSIZE];
        hasher.CalculateDigest(hash2, hash1, sizeof(hash1));

        std::string hashString;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hashString));
        encoder.Put(hash2, sizeof(hash2));
        encoder.MessageEnd();

        return hashString;
    }

    std::string int_to_little_endian(CryptoPP::Integer n, int byte_size)
    {
        CryptoPP::byte b[byte_size];
        n.Encode(b, byte_size);
        std::string s;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(s), false);
        encoder.Put(b, sizeof(b));
        encoder.MessageEnd();
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
        CryptoPP::Integer i = CryptoPP::Integer(n.c_str());
        CryptoPP::byte b[byte_size];
        i.Encode(b, byte_size);
        std::string s;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(s), false);
        encoder.Put(b, sizeof(b));
        encoder.MessageEnd();
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

    std::string int_to_big_endian(CryptoPP::Integer n, int byte_size)
    {
        if (byte_size % 2 != 0 || byte_size > 32) throw std::invalid_argument("Byte size Error");
        CryptoPP::byte b[byte_size];
        n.Encode(b, byte_size);
        std::string s;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(s), false);
        encoder.Put(b, sizeof(b));
        encoder.MessageEnd();
        return s;
    }

    std::string encode_varint(CryptoPP::Integer n)
    {
        __uint128_t i = n.ConvertToLong();

        if ( i < 0xfd)
        {
            std::stringstream stream;
            stream << int_to_little_endian(i,1);
            return stream.str();
        }

        else if (i < 0x10000)
        {
            std::stringstream stream;
            stream << "0xfd" << int_to_little_endian(i,2);
            return stream.str();
        }

        else if (i < 0x100000000)
        {
            std::stringstream stream;
            stream << "0xfe" << int_to_little_endian(i,4); 
            return stream.str();
        }

        else if (i < 0x1000000000000000)
        {
            std::stringstream stream;
            stream << "0xff" << int_to_little_endian(i,8); 
            return stream.str();
        }
        else 
        {
            throw std::invalid_argument("Number too big Error");
        }
        
    }
}
    
#endif