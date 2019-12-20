#include "msgformatter.h"
#include "../table.h"
#include "../unicode.h"
#include "messages.h"


MsgFormatter::MsgFormatter()
{
}

void MsgFormatter::format(const std::wstring &fileName, Buffer &output)
{
    Table table(toMbcs(fileName));
    Messages msg;
    for (auto &field : table)
        msg.add(field.first, field.second->asString());
    msg.save(output);
}

