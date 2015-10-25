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
#include "ScriptOMAll.h"
#include "SCIDocs.h"
#include <format.h>
#include "AppState.h"

using namespace std;
using namespace sci;
using namespace fmt;

#ifdef DOCSUPPORT

class DocContext
{
public:
    const std::string &GetClass()
    {
        return _className;
    }
    void SetClass(const std::string &theClass)
    {
        _className = theClass;
    }
    void clear()
    {
        _className.clear();
    }
private:
    std::string _className;
};

std::string GetMethodLink(const ClassDefinition *pClass, const MethodDefinition *pMethod)
{
    std::string ret = pClass->GetName();
    ret += "__";
    ret += pMethod->GetName();
    ret += ".html";
    return ret;
}

void _WriteSectionHeader(std::stringstream &ssHtml, const std::string &text, const std::string &link)
{
    ssHtml << "\t\t<h3 class=CTitle>\n";
    if (link.empty())
    {
        ssHtml << format("\t\t\t<a name=\"{0}\"></a>{0}\n", text, text);
    }
    else
    {
        ssHtml << format("\t\t\t<a name=\"{0}\"></a><a href=\"{1}.html\">{0}</a>\n", text, link);
    }
    ssHtml << "\t\t</h3>\n";
}

class WriteStandardHeader
{
public:
    WriteStandardHeader(std::stringstream &ss) : _ss(ss)
    {
        _ss << "<html>\n";
        _ss << "<head><link rel=\"stylesheet\" type=\"text/css\" href=\"docs.css\"></head>\n";
        _ss << "<body>\n";
    }
    ~WriteStandardHeader()
    {
        _ss << "</body></html>\n";
    }
private:
    std::stringstream &_ss;

};

class WriteBigTitle
{
public:
    WriteBigTitle(std::stringstream &ss, const std::string &title) : _ss(ss)
    {
        _ss << "<div class=CClass id=MainTopic>\n";
        _ss << "\t<div class=CTopic>\n";
        _ss << "\t\t<h1 class=CTitle>\n";
        _ss << "\t\t\t" << title << "\n";
        _ss << "\t\t</h1>\n";
        _ss << "\t\t<div class=CBody>\n";
    }

    ~WriteBigTitle()
    {
        _ss << "\t\t</div>\n";
        _ss << "\t</div>\n";
        _ss << "</div>\n";    
    }
private:
    std::stringstream &_ss;
};



//
// Namespace for referring to other docs:
//
// method.<methodname>
// method.<class>.<methodname>
// proc.<procedurename>         -> this includes kernels
// class.<classname>
// script.<scriptname>
//

static const std::string strProcPage = "{0}.html";
static const std::string strClassPage = "{0}.html";
static const std::string strMethodPage = "{0}__{1}.html";


typedef std::pair<std::string, std::string> stringpair;
std::pair<std::string, std::string> GetLink(DocContext &context, vector<string> &parts)
{
    std::stringstream retText;
    std::stringstream retLink;
    vector<string>::iterator it = parts.begin();
    if (it != parts.end())
    {
        if (*it == "method")
        {
            ASSERT(parts.size() > 1);
            std::string className;
            std::string methodName;
            if (parts.size() == 2)
            {
                className = context.GetClass();
                methodName = parts[1];
            }
            else
            {
                className = parts[1];
                methodName = parts[2];
            }
            ASSERT(!className.empty());
            retText << format("{0}.{1}", className, methodName);
            retLink << format(strMethodPage, className, methodName);
        }
        else if (*it == "proc")
        {
            ASSERT(parts.size() > 1);
            retText << parts[1];
            retLink << format(strProcPage, parts[1]);
        }
        else if (*it == "class")
        {
            if (parts.size() > 1)
            {
                retText << parts[1];
                retLink << format(strClassPage, parts[1]);
            }
            else
            {
                retText << context.GetClass();
                retLink << format(strClassPage, context.GetClass());
            }
        }
        else if (*it == "script")
        {
            ASSERT(parts.size() > 1);
            retText << format("{0}.scp", parts[1]);
            retLink << format("script_{0}.html", parts[1]);
        }
        else
        {
            ASSERT(FALSE);
        }
    }
    return make_pair(retText.str(), retLink.str());
}

std::string EscapeHtmlString(std::string src)
{
    size_t thePos;
    while ((thePos = src.find('>')) != string::npos)
    {
        src.replace(thePos, 1, "&gt");
    }
    while ((thePos = src.find('<')) != string::npos)
    {
        src.replace(thePos, 1, "&lt");
    }
    return src;
}


class OutputWithLinks
{
public:
    explicit OutputWithLinks(DocContext &context, const string &src) : _context(context), _src(src) {}
    friend std::ostream &operator<<(std::ostream &ss, const OutputWithLinks& owl);
private:
    const string &_src;
    DocContext &_context;
};

std::ostream &operator<<(std::ostream &ss, const OutputWithLinks& owl)
{
    const std::string &_src = owl._src;
    size_t pos = 0;
    size_t linkStart = _src.find_first_of('<', pos);
    while ((pos < _src.length()) && (linkStart != string::npos))
    {
        // Spit out the piece of text before the part we think could be a lnk.
        ss << EscapeHtmlString(_src.substr(pos, linkStart - pos));

        vector<string> parts;
        string part;
        size_t linkPos = linkStart + 1;
        bool fIsALink = false;
        while (linkPos < _src.length())
        {
            char c = _src[linkPos];
            if (isalnum(c))
            {
                part += c;
            }
            else if (c == '.')
            {
                parts.push_back(part);
                part.clear();
            }
            else if (c == '>')
            {
                // It's over.
                parts.push_back(part);
                fIsALink = true;
                break;
            }
            else
            {
                break; // Not a link.
            }
            ++linkPos;
        }
        ++linkPos; // Advance past '>'
        pos = min(linkPos, _src.length());
        if (fIsALink)
        {
            stringpair sp = GetLink(owl._context, parts);
            ss << "<a href=\"" << sp.second << "\">" << sp.first << "</a>";
/*                ss << "\">"; // Close the begin anchor
                ss <<  EscapeHtmlString(_src.substr(linkStart + 1, pos - linkStart - 2)); // The link text
                ss << "</a>";*/
        }
        else
        {
            // It wasn't a link... just spit out the text as is.
            ss <<  EscapeHtmlString(_src.substr(linkStart, pos - linkStart));
        }
        linkStart = _src.find_first_of('<', pos);
    }
    ss << EscapeHtmlString(_src.substr(pos, _src.length() - pos));
    return ss;
}


std::string EnforceFixedSpace(std::string src)
{
    size_t thePos;
    while ((thePos = src.find(' ')) != string::npos)
    {
        src.replace(thePos, 1, "&nbsp");
    }
    return src;
}


class DocTokenBase
{
public:
    virtual void GenerateHtml(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const = 0;
    virtual ~DocTokenBase() {}
    virtual void GenerateSnippet(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const {};
};

class DocTokenContainerBase : public DocTokenBase
{
public:
    void AddChild(unique_ptr<DocTokenBase> pToken)
    {
        _children.push_back(move(pToken));
    }
protected:
    vector<unique_ptr<DocTokenBase>> _children;
};

//
// Actual doc tokens
//
class DocGroup : public DocTokenContainerBase
{
public:
    void GenerateHtml(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        for(auto &pBase : _children)
        {
            pBase->GenerateHtml(context, ss, linePrefix);
        }
    }
    void GenerateSnippet(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        if (!_children.empty())
        {
            _children[0]->GenerateSnippet(context, ss, linePrefix);
        }
    }
    bool MoreThanOneLine() const { return _children.size() > 1; }
};

class DocHeader : public DocTokenBase
{
public:
    DocHeader(const std::string &text) : _headerText(text) {}
    void GenerateHtml(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        ss << linePrefix << "<h4 class=CHeading>" << OutputWithLinks(context, _headerText) << "</h4>\n";
    }

private:
    std::string _headerText;
};

class DocParagraph : public DocTokenBase
{
public:
    DocParagraph(const std::string &line) { AddLine(line); }
    DocParagraph() { }
    void GenerateHtml(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        ss << linePrefix << "<p class=CParagraph>\n";
        ss << linePrefix << OutputWithLinks(context, _text) << "\n";
        ss << linePrefix << "</p>\n";
    }
    void GenerateSnippet(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        GenerateHtml(context, ss, linePrefix);
    }
    void AddLine(const std::string &text)
    {
        _text += " ";
        _text += text;
    }
private:
    std::string _text;
};

class DocCodeSection : public DocTokenBase
{
public:
    void GenerateHtml(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        ss << linePrefix << "<blockquote>\n";
        ss << linePrefix << "<table>\n";
        ss << linePrefix << "<td border=0 cellspacing=0 cellpadding=0 class=CCode>\n";
         ss << linePrefix << "<pre>";
        for (size_t i = 0; i < _lines.size(); ++i)
        {
            const std::string &line = _lines[i];
            //ss << EnforceFixedSpace(line);
            if (i == (_lines.size() - 1))
            {
                // Strip the \n off of the last line.
                ASSERT(line.length());
                ss << EscapeHtmlString(line.substr(0, line.length() - 1));
            }
            else
            {
                ss << EscapeHtmlString(line);
            }
        }
        
        ss << "</pre>\n";
        ss << linePrefix << "</td>\n";
        ss << linePrefix << "</table>\n";
        ss << linePrefix << "</blockquote>\n";
    }
    void AddLine(const std::string &text)
    {
        _lines.push_back(text);
    }
private:
    std::vector<std::string> _lines;
};

class DocBulletList : public DocTokenContainerBase
{
public:
    void GenerateHtml(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        ss << linePrefix << "<ul class=CBulletList>\n";
        for(auto &pBase : _children)
        {
            ss << "\t";
            pBase->GenerateHtml(context, ss, linePrefix);
        }
        ss << linePrefix << "</ul>\n";
    }
};

class DocBullet : public DocTokenBase
{
public:
    DocBullet(const std::string &text) : _text(text) {}
    void AddText(const std::string &text)
    {
        _text += " ";
        _text += text;
    }
    void GenerateHtml(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        ss << linePrefix << format("<li>{0}</li>", OutputWithLinks(context, _text)) << "\n";
    }

private:
    std::string _text;
};

class DocDefinitionList : public DocTokenContainerBase
{
public:
    void GenerateHtml(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        ss << linePrefix << "<table border=0 cellspacing=0 cellpadding=0 class=CDescriptionList>\n";
        std::string myLinePrefix = linePrefix;
        myLinePrefix += "\t";
        for(auto &pBase : _children)
        {
            ss << "\t";
            pBase->GenerateHtml(context, ss, linePrefix);
        }
        ss << linePrefix << "</table>\n";
    }
};

class DocDefinition : public DocTokenBase
{
public:
    DocDefinition(const std::string &item, const std::string &definition) : _item(item), _definition(definition) {}
    void GenerateHtml(DocContext &context, std::stringstream &ss, const std::string &linePrefix) const
    {
        ss << linePrefix << "<tr>\n";
        ss << linePrefix << format("\t<td class=CDLEntry>{0}</td>\n", OutputWithLinks(context, _item)) << "\n";
        ss << linePrefix << format("\t<td class=CDLDescription>{0}</td>\n", OutputWithLinks(context, _definition)) << "\n";
        ss << linePrefix << "</tr>\n";
    }
    void AddText(const std::string &text)
    {
        _definition += " ";
        _definition += text;
    }

private:
    std::string _item;
    std::string _definition;
};


//
// Predicate for sorting syntax nodes by position.
//
bool SortByPosition(SyntaxNode *p1, SyntaxNode *p2)
{
    return p1->GetPosition() < p2->GetPosition();
}



struct CommentInfo
{
    int startLine;
    int endLine;
    std::string comment;
};

class LineInfo
{
public:
    enum Type
    {
        Blank,
        PlainText,                  // Nothing special
        EndsWithColon,              // My Header:
        StartsWithSingleQuote,      // >  a = b;
        StartsWithDash,             // - don't do this
        StartsWithWordDash,         // pNode - the node   (second part is the secondary text)
        StartsCode,                 // [code]
        EndsCode,                   // [/code]
    };

    LineInfo(const std::string &orig, Type type, const std::string &text) : _orig(orig), _type(type), _text(text) {}
    LineInfo(const std::string &orig, Type type, const std::string &text, const std::string &secondary) :
                _orig(orig), 
                _type(type),
                _text(text),
                _secondaryText(secondary)  {}

    Type GetType() const { return _type; }
    const std::string &GetOrig() const { return _orig; }
    const std::string &GetText() const { return _text; }
    const std::string &GetSecondaryText() const { return _secondaryText; }

private:
    std::string _orig;              // Original line text, char for char
    std::string _text;              // Main parsed line text
    std::string _secondaryText;     // Secondary parsed line text
    Type _type;
};

typedef std::vector<LineInfo> LineContainer;
typedef LineContainer::const_iterator LineIterator;

unique_ptr<DocCodeSection> _CreateCodeSectionGT(LineIterator &start, LineIterator end)
{
	unique_ptr<DocCodeSection> pCodeSection = std::make_unique<DocCodeSection>();
    while ((start != end) && (start->GetType() == LineInfo::StartsWithSingleQuote))
    {
        pCodeSection->AddLine(start->GetText());
        ++start;
    }
    return pCodeSection;
}

unique_ptr<DocCodeSection> _CreateCodeSectionBlock(LineIterator &start, LineIterator end)
{
	unique_ptr<DocCodeSection> pCodeSection = std::make_unique<DocCodeSection>();
    ASSERT(start->GetType() == LineInfo::StartsCode);
    ++start; // To get past the StartsCode
    while ((start != end) && (start->GetType() != LineInfo::EndsCode))
    {
        // REVIEW: do we need the secondary text too?
        pCodeSection->AddLine(start->GetOrig());
        ++start;
    }
    if (start->GetType() == LineInfo::EndsCode)
    {
        ++start;
    }
    return pCodeSection;
}

unique_ptr<DocParagraph> _CreateParagraph(LineIterator &start, LineIterator end)
{
	unique_ptr<DocParagraph> pParagraph = std::make_unique<DocParagraph>();
    ASSERT(start->GetType() == LineInfo::PlainText);
    while ((start != end) && (start->GetType() != LineInfo::Blank))
    {
        pParagraph->AddLine(start->GetOrig());
        ++start;
    }
    return pParagraph;
}

unique_ptr<DocBulletList> _CreateBulletList(LineIterator &start, LineIterator end)
{
	unique_ptr<DocBulletList> pBullets = std::make_unique<DocBulletList>();
    ASSERT(start->GetType() == LineInfo::StartsWithDash);
    DocBullet *pBulletLast = NULL;
    while ((start != end) && (start->GetType() != LineInfo::Blank))
    {
        if (start->GetType() == LineInfo::StartsWithDash)
        {
			unique_ptr<DocTokenBase> pBullet = std::make_unique<DocBullet>(start->GetText());
            pBulletLast = static_cast<DocBullet*>(pBullet.get());
            pBullets->AddChild(std::move(pBullet));
        }
        else
        {
            pBulletLast->AddText(start->GetOrig());
        }
        ++start;
    }
    return pBullets;
}

unique_ptr<DocDefinitionList> _CreateDefinitionList(LineIterator &start, LineIterator end)
{
    unique_ptr<DocDefinitionList> pDefinitions = std::make_unique<DocDefinitionList>();
    ASSERT(start->GetType() == LineInfo::StartsWithWordDash);
    DocDefinition *pDefinitionLast = NULL;
    while ((start != end) && (start->GetType() != LineInfo::Blank)) // Stop the list on a blank line
    {
        if (start->GetType() == LineInfo::StartsWithWordDash)
        {
			unique_ptr<DocTokenBase> pDefinition = std::make_unique<DocDefinition>(start->GetText(), start->GetSecondaryText());
            pDefinitionLast = static_cast<DocDefinition*>(pDefinition.get());
            pDefinitions->AddChild(std::move(pDefinition));
        }
        else
        {
            pDefinitionLast->AddText(start->GetOrig());
        }
        ++start;
    }
    return pDefinitions;
}

unique_ptr<DocGroup> _ParseLineInfo(LineContainer &lineInfos)
{
    //
    // list_item = StartsWithDash >> *(~Blank | StartsWithDash);
    // list = list_item >> *list_item;
    //
    // definition_item = StartsWithWordDash >> *(~Blank | StartsWithWordDash);
    // definitions = definition_item >> *definition_item
    //
    // header = EndsWithColon
    //
    // code_section = StartsCode >> *(Blank | PlainText | EndsWithColon |
    //                                StartsWithSingleQuote | StartsWithDash | StartsWithWordDash | StartsCode) >> EndsCode);
    //
    // 
    //

	unique_ptr<DocGroup> pGroup = std::make_unique<DocGroup>();

    LineIterator lineIt = lineInfos.begin();
    while (lineIt != lineInfos.end())
    {
        switch (lineIt->GetType())
        {
        case LineInfo::Blank:
            // Nothing to do
            ++lineIt;
            break;

        case LineInfo::EndsWithColon:
            {
				unique_ptr<DocTokenBase> pHeader = std::make_unique<DocHeader>(lineIt->GetText());
                pGroup->AddChild(std::move(pHeader));
                ++lineIt;
            }
            break;

        case LineInfo::StartsWithSingleQuote:
            pGroup->AddChild(_CreateCodeSectionGT(lineIt, lineInfos.end()));
            break;

        case LineInfo::StartsCode:
            pGroup->AddChild(_CreateCodeSectionBlock(lineIt, lineInfos.end()));
            break;

        case LineInfo::PlainText:
            pGroup->AddChild(_CreateParagraph(lineIt, lineInfos.end()));
            break;

        case LineInfo::StartsWithDash:
            pGroup->AddChild(_CreateBulletList(lineIt, lineInfos.end()));
            break;

        case LineInfo::StartsWithWordDash:
            pGroup->AddChild(_CreateDefinitionList(lineIt, lineInfos.end()));
            break;

        case LineInfo::EndsCode:
            ++lineIt;
            break;

        default:
            ASSERT(FALSE);
            ++lineIt;
            break;
        }
    }
    return pGroup;
}



static const std::string g_whiteSpace = " \t";
std::string _TrimWhitespace(const std::string &line)
{
    size_t posStart = line.find_first_not_of(g_whiteSpace);
    if (posStart == std::string::npos)
    {
        return "";
    }
    else
    {
        size_t posEnd = line.find_last_not_of(g_whiteSpace);
        if (posEnd == std::string::npos)
        {
            posEnd = line.length();
        }
        return line.substr(posStart, posEnd - posStart);
    }
}

std::vector<LineInfo> _ParseDocCommentIntoLines(SyntaxNode &node, const CommentInfo &comment)
{
    std::vector<LineInfo> lineInfos;
    char x = '\n';
    char y = '\r';
    size_t start = 0;
    size_t endLine = comment.comment.find("\n", 0);
    while (endLine != string::npos)
    {
        std::string lineTextOrig = comment.comment.substr(start, (endLine - start + 1));
        std::string lineText = _TrimWhitespace(lineTextOrig);

        // Try to match it to a line type
        if (lineText.empty())
        {
            lineInfos.push_back(LineInfo(lineTextOrig, LineInfo::Blank, lineText));
        }
        else if (*lineText.rbegin() == ':')
        {
            // Strip the colon and add it
            lineInfos.push_back(LineInfo(lineTextOrig, LineInfo::EndsWithColon, lineText.substr(0, lineText.length() - 1)));
        }
        else if (lineText[0] == '\'')
        {
            // Strip the single quote and add it
            lineInfos.push_back(LineInfo(lineTextOrig, LineInfo::StartsWithSingleQuote, lineText.substr(1, lineText.length() - 1)));
        }
        else if ((lineText[0] == '-') && isspace(lineText[1])) // - followed by whitespace
        {
            // Strip the dash and add it
            lineInfos.push_back(LineInfo(lineTextOrig, LineInfo::StartsWithDash, lineText.substr(1, lineText.length() - 1)));
        }
        else if (lineText == "[code]")
        {
            lineInfos.push_back(LineInfo(lineTextOrig, LineInfo::StartsCode, ""));
        }
        else if (lineText == "[/code]")
        {
            lineInfos.push_back(LineInfo(lineTextOrig, LineInfo::EndsCode, ""));
        }
        else
        {
            bool fFound = false;
            // A little more complicated.
            // Find the first whitespace (e.g. skip a piece of text)
            size_t whiteSpacePos = lineText.find_first_of(g_whiteSpace);
            if (whiteSpacePos != std::string::npos)
            {
                size_t nonWhiteSpacePos = lineText.find_first_not_of(g_whiteSpace, whiteSpacePos);
                if ((nonWhiteSpacePos != std::string::npos) && (lineText[nonWhiteSpacePos] == '-'))
                {
                    lineInfos.push_back(LineInfo(lineTextOrig,
                                                  LineInfo::StartsWithWordDash,
                                                  lineText.substr(0, whiteSpacePos),
                                                  lineText.substr(nonWhiteSpacePos + 1, lineText.length() - nonWhiteSpacePos - 1)));
                    fFound = true;
                }
            }

            if (!fFound)
            {
                lineInfos.push_back(LineInfo(lineTextOrig, LineInfo::PlainText, lineText));
            }
        }

        start = endLine + 1;
        endLine = comment.comment.find("\n", start);
    }
    return lineInfos;
}

typedef std::unordered_map<const SyntaxNode*, DocGroup*> NodeToCommentMap;

template <typename _T>
class OpenClose
{
public:
    OpenClose(typename _T &_tIn, const std::string &tagName, const std::string &tagClass, const std::string &extraAttribs) : _t(_tIn), _tagName(tagName)
    {
        _t.Open(_tagName, tagClass, extraAttribs);
    }
    OpenClose(typename _T &_tIn, const std::string &tagName, const std::string &tagClass) : _t(_tIn), _tagName(tagName)
    {
        _t.Open(_tagName, tagClass);
    }
    OpenClose(typename _T &_tIn, const std::string &tagName) : _t(_tIn), _tagName(tagName)
    {
        _t.Open(_tagName);
    }
    ~OpenClose()
    {
        _t.Close(_tagName);
    }
private:
    std::string _tagName;
    typename _T &_t;
};


class DocOutputHelper
{
public:
    DocOutputHelper(NodeToCommentMap &ntc) : _ntc(ntc), _tag(_ss, _linePrefix) {}

    void GenerateProcedurePage(const ProcedureDefinition *pProc)
    {
        _ss.str("");
        _linePrefix.clear();
        _context.clear();
        DocGroup *pGroup = _ntc[pProc];
        if (pProc->IsPublic() && pGroup && pGroup->MoreThanOneLine())
        {
            {
                WriteStandardHeader wsh(_ss);
                {
                    WriteBigTitle(_ss, pProc->GetName()); // temp object
                    _GenerateProcedureDefinition(pProc, false);
                }
            }
            std::stringstream ssFileName;
            ssFileName << format("{0}.html", pProc->GetName());
            std::string docFullPath = appState->GetResourceMap().GetDocPath(ssFileName.str());
            MakeTextFile(_ss.str().c_str(), docFullPath);
        }
    }

    //
    // This generates a summary of the script, which includes all public procedures, public instances,
    //  and classes.
    //
    void GenerateScriptFileSummaryPage(Script &script)
    {
        _ss.str("");
        _linePrefix.clear();
        _context.clear();
        {
            WriteStandardHeader wsh(_ss);

            // First, the big title.
            {
                std::string title = "script: ";
                title += script.GetTitle();
                WriteBigTitle wbt(_ss, title);
                NodeToCommentMap::iterator iter = _ntc.find(&script);
                if (iter != _ntc.end())  // REVIEW -> could be a bug where second is NULL
                {
                    // Insert script summary here
                    iter->second->GenerateHtml(_context, _ss, "\t\t\t");
                }
            }

            {
                OpenCloseTag oc1(_tag, "div", "Summary");
                {
                    {
                        OpenCloseTag oc2(_tag, "div", "STitle");
                        _WriteLine("Summary");
                    }

                    OpenCloseTag oc3(_tag, "div", "SBorder");
                    OpenCloseTag oc4(_tag, "table", "STable", "border=0 cellspacing=0 cellpadding=0");


                    // Start with global variables.
                    bool fOdd = false;
                    if (script.GetScriptNumber() == 0) // Only applies to script 0
                    {
                        // variables sub-header
                        _GenerateSummarySubHeader("Global variables");
                        for (auto &pVar : script.GetScriptVariables())
                        {
                            // Make the test be an anchor
                            std::string varTypeName = _GetVariableTypeAndName(pVar.get());
                            _GenerateSummaryEntry(varTypeName, "", _ntc[pVar.get()], "SConstant", fOdd);
                            fOdd = !fOdd;
                        }
                    }

                    // Then public instances
                    bool fFoundInstance = false;
                    fOdd = false;
					for (auto &pClass : script.GetClasses())
                    {
                        if (pClass->IsInstance() && pClass->IsPublic())
                        {
                            if (!fFoundInstance)
                            {
                                _GenerateSummarySubHeader("Public instances");
                            }
                            _GenerateSummaryEntry(pClass->GetName(), "", _ntc[pClass.get()], "SConstant", fOdd);
                            fOdd = !fOdd;
                            fFoundInstance = true;
                        }
                    }

                    // Then public procedures
                    fOdd = false;
                    bool fFoundPublicProc = false;
                    if (!script.GetProcedures().empty())
                    {
                        for(auto &pProc : script.GetProcedures())
                        {
                            if (pProc->IsPublic())
                            {
                                if (!fFoundPublicProc)
                                {
                                    _GenerateSummarySubHeader("Public procedures");
                                    fFoundPublicProc = true;
                                }
                                std::stringstream ss;
                                ss << format(strProcPage, pProc->GetName());
                                //_GenerateProcedureDefinition(pProc, true);
                                _GenerateSummaryEntry(pProc->GetName(), ss.str(), _ntc[pProc.get()], "SFunction", fOdd);
                                fOdd = !fOdd;
                            }
                        }
                    }

                    // Then classes
                    bool fFoundClass = false;
                    fOdd = false;
					for (auto &classDef : script.GetClasses())
                    {
						if (!classDef->IsInstance())
                        {
                            if (!fFoundClass)
                            {
                                _GenerateSummarySubHeader("Classes");
                                fFoundClass = true;
                            }
							_context.SetClass(classDef->GetName());
                            std::stringstream ss;
							ss << format(strClassPage, classDef->GetName());
							_GenerateSummaryEntry(classDef->GetName(), ss.str(), _ntc[classDef.get()], "SConstant", fOdd);
                            _context.SetClass("");
                        }
                    }
                }
            }
        }

        std::stringstream ssFileName;
        ssFileName << format("script_{0}.html", script.GetTitle());
        std::string docFullPath = appState->GetResourceMap().GetDocPath(ssFileName.str());
        ShowTextFile(_ss.str().c_str(), docFullPath);
    }

    //
    // This generates a page for a class method
    //
    void GenerateMethodPage(const MethodDefinition *pMethod)
    {
        _ss.str("");
        _linePrefix.clear();
        _context.clear();
        const ClassDefinition *pClass = pMethod->GetOwnerClass();
        _context.SetClass(pClass->GetName());
        DocGroup *pGroup = _ntc[pMethod];
        {
            WriteStandardHeader wsh(_ss);
            std::stringstream ssTitle;
            ssTitle << pClass->GetName() << "." << pMethod->GetName();
            WriteBigTitle(_ss, ssTitle.str());
            _GenerateProcedureDefinition(pMethod);
        }
        std::string docFullPath = appState->GetResourceMap().GetDocPath(GetMethodLink(pClass, pMethod));
        MakeTextFile(_ss.str().c_str(), docFullPath);
    }

    //
    // This generates a page for a class
    //
    void GenerateClassPage(const ClassDefinition *pClass)
    {
        _ss.str("");
        _linePrefix.clear();
        _context.clear();
        _context.SetClass(pClass->GetName());
        DocGroup *pGroup = _ntc[pClass];
        {
            WriteStandardHeader wsh(_ss);

            // The title that shows the super class.
            std::stringstream ssTitle;
            if (pClass->GetSuperClass().empty())
            {
                ssTitle << pClass->GetName();
            }
            else
            {
                ssTitle << format("{0} : {1}", pClass->GetName(), pClass->GetSuperClass());
            }
            WriteBigTitle(_ss, ssTitle.str());

            {
                OpenCloseTag oc1(_tag, "div", "CClass");
                {
                    OpenCloseTag oc2(_tag, "div", "CTopic");
                    {
                        OpenCloseTag oc3(_tag, "div", "CBody");
                        // Now the per-class documentation
                        DocGroup *pGroup = _ntc[pClass];
                        if (pGroup)
                        {
                            pGroup->GenerateHtml(_context, _ss, _linePrefix);
                        }
                    }
                }
            }

            // And a summary table
            _GenerateClassSummary(pClass);

        }
        std::stringstream ssFileName;
        ssFileName << format("{0}.html", pClass->GetName());
        std::string docFullPath = appState->GetResourceMap().GetDocPath(ssFileName.str());
        MakeTextFile(_ss.str().c_str(), docFullPath);
    }

private:

    class WriteTagClass
    {
    public:
        WriteTagClass(std::stringstream &ss, std::string &linePrefix) : _ss(ss), _linePrefix(linePrefix) {}
        void Open(const std::string &tagName, const std::string &theClass, const std::string &extraAttribs)
        {
            _ss << _linePrefix << "<" << tagName << " class=" << theClass << " " << extraAttribs << ">\n";
            _linePrefix += "\t";
        }
        void Open(const std::string &tagName, const std::string &theClass)
        {
            _ss << _linePrefix << "<" << tagName << " class=" << theClass << ">\n";
            _linePrefix += "\t";
        }
        void Open(const std::string &tagName)
        {
            _ss << _linePrefix << "<" << tagName << ">\n";
            _linePrefix += "\t";
        }
        void Close(const std::string &tagName)
        {
            _linePrefix.erase(_linePrefix.size() - 1, 1); // Remove \t
            _ss << _linePrefix << "</" << tagName << " >\n";
        }

    private:
        std::stringstream &_ss;
        std::string &_linePrefix;
    };
    typedef OpenClose<WriteTagClass> OpenCloseTag;

    void _WriteTopicTitle(const std::string &topicTitle)
    {
        OpenCloseTag oc1(_tag, "div", "CGroup");
        OpenCloseTag oc2(_tag, "div", "CTopic");
        OpenCloseTag oc3(_tag, "h3", "CTitle");
        _ss << _linePrefix << topicTitle << "\n";
    }

    void _GenerateProcedureDefinition(const FunctionBase *pProc, bool fSummary = false)
    {
        DocGroup *pGroup = _ntc[pProc]; 
        // TODO: What do we do with inherited methods?  We should show them, and indicate theyire inherited.
        //       That should be separate from whether or not we have a comment for them.  In fact, it
        //       should be included on the thing.  So we really need a class browser!
        // TODO: How do we handle overall file comments?
        //
        OpenCloseTag oc1(_tag, "div", "CFunction");
        OpenCloseTag oc2(_tag, "div", "CTopic");
        bool fHaveLink = (fSummary && pGroup && pGroup->MoreThanOneLine());
        _WriteSectionHeader(_ss, pProc->GetName(), fHaveLink ? pProc->GetName() : "");
        OpenCloseTag oc3(_tag, "div", "CBody");

		for (auto &signature : pProc->GetSignatures())
        {
            // Function prototype
            OpenCloseTag oc4(_tag, "blockquote");
            OpenCloseTag oc5(_tag, "table", "Prototype", "border=0 cellspacing=0 cellpadding=0");

			const FunctionParameterVector &params = signature->GetParams();
			bool fVarParams = signature->GetMoreParametersAllowed();
            if (params.empty())
            {
                // No parameters
                OpenCloseTag oc4(_tag, "tr");
                std::string fmt = fVarParams ? "{0}<td class=PBeforeParameters nowrap>{1} {2}(...)</td>\n" :
                                               "{0}<td class=PBeforeParameters nowrap>{1} {2}()</td>\n";
				_ss << format(fmt, _linePrefix, signature->GetDataType(), pProc->GetName());
            }
            else
            {
				size_t cRequired = signature->GetRequiredParameterCount();
                for (size_t i = 0; i < params.size(); ++i)
                {
                    std::string s1 = (i > cRequired) ? "[" : "&nbsp";
                    std::string s2 = (i > cRequired) ? "]" : "&nbsp";
                    OpenCloseTag oc4(_tag, "tr");
                    // Up to four cels in each row.
                    // Cel 1 (function name)
                    if (i == 0)
                    {
                        _ss << format("{0}<td class=PBeforeParameters nowrap>{1} {2}(</td>\n", _linePrefix, signature->GetDataType(), pProc->GetName());
                    }
                    else
                    {
                        OpenCloseTag oc4(_tag, "td");
                    }
                    // Cel 2 (parameter type)
                    _ss << format("{0}<td class=PParameter nowrap>{1}{2}</td>\n", _linePrefix, s1, params[i]->GetDataType());
                    // Cel 3 (parameter name)
                    _ss << format("{0}<td class=PParameterValue nowrap>{1}{2}</td>\n", _linePrefix, params[i]->GetName(), s2);
                    // Cel 4 (closing bracket)
                    if (i == (params.size() - 1))
                    {
                        if (fVarParams)
                        {
                            _ss << _linePrefix << "<td class=PAfterParameters nowrap>...)</td>\n";
                        }
                        else
                        {
                            _ss << _linePrefix << "<td class=PAfterParameters nowrap>)</td>\n";
                        }
                    }
                }
            }
        }

        if (pGroup)
        {
            if (fSummary)
            {
                pGroup->GenerateSnippet(_context, _ss, _linePrefix);
            }
            else
            {
                pGroup->GenerateHtml(_context, _ss, _linePrefix);

                // Also say which script it's from.
                DocHeader("Script").GenerateHtml(_context, _ss, _linePrefix);
                DocParagraph(pProc->GetOwnerScript()->GetName()).GenerateHtml(_context, _ss, _linePrefix);
            }
        }
    }

    void _GenerateClassDefinition(const ClassDefinition *pClass, bool fSummary = false)
    {
        const DocGroup *pGroup = _ntc[pClass];
        if (fSummary)
        {
            OpenCloseTag oc1(_tag, "div", "CVariable");
            OpenCloseTag oc2(_tag, "div", "CTopic");
            {
                OpenCloseTag oc3(_tag, "h3", "CTitle");
                _ss << _linePrefix << "<a name=\"" << pClass->GetName() << "\"></a>\n";
                _GenerateClassLink(pClass->GetName());
            }
            OpenCloseTag oc4(_tag, "div", "CBody");
            OpenCloseTag oc5(_tag, "p", "CParagraph");

            if (pGroup)
            {
                pGroup->GenerateSnippet(_context, _ss, _linePrefix);
            }
        }
        else
        {
            OpenCloseTag oc1(_tag, "div", "CClass", "id=MainTopic");
            OpenCloseTag oc2(_tag, "div", "CTopic");

            {
                OpenCloseTag oc3(_tag, "h1", "CTitle");
                _ss << _linePrefix << "<a name =" << pClass->GetName() << "</a>\n";
                _ss << _linePrefix << pClass->GetName() << "\n";
            }

            {
                OpenCloseTag oc4(_tag, "div", "CBody");
                if (pGroup)
                {
                    if (fSummary)
                    {
                    }
                    else
                    {
                        pGroup->GenerateHtml(_context, _ss, "\t\t\t");
                    }
                }
            }
        }
        // TODO: For the class, we also want to list all the properties and methods, with the first paragraph
        // of each (if available).  Also, each method with a descriptive paragraph will have a link to its
        // real page.
        // We can run through all the procs and stuff now, but we need the DocGroup too.
        // The question is... for properties: what do we show for inherited properties?  Do we need
        // the class table?  Can we just provide a link?  Hmm.... we don't know where the properties
        // come from (immediate super class, or super super class), so we can't just provide a link... we
        // need full on access to the class browser.  Or we could just link to the parent class, with "inherited".
        // Oh but to even list the properties, we may need the parent class.
    }

    std::string _GetVariableTypeAndName(const VariableDecl *pVar)
    {
        std::stringstream ss;
        if (IsPODType(pVar->GetDataType()))
        {
            ss << _linePrefix << format("{0} {1}\n", pVar->GetDataType(), pVar->GetName());
        }
        else
        {
            // Include a link for the type
            ss << _linePrefix << format("<a href=\"{0}\">{1}</a> {2}\n", pVar->GetDataType(), pVar->GetDataType(), pVar->GetName());
        }
        return ss.str();
    }

    void _GenerateVariableDefinition(const VariableDecl *pVar)
    {
        const DocGroup *pGroup = _ntc[pVar];
        OpenCloseTag oc1(_tag, "div", "CVariable");
        OpenCloseTag oc2(_tag, "div", "CTopic");

        {
            OpenCloseTag oc3(_tag, "h3", "CTitle");
            _ss << _GetVariableTypeAndName(pVar);
        }

        {
            OpenCloseTag oc4(_tag, "div", "CBody");
            OpenCloseTag oc5(_tag, "p", "CParagraph");
            if (pGroup)
            {
                pGroup->GenerateHtml(_context, _ss, "\t\t");
            }
        }
    }

    void _GenerateClassLink(const std::string &name)
    {
        _ss << _linePrefix << "<a href=\"" << name << ".html" << "\">" << name << "</a>\n";
    }

    void _GenerateSummarySubHeader(const std::string &headerName)
    {
        OpenCloseTag oc5(_tag, "tr");
        {
            OpenCloseTag oc6(_tag, "td");
            OpenCloseTag oc7(_tag, "div", "SGroup");
            OpenCloseTag oc8(_tag, "div", "SEntry");
            OpenCloseTag oc9(_tag, "div", "SIndent1");
            _WriteLine(headerName);
        }
        {
            OpenCloseTag oc6(_tag, "td");
        }
    }

    void _GenerateSummaryEntry(const std::string &entryName, const std::string &link, DocGroup *pGroup, const std::string &type, bool fOdd)
    {
        OpenCloseTag oc5(_tag, "tr", fOdd ? "" : "SMarked");
        {
            OpenCloseTag oc6(_tag, "td");
            OpenCloseTag oc7(_tag, "div", type);
            OpenCloseTag oc8(_tag, "div", "SEntry");
            OpenCloseTag oc9(_tag, "div", "SIndent2");
            if (link.empty())
            {
                _WriteLine(entryName);
            }
            else
            {
                _ss << _linePrefix << "<a href=\"" << link << "\">" << entryName << "</a>\n";
            }
        }
        {
            OpenCloseTag oc6(_tag, "td");
            if (pGroup)
            {
                OpenCloseTag oc7(_tag, "div", type);
                OpenCloseTag oc8(_tag, "div", "SDescription");
                OpenCloseTag oc9(_tag, "div", "SIndent2");
                pGroup->GenerateSnippet(_context, _ss, _linePrefix);
            }
        }
    }

    void _GenerateClassSummary(const ClassDefinition *pClass)
    {
        DocGroup *pGroup = _ntc[pClass];
        OpenCloseTag oc1(_tag, "div", "Summary");
        {
            {
                OpenCloseTag oc2(_tag, "div", "STitle");
                _WriteLine("Summary");
            }

            OpenCloseTag oc3(_tag, "div", "SBorder");
            OpenCloseTag oc4(_tag, "table", "STable", "border=0 cellspacing=0 cellpadding=0");

            {
                // The "header"
                OpenCloseTag oc5(_tag, "tr");
                {
                    OpenCloseTag oc6(_tag, "td", "SEntrySize");
                    OpenCloseTag oc7(_tag, "div", "SMain");
                    OpenCloseTag oc8(_tag, "div", "SEntry");
                    _WriteLine(pClass->GetName());
                }
                {
                    OpenCloseTag oc6(_tag, "td", "SDescriptionSize");
                    if (pGroup)
                    {
                        OpenCloseTag oc7(_tag, "div", "SMain");
                        OpenCloseTag oc8(_tag, "div", "SDescription");
                        pGroup->GenerateSnippet(_context, _ss, _linePrefix);
                    }
                }
            }

            // Methods sub-header
            _GenerateSummarySubHeader("Methods");

            bool fOdd = false;
			for (auto &method : pClass->GetMethods())
            {
				_GenerateSummaryEntry(method->GetName(), GetMethodLink(pClass, method.get()), _ntc[method.get()], "SFunction", fOdd);
                fOdd = !fOdd;
            }

            _GenerateSummarySubHeader("Properties");
            fOdd = false;
			for (const auto &prop : pClass->GetProperties())
            {
				_GenerateSummaryEntry(prop->GetName(), "", _ntc[prop.get()], "SConstant", fOdd);
                fOdd = !fOdd;
            }
        }
    }

    void _WriteLine(const std::string &text)
    {
        _ss << _linePrefix << text << "\n";
    }

    std::stringstream _ss;
    std::string _linePrefix;
    DocContext _context;
    WriteTagClass _tag;
    NodeToCommentMap &_ntc;
};


void CompileDocs(Script &script)
{
    std::vector<CommentInfo> comments;
    comments.reserve(script.GetComments().size());

    // Coalesce adjacent comments and strip //, */.
	for (auto &comment : script.GetComments())
	{
		size_t lastIndex = comments.size() - 1;
		int start = comment->GetLineNumber();
		if (!comments.empty() && (comments[lastIndex].endLine + 1 == start))
		{
			// Append to the last one.
			comments[lastIndex].comment += "\r\n";
			comments[lastIndex].comment += comment->GetSanitizedText();
			comments[lastIndex].endLine = comment->GetEndLineNumber();
		}
		else
		{
			CommentInfo commentInfo = { comment->GetLineNumber(), comment->GetEndLineNumber(), comment->GetSanitizedText() };
			comments.push_back(commentInfo);
		}
	}

    // Assemble the list of syntax nodes
    vector<SyntaxNode*> nodes;
	for (auto &classDef : script.GetClassesNC())
    {
        // Add the class
        nodes.push_back(classDef.get());

        // Now add all its methods
		MethodVector &methods = classDef->GetMethodsNC();
		transform(methods.begin(), methods.end(), back_inserter(nodes),
			[](const unique_ptr<MethodDefinition> &method) { return method.get(); }
		);

        // And the properties (they can have comments too)
		ClassPropertyVector &properties = classDef->GetPropertiesNC();
		transform(properties.begin(), properties.end(), back_inserter(nodes),
            [](const unique_ptr<ClassProperty> &prop) { return prop.get(); }
			);

    }
    // Now the procedures.
    ProcedureVector &procs = script.GetProceduresNC();
	transform(procs.begin(), procs.end(), back_inserter(nodes),
		[](const unique_ptr<ProcedureDefinition> &proc) { return proc.get(); }
	);

    // And script variables - but only for main
    if (script.GetScriptNumber() == 0)
    {
        const VariableDeclVector &vars = script.GetScriptVariables();
		transform(vars.begin(), vars.end(), back_inserter(nodes),
			[](const unique_ptr<VariableDecl> &var) { return var.get(); }
			);
    }

    // Now we'll sort by position.
    sort(nodes.begin(), nodes.end(), SortByPosition);

    // Handy map of SyntaxNodes to the actual doc comments.
    NodeToCommentMap nodeToComment;

    vector<unique_ptr<DocGroup>> docComments;
    // Now both the SyntaxNodes and the comments are in order.
    // Let's look at end syntax node.
    vector<CommentInfo>::const_iterator commentIt = comments.begin();
    // Special case: is there a comment right at the top of the file?  Assign it to the script.
    if ((commentIt != comments.end()) && (commentIt->startLine == 0))
    {
        std::vector<LineInfo> lineInfos = _ParseDocCommentIntoLines(script, *commentIt);
        unique_ptr<DocGroup> pGroup = _ParseLineInfo(lineInfos);
        nodeToComment[&script] = pGroup.get();
        docComments.push_back(move(pGroup));
    }
    for(SyntaxNode *pNode : nodes)
    {
        int line = pNode->GetPosition().Line();
        // Is there a comment immediately above it?
        while ((commentIt != comments.end()) && ((commentIt->startLine == line) || (commentIt->endLine < line)))
        {
            // REVIEW -> having trouble getting the gEgo comment to work...
            if ((commentIt->startLine == line) || (commentIt->endLine == (line - 1)))
            {
                // It's right above the syntax node, or next to it.
                // Parse it into lines.
                std::vector<LineInfo> lineInfos = _ParseDocCommentIntoLines(*pNode, *commentIt);
                unique_ptr<DocGroup> pGroup = _ParseLineInfo(lineInfos);
                nodeToComment[pNode] = pGroup.get();
                docComments.push_back(move(pGroup));
                ++commentIt;
                break;
            }
            ++commentIt;
        }
    }

    DocOutputHelper helper(nodeToComment);
    helper.GenerateScriptFileSummaryPage(script);

	for(auto &pProc : script.GetProcedures())
    {
        helper.GenerateProcedurePage(pProc.get());
    }

	for (auto &classDef : script.GetClasses())
    {
		if (!classDef->IsInstance())
        {
			helper.GenerateClassPage(classDef.get());
			for (auto &method : classDef->GetMethods())
            {
				helper.GenerateMethodPage(method.get());
            }
        }
    }

    // REVIEW, what we want to do is create the following files:
    // script_scriptname.html -> the summary of stuff in this script.  This could include instances...
    // classname.html for each class
    // classname__methodname.html for each specifically commented method (note two underscores)
    // procedurename.html for each public procedure
}




#endif // DOCSUPPORT