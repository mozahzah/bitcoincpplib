#include "helper.h"

/*--------------------Helper--------------------*/

/* Hashing Algorithms */
string Helper::Hash256(string s, bool is_string)
{   
    char outputBuffer[65];
    string result;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    if (is_string)
    {
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, s.data(), s.length());
        SHA256_Final(hash, &sha256);
    }
    else
    {
        cpp_int i("0x"+ s);
        unsigned char bytes[s.length()/2];
        export_bits(i, bytes, 8);
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, bytes, sizeof(bytes));
        SHA256_Final(hash, &sha256);
    }
    
    unsigned char hash2[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256_2;
    SHA256_Init(&sha256_2);
    SHA256_Update(&sha256_2, hash, SHA256_DIGEST_LENGTH);
    SHA256_Final(hash2, &sha256_2);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2),"%02x", hash2[i]);
    }
    return outputBuffer;
}

string Helper::Hash160(string s)
{   
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned char hash2[RIPEMD160_DIGEST_LENGTH];
    cpp_int i("0x"+ s);
    unsigned char bytes[s.length()/2];
    export_bits(i, bytes, 8);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, bytes, sizeof(bytes));
    SHA256_Final(hash, &sha256);
    char outputBuffer[65];
    string result;
    RIPEMD160_CTX ripemd160;
    RIPEMD160_Init(&ripemd160);
    RIPEMD160_Update(&ripemd160, hash, SHA256_DIGEST_LENGTH);
    RIPEMD160_Final(hash2, &ripemd160);
    for (int i = 0; i < RIPEMD160_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2),"%02x", hash2[i]);
    }
    return outputBuffer;
}

/* Base 58 Encoding and Decoding */
static const string BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

string Helper::Encode_Base_58(string s)
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
    cpp_int i("0x" + s);
    string prefix = string(c, '1');
    string result;
    while (i > 0)
    {
        cpp_int mod = i % 58;
        i = i / 58;
        result = BASE58_ALPHABET[mod.convert_to<int>()] + result;
    }
    return prefix + result;
}

string Helper::Encode_Base_58_Checksum(string s)
{
    auto postsFix = Hash256(s, false).substr(0,8);
    return Encode_Base_58(s + postsFix);
}

string Helper::Decode_Base_58(string s)
{
    cpp_int num = 0;
    for (char c : s)
    {
        num *= 58;
        num += BASE58_ALPHABET.find(c);
    }
    string result = int_to_big_endian(num, 25);
    string checksum = result.substr(result.length() - 8);
    if (Hash256(result.substr(0 , result.length() - 8), false).substr(0, 8) != checksum)
    {
        throw invalid_argument("Bad Address");
    }
    return result.substr(2,result.length() - 10);
}

/* Integer encoding and decoding to Hex bytes */
string Helper::int_to_little_endian(cpp_int n, int byte_size)
{
    stringstream stream;
    stream << hex << setfill('0') << setw(byte_size*2) << n;
    string s(stream.str());
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

string Helper::int_to_little_endian(string n, int byte_size)
{
    cpp_int i (n);
    stringstream stream;
    stream << hex << setfill('0') << setw(byte_size*2) << n;
    string s(stream.str());
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

string Helper::int_to_big_endian(cpp_int n, int byte_size)
{
    //if (byte_size % 2 != 0 || byte_size > 32) throw invalid_argument("Byte size Error"); TODO
    stringstream stream;
    stream << hex << setfill('0') << setw(byte_size*2) << n;
    string s(stream.str());
    return s;
}

cpp_int Helper::little_endian_to_int(string s)
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
    string sh = "0x" + s;
    return cpp_int(sh.c_str());
}

/* Variable Integers */
string Helper::encode_varint(cpp_int n)
{
    stringstream stream;
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
        throw invalid_argument("Number too big Error");
    } 
}

cpp_int Helper::read_varint(string s)
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
        string ss = "0x" + s;
        cpp_int i = cpp_int(ss.c_str());
        return i;
    }
}

/* Reading from a string source */
string Helper::Extract(string& s, int bytes)
{
    string ss;
    for (int j = 0; j < bytes*2; j++)
    {   
        ss += s.at(j);
    }
    s.erase(0,bytes*2);
    return ss;
}





