#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include "helper.h"
#include "op.h"

using namespace std;

class Script
{  
    public: 
        /* Data */
        vector<string> commands;

        /* Constructors */
        Script(vector<string> commands);
        Script() : Script({"0"}){}

        /* Parsing and serialization */
        string Serialize();
        Script static Parse(string& s);
        
        /* Return a Pay-to-Pubkey-Hash Template */
        Script P2PKH(string h160);

        /* TODO */
        //Script operator+(Script& script);
        //bool Evaluate(string hash);
};

#endif