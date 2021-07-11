#include "hex.h"
#include "files.h"
#include "sha.h"
#include "cryptlib.h"
#include <iomanip>
#include <sstream>
#include <string>
#include "curl.h"


#ifndef HELPER_CPP
#define HELPER_CPP

namespace Helper
{   
    std::string Hash256(std::string hex)
    {   
        hex = hex + "h";
        CryptoPP::Integer i = CryptoPP::Integer(hex.c_str()); 
        CryptoPP::byte b[i.MinEncodedSize()];
        i.Encode(b, i.MinEncodedSize());

        CryptoPP::SHA256 hasher;
        CryptoPP::byte hash1[CryptoPP::SHA256::DIGESTSIZE];
        hasher.CalculateDigest(hash1, b, i.MinEncodedSize());
    
        CryptoPP::byte hash2[CryptoPP::SHA256::DIGESTSIZE];
        hasher.CalculateDigest(hash2, hash1, 32);

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

    //std::string little_endian_to_big_endian(std::string )

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
            stream << "fd" << int_to_little_endian(i,2);
            return stream.str();
        }

        else if (i < 0x100000000)
        {
            std::stringstream stream;
            stream << "fe" << int_to_little_endian(i,4); 
            return stream.str();
        }

        else if (i < 0x1000000000000000)
        {
            std::stringstream stream;
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

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    std::string TxFetcher(std::string prev_tx_hex)
    {
        CURL* curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if(curl) 
        {
            std::string url = "https://blockstream.info/testnet/api/tx/%s/hex";
            url.replace(40, 2, prev_tx_hex);
            
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
           return readBuffer;
        }
        else 
        {
            throw std::invalid_argument("Invalid URL");
        }
    }
}
    
#endif