#include<iostream>
#include<cmath>
#include <string>

//#include "cryptlib.h"
//#include "gfpcrypt.h"
//#include "sha.h"
#include "boost/multiprecision/cpp_int.hpp"
#include "openssl/ecdsa.h"


#ifndef ECC_CPP
#define ECC_CPP

//using namespace CryptoPP;
using namespace boost::multiprecision;

cpp_int A = 0;
cpp_int B = 7;
cpp_int P = cpp_int("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
cpp_int N = cpp_int("0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
cpp_int GX = cpp_int("0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
cpp_int GY = cpp_int("0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");

namespace ECC
{
    class S256FieldElement
    {
        public:
            cpp_int num;
            cpp_int prime;
            // Constructors
            S256FieldElement(cpp_int num, cpp_int prime);
            S256FieldElement(cpp_int num) : S256FieldElement(num, P){}
            S256FieldElement() : S256FieldElement(0, P){}
            // Overloading cout << operator for representation
            friend std::ostream& operator<<(std::ostream& os, const S256FieldElement& f)
            {
                os << "S256FieldElement: " << f.num << " of Prime Field " << f.prime << std::endl; 
                return os;
            }
            // Overloading comparaison Operators
            bool operator== (const S256FieldElement& f) const;
            bool operator!= (const S256FieldElement& f) const;
            bool operator== (const cpp_int& n) const;
            bool operator!= (const cpp_int& n) const;

            // Overloading Arithmetics for Prime Fields
            S256FieldElement operator+(const S256FieldElement& f) const;
            S256FieldElement operator-(const S256FieldElement& f) const;
            S256FieldElement operator*(const S256FieldElement& f) const;
            S256FieldElement operator/(const S256FieldElement& f) const;
            S256FieldElement operator*(const cpp_int coeff) const;
            S256FieldElement pow(const cpp_int exponent) const;

            
    };

    class Signature
    {
        public:
            cpp_int r;
            cpp_int s;
            Signature(cpp_int r, cpp_int s);
            Signature() : Signature(0, 0){};
            static Signature Parse(std::string der_signature);
            std::string Der();
    };

    class S256Point
    {
        public:
            S256FieldElement x;
            S256FieldElement y;
            S256FieldElement a;
            S256FieldElement b;
            S256Point();
            S256Point(S256FieldElement x, S256FieldElement y, S256FieldElement a, S256FieldElement b);
            S256Point(cpp_int x, cpp_int y, cpp_int a, cpp_int b) : 
            S256Point(S256FieldElement(x), S256FieldElement(y), S256FieldElement(a), S256FieldElement(b)){}
            friend std::ostream& operator<<(std::ostream& os, const S256Point& p)
            {
                os << "S256Point (" << p.x.num << ", "<< p.y.num << ") on curve " << p.a.num << " and " << p.b.num << std::endl;
                return os;
            }
            bool operator== (const S256Point& p);
            bool operator!= (const S256Point& p);
            S256Point operator+ (const S256Point& p);
            S256Point operator* (const cpp_int coeff);

            static S256Point Parse(std::string sec_pubkey);
            bool Verify(cpp_int z, Signature sig);
            std::string Sec(bool compressed);
            std::string Address(bool compressed, bool testnet);
    };

    S256Point G = S256Point(GX,GY,A,B);

    class PrivateKey
    {
        public:
            cpp_int secret;
            S256Point point;
            S256Point publicPoint = S256Point(0,0,0,0);
            PrivateKey(cpp_int secret);
            Signature Sign(cpp_int z);  
    };
}

#endif