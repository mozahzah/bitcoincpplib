#include "ecc.cpp"
#include "tx.cpp"
//#include "helper.cpp"

using namespace std;

int main ()
{
    std::string passphrase = "mazeebitcoin@gmail.commzmzmzmzmzmzm";
    auto secret = Helper::little_endian_to_int(Helper::Hash256(passphrase, true));
    ECC::PrivateKey Priv = ECC::PrivateKey(secret);
    std::string prev_tx = "ce4f1799f145a1f1c1de6ab1f63cf780f9a2ce77c39522406c18bd74fd9481b8";
    int prev_index = 0;
    
    Txin tx_in = Txin(prev_tx, prev_index, Script(), Integer("0xffffffff"));
    std::vector<Txin> txins {tx_in};
    Script p2bkh = Script();
    Txout tx_out = Txout(457000, p2bkh.P2BKH("fbb48feaea1944cd5498d012a6a72890f88604e5"));
    std::vector<Txout> txouts {tx_out};

    Tx tx = Tx(1, txins, txouts, 0, true);
    tx.SignInput(0,Priv);
    std::cout << tx.Serialize();
    Tx::Parse(tx.Serialize());
    return 0;
}
