#include "tx.h"

Tx::Tx(Integer version, std::vector<Txin> tx_ins,std::vector<Txout> tx_outs,Integer locktime,bool testnet)
{
    this->version = version;
    this->tx_ins = tx_ins;
    this->tx_outs = tx_outs;
    this->locktime = locktime;
    this->testnet = testnet;
}

Integer Tx::TxId()
{
    //return this->TxHash();
    return 0;
}

std::string Tx::TxHash()
{
    //return HashLib::Hash256();
    return 0;
    
}

std::string Tx::Serialize()
{
    std::string result;
    result = HashLib::int_to_little_endian(this->version, 4).c_str();

    result += HashLib::encode_varint(tx_ins.size()).c_str();

    for (Txin tx_in : tx_ins)
    {
        result += tx_in.Serialize();
    }
    for (Txout tx_out : tx_outs)
    {
        result += tx_out.Serialize();
    }
    result += HashLib::int_to_little_endian(this->locktime, 4).c_str();
    return result;
}

Tx Tx::Parse(std::string s, bool testnet)
{
    std::vector<Txin> txins;
    std::vector<Txout> txouts;
    return Tx(0, txins,txouts,0,false);
}

std::string Txin::Serialize()
{
    std::string result = this->prev_tx;
    result += HashLib::int_to_little_endian(this->prev_index, 4);
    result += this->script_sig.Serialize();
    result += HashLib::int_to_little_endian(this->sequence, 4);    
    return result.c_str();
}

std::string Txout::Serialize()
{
    std::string result = HashLib::int_to_little_endian(this->amount, 8);
    result += this->script_pub_key.Serialize();
    return result.c_str();
}


