#include "ecc.h"

/*--------------------Points--------------------*/

/* Constructor */
Secp256Point::Secp256Point(Secp256FieldElement x, Secp256FieldElement y, Secp256FieldElement a, Secp256FieldElement b)
{
    this->x = x;
    this->y = y;
    this->a = a;
    this->b = b;
    if (this->x == 0 || this->y == 0 || this->a == 0 || this->b == 0) return;
    Secp256FieldElement y2 = this->y.pow(2);
    Secp256FieldElement x3 = this->x.pow(3) + this->a * this->x + this->b;
    if (y2 != x3)
    {
        throw invalid_argument("Secp256Point is not on curve");
    }
}

/* Overloading comparaison operators */
bool Secp256Point::operator==(const Secp256Point& p) const
{
    return this->x == p.x && this->y == p.y && this->a == p.a && this->b == p.b;
}
bool Secp256Point::operator!= (const Secp256Point& p) const
{
    return !(*this == p);
}

/* Overloading arithmetics */
Secp256Point Secp256Point::operator+(const Secp256Point& p) const
{
    if (this->a != p.a || this->b != p.b) throw invalid_argument("The Two Secp256Points are not on the same curve");
    if (this->x == 0)
    {
        return p;
    }
    if (p.x == 0)
    {
        return *this;
    }
    if (this->x == p.x && this->y != p.y)
    {
        return Secp256Point(Secp256FieldElement(0), Secp256FieldElement(0), this->a, this->b);
    }
    if (this->x != p.x)
    {
        Secp256FieldElement s = (p.y - this->y) / (p.x - this->x);
        Secp256FieldElement x = s.pow(2) - this->x - p.x;
        Secp256FieldElement y = s * (this->x - x) - this->y;
        return Secp256Point(x,y, this->a, this->b);
    }
    if (*this == p && this->y.num == 0)
    {
        return Secp256Point(Secp256FieldElement(0), Secp256FieldElement(0), this->a, this->b);
    }
    if (*this == p)
    {
        Secp256FieldElement s = (this->x.pow(2) * 3 + this->a) / (this->y * 2);
        Secp256FieldElement x = s.pow(2) - (this->x * 2);
        Secp256FieldElement y = s * (this->x - x) - this->y;
        return Secp256Point(x,y,this->a,this->b);
    }
    else
    {
        throw invalid_argument("something went wrong");
    }
}
Secp256Point Secp256Point::operator*(const cpp_int coeff) const
{
    
    cpp_int c = coeff % N; // = coeff % cpp_int(N);
    Secp256Point current = *this; 
    Secp256Point res = Secp256Point(Secp256FieldElement(0),Secp256FieldElement(0), this->a, this->b);
    while (c > 0)
    {
        if ((c & 1) == 1)
        {
            res = res + current;
        }
        current = current + current;
        c >>= 1;
    }
    return res;
}

/* Serialization */
string Secp256Point::Sec(bool compressed)
{
    string result;
    if (compressed)
    {
        if (this->y.num % 2 == 0)
        {
            result = "02" + Helper::int_to_big_endian(this->x.num, 32);
        }
        else
        {
            result = "03" + Helper::int_to_big_endian(this->x.num, 32);
        }
    }
    else
    {
        result = "04" + Helper::int_to_big_endian(this->x.num, 32);
        result += Helper::int_to_big_endian(this->y.num, 32);
    }
    return result;
}

string Secp256Point::Address(bool compressed, bool testnet)
{
    auto h160 = Helper::Hash160(this->Sec(compressed));
    if (testnet){
        h160 = "6f" + h160;
    }
    else{
        h160 = "00" + h160;
    }
    return Helper::Encode_Base_58_Checksum(h160);
}

/* Parsing and Verification */
Secp256Point Secp256Point::Parse(string sec_pubkey)
{
    return Secp256Point(0,0,A,B);
}

bool Secp256Point::Verify(cpp_int z, Signature sig)
{
    cpp_int s_inv = powm(sig.s, N-2, N);
    cpp_int u = z * s_inv % N;
    cpp_int v = sig.r * s_inv % N;
    Secp256Point total = G * u + *this * v;
    return total.x.num == sig.r;
}
