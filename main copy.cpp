#include <iostream>
#include "ecc.h"
#include "helper.h"
#include "tx.h"

int main ()
{
    string passphrase = "mazeebitcoin@gmail.commzmzmzmzmzmzm";
    ECC::PrivateKey Priv = PrivateKey(123);//Helper::little_endian_to_int(Helper::Hash256(passphrase, true)));
    std::string prev_tx = "00dceb59a75a49c2db133c5b5fbbaa6b1d4a3222aec4ce3c07e3399e9d273d0e";
    int prev_index = 0;
    Txin tx_in = Txin(prev_tx, prev_index, Script(), cpp_int("0xffffffff"));
    std::vector<Txin> txins {tx_in};
    Script p2bkh = Script();
    auto amount = Tx::TxFetcher(prev_tx).tx_outs[prev_index].amount*0.995;
    Txout tx_out = Txout(amount, p2bkh.P2PKH(Helper::Decode_Base_58("n4TrCtCVPQaorW2WdHvJfH2bWh3RvL1StM")));
    std::vector<Txout> txouts {tx_out};
    Tx tx = Tx(1, txins, txouts, 0, true);
    tx.SignInput(prev_index, Priv, false);
    std::cout << tx.Serialize();
    return 0;
}
