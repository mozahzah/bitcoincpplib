#include "ecc.h"
#include "helper.cpp"
//#include <rsa.h>

using namespace ECC;
/*--------------------FIELD ELEMENTS--------------------*/

S256FieldElement::S256FieldElement(cpp_int num, cpp_int prime)
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
bool S256FieldElement::operator==(const cpp_int& n) const
{
    return this->num == n;
}
bool S256FieldElement::operator!=(const cpp_int& n) const
{
    return !(*this == n);
}

// Overloading Arithmetics for Prime Fields
S256FieldElement S256FieldElement::operator+(const S256FieldElement& f) const
{
    if (this->prime != f.prime) throw std::invalid_argument("Cannot add Two Number in Different Fields");
    cpp_int num = (this->num + f.num) % this->prime;
    return S256FieldElement(num, this->prime);
}
S256FieldElement S256FieldElement::operator-(const S256FieldElement& f) const
{
    if (this->prime != f.prime) throw std::invalid_argument("Cannot subtract Two Number in Different Fields");
    cpp_int num = ((this->num - f.num) + this->prime) % this->prime; // c++ modulos needs to be positive at all time
    return S256FieldElement(num, this->prime);
}
S256FieldElement S256FieldElement::operator*(const S256FieldElement& f) const
{
    if (this->prime != f.prime) throw std::invalid_argument("Cannot multiply Two Number in Different Fields");
    cpp_int num = this->num * f.num % this->prime;
    return S256FieldElement(num, this->prime);
}
S256FieldElement S256FieldElement::pow(const cpp_int exponent) const
{ 
    cpp_int n = exponent % (this->prime - 1);
   // ModularArithmetic ma(this->prime);
    cpp_int num = powm(this->num, n, this->prime);//ma.Exponentiate(this->num,n);
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
    cpp_int i = powm(f.num, (this->prime - 2), this->prime);
    if (i < 0)
    {
        i = i + this->prime;
    }
    cpp_int num = (this->num * i) % this->prime;  
    return S256FieldElement(num, this->prime);
}
S256FieldElement S256FieldElement::operator*(const cpp_int coeff) const
{
    cpp_int num = (this->num * coeff) % this->prime;
    return S256FieldElement(num, this->prime);
}

/*--------------------S256Point--------------------*/

S256Point::S256Point()
{

}

S256Point::S256Point(S256FieldElement x, S256FieldElement y, S256FieldElement a, S256FieldElement b)
{
    this->x = x;
    this->y = y;
    this->a = a;
    this->b = b;
    if (this->x == 0 || this->y == 0 || this->a == 0 || this->b == 0) return;
    ECC::S256FieldElement y2 = this->y.pow(2);
    ECC::S256FieldElement x3 = this->x.pow(3) + this->a * this->x + this->b;
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
S256Point S256Point::operator*(const cpp_int coeff){
    
    cpp_int c = coeff % N; // = coeff % cpp_int(N);
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

bool S256Point::Verify(cpp_int z, Signature sig)
{
    cpp_int s_inv = powm(sig.s, N-2, N);
    cpp_int u = z * s_inv % N;
    cpp_int v = sig.r * s_inv % N;
    S256Point total = G * u + *this * v;
    return total.x.num == sig.r;
}

std::string S256Point::Sec(bool compressed)
{
    std::string result;
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

std::string S256Point::Address(bool compressed, bool testnet)
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

Signature::Signature(cpp_int r, cpp_int s)
{
    this->r = r;
    this->s = s;
}

Signature Parse(std::string der_signature)
{
    return Signature(1,1); //TODO
}

Signature ECC::PrivateKey::Sign(cpp_int z)
{
    //DL_Algorithm_DSA_RFC6979<std::string, SHA256> d;
    
    std::stringstream stream;
    stream << std::hex << this->secret;
    cpp_int k = uint256_t("0x" + Helper::Hash256(stream.str(), false));//d.GenerateRandom(this->secret, N, z);
    std::cout << k << std::endl;
    cpp_int r = (G * k).x.num;
    cpp_int k_inv = powm(k, N-2, N);
    auto kk = r * this->secret;
    std::cout << kk << std::endl;
    cpp_int s = (z + r * this->secret) * k_inv % N;
    std::cout << s << std::endl;
    if (s > N/2){s = N - s;}
    return Signature(r, s);
}

ECC::PrivateKey::PrivateKey(cpp_int secret)
{
    this->secret = secret;
    this->publicPoint = G * secret;
}

std::string ECC::Signature::Der()
{   
    std::string result;
    std::string rbin = Helper::int_to_big_endian(this->r, 32);
    for (std::string::iterator i = rbin.begin(); i < rbin.end(); i+=2)
    {
        if (*i == '0' && *(i+1) == '0')
        {
            continue;
        }
        else
        {
            rbin.erase(remove(rbin.begin(), i, '0'), i);
            break;
        }
    }
    std::string firstbyte;
    firstbyte.push_back(rbin[0]);
    firstbyte.push_back(rbin[1]);
    if (cpp_int("0x" + firstbyte) & 0x80)
    {
        rbin = "00" + rbin;
    }
    result = Helper::int_to_little_endian(2, 1) + Helper::int_to_little_endian(rbin.size()/2, 1) + rbin;
    auto sbin = Helper::int_to_big_endian(this->s, 32);
    for (std::string::iterator i = sbin.begin(); i <= sbin.end(); i+=2)
    {
        if (*i == '0' && *(i+1) == '0')
        {
            continue;
        }
        else
        {
            sbin.erase(remove(sbin.begin(), i, '0'), i);
            break;
        }
    }
    firstbyte.clear();
    firstbyte.push_back(sbin[0]);
    firstbyte.push_back(sbin[1]);
    if (cpp_int("0x" + firstbyte) & 0x80)
    {
        sbin = "00" + sbin;   
    }
    result += Helper::int_to_little_endian(2, 1) + Helper::int_to_little_endian(sbin.size()/2, 1) + sbin;
    result = "30" + Helper::int_to_little_endian(result.size()/2, 1) + result;
    return result;
}

