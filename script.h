#include <string>

#include "op.h"



class Script
{  
    public: 
    Script();
    Script(std::vector<std::string> commands);

    std::string Serialize();
    Script static Parse(std::string& s);
    std::vector<std::string> commands;

    bool Evaluate(std::string hash);

    Script P2BKH(std::string h160);

    Script operator+(Script& script);
};