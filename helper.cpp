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
        uint64_t i = n.ConvertToLong();
        if (byte_size == 1)
        {
            std::stringstream stream;
            stream << std::setfill ('0') << std::setw(byte_size*2) 
            << std::hex << i;
            return stream.str();
        }

        if (byte_size % 2 != 0 || byte_size > 32) throw std::invalid_argument("Byte size Error");
        
        uint64_t swapped;

        if (byte_size == 2)
        {
            swapped =  ((i<<8)&0x00FF) | ((i>>8)&0xFF00);
        }

        if (byte_size == 4)
        {
            swapped = ((i>>24)) | 
                    ((i<<8)&0x00FF0000) | 
                    ((i>>8)&0x0000FF00) | 
                    ((i<<24));
        }

        if (byte_size == 8)
        {
            swapped = ((((i) >> 56)&0x00000000000000FF) | (((i) >> 40)&0x000000000000FF00) | 
                        (((i) >> 24)&0x0000000000FF0000) | (((i) >>  8)&0x00000000FF000000) | 
                        (((i) <<  8)&0x000000FF00000000) | (((i) << 24)&0x0000FF0000000000) | 
                        (((i) << 40)&0x00FF000000000000) | (((i) << 56)&0xFF00000000000000));
        }

        std::stringstream stream;
        stream << std::setfill ('0') << std::setw(byte_size*2) 
        << std::hex << swapped;
        return stream.str();
    }

    std::string int_to_little_endian(std::string n, int byte_size)
    {
        uint64_t i = CryptoPP::Integer(n.c_str()).ConvertToLong();
        if (byte_size == 1)
        {
            std::stringstream stream;
            stream << std::setfill ('0') << std::setw(byte_size*2) 
            << std::hex << i;
            return stream.str();
        }

        if (byte_size % 2 != 0 || byte_size > 32) throw std::invalid_argument("Byte size Error");
        
        uint64_t swapped;

        if (byte_size == 2)
        {
            swapped =  ((i<<8)&0x00FF) | ((i>>8)&0xFF00);
        }

        if (byte_size == 4)
        {
            swapped = ((i>>24)) | 
                    ((i<<8)&0x00FF0000) | 
                    ((i>>8)&0x0000FF00) | 
                    ((i<<24));
        }

        if (byte_size == 8)
        {
            swapped = ((((i) >> 56)&0x00000000000000FF) | (((i) >> 40)&0x000000000000FF00) | 
                        (((i) >> 24)&0x0000000000FF0000) | (((i) >>  8)&0x00000000FF000000) | 
                        (((i) <<  8)&0x000000FF00000000) | (((i) << 24)&0x0000FF0000000000) | 
                        (((i) << 40)&0x00FF000000000000) | (((i) << 56)&0xFF00000000000000));
        }

        std::stringstream stream;
        stream << std::setfill ('0') << std::setw(byte_size*2) 
        << std::hex << swapped;
        return stream.str();
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