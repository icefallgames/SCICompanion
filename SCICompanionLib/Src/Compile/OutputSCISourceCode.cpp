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
#include "OutputCodeHelper.h"
#include "OutputSourceCodeBase.h"

// Still a WIP.

/*
    ; Information about the original SCI Syntax
    ; headers.. they can pretty much contain anything.

    ; A comment

    (include    game.sh)    // No quotes
    (define ID_GAME   1)
    (define ID_FOO    $01)

    (enum 1                 // Starts at 1. If ommitted, starts at 0. Can be any number, even -ve
        FOO                 // 1
        BAR                 // 2
        BLAH                // 3
    )

    (extern
        Print   PRINT_SCRIPT 0      // Name, script, export. We could use use for this.
    )

    (define	Bloo	(ModuleID SOMETHING_SCRIPT 4))

    (define	SOME_MASK		(~             %111100))

    ; defines seem like macros
    (define viewTHING		(if isAThing vThing1 else vThing2))


    ; Variable types
    (global
        int bloop 100
        id blarg 105
    )

    ; locals
    (local
        int bloop = 4
        int balh
        id bam
        id fooo
    )

    ; procedures
    (procedure (MyProc aParam)
    )

*/

/*
    ; Script declaration? ACTOR is a define?
    (script# ACTOR)
    (module# BLOOP) ; what's the diff with above?

    ; of
    (instance bloop of Game
    )

    ; kindof for classes
    (class Moop kindof BaseClass
        (properties
            prop1 2
            prop2 (| stopUpdOn staticView)
        )
        ; props could be pre-pended by int or id? Maybe that's only later versions.

        ; Explicit declaration of methods before use, weird
        (methods
            method1
            method2
        )

        (method (methodName param1 param2 &tmp var1 var2)
            (= var1
                5
            )
            (&= bloop (~blap)) ; ~ instead of bnot

            (KernelFunc someParam1 someParam2)

            (object selector: someParam1 somePAram2)
            (self 
                selector1:,
                selector2:,
                selector3:
            )
        )

        ; binary ops:
        ; ==        equal       (this can have multiple args)
        ; !=        not equal
        ; ~         binary not

        ; Some contants:
        TRUE
        FALSE
        NULL

        (method (varParamsMethod param1 param2)
            (if (>= argc 1)
                (= myProp param1)
                (if (>= argc 2)
                    (= someOtherProp param2)
                )
            )

            ; Takes the place of if,else if, else if, else:
            (cond
                (someExpression stuff stuff stuff stuff
                )
                (someExpression stuff stuff stuff
                )
                (else stuff stuff stuff
                )
            )

            (return)
        )

        (method (someStuff arg)

            ; rest takes no parameters. just passes remaining stuff
            (object aMethod: aparam &rest) 

            ; boolean not
            (if (not something)
            )

            (if (& (theOBj -info-?) CLASS)
                ; do something
            )

            ; keywords: or, and. Take multiple args.
            (if (or (anObject prop1?) (anObject prop2?))
                stuff
                stuff
                (return 0)
            else ; Note, no starting paren before else
                stuff
                stuff
                (return 1)
            )   
        )

        ; lots of tmp vars. what do array look like?
        (method (doit aParam &tmp thing1 thing2 thing3 [arrayDecl 100])
            ; indexing an array (or rather, parameter)
            (= foo [aParam thing1])

            ; for loop
            (for ('init') ('condition') ('post')
                stuff stuff stuff stuff
            )

            (= ret
                (cond
                    ((expression1)) ; value would be expression1

                    ((expression2)
                        -1          ; value would be -1
                    )

                    ((expression3)
                        -2          ; value would be -2
                    )
                    ; not sure what happens in else case, I guess 0
            )
        )

        ; Value of an expression is the last guy in the list of guys


        (method (doSomeText &tmp [buffer 100])
            (SomeProcedure @[buffer 0] blah blah balh)
            ; or...
            (SomeProcedure @buffer blah blah balh)

            (define SHIFT_AMOUNT 8) ; defines can be declared in methods

            ; while loops

            (while expr stuff stuff stuff stuff)

            ; selector literals end in ':'
            (self eachElementDo: #setSize:)
            ; but not always...
            (gameStr boop: #right)
            ; it might be for disambiguating:
            (cast
                eachElementDo: #dispose:,
                eachElementDo: #delete
            )
            ; but...
            (timers eachElementDo: #delete:)

            
            ; switches
            (switch value
                (value
                    stuff
                    stuff
                )
                (value2
                    stuff
                    stggg
                )
                (else
                    stuff
                    stuff
                )
            )

            ; switchto
            (switchto value
                (stuff stuff stuff)
                (stuff stuff stuff)
                (stuff stuff stuff)
            )

        )

    )

    ; how about exports?
    (public
        ExportName      0
        AnotherProcName 1
    )



    ; UNSURE ABOUT
    ; {} ??? Literal strings.
*/

/*
    ; some contants
    ; ignrHrz
    ; CMAP, VMAP, PMAP
*/


/*
    ; Some long names
    (class Room kindof Region
        (properties
            id		name "Rm"
            ; etc...


*/

/*
    Some kernels

    KSound
        (enum
            MasterVol
            SoundOn
            RestoreSound
            NumVoices
            ChangeSndState
            InitSound
            KillSound
            PlaySound
            StopSound
            PauseSound
            FadeSound
            UpdateCues
            MidiSend
            SetReverb
            HoldSound
        )

    ;File handling.
        FileSystem
            (enum
                FSOpen
                FSClose
                FSPuts
                FSGets
                FSGetCWD
                FSPutByte
                FSGetByte
                FSFreeSpace
                FSValidPath
                FSSeek
                FSUnlink
                FSAsArray
                FSFromArray
                FSFileLength
                FSDirFirst
                FSDirNext
            )
            (enum
                FSFromStart
                FSFromCur
                FSFromEnd
            )

    Graph
        (enum					; ARGS								RETURNS
            GLoadBits		; bitmap number
            GDetect			; none 								# of colors available
            GSetPalette		; Palette number
            GDrawLine  		; yStart/xStart/yEnd/xEnd mapSet colors...
            GFillArea  		; x/y/ mapSet colors...
            GDrawBrush 		; x/y/ size randomSeed mapSet colors...
            GSaveBits		; rectangle mapset             saveID of area
            GRestoreBits	; saveID from SaveBits
            GEraseRect		; top/left/bottom/right (draws visual in background color)
            GPaintRect		; top/left/bottom/right (draws visual in foreground color)
            GFillRect  		; rectangle mapSet colors...
            GShowBits		; rectangle mapSet
            GReAnimate		; rectangle
            GInitPri			; horizon/base Rebuild priority tables
        )


*/

using namespace sci;
using namespace std;

class OutputSCISourceCode : public OutputSourceCodeBase
{
public:
    OutputSCISourceCode(SourceCodeWriter &out) : OutputSourceCodeBase(out) {}

    void Visit(const Script &script) override
    {
        for (const auto &include : script.GetIncludes())
        {
            out.out << "(include \"" << include << "\")" << out.NewLineString();
        }
        for (const auto &uses : script.GetUses())
        {
            out.out << "(use \"" << uses << "\")" << out.NewLineString();
        }
        ScriptId scriptId = script.GetScriptId();
        if (scriptId.GetResourceNumber() == InvalidResourceNumber)
        {
            if (!script.GetScriptNumberDefine().empty())
            {
                out.out << "(script " << script.GetScriptNumberDefine() << ")" << out.NewLineString();
            }
            // else, this is a legit (e.g. for outputting header files)
        }
        else
        {
            out.out << "(script " << scriptId.GetResourceNumber() << ")" << out.NewLineString();
        }

        Forward(script.GetSynonyms());
        out.out << endl;

        Forward(script.GetDefines());
        out.out << endl;

        ForwardOptionalSection("local", script.GetScriptVariables());

        ForwardOptionalSection("string", script.GetScriptStringsDeclarations());

        Forward(script.GetProcedures());
        out.out << endl;
        Forward(script.GetClasses());
    }

    void Visit(const ClassDefinition &classDef) override
    {
        out.SyncComments(classDef);

        if (classDef.IsInstance())
        {
            out.out << "(instance ";
            if (classDef.IsPublic())
            {
                out.out << "public ";
            }
        }
        else
        {
            out.out << "(class ";
        }
        out.out << CleanToken(classDef.GetName());
        if (!classDef.GetSuperClass().empty())
        {
            out.out << " of " << CleanToken(classDef.GetSuperClass());
        }
        out.out << out.NewLineString();

        {
            DebugIndent indent(out);
            {
                DebugLine line(out);
                if (classDef.GetProperties().empty())
                {
                    out.out << "(properties)";
                }
                else
                {
                    out.out << "(properties" << out.NewLineString();
                    {
                        DebugIndent indent2(out);
                        Forward(classDef.GetProperties());
                    }
                    Indent(out);
                    out.out << ")";
                }
            }

            // Body (methods)
            Forward(classDef.GetMethods());
        }
        out.out << ")" << out.NewLineString();
    }

    void Visit(const FunctionParameter &param) override {}
    void Visit(const FunctionSignature &sig) override {}

    void _VisitPropertyValue(const PropertyValueBase &prop)
    {
        out.SyncComments(prop);
        DebugLine line(out);
        switch (prop.GetType())
        {
        case ValueType::String:
            out.out << "\"" << UnescapeString(prop.GetStringValue()) << "\"";
            break;
        case ValueType::Said:
            out.out << "'" << prop.GetStringValue() << "'";
            break;
        case ValueType::Number:
            _OutputNumber(out.out, prop.GetNumberValue(), prop._fHex, prop._fNegate);
            break;
        case ValueType::Token:
            // Surround in braces if there are spaces in the string.
            out.out << CleanToken(prop.GetStringValue());
            // REVIEW: When this is C++ syntax, we should strip them out... and if it's -info-, use the replacement!
            // (objectSpecies)
            break;
        case ValueType::Selector:
            out.out << "#" << prop.GetStringValue();
            break;
        case ValueType::Pointer:
            out.out << "@" << prop.GetStringValue();
            break;
        }
    }

    void Visit(const PropertyValue &prop) override
    {
        _VisitPropertyValue(prop);
    }

    void Visit(const ComplexPropertyValue &prop) override
    {
        _VisitPropertyValue(prop);
        if (prop.GetIndexer())
        {
            Inline inln(out, true);
            out.out << "[";
            prop.GetIndexer()->Accept(*this);
            out.out << "]";
        }
    }

    void Visit(const Define &define) override
    {
        out.SyncComments(define);
        bool fHex = IsFlagSet(define.GetFlags(), IntegerFlags::Hex);
        bool fNegate = IsFlagSet(define.GetFlags(), IntegerFlags::Negative);
        out.out << "(define " << define.GetLabel() << " ";
        _OutputNumber(out.out, define.GetValue(), fHex, fNegate);
        out.out << ")" << out.NewLineString();
    }

    void Visit(const ClassProperty &classProp) override
    {
        out.SyncComments(classProp);
        DebugLine line(out);
        out.out << CleanToken(classProp.GetName()) << " ";
        Inline inln(out, true);
        classProp.GetStatement1()->Accept(*this);
    }

    void Visit(const SingleStatement &statement) override
    {
        out.SyncComments(statement);
        if (statement.GetSegment())
        {
            statement.GetSegment()->Accept(*this);
        }
        else
        {
            out.out << "ERROR - EMPTY STATEMENT";
        }
    }

    void Visit(const VariableDecl &varDecl) override
    {
        out.SyncComments(varDecl);
        DebugLine line(out);
        // TODO: in caller, put a local block around things if these are script variables.
        _OutputVariableAndSize(*this, out, varDecl.GetDataType(), varDecl.GetName(), varDecl.GetSize(), varDecl.GetStatements());
    }

    void _VisitFunctionBase(const FunctionBase &function)
    {
        out.SyncComments(function);

        if (function.GetSignatures().empty())
        {
            out.out << "CORRUPT FUNCTION " << function.GetName() << out.NewLineString();
        }
        else
        {
            out.out << "(" << function.GetName();
            const FunctionParameterVector &params = function.GetSignatures()[0]->GetParams();

            bool fFirst = false;
            for (auto &functionParam : params)
            {
                if (!fFirst)
                {
                    out.out << " ";
                }
                out.out << functionParam->GetName();
            }
            out.out << ")" << out.NewLineString();

            // Body of function
            {
                DebugIndent indent(out);
                {
                    Inline inln(out, true);
                    if (!function.GetVariables().empty())
                    {
                        Indent(out);
                        out.out << "(";
                        Forward(function.GetVariables(), ", ");
                        out.out << ")" << out.NewLineString();
                    }
                    // TODO: spit out the values here too.  _tempVarValues
                    // And somehow coalesce variables types? Or Maybe not...
                }
                Forward(function.GetCodeSegments());
            }

            DebugLine line(out);
            out.out << ")" << out.NewLineString();
        }
    }

    void Visit(const MethodDefinition &function) override
    {
        out.SyncComments(function);
        NewLine(out);
        out.out << "(method ";
        _VisitFunctionBase(function);
    }

    void Visit(const ProcedureDefinition &function) override
    {
        out.SyncComments(function);
        _VisitFunctionBase(function);
    }

    void Visit(const Synonym &syn) override {}

    void Visit(const CodeBlock &block) override
    {
        out.SyncComments(block);
        if (out.fExpandCodeBlock || out.fAlwaysExpandCodeBlocks)
        {
            Inline inln(out, true);
            Forward(block.GetList());
        }
        else
        {
            DebugLine debugLine(out);
            out.out << "(";
            Inline inln(out, true);
            Forward(block.GetList());
            out.out << ")";
        }
    }

    void Visit(const ConditionalExpression &conditional) override
    {
        out.SyncComments(conditional);
        DebugLine debugLine(out);
        out.out << "(";
        Inline inln(out, true);
        Forward(conditional.GetStatements());
        out.out << ")";
    }

    void Visit(const Comment &comment) override
    {
        // TODO: convert to ;
        /*
        DebugLine line(out);
        out.out << comment.GetName();*/
    }

    void Visit(const SendParam &sendParam) override
    {
        out.SyncComments(sendParam);
        DebugLine debugLine(out);
        out.out << CleanToken(sendParam.GetSelectorName()) << ": ";
        if (!sendParam.GetSelectorParams().empty())
        {
            // Space-separated values
            Inline inln(out, true);
            Forward(sendParam.GetSelectorParams(), " ");
        }
    }

    void Visit(const LValue &lValue) override
    {
        out.SyncComments(lValue);
        out.out << lValue.GetName();
        if (lValue.HasIndexer())
        {
            out.out << "[";
            Inline inln(out, true);
            lValue.GetIndexer()->Accept(*this);
            out.out << "]";
        }
    }

    void Visit(const RestStatement &rest) override
    {
        out.SyncComments(rest);
        DebugLine line(out);
        out.out << "rest " << rest.GetName();
    }

    void Visit(const Cast &cast) override {}

    void Visit(const SendCall &sendCall) override
    {
        out.SyncComments(sendCall);
        DebugLine debugLine(out);
        out.out << "(";
        // This sucks because of all the different forms we have.
        if (!sendCall.GetTargetName().empty())
        {
            out.out << CleanToken(sendCall.GetTargetName()); 
        }
        else if (sendCall.GetStatement1() && (sendCall.GetStatement1()->GetType() != NodeTypeUnknown))
        {
            sendCall.GetStatement1()->Accept(*this);
        }
        else
        {
            if (sendCall._object3)
            {
                sendCall._object3->Accept(*this);
            }
            else
            {
                out.out << "WARNING_UNKNOWN_OBJECT";
            }
        }

        size_t count = sendCall.GetParams().size() + (sendCall._rest ? 1 : 0);
        // Now the params.
        // If there is more than one, then indent, because we'll do one on each line.
        if (count > 1)
        {
            Inline goInline(out, false);
            {
                DebugIndent indent(out);
                out.out << out.NewLineString();	// newline to start it out
                Forward(sendCall.GetParams(), ",");
                if (sendCall._rest)
                {
                    sendCall._rest->Accept(*this);
                }
            }
            DebugLine line(out);
            out.out << ")";
        }
        else
        {
            Inline goInline(out, true);
            Forward(sendCall.GetParams());
            out.out << ")";
        }
    }

    void Visit(const ProcedureCall &procCall) override
    {
        out.SyncComments(procCall);
        DebugLine line(out);
        out.out << procCall.GetName() << " ";
        Inline inln(out, true);
        Forward(procCall.GetStatements());
    }

    void Visit(const ReturnStatement &ret) override
    {
        out.SyncComments(ret);
        DebugLine line(out);
        out.out << "return ";
        Inline inln(out, true);
        if (ret.GetStatement1() && (ret.GetStatement1()->GetType() != NodeTypeUnknown))
        {
            ret.GetStatement1()->Accept(*this);
        }
    }

    void Visit(const ForLoop &forLoop) override
    {
        out.SyncComments(forLoop);
        // CodeBlocks are just used in for loops for now I think. And perhaps random enclosures ().
        ExpandNextCodeBlock expandCB(out, false);
        {
            DebugLine line(out);
            out.out << "(for ";
            Inline inln(out, true);
            forLoop.GetInitializer()->Accept(*this);
            out.out << " (";
            forLoop.GetCondition()->Accept(*this);
            out.out << ") ";
            forLoop._looper->Accept(*this);
        }
        // Now the code
        {
            DebugIndent indent(out);
            Forward(forLoop.GetStatements());
        }
        {
            DebugLine line(out);
            out.out << ")";
        }
    }

    void Visit(const WhileLoop &whileLoop) override
    {
        out.SyncComments(whileLoop);
        {
            DebugLine line(out);
            out.out << "(while (";
            Inline inln(out, true);
            whileLoop.GetCondition()->Accept(*this);
            out.out << ")";
        }
        // Now the code, indented.
        {
            DebugIndent indent(out);
            Forward(whileLoop.GetStatements());
        }
        {
            DebugLine line(out);
            out.out << ")";
        }
    }

    void Visit(const DoLoop &doLoop) override
    {
        // TODO: Switch to do-until?
        out.SyncComments(doLoop);
        {
            DebugLine line(out);
            out.out << "(do ";
        }
        // Now the code, indented.
        {
            DebugIndent indent(out);
            Forward(doLoop.GetStatements());
        }
        {
            DebugLine line(out);
            {
                Inline inln(out, true);
                out.out << ") while (";
                doLoop.GetCondition()->Accept(*this);
                out.out << ")";
            }
        }
    }

    void Visit(const BreakStatement &breakStatement) override
    {
        out.SyncComments(breakStatement);
        DebugLine line(out);
        out.out << "break";
    }

    // TODO: get rid of numbers if they are in order.
    void Visit(const CaseStatement &caseStatement) override
    {
        out.SyncComments(caseStatement);
        {
            DebugLine line(out);
            if (caseStatement.IsDefault())
            {
                out.out << "(else ";
            }
            else
            {
                out.out << "(case ";
                Inline inln(out, true);
                caseStatement.GetStatement1()->Accept(*this);
            }
        }
        // Now the code, indented.
        {
            DebugIndent indent(out);
            Forward(caseStatement.GetCodeSegments());
        }
        {
            DebugLine line(out);
            out.out << ")";
        }
    }

    void Visit(const SwitchStatement &switchStatement) override
    {
        out.SyncComments(switchStatement);
        {
            DebugLine line(out);
            out.out << "(switchto (";
            Inline inln(out, true);
            switchStatement.GetStatement1()->Accept(*this);
            out.out << ")";
        }
        // Now the cases, indented.
        {
            DebugIndent indent(out);
            Forward(switchStatement._cases);
        }
        {
            DebugLine line(out);
            out.out << ")";
        }
    }

    void Visit(const Assignment &assignment) override
    {
        DebugLine line(out);
        Inline inln(out, true);
        out.out << assignment.GetAssignmentOp() << " ";
        assignment._variable->Accept(*this);
        out.out << " ";
        assignment.GetStatement1()->Accept(*this);
    }

    void Visit(const BinaryOp &binaryOp) override
    {
        out.SyncComments(binaryOp);
        DebugLine line(out);
        OutputBrackets brackets(out);
        {
            Inline inln(out, true);
            out.out << binaryOp.GetOpName() << " ";
            binaryOp.GetStatement1()->Accept(*this);
            out.out << " ";
            binaryOp.GetStatement2()->Accept(*this);
        }
    }

    void Visit(const UnaryOp &unaryOp) override
    {
        out.SyncComments(unaryOp);
        DebugLine line(out);
        Inline inln(out, true);
        BracketScope bracketScope(out, true);
        out.out << unaryOp.GetOpName();
        out.out << " ";
        unaryOp.GetStatement1()->Accept(*this);
    }

    void Visit(const CppIfStatement &ifStatement) override
    {
        // When decompiling, we always create CppIfStatements, even if we're outputting SCI syntax
        if (ifStatement._fTernary)
        {
            Inline inln(out, true);
            {
                out.out << "? (";
                ifStatement.GetCondition()->Accept(*this);
                out.out << ") ";
                ifStatement.GetStatement1()->Accept(*this);
                out.out << " ";
                ifStatement.GetStatement2()->Accept(*this);
            }
        }
        else
        {
            Inline inln(out, false);	// Line by line now, overall
            {
                {
                    DebugLine ifLine(out);
                    out.out << "(if (";
                    Inline inlineCondition(out, true); // But the condition is inline
                    ifStatement.GetCondition()->Accept(*this);
                    out.out << ")";
                }
                {
                    DebugIndent indent(out);
                    ExpandNextCodeBlock expandCB(out, true);
                    Inline inlineCondition(out, false);
                    ifStatement.GetStatement1()->Accept(*this);
                }
                {
                    DebugLine closeLine(out);
                    out.out << (ifStatement.HasElse() ? ")(else" : ")");
                }
                if (ifStatement.HasElse())
                {
                    {
                        DebugIndent indent(out);
                        ExpandNextCodeBlock expandCB(out, true);
                        Inline inlineCondition(out, false);
                        ifStatement.GetStatement2()->Accept(*this);
                    }
                    DebugLine closeLine(out);
                    out.out << ")";
                }
            }
        }
    }


    void Visit(const Asm &asmSection) override
    {
        DebugLine line(out);
//        out.out << asmSection.GetInstructionName() << " " << asmSection._arguments;
    }

    void Visit(const AsmBlock &asmSection) override
    {

    }

    void Visit(const ExportEntry &exportEntry) override {}
};

void OutputSourceCode_SCI(const sci::Script &script, sci::SourceCodeWriter &out)
{
    OutputSCISourceCode output(out);
    output.Visit(script);
}

void OutputSourceCode_SCI(const sci::ClassDefinition &classDef, sci::SourceCodeWriter &out)
{
    OutputSCISourceCode output(out);
    output.Visit(classDef);
}