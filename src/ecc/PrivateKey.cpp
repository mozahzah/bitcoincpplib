#include "ecc.h"

Secp256Point const ECC::G = Secp256Point(GX,GY,A,B);

/*--------------------Private Key--------------------*/

/* Constructor */
PrivateKey::PrivateKey(cpp_int secret)
{
    this->secret = secret;
    this->publicPoint = G * secret;
}

/* Signing */
Signature PrivateKey::Sign(cpp_int z)
{
    stringstream stream;
    stream << hex << this->secret << z;
    cpp_int k = uint256_t("0x" + Helper::Hash256(stream.str(), false));
    cpp_int r = (G * k).x.num;
    cpp_int k_inv = powm(k, N-2, N);
    auto kk = r * this->secret;
    cpp_int s = (z + r * this->secret) * k_inv % N;
    if (s > N/2){s = N - s;}
    return Signature(r, s);
}

