#include "script.h"
#include "helper.cpp"

std::string Script::Serialize()
{
        std::string result;
        for (auto cmd : this->commands)
        {
            if (typeid(cmd).name() == "int")
            {
                result = HashLib::int_to_little_endian(cmd, 1);
            }
            else
            {
                int length = sizeof(cmd);
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
    return result;
}