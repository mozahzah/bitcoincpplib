#include <vector>
#include <string>
#include "cryptlib.h"
#include "ecc.h"
#include "script.cpp"
#include "helper.cpp"

using namespace CryptoPP;

class Txin
{
    public:
        Txin();
        Txin(std::string prev_tx, int prev_index, Script script_sig, Integer sequence);

        std::string prev_tx;
        int prev_index;
        Script script_sig;
        Integer sequence;

        std::string Serialize();
        Script GetPreviousScriptPubKey();
        Txin static Parse(std::string& s);

};

class Txout
{
    public:
        Txout();
        Txout(int amount, Script script_pub_key);
        int amount;
        Script script_pub_key;

        std::string Serialize();
        Txout static Parse(std::string& s);
};

class Tx
{
    public:
        Integer version;
        std::vector<Txin> tx_ins;
        std::vector<Txout> tx_outs;
        Integer locktime;
        bool testnet;


    public:
        Tx(Integer version, std::vector<Txin> tx_ins,std::vector<Txout> tx_outs,Integer locktime, bool testnet);
        Integer TxId();

        Tx static Parse(std::string s);
        Integer HashToSign(uint64_t Input_Index);
        bool SignInput(uint64_t Input_Index, ECC::PrivateKey Private_Key);
        bool VerifyInput(uint64_t Input_Index);

        std::string TxHash();
        std::string Serialize();


        Tx Parse(std::string s, bool testnet);

};

