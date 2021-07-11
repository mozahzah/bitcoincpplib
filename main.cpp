#include "ecc.cpp"
#include "tx.cpp"
//#include "helper.cpp"

using namespace std;

int main ()
{
    // Integer x = Integer("0x887387e452b8eacc4acfde10d9aaf7f6d9a0f975aabb10d006e4da568744d06c");
    // Integer y = Integer("0x61de6d95231cd89026e286df3b6ae4a894a3378e393e93a0f45b666329a0ae34");
    // S256Point point = S256Point(x,y,A,B);
    // Integer z = Integer("0xec208baa0fc1c19f708a9ca96fdeff3ac3f230bb4a7ba4aede4942ad003c0f60");
    // Integer r = Integer("0xac8d1c87e51d0d441be8b3dd5b05c8795b48875dffe00b7ffcfac23010d3a395");
    // Integer s = Integer("0x68342ceff8935ededd102dd876ffd6ba72d6a427a3edb13d26eb0781cb423c4");

    // ECC::PrivateKey priv = ECC::PrivateKey(56434636);
    // Signature sig = priv.Sign(z);
    // bool b = priv.publicPoint.Verify(z, sig);
    // cout << boolalpha;
    // cout << b << endl;

    // std::string passphrase = "mazeebitcoin@gmail.commzmzmzmzmzmzm";
    // std::cout << Helper::Hash256(passphrase);

    ECC::PrivateKey Priv = ECC::PrivateKey(16749689);
    std::string prev_tx = "2813e203b6720dea3caff9983a5a0ca5e0a24bb73ccb93a8a6974b7e9a5e2a36";
    int prev_index = 1;
    
    Txin tx_in = Txin(prev_tx, prev_index, Script(), Integer("0xffffffff"));
    std::vector<Txin> txins {tx_in};
    Script p2bkh = Script();
    Txout tx_out = Txout(5, p2bkh.P2BKH("ad346f8eb57dee9a37981716e498120ae80e44f7"));
    std::vector<Txout> txouts {tx_out};

    //Tx txObj = Tx(1, txins,txouts,0,true);

    Tx tx = Tx(1, txins, txouts, 0, true);
    //Priv.Sign(123).Der();
    tx.SignInput(0,Priv);

    //std::cout << Helper::TxFetcher("2813e203b6720dea3caff9983a5a0ca5e0a24bb73ccb93a8a6974b7e9a5e2a36");
   
    //auto new_tx = Tx::Parse(tx.Serialize());
    //std::cout << new_tx.Serialize() << std::endl;
    std::cout << tx.Serialize();
    return 0;
}
