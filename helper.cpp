#include "hex.h"
#include "files.h"
#include "sha.h"
#include "ripemd.h"
#include "cryptlib.h"
#include <iomanip>
#include <sstream>
#include <string>

#ifndef HELPER_CPP
#define HELPER_CPP

std::string BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

namespace Helper
{   
    std::string Hash256(std::string s, bool isPassPhrase)
    {   
        CryptoPP::SHA256 hasher;
        CryptoPP::byte hash1[CryptoPP::SHA256::DIGESTSIZE];
        if (isPassPhrase)
        {
            CryptoPP::byte* b = (CryptoPP::byte*)s.data();
            hasher.CalculateDigest(hash1, b, s.size());
        }
        else
        {
            s = s + "h";
            CryptoPP::Integer i = CryptoPP::Integer(s.c_str()); 
            CryptoPP::byte b[i.MinEncodedSize()];
            i.Encode(b, i.MinEncodedSize());
            hasher.CalculateDigest(hash1, b, i.MinEncodedSize());
        }
        CryptoPP::byte hash2[CryptoPP::SHA256::DIGESTSIZE];
        hasher.CalculateDigest(hash2, hash1, 32);

        std::string hashString;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hashString), false);
        encoder.Put(hash2, sizeof(hash2));
        encoder.MessageEnd();
        return hashString;
    }

    std::string Hash160(std::string s)
    {   
        CryptoPP::SHA256 hasher;
        CryptoPP::RIPEMD160 hasher2;

        CryptoPP::byte hash1[CryptoPP::SHA256::DIGESTSIZE];
        s = s + "h";
        CryptoPP::Integer i = CryptoPP::Integer(s.c_str()); 
        CryptoPP::byte b[i.MinEncodedSize()];
        i.Encode(b, i.MinEncodedSize());
        hasher.CalculateDigest(hash1, b, i.MinEncodedSize());
        
        CryptoPP::byte hash2[CryptoPP::RIPEMD160::DIGESTSIZE];
        hasher2.CalculateDigest(hash2, hash1, 32);

        std::string hashString;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hashString),false);
        encoder.Put(hash2, sizeof(hash2));
        encoder.MessageEnd();
        return hashString;
    }

    std::string Encode_Base_58(std::string s)
    {   
        s = s + "h";
        CryptoPP::Integer i = CryptoPP::Integer(s.c_str()); 
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
        std::string prefix = std::string(c, '1');
        std::string result;
        while (i > 0)
        {
            Integer mod = i % 58;
            i = i / 58;
            result = BASE58_ALPHABET[mod.ConvertToLong()] + result;
        }
        return prefix + result;
    }

    std::string Encode_Base_58_Checksum(std::string s)
    {
        auto postsFix = Hash256(s, false).substr(0,8);
        return Encode_Base_58(s + postsFix);
    }

    std::string int_to_little_endian(CryptoPP::Integer n, int byte_size)
    {
        CryptoPP::byte b[byte_size];
        n.Encode(b, byte_size);
        std::string s;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(s), false);
        encoder.Put(b, sizeof(b));
        encoder.MessageEnd();
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
        //if (byte_size % 2 != 0 || byte_size > 32) throw std::invalid_argument("Byte size Error");
        CryptoPP::byte b[byte_size];
        n.Encode(b, byte_size);
        std::string s;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(s), false);
        encoder.Put(b, sizeof(b));
        encoder.MessageEnd();
        return s;
    }

    std::string Decode_Base_58(std::string s)
    {
        Integer num = 0;
        for (char c : s)
        {
            num *= 58;
            num += BASE58_ALPHABET.find(c);
        }
        std::string result = int_to_big_endian(num,25);
        std::string checksum = result.substr(result.length() - 8);
        if (Hash256(result.substr(0, result.length() - 8), false).substr(0, 8) != checksum)
        {
            throw std::invalid_argument("Bad Address");
        }
        return result.substr(2,result.length() - 10);
    }

    std::string encode_varint(CryptoPP::Integer n)
    {
        __uint128_t i = n.ConvertToLong();
        std::stringstream stream;
        if ( i < 0xfd)
        {
            stream << int_to_little_endian(i,1);
            return stream.str();
        }

        else if (i < 0x10000)
        {
            stream << "fd" << int_to_little_endian(i,2);
            return stream.str();
        }

        else if (i < 0x100000000)
        {
            stream << "fe" << int_to_little_endian(i,4); 
            return stream.str();
        }

        else if (i < 0x1000000000000000)
        {
            stream << "ff" << int_to_little_endian(i,8); 
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

    Integer little_endian_to_int(std::string s)
    {
        std::string sh = s + 'h';
        return Integer(sh.c_str(), LITTLE_ENDIAN_ORDER);
    }

    Integer read_varint(std::string s)
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
            Integer i = Integer(ss.c_str(), BIG_ENDIAN_ORDER);
            return i;
        }
    }
}
    
#endif