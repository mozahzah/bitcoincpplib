#include <vector>
#include "cryptlib.h"
#include "script.h"
#include "helper.cpp"

using namespace CryptoPP;

class Txin
{
    public:
        std::string prev_tx;
        int prev_index;
        Script script_sig;
        int sequence;
        std::string Serialize();

};

class Txout
{
    public:
        std::string Serialize();
        int amount;
        Script script_pub_key;
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

