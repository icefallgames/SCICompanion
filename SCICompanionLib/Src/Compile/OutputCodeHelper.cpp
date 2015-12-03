/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#include "stdafx.h"
#include "ScriptOM.h"
#include "OutputCodeHelper.h"

using namespace sci;
using namespace std;

void PrepForLanguage(LangSyntax langSyntax, sci::Script &script, GlobalCompiledScriptLookups *lookups)
{
    if (langSyntax == LangSyntaxSCI)
    {
        ConvertToSCISyntaxHelper(script, lookups);
    }
}

void _OutputVariableAndSizeStudio(sci::ISyntaxNodeVisitor &visitor, sci::SourceCodeWriter &out, const std::string &type, const std::string &name, WORD wSize, const sci::SyntaxNodeVector &initValues)
{
    out.out << name;
    if (wSize > 1)
    {
        out.out << "[" << wSize << "]"; // array
    }

    if (!initValues.empty())
    {
        if (wSize > 1)
        {
            out.out << " = (";
            bool first = true;
            for (auto &initValue : initValues)
            {
                if (!first)
                {
                    out.out << " ";
                }
                Inline inln(out, true);
                initValue->Accept(visitor);
                first = false;
            }
            out.out << ")";
        }
        else
        {
            out.out << " = ";
            initValues[0]->Accept(visitor);

        }
    }
}

template<char Q1, char Q2>
std::string EscapeString(const std::string &src)
{
    std::string result;
    for (char ch : src)
    {
        char escaped = 0;
        switch (ch)
        {
            case '\n':
                escaped = 'n';
                break;
            case '\t':
                escaped = 't';
                break;
            case '\\':
                escaped = '\\';
                break;
            case Q1:
                escaped = Q1;
                break;
            case Q2:
                escaped = Q2;
                break;
        }
        if (escaped)
        {
            result.push_back('\\');
            result.push_back(escaped);
        }
        else
        {
            result.push_back(ch);
        }
    }
    return result;
}

// Turn spaces to underscores and escape underscores.
std::string EscapeSpaces(const std::string &src)
{
    // If there is more than one space in a row, replace them with underscores.
    std::string result;
    int spaceCount = 0;
    for (char ch : src)
    {
        if (ch == ' ')
        {
            spaceCount++; // Take note, but don't output anything.
        }
        else
        {
            if (spaceCount > 1)
            {
                std::fill_n(std::back_inserter(result), spaceCount, '_');
            }
            else if (spaceCount == 1)
            {
                result.push_back(' '); // Just a single space
            }
            spaceCount = 0;
            if (ch == '_')
            {
                // Escape the underscore
                result.push_back('\\');
                result.push_back('_');
            }
            else
            {
                result.push_back(ch);
            }
        }
    }
    if (spaceCount > 0)
    {
        std::fill_n(std::back_inserter(result), spaceCount, '_');
    }
    return result;
}

std::string EscapeQuotedString(const std::string &src)
{
    return EscapeString<'"', '\0'>(src);
}
std::string EscapeBraceString(const std::string &src)
{
    return EscapeString<'{', '}'>(src);
}

bool IsNonAlphaOperator(const std::string &op)
{
    return (op == "--") || (op == "++") || (op == "!");
}


void Indent(sci::SourceCodeWriter &out)
{
    std::string spaces;
    spaces.append(out.iIndent, out.indentChar);
    out.out << spaces;
}

void NewLine(sci::SourceCodeWriter &out)
{
    if (!out.fInline)
    {
        out.NewLine();
        Indent(out);
    }
}

std::string _DeduceReturnType(sci::FunctionBase &function)
{
    CheckForReturnValue en;
    function.Traverse(en);
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

// Scripts compiled with v1 of the SCI Studio syntax don't support explicit export lists.
// When outputting source code, we need to list these.
// We should follow the same order as GetExportTableOrder
void EnsurePublicsInExports(sci::Script &script)
{
    if (script.GetExports().empty())
    {
        int exportSlot = 0;
        // public instances, followed by procedures
        for (auto &classDef : script.GetClassesNC())
        {
            if (classDef->IsPublic())
            {
                script.GetExports().push_back(std::make_unique<ExportEntry>(exportSlot, classDef->GetName()));
                exportSlot++;
            }
        }
        for (auto &proc : script.GetProcedures())
        {
            if (proc->IsPublic())
            {
                script.GetExports().push_back(std::make_unique<ExportEntry>(exportSlot, proc->GetName()));
                exportSlot++;
            }
        }
    }
}

// Wrap the string in braces if it doesn't start with A-Za-z_ and stuff...
std::string CleanToken(const std::string &src, std::unordered_set<std::string> *disallowedList)
{
    bool ok = true;
    // Special hack: sometimes said strings come through here
    bool isSaid = (!src.empty() && src[0] == '\'' && src.back() == '\'');
    if (!isSaid)
    {
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
    }

    if (ok && disallowedList)
    {
        // Ok if not present in disallowed list.
        ok = (disallowedList->find(src) == disallowedList->end());
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

DetectIfWentNonInline::DetectIfWentNonInline(sci::SourceCodeWriter &out) : _out(out), WentInline(false) { out.detectNonInLine.push_back(this); }
DetectIfWentNonInline::~DetectIfWentNonInline() { _out.detectNonInLine.pop_back(); }

Inline::Inline(sci::SourceCodeWriter &out, bool fInline) : _out(out)
{
    _fOld = _out.fInline;
    _out.fInline = fInline;
    if (!fInline)
    {
        for (auto &detect : out.detectNonInLine)
        {
            detect->WentInline = true;
        }
    }
}
Inline::~Inline()
{
    _out.fInline = _fOld;
}

// Ensure there are no variable names that are keywords.
class CleanVariableNamesWorker : public IExploreNode
{
public:
    CleanVariableNamesWorker(sci::Script &script, const vector<pair<string, string>> &fromToMapping) : _fromToMapping(fromToMapping) { script.Traverse(*this); }

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            _Transform(SafeSyntaxNode<RestStatement>(&node));
            _Transform(SafeSyntaxNode<SendCall>(&node));
            _Transform(SafeSyntaxNode<LValue>(&node));
            _Transform(SafeSyntaxNode<FunctionParameter>(&node));
            // Note: if one script is converted but not others, this won't compile properly:
            _Transform(SafeSyntaxNode<SendParam>(&node));
            _Transform(SafeSyntaxNode<ClassProperty>(&node));

            switch (node.GetNodeType())
            {
                case NodeType::NodeTypeValue:
                case NodeType::NodeTypeComplexValue:
                {
                    PropertyValueBase *value = static_cast<PropertyValueBase*>(&node);
                    if (value->GetType() == ValueType::Token)
                    {
                        std::string name = value->GetStringValue();
                        if (_Transform(name))
                        {
                            value->SetValue(name, value->GetType());
                        }
                    }
                    break;
                }
                case NodeType::NodeTypeVariableDeclaration:
                {
                    VariableDecl *value = static_cast<VariableDecl*>(&node);
                    std::string name = value->GetName();
                    if (_Transform(name))
                    {
                        value->SetName(name);
                    }
                    break;
                }
            }
        }
    }

private:
    bool _Transform(std::string &name)
    {
        auto it = find_if(_fromToMapping.begin(), _fromToMapping.end(), [&name](const pair<string, string> &entry) { return entry.first == name; });
        if (it != _fromToMapping.end())
        {
            name = it->second;
            return true;
        }
        return false;
    }
    void _Transform(NamedNode *node)
    {
        if (node)
        {
            std::string name = node->GetName();
            if (_Transform(name))
            {
                node->SetName(name);
            }
        }
    }

    const vector<pair<string, string>> &_fromToMapping;
};

void CleanVariableNames(sci::Script &script, const vector<pair<string, string>> &fromToMapping)
{
    CleanVariableNamesWorker worker(script, fromToMapping);
}