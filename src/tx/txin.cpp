#include "tx.h"

/*--------------------Txin--------------------*/

/* Constructors */
Txin::Txin(string prev_tx, int prev_index, Script script_sig, cpp_int sequence)
{
    this->prev_tx = prev_tx;
    this->prev_index = prev_index;
    this->script_sig = script_sig;
    if (sequence == -1)
    {
        this->sequence = cpp_int("0xffffffff");
    }
    else
    {
        this->sequence = sequence;
    }
}

/* Parsing and serialization */
string Txin::Serialize()
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
    string result;
    result += prev_tx;
    result += Helper::int_to_little_endian(this->prev_index, 4);
    result += this->script_sig.Serialize();
    result += Helper::int_to_little_endian(this->sequence, 4);    
    return result.c_str();
}

Txin Txin::Parse(string& s)
{
    string prev_tx = Helper::Extract(s,32);
    int l = strlen(prev_tx.c_str());
    for (int i = 0,j = l; i < l/2; i += 2, j-=2) 
    {   
        auto temp = prev_tx[j-2];
        prev_tx[j-2] = prev_tx[i];
        prev_tx[i] = temp;

        auto temp2 = prev_tx[j-1];
        prev_tx[j-1] = prev_tx[i+1];
        prev_tx[i+1] = temp2;
    }
    int prev_index = Helper::little_endian_to_int(Helper::Extract(s,4)).convert_to<int>();
    Script script_sig = Script::Parse(s);
    int sequence = Helper::little_endian_to_int(Helper::Extract(s,4)).convert_to<int>();
    return Txin(prev_tx, prev_index, script_sig, sequence);
}

Script Txin::GetPreviousScriptPubKey()
{
    Tx prev = Tx::TxFetcher(this->prev_tx);
    return prev.tx_outs[this->prev_index].script_pub_key;
}