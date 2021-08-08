#include "tx.h"

/*--------------------Txout--------------------*/

/* Constructor */
Txout::Txout(int amount, Script script_pub_key)
{
    this->amount = amount;
    this->script_pub_key = script_pub_key;
}

/* Parsing and Serialization */
string Txout::Serialize()
{
    string result = Helper::int_to_little_endian(this->amount, 8);
    result += this->script_pub_key.Serialize();
    return result.c_str();
}

Txout Txout::Parse(string& s)
{
    int amount = Helper::little_endian_to_int(Helper::Extract(s,8)).convert_to<int>();
    Script script_pubkey = Script::Parse(s);
    return Txout(amount, script_pubkey);
}