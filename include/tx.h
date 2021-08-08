#ifndef TX_H
#define TX_H

#include <vector>
#include <string>
#include "ecc.h"
#include "script.h"
#include "helper.h"
#include "curl.h"

using namespace boost::multiprecision;
using namespace std;

class Txin
{
    public:
        /* Data */
        string prev_tx;
        int prev_index;
        Script script_sig;
        cpp_int sequence;

        /* Constructors */
        Txin(string prev_tx, int prev_index, Script script_sig, cpp_int sequence);
        Txin() : Txin("0", 0, Script(), 0){}

        /* Parsing and serialization */
        string Serialize();
        Txin static Parse(string& s);
        Script GetPreviousScriptPubKey();

};

class Txout
{
    public:
        /* Data */
        int amount;
        Script script_pub_key;

        /* Constructors */
        Txout(int amount, Script script_pub_key);
        Txout() : Txout(0, Script()){}

        /* Parsing and Serialization */
        string Serialize();
        Txout static Parse(string& s);
};

class Tx
{
    public:
        /* Data */
        cpp_int version;
        vector<Txin> tx_ins;
        vector<Txout> tx_outs;
        cpp_int locktime;
        bool testnet;

        /* Constructors */
        Tx(cpp_int version, vector<Txin> tx_ins,vector<Txout> tx_outs,cpp_int locktime, bool testnet);
        Tx() : Tx(0, {Txin()}, {Txout()}, 0, false){}
        string TxId();

        /* Parsing and Serialization */
        string Serialize();
        Tx static Parse(string s);

        /* Signing and Verification */
        cpp_int HashToSign(uint64_t Input_Index);
        bool SignInput(uint64_t Input_Index, ECC::PrivateKey Private_Key, bool Compressed);
        bool VerifyInput(uint64_t Input_Index);

        /* Tx Fetching */
        Tx static TxFetcher(string prev_tx_hex);
};

#endif
