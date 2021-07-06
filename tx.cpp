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
    //result = "version";
    result += HashLib::int_to_little_endian(this->version, 4);
    //result += "varint";
    result += HashLib::encode_varint(tx_ins.size());
    //result += "txin";
    for (Txin tx_in : tx_ins)
    {
        result += tx_in.Serialize();
    }
    //result += "txout";
    result += HashLib::encode_varint(tx_outs.size());
    for (Txout tx_out : tx_outs)
    {
        result += tx_out.Serialize();
    }
    //result += "locktime";
    result += HashLib::int_to_little_endian(this->locktime, 4);
    return result;
}

Tx Tx::Parse(std::string s, bool testnet)
{
    std::vector<Txin> txins;
    std::vector<Txout> txouts;
    return Tx(0, txins,txouts,0,false);
}

Txin::Txin(std::string prev_tx, int prev_index, Script script_sig, Integer sequence)
{
    this->prev_tx = prev_tx;
    this->prev_index = prev_index;
    this->script_sig = script_sig;
    this->sequence = sequence;
}

std::string Txin::Serialize()
{
    int l = strlen(this->prev_tx.c_str());
    for (int i = 0,j = l; i < l/2; i += 2, j-=2) 
    {   
        auto temp = this->prev_tx[j-2];
        this->prev_tx[j-2] = this->prev_tx[i];
        this->prev_tx[i] = temp;

        auto temp2 = this->prev_tx[j-1];
        this->prev_tx[j-1] = this->prev_tx[i+1];
        this->prev_tx[i+1] = temp2;
    }
    std::string result;
    result += prev_tx;
    result += HashLib::int_to_little_endian(this->prev_index, 4);
    result += this->script_sig.Serialize();
    result += HashLib::int_to_little_endian(this->sequence, 4);    
    return result.c_str();
}

bool Tx::SignInput(uint64_t Input_Index, ECC::PrivateKey Private_Key, Script script_pubkey)
{
    Integer z = this->HashToSign(Input_Index, script_pubkey);
    auto der = Private_Key.Sign(z).Der();
    der += "01";
    auto pub_key = Private_Key.publicPoint.Sec();
    auto script_sig = Script({der, pub_key});
    this->tx_ins[Input_Index].script_sig = script_sig;
    return true;
}

Integer Tx::HashToSign(uint64_t Input_Index, Script script_pubkey)
{
    std::string result;
    result += HashLib::int_to_little_endian(this->version, 4);
    result += HashLib::encode_varint(this->tx_ins.size());
    for (int i = 0; i < this->tx_ins.size(); i++)
    {
        if (i == Input_Index)
        {
            result += Txin(this->tx_ins[i].prev_tx, this->tx_ins[i].prev_index, 
            script_pubkey, this->tx_ins[i].sequence).Serialize();
        }
        else 
        {
            Txin(this->tx_ins[i].prev_tx, this->tx_ins[i].prev_index, Script(), this->tx_ins[i].sequence).Serialize();
        }
    }
    result += HashLib::encode_varint(this->tx_outs.size());
    for (int i = 0; i < this->tx_outs.size(); i++){
        result += tx_outs[i].Serialize();
    }
    result += HashLib::int_to_little_endian(this->locktime,4);
    result += HashLib::int_to_little_endian(1, 4);
    std::string h256 = HashLib::Hash256(result) + 'h';
    return Integer(h256.c_str());
}

Txout::Txout(int amount, Script script_pub_key)
{
    this->amount = amount;
    this->script_pub_key = script_pub_key;
}

std::string Txout::Serialize()
{
    std::string result = HashLib::int_to_little_endian(this->amount, 8);
    result += this->script_pub_key.Serialize();
    return result.c_str();
}


