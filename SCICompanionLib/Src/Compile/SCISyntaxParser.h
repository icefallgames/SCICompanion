#pragma once

#include "ScriptOM.h"
#include "CrystalScriptStream.h"
#include "ParserCommon.h"
#include "AutoCompleteContext.h"
#include "StudioSyntaxParser.h"
#include <stack>
#include <deque>

// Our parser...
typedef ParserBase<SyntaxContext, streamIt, EatCommentSemi> ParserSCI;

class SCISyntaxParser
{
public:
    SCISyntaxParser();

    bool Parse(sci::Script &script, CCrystalScriptStream::const_iterator &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError, bool addCommentsToOM);
    bool Parse(sci::Script &script, CCrystalScriptStream::const_iterator &stream, std::unordered_set<std::string> preProcessorDefines, SyntaxContext &context);
    bool ParseHeader(sci::Script &script, CCrystalScriptStream::const_iterator &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError);
    void Load();

private:
    bool _fLoaded;


    ParserSCI entire_script;

    // Basics
    ParserSCI immediateValue;
    ParserSCI string_immediateValue;
    ParserSCI string_immediateValue2;
    ParserSCI array_init;
    ParserSCI pointer;
    ParserSCI general_token;
    ParserSCI selector_literal;

    // Top level constructs
    ParserSCI include;
    ParserSCI use;
    ParserSCI scriptNum;
    ParserSCI define;
    ParserSCI enumStatement;
    ParserSCI var_decl;
    ParserSCI script_var;
    ParserSCI procedure_decl;
    ParserSCI procedure_base;
    ParserSCI function_var_decl;
    ParserSCI exports;
    ParserSCI export_entry;
    ParserSCI class_decl;
    ParserSCI instance_decl;
    ParserSCI classbase_decl;
    ParserSCI properties_decl;
    ParserSCI property_decl;
    ParserSCI method_decl;
    ParserSCI method_base;

    // Statements
    ParserSCI statement;
    ParserSCI return_statement;
    ParserSCI value;
    ParserSCI assignment;
    ParserSCI assignment_operator;
    ParserSCI binary_operator;
    ParserSCI binary_operation;
    ParserSCI naryassoc_operator;
    ParserSCI naryassoc_operation;
    ParserSCI narycompare_operator;
    ParserSCI narycompare_operation;
    ParserSCI unary_operator;
    ParserSCI unary_operation;
    ParserSCI lvalue;
    ParserSCI rvalue_variable;
    ParserSCI rest_statement;
    ParserSCI break_statement;
    ParserSCI breakif_statement;
    ParserSCI continue_statement;
    ParserSCI contif_statement;
    ParserSCI repeat_statement;
    ParserSCI procedure_call;
    ParserSCI send_call;
    ParserSCI send_param_call;
    ParserSCI prop_get_call;
    ParserSCI if_statement;
    ParserSCI bare_code_block;
    ParserSCI wrapped_code_block;
    ParserSCI conditional;
    ParserSCI while_loop;
    ParserSCI for_loop;
    ParserSCI switch_statement;
    ParserSCI case_statement;
    ParserSCI switchto_statement;
    ParserSCI switchto_case_statement;
    ParserSCI asm_statement;
    ParserSCI asm_block;
    ParserSCI asm_label;
    ParserSCI asm_arg;
    ParserSCI cond_statement;

    // TODO:
    ParserSCI entire_header;

    // Utility
    static ParserSCI char_p(const char *psz);
    static ParserSCI keyword_p(const char *psz);
    static ParserSCI operator_p(const char *psz);
    static ParserSCI generateSyntaxNodeD();

    ParserSCI syntaxnode_d;
    ParserSCI alphanumAsmLabel_p;
    ParserSCI selector_p;
    ParserSCI selector_send_p;
    ParserSCI propget_p;
    ParserSCI filename_p;
    ParserSCI asmInstruction_p;
    ParserSCI alphanumNK_p;
    ParserSCI alphanumSendToken_p;
    ParserSCI alwaysmatch_p;
    ParserSCI bracestring_p;
    ParserSCI squotedstring_p;
    ParserSCI quotedstring_p;
    ParserSCI oppar;
    ParserSCI clpar;
    ParserSCI integer_p;
    ParserSCI integerNonZero_p;
    ParserSCI opbracket;
    ParserSCI clbracket;
    ParserSCI pound;
    ParserSCI atsign;
    ParserSCI comma;
    ParserSCI colon;
    ParserSCI equalSign;
    ParserSCI question;
};

