//
// CPP tokenizer implementation
//
#include "stdafx.h"
#include "CrystalScriptStream.h"
#include "SyntaxParser.h"
#include "ScriptOM.h"
#include "CompileInterfaces.h"

using namespace std;

// PERF: reconsider commenting out things like string and said_String.... it means we read
// the chars each time... (or does it?).  Some of these tokens should be allowed where alphanum things
// are allowed.  We could also classify them as types
const key_value_pair<SyntaxToken, const char *> c_tokenMap[] =
{
    //{ TokenBool, "bool" },
    { TokenBreak, "break" },
    { TokenCase, "case" },
    { TokenClass2, "class" },
    { TokenDefault, "default" },
    { TokenDo, "do" },
    { TokenElse, "else" },
    { TokenFor, "for" },
    { TokenIf, "if" },
    { TokenInclude, "include" },
    { TokenInstance2, "instance" },
    //{ TokenInt, "int" },
    { TokenPublic, "public" },
    { TokenRest, "rest" },
    { TokenRes, "res" },
    { TokenPrivate, "private" },
    { TokenReturn, "return" },
    //{ TokenSaidString, "said_string" },
    // We don't want to tokenize scriptNumber
    //{ TokenScriptNumber, "scriptNumber" },
    //{ TokenSelf2, "self" },
    //{ TokenString, "string" }, // the var type!
    //{ TokenSuper, "super" },
    { TokenSwitch, "switch" },
    //{ TokenUInt, "uint" },
    { TokenUse, "use" },
    //{ TokenVoid, "void" },
    //{ TokenVar, "var" },
    { TokenWhile, "while" },
    { TokenDefine, "define" },
    { TokenSynonym, "synonym" },
    { TokenCast, "cast" },

    // The order of these is very important.
    // e.g. 'xxy' needs to come before 'xx'
    { TokenEllipsis, "..." },
    { TokenRightAssign, ">>=" },
    { TokenLeftAssign, "<<=" },
    { TokenAddAssign, "+=" },
    { TokenSubAssign, "-=" },
    { TokenMulAssign, "*=" },
    { TokenDivAssign, "/=" },
    { TokenModAssign, "%=" },
    { TokenAndAssign, "&=" },
    { TokenOrAssign, "|=" },
    { TokenXorAssign, "^=" },
    { TokenRightOp, ">>" },
    { TokenLeftOp, "<<" },
    { TokenIncOp, "++" },
    { TokenDecOp, "--" },
    { TokenAndOp, "&&" },
    { TokenOrOp, "||" },
    { TokenLeOp, "<=" },
    { TokenGeOp, ">=" },
    { TokenEqOp, "==" },
    { TokenNeOp, "!=" },
};

SyntaxToken SyntaxTokenFromString(const char *psz)
{
    return LookupKey(c_tokenMap, ARRAYSIZE(c_tokenMap), psz, TokenInvalid);
}
const char *StringFromSyntaxToken(SyntaxToken token)
{
    const char *pszUnknown = "UNKNOWN";
    const char *pszRet = LookupKeyValue(c_tokenMap, ARRAYSIZE(c_tokenMap), token, pszUnknown);
    ASSERT(pszRet != pszUnknown);
    return pszRet;
}

const char *StringFromSyntaxTokenDEBUG(SyntaxToken token)
{
    const char *pszUnknown = "\0";
    const char *pszRet = LookupKeyValue(c_tokenMap, ARRAYSIZE(c_tokenMap), token, pszUnknown);
    return pszRet;
}


/*
std::string StringFromSyntaxTokenFallback(SyntaxToken token)
{
    const char *pszUnknown = "UNKNOWN";
    const char *pszRet = LookupKeyValue(c_tokenMap, ARRAYSIZE(c_tokenMap), token, pszUnknown);
    if ((pszRet == pszUnknown) && (token < 128))
    {
        std::string name = static_cast<char>(token);
        return name;
    }
    return pszRet;
}*/

//
// Handles negation, hex, etc...
//
template<typename _It>
bool IntegerPCopiedFromSomewhere(_It &stream, int &i, IntegerFlags &flags)
{
    i = 0;
    bool fNeg = false;
    bool fRet = false;
    bool fHex = false;
    _It streamSave = stream;
    if (*stream == '$')
    {
        fHex = true;
        ++stream;
        while (isxdigit(*stream))
        {
            i *= 16;
            i += charToI(*stream);
            fRet = true;
            ++stream;
        }
    }
    else
    {
        if (*stream == '-')
        {
            fNeg = true;
            ++stream;
        }
        while (isdigit(*stream))
        {
            i *= 10;
            i += charToI(*stream);
            fRet = true;
            ++stream;
        }
    }
    if (fRet)
    {
        // Make sure that the number isn't followed by an alphanumeric char
        fRet = !isalnum(*stream);
    }
    if (fRet)
    {
        if (fNeg)
        {
            flags = static_cast<IntegerFlags>(flags | IntegerFlags::Negative);
        }
        if (fHex)
        {
            flags = static_cast<IntegerFlags>(flags | IntegerFlags::Hex);
        }
    }
    else
    {
        stream = streamSave; // Restore...
    }
    if (fNeg)
    {
        // Actually negate the number.  Remember, fNeg just refers to how it appeared in script.
        // It doesn't refer to the value itself.
        i = -i;
    }
    return fRet;
    // REVIEW: need to account for int overflows (we already account for WORD overflows in the AST processing).
}

template<typename _It>
bool AlphanumPCopiedFromSomewhere(_It &stream, std::string &str)
{
    bool fRet = false;
    str.clear();
    char ch = *stream;
    if (isalpha(ch) || (ch == '_'))     // First character must be a letter or _
    {
        fRet = true;
        str += ch;
        ch = *(++stream);
        while (isalnum(ch) || (ch == '_'))  // Then any alphanumeric character is fine.
        {
            fRet = true;
            str += ch;
            ch = *(++stream);
        }
    }
    return fRet;
}

//
// REVIEW: I'm not sure this is the right logic.
//
bool IsReallyEndOfToken(char nextChar, char lastTokenChar)
{
    // If the last char of our token was alphanumeric, and the next char in the stream is too, it isn't
    // really a match...
    // This prevents "script" from matching "scriptNum"
    if (isalnum(lastTokenChar) && isalnum(nextChar))
    {
        return false;
    }
    // We don't need to check operators, because we have ordered them properly to avoid this.
    // However, this does mean that >>== will be treated as ">>=" "=", which is fine.
    return true;
}

//
// This is a HACK
//
//
// The c++ parser doesn't handle minus signs in variable names. Unfortunately, some common
// SCI properties have minus signs.  in c++, we'll use underscores instead.
//
const key_value_pair<std::string, std::string> c_propAliases[] =
{
    { "b_moveCnt", "b-moveCnt" },
    { "b_i1", "b-i1" },
    { "b_i2", "b-i2" },
    { "b_di", "b-di" },
    { "b_xAxis", "b-xAxis" },
    { "b_incr", "b-incr" },
    { "_info_", "-info-" },
};
void CrystalScriptTokenStream::_ConvertAliasedTokens(std::string &token)
{
    std::string strEmpty;
    std::string foo = LookupKeyValue(c_propAliases, ARRAYSIZE(c_propAliases), token, strEmpty);
    if (foo != strEmpty)
    {
        token = foo;
    }
}
std::string GetTokenAlias(const std::string &token)
{
    std::string strEmpty;
    return LookupKey(c_propAliases, ARRAYSIZE(c_propAliases), token, token);
}

class CommentScript
{
public:
    CommentScript(sci::Script *pScript) : _pScript(pScript) {}
    sci::Script &Script() { return *_pScript; }
    void TryAddCommentDirectly(std::unique_ptr<sci::Comment> &comment) {}
private:
    sci::Script *_pScript;
};

void CrystalScriptTokenStream::Tokenize(sci::Script *pScriptForComments)
{
    CommentScript commentScript(pScriptForComments);

    CCrystalScriptStream::const_iterator it = _stream.begin();

    while (*it) // end == NULL
    {
        if (pScriptForComments)
        {
            _EatWhitespaceAndComments<CommentScript, CCrystalScriptStream::const_iterator>(&commentScript, it); // Hacky
        }
        else
        {
            _EatWhitespaceAndComments<SyntaxContext, CCrystalScriptStream::const_iterator>(NULL, it); // Hacky
        }

        if (*it)
        {
            TokenInfo info;
            info.start = it.GetPosition();
            if (_ReadString<SyntaxContext, CCrystalScriptStream::const_iterator, '"', '"'>(it, info.string))
            {
                info.type = TokenQuotedString;
                info.end = it.GetPosition();
                _tokens.push_back(info);
            }
            else if (_ReadString<SyntaxContext, CCrystalScriptStream::const_iterator, '\'', '\''>(it, info.string))
            {
                info.type = TokenSingleQuotedString;
                info.end = it.GetPosition();
                _tokens.push_back(info);
            }
            else if (IntegerPCopiedFromSomewhere(it, info.integer, info.intFlags))
            {
                info.type = TokenInteger;
                info.end = it.GetPosition();
                _tokens.push_back(info);
            }
            else
            {
                // Maybe optimize and divy keywords and operators up... 

                // Map it to a token... otherwise, turn it into a char
                int i = 0;
                for (; i < ARRAYSIZE(c_tokenMap); i++)
                {
                    CCrystalScriptStream::const_iterator itCur(it);
                    int j = 0;
                    for (; c_tokenMap[i].value[j]; j++, ++itCur)
                    {
                        if ((*itCur == 0) || (c_tokenMap[i].value[j] != *itCur))
                        {
                            break;
                        }
                    }
                    if (c_tokenMap[i].value[j] == 0)
                    {
                        // Match!  But not if there are more similar characters after...
                        if (IsReallyEndOfToken(*itCur, c_tokenMap[i].value[j - 1]))
                        {
                            info.type = c_tokenMap[i].key;
                            info.start = it.GetPosition();
                            info.end = itCur.GetPosition();
                            it = itCur; // Advance iterator
                            _tokens.push_back(info);
                            break;
                        }
                    }
                }
                if (i == ARRAYSIZE(c_tokenMap))
                {
                    // No match = check for alphanum.
                    if (AlphanumPCopiedFromSomewhere(it, info.string))
                    {
                        _ConvertAliasedTokens(info.string);
                        info.type = TokenAlphanum;
                        info.end = it.GetPosition();
                        _tokens.push_back(info);
                    }
                    else
                    {
                        // just add the char by itself.
                        info.start = it.GetPosition();
                        info.type = static_cast<SyntaxToken>(*it);
                        ++it;
                        info.end = it.GetPosition();
                        _tokens.push_back(info);
                    }
                }
            }
        }
    }

    // Add a token indicating the end.
    TokenInfo tokenEnd;
    tokenEnd.type = TokenEnd;
    _tokens.push_back(tokenEnd);

}

void CrystalScriptTokenStream::DebugPrint()
{
    // TODO: Put some text in a stream for debugging, and output it as a .txt file.
    // Maybe Disable the compile that happens when you click on a piece of text.
    // Maybe make a token that has an allocated string
    // Maybe make tokens for all characters....

    const char *pszUnknown = "UNKNOWN";
    std::stringstream out;
    for (size_t i = 0; i < _tokens.size(); ++i)
    {
        TokenInfo &token = _tokens[i];
        if (token.type < 128)
        {
            out << (char)token.type;
            if (token.type == ';')
            {
                out << '\n'; // Just for clarity.
            }
            else
            {
                out << ' ';
            }
        }
        else
        {
            // It's in our table.
            const char *pszText = LookupKeyValue(c_tokenMap, ARRAYSIZE(c_tokenMap), token.type, pszUnknown);
            if (lstrcmp(pszText, pszUnknown) == 0)
            {
                // Grab the text from the thing itself.
                if ((token.type == TokenAlphanum) || (token.type == TokenQuotedString)  || (token.type == TokenSingleQuotedString))
                {
                    out << "<" << token.string << ">";
                }
                else if (token.type == TokenInteger)
                {
                    out << "<" << token.integer << ">";
                }
                else
                {
                    CCrystalScriptStream::const_iterator itStart = _stream.get_at(token.start);
                    CCrystalScriptStream::const_iterator itEnd = _stream.get_at(token.end);
                    std::string strTemp;
                    copy(itStart, itEnd, back_inserter(strTemp));
                    out << "<" << strTemp << ">";
                }
            }
            else
            {
                out << pszText;
            }
            out << ' ';
        }
    }
    ShowTextFile(out.str().c_str(), "TokenTest.txt");

}