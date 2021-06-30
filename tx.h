#include <vector>
#include <string>
#include "cryptlib.h"
#include "script.cpp"
#include "helper.cpp"

using namespace CryptoPP;

class Txin
{
    public:
        Txin(std::string prev_tx, int prev_index, Script script_sig, uint64_t sequence);

        std::string prev_tx;
        int prev_index;
        Script script_sig;
        uint64_t sequence;

        std::string Serialize();

};

class Txout
{
    public:
        Txout(int amount, Script script_pub_key);
        int amount;
        Script script_pub_key;

        std::string Serialize();
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
        Tx(Integer version, std::vector<Txin> tx_ins,std::vector<Txout> tx_outs,Integer locktime,bool testnet);
        Integer TxId();
        std::string TxHash();
        std::string Serialize();
        Tx Parse(std::string s, bool testnet);

};

