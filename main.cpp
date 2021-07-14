#include "ecc.cpp"
#include "tx.cpp"
//#include "helper.cpp"

using namespace std;

int main ()
{
    std::string passphrase = "mazeebitcoin@gmail.commzmzmzmzmzmzm";
    auto secret = Helper::little_endian_to_int(Helper::Hash256(passphrase, true));
    ECC::PrivateKey Priv = ECC::PrivateKey(secret);
    std::string prev_tx = "4b735347e842e315ba7b5f7c1b1afc69e599ec28d6e80ba181e03987259df1b2";
    int prev_index = 0;
    Txin tx_in = Txin(prev_tx, prev_index, Script(), Integer("0xffffffff"));
    std::vector<Txin> txins {tx_in};
    Script p2bkh = Script();
    Txout tx_out = Txout(454000, p2bkh.P2BKH(Helper::Decode_Base_58("n4TrCtCVPQaorW2WdHvJfH2bWh3RvL1StM")));
    std::vector<Txout> txouts {tx_out};
    Tx tx = Tx(1, txins, txouts, 0, true);
    tx.SignInput(prev_index, Priv, false);
    std::cout << tx.Serialize();
    //Tx::Parse(tx.Serialize());

    return 0;
}
