#include "ecc.h"
#include "helper.cpp"
#include <rsa.h>

using namespace ECC;
/*--------------------FIELD ELEMENTS--------------------*/

S256FieldElement::S256FieldElement(Integer num, Integer prime)
{
    if (num >= prime || num < 0) throw std::invalid_argument("Num not in Field or Negative or NULL Initialized");
    this->num = num;
    this->prime = prime;
}

// Overloading comparaison Operators
bool S256FieldElement::operator==(const S256FieldElement& f) const
{
    if (&f == nullptr)
    {
        return false;
    }
    return this->num == f.num && this->prime == f.prime;
} 
bool S256FieldElement::operator!= (const S256FieldElement& f) const
{
    return !(*this == f);
}
bool S256FieldElement::operator==(const Integer& n) const
{
    return this->num == n;
}
bool S256FieldElement::operator!=(const Integer& n) const
{
    return !(*this == n);
}

// Overloading Arithmetics for Prime Fields
S256FieldElement S256FieldElement::operator+(const S256FieldElement& f) const
{
    if (this->prime != f.prime) throw std::invalid_argument("Cannot add Two Number in Different Fields");
    Integer num = (this->num + f.num) % this->prime;
    return S256FieldElement(num, this->prime);
}
S256FieldElement S256FieldElement::operator-(const S256FieldElement& f) const
{
    if (this->prime != f.prime) throw std::invalid_argument("Cannot subtract Two Number in Different Fields");
    Integer num = ((this->num - f.num) + this->prime) % this->prime; // c++ modulos needs to be positive at all time
    return S256FieldElement(num, this->prime);
}
S256FieldElement S256FieldElement::operator*(const S256FieldElement& f) const
{
    if (this->prime != f.prime) throw std::invalid_argument("Cannot multiply Two Number in Different Fields");
    Integer num = this->num * f.num % this->prime;
    return S256FieldElement(num, this->prime);
}
S256FieldElement S256FieldElement::pow(const Integer exponent) const
{ 
    Integer n = exponent % (this->prime - 1);
   // ModularArithmetic ma(this->prime);
    Integer num = a_exp_b_mod_c(this->num, n, this->prime);//ma.Exponentiate(this->num,n);
    if (num < 0)
    {
        num = num + this->prime;
    }
    return S256FieldElement(num, this->prime);
}
S256FieldElement S256FieldElement::operator/(const S256FieldElement& f) const
{
    if (this->prime != f.prime) throw std::invalid_argument("Cannot divide Two Number in Different Fields");
    //ModularArithmetic ma(this->prime);
    Integer i = a_exp_b_mod_c(f.num, (this->prime - 2), this->prime);
    if (i < 0)
    {
        i = i + this->prime;
    }
    Integer num = (this->num * i) % this->prime;  
    return S256FieldElement(num, this->prime);
}
S256FieldElement S256FieldElement::operator*(const Integer coeff) const
{
    Integer num = (this->num * coeff) % this->prime;
    return S256FieldElement(num, this->prime);
}

/*--------------------S256PointS--------------------*/

S256Point::S256Point(S256FieldElement x, S256FieldElement y, S256FieldElement a, S256FieldElement b)
{
    this->x = x;
    this->y = y;
    this->a = a;
    this->b = b;
    if (this->x == 0 || this->y == 0 || this->a == 0 || this->b == 0) return;
    S256FieldElement y2 = this->y.pow(2);
    S256FieldElement x3 = this->x.pow(3) + this->a * this->x + this->b;
    if (y2 != x3)
    {
        throw std::invalid_argument("S256Point is not on curve");
    }
}
bool S256Point::operator==(const S256Point& p)
{
    return this->x == p.x && this->y == p.y && this->a == p.a && this->b == p.b;
}
bool S256Point::operator!= (const S256Point& p)
{
    return !(*this == p);
}
S256Point S256Point::operator+(const S256Point& p)
{
    if (this->a != p.a || this->b != p.b) throw std::invalid_argument("The Two S256Points are not on the same curve");
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
        return S256Point(S256FieldElement(0), S256FieldElement(0), this->a, this->b);
    }
    if (this->x != p.x)
    {
        S256FieldElement s = (p.y - this->y) / (p.x - this->x);
        S256FieldElement x = s.pow(2) - this->x - p.x;
        S256FieldElement y = s * (this->x - x) - this->y;
        return S256Point(x,y, this->a, this->b);
    }
    if (*this == p && this->y.num == 0)
    {
        return S256Point(S256FieldElement(0), S256FieldElement(0), this->a, this->b);
    }
    if (*this == p)
    {
        S256FieldElement s = (this->x.pow(2) * 3 + this->a) / (this->y * 2);
        S256FieldElement x = s.pow(2) - (this->x * 2);
        S256FieldElement y = s * (this->x - x) - this->y;
        return S256Point(x,y,this->a,this->b);
    }
    else
    {
        throw std::invalid_argument("something went wrong");
    }
}
S256Point S256Point::operator*(const Integer coeff){
    
    Integer c = coeff % N; // = coeff % Integer(N);
    S256Point current = *this; 
    S256Point res = S256Point(S256FieldElement(0),S256FieldElement(0), this->a, this->b);
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

S256Point S256Point::Parse(std::string sec_pubkey)
{
    return S256Point(0,0,A,B);
}

bool S256Point::Verify(Integer z, Signature sig)
{
    //ModularArithmetic ma(N);
    Integer s_inv = a_exp_b_mod_c(sig.s, N-2, N);
    Integer u = z * s_inv % N;
    Integer v = sig.r * s_inv % N;
    S256Point total = G * u + *this * v;
    return total.x.num == sig.r;
}

Signature::Signature(Integer r, Integer s)
{
    this->r = r;
    this->s = s;
}

Signature Parse(std::string der_signature){
   
    return Signature(1,1);
}

Signature ECC::PrivateKey::Sign(Integer z)
{
    DL_Algorithm_DSA_RFC6979<std::string, SHA256> d;
    Integer k = d.GenerateRandom(this->secret, N, z);
    Integer r = (G * k).x.num;
    Integer k_inv = a_exp_b_mod_c(k, N-2, N);
    Integer s = (z + r * this->secret) * k_inv % N;
    if (s > N/2){s = N - s;}
    return Signature(r, s);
}

ECC::PrivateKey::PrivateKey(Integer secret)
{
    this->secret = secret;
    this->publicPoint = G * secret;
}