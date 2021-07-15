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
            result += Helper::int_to_little_endian(cmd, 1);
        }
        else 
        {   
            int i = cmd.size();
            int length = cmd.size()/2;
            if (length < 75)
            {
                result += Helper::int_to_little_endian(length, 1);
            }  
            else if (length > 75 and length < 0x100)
            {
                result += Helper::int_to_little_endian(76, 1);
                result += Helper::int_to_little_endian(length, 1);
            }
            else if (length >= 0x100 and length <= 520)
            {
                result += Helper::int_to_little_endian(77, 1);
                result += Helper::int_to_little_endian(length, 2);
            }
            else
            {
                throw std::invalid_argument("cmd too long");
            }
            result += cmd;
        } 
    }
    int64_t size = result.size()/2;
    std::string final_result = Helper::encode_varint(size) + result;
    return final_result;
}

Script Script::Parse(std::string& s)
{
    int lenght = Helper::read_varint(Helper::Extract(s,1)).convert_to<int>();
    std::vector<std::string> commands;
    int i = 0;
    while( i < lenght)
    {
        std::string current = Helper::Extract(s,1);
        i += 1;
        std::string current_hex = current + 'h';
        int current_byte = cpp_int(current_hex.c_str()).convert_to<int>();
        if (current_byte >= 1 && current_byte <= 75)
        {
            commands.push_back(Helper::Extract(s, current_byte));
            i += current_byte;
        }
        else if (current_byte == 76)
        {
            int data_lenght = Helper::little_endian_to_int(Helper::Extract(s,1)).convert_to<int>();
            commands.push_back(Helper::Extract(s, data_lenght));
            i += data_lenght + 1;
        }
        else if (current_byte == 77)
        {
            int data_lenght = Helper::little_endian_to_int(Helper::Extract(s,2)).convert_to<int>();
            commands.push_back(Helper::Extract(s, data_lenght));
            i += data_lenght + 2;
        }
        else
        {
            commands.push_back(std::to_string(current_byte));
        }
    }
    return Script(commands);
}