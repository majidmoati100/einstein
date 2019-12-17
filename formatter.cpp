#include "formatter.h"
#include "utils.h"
#include "convert.h"


Formatter::Formatter(unsigned char *data, int offset)
{
    int cnt = readInt(data + offset);
    if (! cnt) {
        commandsCnt = 0;
        commands = NULL;
        args = NULL;
    }

    argsCnt = 0;
    offset += 4;
    commands = new Command[cnt];
    commandsCnt = 0;
    
    for (int i = 0; i < cnt; i++) {
        int type = data[offset];
        offset++;
        int size = readInt(data + offset);
        offset += 4;
        switch (type) {
            case 1:
                commands[commandsCnt].type = TEXT_COMMAND;
                commands[commandsCnt].data.ptr = new std::wstring(
                        fromUtf8((char*)data + offset, size));
                commandsCnt++;
                break;
            
            case 2: add_arg<INT_ARG>(data, offset); break;
            case 3: add_arg<STRING_ARG>(data, offset); break;
            case 4: add_arg<FLOAT_ARG>(data, offset); break;
            case 5: add_arg<DOUBLE_ARG>(data, offset); break;
        }
        offset += size;
    }

    if (! argsCnt)
        args = NULL;
    else {
        args = new CmdType[argsCnt];
        memset(args, 0, sizeof(CmdType) * argsCnt);
        for (int i = 0; i < commandsCnt; i++) {
            Command &c = commands[i];
            if ((c.type == INT_ARG) || (c.type == STRING_ARG) ||
                    (c.type == FLOAT_ARG) || (c.type == DOUBLE_ARG))
            {
                int no = *(int*)c.data.raw;
                args[no - 1] = c.type;
            }
        }
    }
}

Formatter::~Formatter()
{
    for (int i = 0; i < commandsCnt; i++)
        if (TEXT_COMMAND == commands[i].type)
            delete (std::wstring*)(commands[i].data.ptr);
    if (commands)
        delete[] commands;
    if (args)
        delete[] args;
}

std::wstring Formatter::getMessage() const
{
    std::wstring s;

    for (int i = 0; i < commandsCnt; i++)
        if (TEXT_COMMAND == commands[i].type)
            s += *(std::wstring*)(commands[i].data.ptr);
    return s;
}


class ArgValue
{
    public:
        virtual ~ArgValue() { };
        virtual std::wstring format(Formatter::Command *command) = 0;
};

template <typename T>
class TemplatedArgValue: public ArgValue
{
    private:
        T value;
    
    public:
        TemplatedArgValue(const T &v) { value = v; };
        virtual std::wstring format(Formatter::Command *command) { 
            return toString(value);
        };
};

class StrArgValue: public ArgValue
{
    private:
        std::wstring value;

    public:
        StrArgValue(const std::wstring &v): value(v) { };
        virtual std::wstring format(Formatter::Command *command) { 
            return value;
        };
};


std::wstring Formatter::format(std::vector<ArgValue*> &argValues) const
{
    std::wstring s;
    int no;

    for (int i = 0; i < commandsCnt; i++) {
        Command *cmd = &commands[i];

        switch (cmd->type) {
            case TEXT_COMMAND:
                s += *(std::wstring*)(cmd->data.ptr);
                break;
                
            case STRING_ARG:
            case INT_ARG:
                no = *(int*)cmd->data.raw - 1;
                if (no < (int)argValues.size())
                    s += argValues[no]->format(cmd);
                break;

            default: ;
        }
    }
    
    return s;
}

std::wstring Formatter::format(va_list ap) const
{
    if (! argsCnt)
        return getMessage();
    
    std::vector<ArgValue*> argValues;
    
    for (int i = 0; i < argsCnt; i++) {
        switch (args[i]) {
            case INT_ARG:
                argValues.push_back(new TemplatedArgValue<int>
                        (va_arg(ap, int))); 
                break;
            case STRING_ARG:
                argValues.push_back(new StrArgValue(va_arg(ap, wchar_t*)));
                break;
            case DOUBLE_ARG:
                argValues.push_back(new TemplatedArgValue<double>
                        (va_arg(ap, double)));
                break;
            case FLOAT_ARG:
                argValues.push_back(new TemplatedArgValue<float>
                        ((float)va_arg(ap, double)));
                break;
            default:
                i = argsCnt;
        }
    }
 
    std::wstring s = format(argValues);

    for (std::vector<ArgValue*>::iterator i = argValues.begin();
            i != argValues.end(); i++)
        delete *i;
    
    return s;
}

template<Formatter::CmdType T>
void Formatter::add_arg(unsigned char *data, int offset)
{
    int argNo = readInt(data + offset);
    argsCnt = std::max(argsCnt, argNo);

    commands[commandsCnt].type = T;
    *(int*)commands[commandsCnt].data.raw = argNo;
    commandsCnt++;
}

