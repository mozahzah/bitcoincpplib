#include "ecc.h"

cpp_int const A = 0;
cpp_int const B = 7;
cpp_int const P = cpp_int("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
cpp_int const N = cpp_int("0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
cpp_int const GX = cpp_int("0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
cpp_int const GY = cpp_int("0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");

/*--------------------FIELD ELEMENTS--------------------*/

/* Constructor */
Secp256FieldElement::Secp256FieldElement(cpp_int num, cpp_int prime)
{
    if (num >= prime || num < 0) throw invalid_argument("Num not in Field or Negative or NULL Initialized");
    this->num = num;
    this->prime = prime;
}

/* Overloading comparaison operators */
bool Secp256FieldElement::operator==(const Secp256FieldElement& f) const
{
    if (&f == nullptr)
    {
        return false;
    }
    return this->num == f.num && this->prime == f.prime;
} 
bool Secp256FieldElement::operator!=(const Secp256FieldElement& f) const
{
    return !(*this == f);
}
bool Secp256FieldElement::operator==(const cpp_int& n) const
{
    return this->num == n;
}
bool Secp256FieldElement::operator!=(const cpp_int& n) const
{
    return !(*this == n);
}

/* Overloading arithmetics operators*/
Secp256FieldElement Secp256FieldElement::operator+(const Secp256FieldElement& f) const
{
    if (this->prime != f.prime) throw invalid_argument("Cannot add two number in different fields");
    cpp_int num = (this->num + f.num) % this->prime;
    return Secp256FieldElement(num, this->prime);
}
Secp256FieldElement Secp256FieldElement::operator-(const Secp256FieldElement& f) const
{
    if (this->prime != f.prime) throw invalid_argument("Cannot subtract two number in different fields");
    cpp_int num = ((this->num - f.num) + this->prime) % this->prime; // c++ modulos needs to be positive at all time
    return Secp256FieldElement(num, this->prime);
}
Secp256FieldElement Secp256FieldElement::operator*(const Secp256FieldElement& f) const
{
    if (this->prime != f.prime) throw invalid_argument("Cannot multiply two number in different fields");
    cpp_int num = this->num * f.num % this->prime;
    return Secp256FieldElement(num, this->prime);
}
Secp256FieldElement Secp256FieldElement::pow(const cpp_int exponent) const
{ 
    cpp_int n = exponent % (this->prime - 1);
    cpp_int num = powm(this->num, n, this->prime);
    if (num < 0)
    {
        num = num + this->prime;
    }
    return Secp256FieldElement(num, this->prime);
}
Secp256FieldElement Secp256FieldElement::operator/(const Secp256FieldElement& f) const
{
    if (this->prime != f.prime) throw invalid_argument("Cannot divide two number in different fields");
    cpp_int i = powm(f.num, (this->prime - 2), this->prime);
    if (i < 0)
    {
        i = i + this->prime;
    }
    cpp_int num = (this->num * i) % this->prime;  
    return Secp256FieldElement(num, this->prime);
}
Secp256FieldElement Secp256FieldElement::operator*(const cpp_int coeff) const
{
    cpp_int num = (this->num * coeff) % this->prime;
    return Secp256FieldElement(num, this->prime);
}