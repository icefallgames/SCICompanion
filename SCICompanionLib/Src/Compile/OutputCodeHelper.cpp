#include "stdafx.h"
#include "ScriptOM.h"
#include "OutputCodeHelper.h"

void _OutputVariableAndSize(sci::SourceCodeWriter &out, const std::string &type, const std::string &name, WORD wSize, std::vector<WORD> initValues)
{
    out.out << (type.empty() ? "var" : type) << " " << name;
    if (wSize > 1)
    {
        out.out << "[" << wSize << "]"; // array
    }

    if (!initValues.empty())
    {
        if (wSize > 1)
        {
            out.out << " = (";
            for (WORD &initValue : initValues)
            {
                out.out << initValue << " ";
            }
            out.out << ")";
        }
        else
        {
            out.out << " = " << initValues[0];
        }
    }
}

std::string UnescapeString(std::string src)
{
    size_t thePos;
    while ((thePos = src.find('\n')) != std::string::npos)
    {
        src.replace(thePos, 1, "\\n");
    }
    while ((thePos = src.find('\t')) != std::string::npos)
    {
        src.replace(thePos, 1, "\\t");
    }

    // Escape double-quote characters.
    size_t i = 0;
    while ((thePos = src.find('"', i)) != std::string::npos)
    {

        src.replace(thePos, 1, "\\\"");
        i = thePos + 2;
    }
    return src;
}

bool IsNonAlphaOperator(const std::string &op)
{
    return (op == "--") || (op == "++") || (op == "!");
}


void Indent(sci::SourceCodeWriter &out)
{
    std::string spaces;
    spaces.append(out.iIndent, ' ');
    out.out << spaces;
}

void NewLine(sci::SourceCodeWriter &out)
{
    if (!out.fInline)
    {
        out.out << out.NewLineString();
        Indent(out);
    }
}


void _OutputNumber(sci::SourceCodeWriter &out, WORD wNum, bool fHex, bool fNegate)
{
    if (fNegate)
    {
        assert(!fHex);
        wNum = (~wNum) + 1; // two's complement
        out.out << "-";
    }
    out.out << (fHex ? std::hex : std::dec);
    if (fHex)
    {
        out.out << "$" << std::setw(4) << std::setfill('0') << wNum;
    }
    else
    {
        out.out << wNum;
    }
}

std::string _DeduceReturnType(sci::FunctionBase &function)
{
    CheckForReturnValue en;
    function.Traverse(nullptr, en);
    return en.GetSugggestReturnValue();
}

void EndStatement(sci::SourceCodeWriter &out)
{
    if (out.fInline)
    {
    }
    else
    {
        out.out << ";";
    }
}

// Wrap the string in braces if it doesn't start with A-Za-z_ and stuff...
std::string CleanToken(const std::string &src)
{
    bool ok = true;
    for (size_t i = 0; ok && (i < src.length()); i++)
    {
        if ((i == 0) && !isalpha(src[i]) && (src[i] != '_'))
        {
            ok = false;
        }
        else if ((i > 0) && !isalnum(src[i]) && (src[i] != '_'))
        {
            ok = false;
        }
    }
    if (!ok)
    {
        std::string braceString = "{";
        braceString += src;
        braceString += "}";
        return braceString;
    }
    else
    {
        return src;
    }
}