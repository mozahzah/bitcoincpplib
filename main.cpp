#include <iostream>
#include "ecc.cpp"
#include "helper.cpp"
#include "tx.cpp"

int main ()
{
    std::string passphrase = "mazeebitcoin@gmail.commzmzmzmzmzmzm";
    //auto secret = Helper::little_endian_to_int(Helper::Hash256(passphrase));
    ECC::PrivateKey Priv = ECC::PrivateKey(Helper::little_endian_to_int("d35a6caa8c2113cf1339e73c064aae8fd031b581e28aff206d0ae53f60f525a0"));
    std::string prev_tx = "998d7040a7ac2de6a28eab670ef86d3916b11fbc8a795475bc72b029b28b2f80";
    int prev_index = 0;
    Txin tx_in = Txin(prev_tx, prev_index, Script(), cpp_int("0xffffffff"));
    std::vector<Txin> txins {tx_in};
    Script p2bkh = Script();
    auto amount = TxFetcher(prev_tx).tx_outs[prev_index].amount*0.95;
    Txout tx_out = Txout(amount, p2bkh.P2BKH(Helper::Decode_Base_58("n4TrCtCVPQaorW2WdHvJfH2bWh3RvL1StM")));
    std::vector<Txout> txouts {tx_out};
    Tx tx = Tx(1, txins, txouts, 0, true);
    tx.SignInput(prev_index, Priv, false);
    std::cout << tx.Serialize();
    //Tx::Parse(tx.Serialize());
    return 0;
}
