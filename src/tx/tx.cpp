#include "tx.h"

/*--------------------TX--------------------*/

/* Constructor */
Tx::Tx(cpp_int version, vector<Txin> tx_ins, vector<Txout> tx_outs,cpp_int locktime, bool testnet)
{
    this->version = version;
    this->tx_ins = tx_ins;
    this->tx_outs = tx_outs;
    this->locktime = locktime;
    this->testnet = testnet;
}

string Tx::TxId()
{
    return Hash256(this->Serialize(), false);
}

/* Parsing and Serialization */
string Tx::Serialize()
{
    string result;
    result += Helper::int_to_little_endian(this->version, 4);
    result += Helper::encode_varint(tx_ins.size());
    for (Txin tx_in : tx_ins)
    {
        result += tx_in.Serialize();
    }
    result += Helper::encode_varint(tx_outs.size());
    for (Txout tx_out : tx_outs)
    {
        result += tx_out.Serialize();
    }
    result += Helper::int_to_little_endian(this->locktime, 4);
    return result;
}

Tx Tx::Parse(string s)
{
    string temp;
    int version = Helper::little_endian_to_int(Helper::Extract(s,4)).convert_to<int>();
    int num_inputs = Helper::read_varint(Helper::Extract(s,1)).convert_to<int>();

    vector<Txin> inputs;
    for (int i = 0; i < num_inputs; i++)
    {
        inputs.push_back(Txin::Parse(s));
    }
    int num_outputs = Helper::read_varint(Helper::Extract(s,1)).convert_to<int>();
    vector<Txout> outputs;
    for (int i = 0; i < num_outputs; i++)
    {
        outputs.push_back(Txout::Parse(s));
    }
    cpp_int locktime = Helper::little_endian_to_int(Helper::Extract(s,4));
    return Tx(version, inputs,outputs,locktime ,true);
}

/* Signing and Verification */
bool Tx::SignInput(uint64_t Input_Index, PrivateKey Private_Key, bool Compressed)
{
    cpp_int z = this->HashToSign(Input_Index);
    string der = Private_Key.Sign(z).Der();
    der += "01";
    string pub_key = Private_Key.publicPoint.Sec(Compressed);
    Script script_sig = Script({der, pub_key});
    this->tx_ins[Input_Index].script_sig = script_sig;
    return true;
}

cpp_int Tx::HashToSign(uint64_t Input_Index)
{
    Script p2bkh = Script();
    string result;
    result += Helper::int_to_little_endian(this->version, 4);
    result += Helper::encode_varint(this->tx_ins.size());
    for (int i = 0; i < this->tx_ins.size(); i++)
    {
        if (i == Input_Index)
        {
            auto current_tx_in = tx_ins.at(i);
            result += Txin(this->tx_ins[i].prev_tx, this->tx_ins[i].prev_index, 
            current_tx_in.GetPreviousScriptPubKey(), 
            this->tx_ins[i].sequence).Serialize();
        }
        else 
        {
            Txin(this->tx_ins[i].prev_tx, this->tx_ins[i].prev_index, Script(), this->tx_ins[i].sequence).Serialize();
        }
    }
    result += Helper::encode_varint(this->tx_outs.size());
    for (int i = 0; i < this->tx_outs.size(); i++)
    {
        result += tx_outs[i].Serialize();
    }
    result += Helper::int_to_little_endian(this->locktime,4);
    result += Helper::int_to_little_endian(1, 4);
    string h256 = "0x" + Helper::Hash256(result, false);
    return cpp_int(h256.c_str());
}

/* Tx Fetching */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

Tx Tx::TxFetcher(string prev_tx_hex)
{
    CURL* curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if(curl) 
    {
        string url = "https://blockstream.info/testnet/api/tx/%s/hex";
        url.replace(40, 2, prev_tx_hex);
    
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (readBuffer[9]== '0')
        {
            readBuffer.erase(9,4);
            auto tx = Tx::Parse(readBuffer);
            tx.locktime = Helper::little_endian_to_int(readBuffer.substr(readBuffer.length() - 8));
            return tx;
        }
        return Tx::Parse(readBuffer);
    }
    else 
    {
        throw invalid_argument("Invalid URL");
    }
}

