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
#pragma once

// Common parser routines


#ifdef DEBUG
// Uncomment this to get parsing debug spew. It's still very flaky
// #define PARSE_DEBUG 1
#endif

//
// Syntax parser inspired by boost's Spirit (but much less flexible and more performant)
// 

#ifdef DEBUG
#define _DEBUG_PARSER(x) (x).SetName(#x);
#else
#define _DEBUG_PARSER(x)
#endif

//
// A comment was detected - add it (text and endpoints) to the script.
//
template<typename TContext, typename _It>
inline void _DoComment(TContext *pContext, const _It &streamBegin, const _It &streamEnd)
{
    std::string comment;
    // Transfer to string:
    copy(streamBegin, streamEnd, back_inserter(comment));
    // Create a new Comment syntax node and add it to the script
	unique_ptr<sci::Comment> pComment = make_unique<sci::Comment>();
    pComment->SetPosition(streamBegin.GetPosition());
    pComment->SetEndPosition(streamEnd.GetPosition());
    pComment->SetName(comment); // The comment text
    pContext->TryAddCommentDirectly(pComment);
    if (pComment)
    {
        pContext->Script().AddComment(move(pComment));
    }
}

class EatCommentCpp
{
public:
    // c++ style comments:
    // // and /***/
    template<typename TContext, typename _It>
    static void EatWhitespaceAndComments(TContext *pContext, _It &stream)
    {
        bool fDone = false;
        while (!fDone)
        {
            fDone = true;
            // Eat whitespace
            while (isspace(*stream))
            {
                fDone = false;
                ++stream;
            }
            _It streamSave(stream);
            if (*stream == '/')
            {
                char ch = *(++stream);
                if (ch == '/')
                {
                    // Indicate we're in a comment for autocomplete's sake
                    if (pContext)
                    {
                        pContext->PushParseAutoCompleteContext(ParseAutoCompleteContext::Block);
                    }

                    // Go until end of line
                    while ((ch = *(++stream)) && (ch != '\n')) {} // Look for \n or EOF
                    fDone = false; // Check for whitespace again

                    // Comment gathering.  This may be expensive, so only do this if pContext is non-NULL
                    if (pContext)
                    {
                        pContext->PopParseAutoCompleteContext();
                        _DoComment(pContext, streamSave, stream);
                    }

                    if (ch == '\n') // As opposed to EOF
                    {
                        ++stream; // Move past \n now that we've done _DoComment. Otherwise the comment will be tagged as ending on the next line.
                    }
                }
                else if (ch == '*')
                {
                    // Indicate we're in a comment for autocomplete's sake
                    if (pContext)
                    {
                        pContext->PushParseAutoCompleteContext(ParseAutoCompleteContext::Block);
                    }

                    // Go until */
                    bool fLookingForSlash = false;
                    while (TRUE)
                    {
                        ch = *(++stream);
                        if (ch == 0)
                        {
                            break;
                        }
                        else if (ch == '*')
                        {
                            fLookingForSlash = true;
                        }
                        else if (fLookingForSlash)
                        {
                            if (ch == '/')
                            {
                                break; // We found a */
                            }
                            else
                            {
                                fLookingForSlash = false;
                            }
                        }
                    }
                    if (ch == '/') // As opposed to 0
                    {
                        ++stream; // Move past '/'
                    }

                    if (pContext)
                    {
                        pContext->PopParseAutoCompleteContext();
                        _DoComment(pContext, streamSave, stream);
                    }
                    fDone = false; // Check for whitespace again
                }
                else
                {
                    // We're done.
                    stream = streamSave;
                    fDone = true;
                }
            }
        }
    }
};

class EatCommentSemi
{
public:
    // Semi-colon style comments:
    // ;
    template<typename TContext, typename _It>
    static void EatWhitespaceAndComments(TContext *pContext, _It &stream)
    {
        bool fDone = false;
        while (!fDone)
        {
            fDone = true;
            // Eat whitespace
            while (isspace(*stream))
            {
                fDone = false;
                ++stream;
            }
            _It streamSave(stream);
            if (*stream == ';')
            {
                // Indicate we're in a comment for autocomplete's sake
                if (pContext)
                {
                    pContext->PushParseAutoCompleteContext(ParseAutoCompleteContext::Block);
                }

                // Go until end of line
                while ((ch = *(++stream)) && (ch != '\n')) {} // Look for \n or EOF
                fDone = false; // Check for whitespace again

                // Comment gathering.  This may be expensive, so only do this if pContext is non-NULL
                if (pContext)
                {
                    pContext->PopParseAutoCompleteContext();
                    _DoComment(pContext, streamSave, stream);
                }

                if (ch == '\n') // As opposed to EOF
                {
                    ++stream; // Move past \n now that we've done _DoComment. Otherwise the comment will be tagged as ending on the next line.
                }
            }
        }
    }
};

//
// Optimized delimiter reader
//
template<typename _TContext, typename _CommentPolicy, typename _It, char Q1, char Q2>
bool _ReadString(_It &stream, std::string &str)
{
    str.clear();
    while (Q1 == *stream)
    {
        char chPrev = 0;
        char ch;
        // Continue while no EOF, and while the character isn't the closing quote
        // (unless the previous character was an escape char)
        bool fEscape = false;
        while ((ch = *(++stream)) && ((ch != Q2) || (chPrev == '\\')))
        {
            chPrev = ch;
            if (!fEscape)
            {
                if (ch == '\\')
                {
                    // Escape char, don't add to strign.
                    fEscape = true;
                }
                else
                {
                    str += ch;
                }
            }
            else
            {
                // Last char was an escape char.
                fEscape = false;
                switch(ch)
                {
                case 'n':
                    str += '\n';
                    break;
                case 't':
                    str += '\t';
                    break;
                    // Any others we should handle?
                default:
                    str += ch;
                    break;
                }
            }
        }
        if (ch == Q2)
        {
            ++stream;
            if (*stream == '+')
            {
                // If we encountered a '+', then skip whitespace until we hit another open quote
                // "huwehuierger"+   "gejriger"
                ++stream;
                _CommentPolicy::EatWhitespaceAndComments<_TContext, _It>(nullptr, stream);
                // ... and continue...
            }
            else
            {
                return true; // We're done
            }
        }
        else
        {
            ASSERT(ch == 0); // EOF
            return false;
        }
    }
    return false;
}

enum class ParseAutoCompleteContext
{
    None,
    Selector,
    ClassSelector,
    Value,
    TopLevelKeyword,
    ClassLevelKeyword,
    SuperClass,
    ScriptName,
    DefineValue,        // Only define constants allowed (not variables, methods, etc...)
    Export,
    LValue,
    Block               // Use this in parsing in order to block searching up the context stack.
};

