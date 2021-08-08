#ifndef ECC_H
#define ECC_H

#include <iostream>
#include <string>
#include "multiprecision/cpp_int.hpp"
#include "ecdsa.h"
#include "helper.h"

using namespace boost::multiprecision;
using namespace std;

/* Bitcoin secp256k1 curve constants */
extern cpp_int const A;
extern cpp_int const B;
extern cpp_int const P;
extern cpp_int const N; 
extern cpp_int const GX; 
extern cpp_int const GY;

namespace ECC
{
    /* Secp256k1 Field Elements */
    class Secp256FieldElement
    {
        public:
            /* Data */
            cpp_int num;
            cpp_int prime;

            /* Constructors */
            Secp256FieldElement(cpp_int num, cpp_int prime);
            Secp256FieldElement(cpp_int num) : Secp256FieldElement(num, P){}
            Secp256FieldElement() : Secp256FieldElement(0, P){}

            /* Overloading cout << operator for representation */
            friend ostream& operator<<(ostream& os, const Secp256FieldElement& f)
            {
                os << "Secp256FieldElement: " << f.num << " of Prime Field " << f.prime << endl; 
                return os;
            }

            /* Overloading comparaison operators */
            bool operator==(const Secp256FieldElement& f) const;
            bool operator!=(const Secp256FieldElement& f) const;
            bool operator==(const cpp_int& n) const;
            bool operator!=(const cpp_int& n) const;

            /* Overloading arithmetics operators*/
            Secp256FieldElement operator+(const Secp256FieldElement& f) const;
            Secp256FieldElement operator-(const Secp256FieldElement& f) const;
            Secp256FieldElement operator*(const Secp256FieldElement& f) const;
            Secp256FieldElement operator/(const Secp256FieldElement& f) const;
            Secp256FieldElement operator*(const cpp_int coeff) const;
            Secp256FieldElement pow(const cpp_int exponent) const;
    };

    /* ECDSA Signature */
    class Signature
    {
        public:
            /* Data */
            cpp_int r;
            cpp_int s;

            /* Constructors */
            Signature(cpp_int r, cpp_int s);
            Signature() : Signature(0, 0){};

            /* Parsing and serialization */
            static Signature Parse(string der_signature);
            string Der();
    };

    /* Secp256k1 Points */
    class Secp256Point
    {
        public:
            /* Data */
            Secp256FieldElement x;
            Secp256FieldElement y;
            Secp256FieldElement a;
            Secp256FieldElement b;

            /* Constructors */
            Secp256Point(Secp256FieldElement x, Secp256FieldElement y, Secp256FieldElement a, Secp256FieldElement b);
            Secp256Point(cpp_int x, cpp_int y, cpp_int a, cpp_int b) : 
            Secp256Point(Secp256FieldElement(x), Secp256FieldElement(y), Secp256FieldElement(a), Secp256FieldElement(b)){}
            Secp256Point() : Secp256Point(0, 0, 0, 0){}

            /* Overloading cout << operator for representation */
            friend ostream& operator<<(ostream& os, const Secp256Point& p)
            {
                os << "Secp256Point (" << p.x.num << ", "<< p.y.num << ") on curve " << p.a.num << " and " << p.b.num << endl;
                return os;
            }

            /* Overloading comparaison operators */
            bool operator==(const Secp256Point& p) const;
            bool operator!=(const Secp256Point& p) const;

            /* Overloading arithmetics */
            Secp256Point operator+(const Secp256Point& p) const;
            Secp256Point operator*(const cpp_int coeff) const;

            /* Serialization */
            string Sec(bool compressed);
            string Address(bool compressed, bool testnet);

            /* Parsing and verification */
            static Secp256Point Parse(string sec_pubkey);
            bool Verify(cpp_int z, Signature sig);
    };

    /* Initial elliptic curve point for bitcoin */         
    extern Secp256Point const G;

    /* Master Private Key */
    class PrivateKey
    {
        public:
            /* Data */
            cpp_int secret;
            Secp256Point publicPoint;

            /* Constructors */
            PrivateKey(cpp_int secret);
            PrivateKey() : PrivateKey(0){}

            /* Signing */
            Signature Sign(cpp_int z);  
    };
}

using namespace ECC;

#endif