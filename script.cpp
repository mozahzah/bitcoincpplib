#include "script.h"
#include "helper.cpp"

Script::Script()
{

}

Script::Script(std::vector<std::string> cmds)
{
    this->commands = cmds;
}

Script Script::P2BKH(std::string h160)
{
    //(int)0x76
    //(int)0xa9;
    //(int)0xa9;
    //(int)0x88;
    //(int)0xac;
    std::vector<std::string> cmds = {"118", "169", h160, "136", "172"};
    return Script(cmds);
}

std::string Script::Serialize()
{
    std::string result;
    for (auto cmd : this->commands)
    {
        if (cmd.size() < 4) 
        {   
            std::stoi(cmd);
            result += HashLib::int_to_little_endian(cmd, 1);
        }
        else 
        {   
            int length = cmd.size()/2;
            if (length < 75)
            {
                result += HashLib::int_to_little_endian(length, 1);
            }  
            else if (length > 75 and length < 0x100)
            {
                result += HashLib::int_to_little_endian(76, 1);
                result += HashLib::int_to_little_endian(length, 1);
            }
            else if (length >= 0x100 and length <= 520)
            {
                result += HashLib::int_to_little_endian(77, 1);
                result += HashLib::int_to_little_endian(length, 2);
            }
            else
            {
                throw std::invalid_argument("cmd too long");
            }
            result += cmd;
        } 
    }
    int64_t size = result.size()/2;
    std::string final_result = HashLib::encode_varint(size) + result;
    return final_result;
}