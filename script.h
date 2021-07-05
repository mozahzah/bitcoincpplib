#include <string>
#include "cryptlib.h"
//#include "op.h"



class Script
{  
    public: 
    Script();
    Script(std::vector<std::string> commands);

    std::string Serialize();
    std::vector<std::string> commands;

    bool Evaluate(std::string hash);

    Script P2BKH(std::string h160);

    Script operator+(Script& script);
};