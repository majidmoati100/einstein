#include "format.h"
#include "msgformatter.h"


FormatRegistry formatRegistry;


FormatRegistry::FormatRegistry()
{
    formatters[L"messages"] = new MsgFormatter();
}


FormatRegistry::~FormatRegistry()
{
    for (auto f : formatters)
        delete f.second;
}


Formatter* FormatRegistry::get(const std::wstring &name)
{
    return formatters[name];
}

