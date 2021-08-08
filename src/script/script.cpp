#include "script.h"

/*--------------------Script--------------------*/

/* Constructors */
Script::Script(vector<string> cmds)
{
    this->commands = cmds;
}

/* Parsing and serialization */
string Script::Serialize()
{
    string result;
    for (auto cmd : this->commands)
    {
        if (cmd.size() < 4) 
        {   
            int i = stoi(cmd);
            result += int_to_little_endian(i, 1);
        }
        else 
        {   
            int i = cmd.size();
            int length = cmd.size()/2;
            if (length < 75)
            {
                result += int_to_little_endian(length, 1);
            }  
            else if (length > 75 and length < 0x100)
            {
                result += int_to_little_endian(76, 1);
                result += int_to_little_endian(length, 1);
            }
            else if (length >= 0x100 and length <= 520)
            {
                result += int_to_little_endian(77, 1);
                result += int_to_little_endian(length, 2);
            }
            else
            {
                throw invalid_argument("cmd too long");
            }
            result += cmd;
        } 
    }
    int64_t size = result.size()/2;
    string final_result = encode_varint(size) + result;
    return final_result;
}

Script Script::Parse(string& s)
{
    int lenght = read_varint(Extract(s,1)).convert_to<int>();
    vector<string> commands;
    int i = 0;
    while( i < lenght)
    {
        string current = Extract(s,1);
        i += 1;
        string current_hex = "0x" + current;
        int current_byte = cpp_int(current_hex.c_str()).convert_to<int>();
        if (current_byte >= 1 && current_byte <= 75)
        {
            commands.push_back(Extract(s, current_byte));
            i += current_byte;
        }
        else if (current_byte == 76)
        {
            int data_lenght = little_endian_to_int(Extract(s,1)).convert_to<int>();
            commands.push_back(Extract(s, data_lenght));
            i += data_lenght + 1;
        }
        else if (current_byte == 77)
        {
            int data_lenght = little_endian_to_int(Extract(s,2)).convert_to<int>();
            commands.push_back(Extract(s, data_lenght));
            i += data_lenght + 2;
        }
        else
        {
            commands.push_back(to_string(current_byte));
        }
    }
    return Script(commands);
}

/* Return a Pay-to-Pubkey-Hash Template */
Script Script::P2PKH(string h160)
{
    vector<string> cmds = {"118", "169", h160, "136", "172"};
    return Script(cmds);
}