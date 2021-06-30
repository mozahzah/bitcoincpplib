#include <string>
#include "cryptlib.h"
//#include "op.h"



class Script
{  
    public: 
    std::string Serialize();
    std::vector<std::string> commands;
};