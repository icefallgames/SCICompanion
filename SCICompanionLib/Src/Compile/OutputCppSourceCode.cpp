#include "stdafx.h"
#include "ScriptOMAll.h"
#include "OutputCodeHelper.h"
#include "OutputSourceCodeBase.h"

using namespace sci;
using namespace std;

//
// Convert some operators between SCIStudio and CPP syntax.
//
key_value_pair<PCSTR, PCSTR> c_rgSCIStudio2CPP[] =
{
    { "bnot", "~" },
    { "not", "!" },
    { "<>", "!=" },
    { "neg", "-" },
};
PCSTR ToCPP(const string &name)
{
    return LookupStringValue(c_rgSCIStudio2CPP, ARRAYSIZE(c_rgSCIStudio2CPP), name, name.c_str());
}

// This is for a potential new language more accurately based on the original SCI syntax.
class OutputCPPSourceCode : public OutputSourceCodeBase
{
public:
    OutputCPPSourceCode(SourceCodeWriter &out) : OutputSourceCodeBase(out) {}

    void Visit(const Script &script) override
    {
        // C++
        for (const auto &include : script.GetIncludes())
        {
            // If this is a conversion from SCI Syntax, don't include the standard headers.
            if ((include != "game.sh") && (include != "keys.sh") && (include != "sci.sh"))
            {
                out.SyncComments(script);
                out.out << "include \"" << include << "\";" << out.NewLineString();
            }
        }
        NewLine(out);
        for (const auto &uses : script.GetUses())
        {
            out.SyncComments(script);
            out.out << "use \"" << uses << "\";" << out.NewLineString();
        }

        // c-style syntax has no script number.
        NewLine(out);

        Forward(script.GetSynonyms());
        out.out << endl;

        Forward(script.GetDefines());
        out.out << endl;

        Forward(script.GetScriptVariables());
        out.out << endl;

        Forward(script.GetScriptStringsDeclarations());
        out.out << endl;

        Forward(script.GetProcedures());
        Forward(script.GetClasses());
    }

    void Visit(const ClassDefinition &classDef) override
    {
        out.SyncComments(classDef);

        if (classDef.IsPublic())
        {
            out.out << "public ";
        }
        if (classDef.IsInstance())
        {
            out.out << "instance ";
        }
        else
        {
            out.out << "class ";
        }
        out.out << classDef.GetName();
        if (!classDef.GetSuperClass().empty())
        {
            out.out << " : " << classDef.GetSuperClass();
        }
        out.out << out.NewLineString() << "{";

        {
            DebugIndent indent(out);
            {
                Forward(classDef.GetProperties());
            }
            if (!classDef.GetProperties().empty() && !classDef.GetMethods().empty())
            {
                NewLine(out); // Separator between props and methods.
            }
            // Body (methods)
            Forward(classDef.GetMethods());
        }
        NewLine(out);
        out.out << "}" << out.NewLineString();
    }

    void Visit(const FunctionParameter &param) override {}
    void Visit(const FunctionSignature &sig) override {}

    void _VisitPropertyValue(const PropertyValueBase &prop)
    {
        out.SyncComments(prop);
        DebugLine line(out);
        switch (prop.GetType())
        {
        case ValueType::ResourceString:
            // This syntax is only available in LangSyntaxCpp
            out.out << "res ";
            break;
            // fall through to ValueType::String
        case ValueType::String:
            out.out << "\"" << UnescapeString(prop.GetStringValue()) << "\"";
            break;
        case ValueType::Said:
            out.out << "'" << prop.GetStringValue() << "'";
            break;
        case ValueType::Number:
            _OutputNumber(out, prop.GetNumberValue(), prop._fHex, prop._fNegate);
            break;
        case ValueType::Token:
            // Surround in braces if there are spaces in the string.
            // REVIEW: When this is C++ syntax, we should strip them out... and if it's -info-, use the replacement!
            // (objectSpecies)
            if (prop.GetStringValue().find(' ') != std::string::npos)
            {
                out.out << '{' << prop.GetStringValue() << '}';
            }
            else
            {
                out.out << prop.GetStringValue();
            }
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
        bool fHex = !!(define.GetFlags() & IFHex);
        bool fNegate = !!(define.GetFlags() & IFNegative);
        out.out << "define " << define.GetLabel() << " ";
        _OutputNumber(out, define.GetValue(), fHex, fNegate);
        out.out << ";" << out.NewLineString();
    }

    void Visit(const ClassProperty &classProp) override
    {
        out.SyncComments(classProp);
        // Don't output the property type, as its only used in some cases
        // (in an object that newly defines a property)
        out.out << classProp.GetName() << " = ";
        classProp.GetStatement1()->Accept(*this);
        out.out << ";";
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

    void _VisitFunctionBase(const FunctionBase &function, const std::string &procClass)
    {
        out.SyncComments(function);

        if (function.GetSignatures().empty())
        {
            out.out << "CORRUPT FUNCTION " << function.GetName() << out.NewLineString();
        }
        else
        {
            std::string returnType = function.GetSignatures()[0]->GetDataType();
            if (returnType.empty())
            {
                returnType = _DeduceReturnType(const_cast<sci::FunctionBase&>(function));
            }
            out.out << returnType << " " << function.GetName() << "(";
            const FunctionParameterVector &params = function.GetSignatures()[0]->GetParams();
            bool fFirst = true;
            for (const auto &param : params)
            {
                if (!fFirst)
                {
                    out.out << ", ";
                }
                std::string type = param->GetDataType();
                if (type.empty())
                {
                    // If none specified (e.g. conversion from SCI syntax), use "var"
                    type = "var";
                }
                out.out << type << " " << param->GetName();
                fFirst = false;
            }
            out.out << ")";

            // A little hack.
            out.SyncComments(function);
            if (!procClass.empty())
            {
                out.out << " : " << procClass;
            }

            // Body of function
            NewLine(out);
            {
                DebugIndent indent(out);
                out.out << "{";
                {
                    // Do something special with the variables.  Lay them out like:
                    // var foo, bar, thing;
                    // int x, yt;
                    Inline inln(out, true);
                    if (!function.GetVariables().empty())
                    {
                        out.out << out.NewLineString();
                        Indent(out);
                        std::string currentDataType;
                        bool fFirstInLine = true;
                        for (size_t i = 0; i < function.GetVariables().size(); ++i)
                        {
                            std::string dataType = function.GetVariables()[i]->GetDataType();
                            if (dataType.empty())
                            {
                                dataType = "var";
                            }
                            if (dataType != currentDataType)
                            {
                                if (i > 0)
                                {
                                    out.out << ";" << out.NewLineString(); // Next line...
                                    Indent(out);
                                }
                                out.out << dataType << " "; //... since we're starting a new type
                                currentDataType = dataType;
                                fFirstInLine = true;
                            }

                            if (!fFirstInLine)
                            {
                                out.out << ", "; // Separate subsequent variables with commas
                            }
                            out.out << function.GetVariables()[i]->GetName();
                            if (function.GetVariables()[i]->GetSize() > 1)
                            {
                                out.out << "[" << function.GetVariables()[i]->GetSize() << "]"; // array
                            }
                            fFirstInLine = false;
                        }
                        out.out << ";" << out.NewLineString();
                    }
                }
                Forward(function.GetCodeSegments());
            }
            NewLine(out);
            out.out << "}";
        }
    }

    void Visit(const MethodDefinition &function) override
    {
        out.SyncComments(function);
        NewLine(out);
        _VisitFunctionBase(function, "");
    }

    void Visit(const ProcedureDefinition &function) override
    {
        out.SyncComments(function);
        _VisitFunctionBase(function, function.GetClass());
    }

    void Visit(const Synonym &syn) override {}

    void Visit(const CodeBlock &block) override
    {
        out.SyncComments(block);
        /*
        if (_code.size() == 1)
        {
        // Don't bother with {}
        _code[0]->Accept(*this);
        }
        else
        {
        out.out << "{";
        {
        Inline inln(out, false);
        DebugIndent indent(out);
        NewLine(out);
        FwdDebugString(_code, out);
        }
        out.out << "}";
        }*/

        // REVIEW: I changed this for decompilation ... it may mess up script conversions.
        NewLine(out);
        out.out << "{";
        // Now the code, indented
        {
            DebugIndent indent(out);
            Forward(block.GetStatements());
        }
        NewLine(out);
        out.out << "}";
    }

    void Visit(const ConditionalExpression &conditional) override
    {
        out.SyncComments(conditional);
        SingleStatementVector::const_iterator stateIt = conditional.GetStatements().begin();
        bool fLastOld = out.fLast;
        out.fLast = true; // Treat all items like "last items" - we don't want spaces around them.
        ExplicitOrder order(out, conditional.GetStatements().size() > 1);// Enclose terms in () if there are more than one.
        while (stateIt != conditional.GetStatements().end())
        {
            (*stateIt)->Accept(*this);
            ++stateIt;
        }
        out.fLast = fLastOld;
    }

    void Visit(const Comment &comment) override
    {
        out.out << comment.GetName();
        EndStatement(out);
    }

    void Visit(const SendParam &sendParam) override
    {
        out.SyncComments(sendParam);
        out.out << sendParam.GetSelectorName();
        if (!sendParam.GetStatements().empty() || sendParam.IsMethod())
        {
            out.out << "(";
            Inline inln(out, true);
            Forward(sendParam.GetStatements(), ", ");
            out.out << ")";
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
        out.out << "rest " << rest.GetName();
    }

    void Visit(const Cast &cast) override {}

    void Visit(const SendCall &sendCall) override
    {
        out.SyncComments(sendCall);
        bool fFirst = true;
        for (auto &param : sendCall.GetParams())
        {
            if (!fFirst)
            {
                NewLine(out);
            }
            fFirst = false;
            if (!sendCall.GetTargetName().empty())
            {
                out.out << sendCall.GetTargetName();
            }
            else if (sendCall.GetStatement1() && (sendCall.GetStatement1()->GetType() != NodeTypeUnknown))
            {
                Inline inln(out, true);
                out.out << "(";
                sendCall.GetStatement1()->Accept(*this);
                out.out << ")";
            }
            else
            {
                Inline inln(out, true);
                sendCall._object3->Accept(*this);
            }
            out.out << ".";
            {
                Inline inln(out, true);
                param->Accept(*this);
            }
            EndStatement(out);
        }
    }

    void Visit(const ProcedureCall &procCall) override
    {
        out.SyncComments(procCall);
        out.out << procCall.GetName() << "(";
        {
            Inline inln(out, true);
            Forward(procCall.GetStatements(), ", ");
            out.out << ")";
        }
        EndStatement(out);
    }

    void Visit(const ReturnStatement &ret) override
    {
        out.SyncComments(ret);
        out.out << "return";
        {
            Inline inln(out, true);
            if (ret.GetStatement1() && (ret.GetStatement1()->GetType() != NodeTypeUnknown))
            {
                out.out << " ";
                ret.GetStatement1()->Accept(*this);
            }
        }
        EndStatement(out);
    }

    void Visit(const ForLoop &forLoop) override
    {
        out.SyncComments(forLoop);
        {
            out.out << "for (";
            Inline inln(out, true);
            forLoop.GetInitializer()->Accept(*this);
            out.out << "; ";
            forLoop.GetCondition()->Accept(*this);
            out.out << "; ";
            forLoop._looper->Accept(*this);
            out.out << ") ";
        }
        NewLine(out);
        out.out << "{";
        // Now the code
        {
            DebugIndent indent(out);
            Forward(forLoop.GetStatements());
        }
        NewLine(out);
        out.out << "}";
    }

    void Visit(const WhileLoop &whileLoop) override
    {
        out.SyncComments(whileLoop);
        out.out << "while (";
        {
            Inline inln(out, true);
            whileLoop.GetCondition()->Accept(*this);
        }
        out.out << ")";
        NewLine(out);
        out.out << "{";
        {
            DebugIndent indent(out);
            Forward(whileLoop.GetStatements());
        }
        NewLine(out);
        out.out << "}";
        NewLine(out);
    }

    void Visit(const DoLoop &doLoop) override
    {
        out.SyncComments(doLoop);
        out.out << "do";
        NewLine(out);
        out.out << "{";
        {
            DebugIndent indent(out);
            Forward(doLoop.GetStatements());
        }
        NewLine(out);
        out.out << "} while (";
        {
            Inline inln(out, true);
            doLoop.GetCondition()->Accept(*this);
        }
        out.out << ")";
        EndStatement(out);
    }

    void Visit(const BreakStatement &breakStatement) override
    {
        out.SyncComments(breakStatement);
        out.out << "break";
        EndStatement(out);
    }

    void Visit(const CaseStatement &caseStatement) override
    {
        out.SyncComments(caseStatement);
        {
            NewLine(out);
            if (caseStatement.IsDefault())
            {
                out.out << "default:";
            }
            else
            {
                out.out << "case ";
                Inline inln(out, true);
                caseStatement.GetStatement1()->Accept(*this);
                out.out << ":";
            }
        }
        // Now the code, indented.
        {
            DebugIndent indent(out);
            Forward(caseStatement.GetStatements());
        }

    }

    void Visit(const SwitchStatement &switchStatement) override
    {
        out.SyncComments(switchStatement);
        {
            out.out << "switch (";
            Inline inln(out, true);
            switchStatement.GetStatement1()->Accept(*this);
            out.out << ")";
        }
        {
            NewLine(out);
            out.out << "{";
        }
        // Now the cases.
        {
            Forward(switchStatement._cases);
        }
        {
            NewLine(out);
            out.out << "}";
        }
    }

    void Visit(const Assignment &assignment) override
    {
        {
            Inline inln(out, true);
            OutputBrackets brackets(out);
            assignment._variable->Accept(*this);
            out.out << " " << assignment.GetAssignmentOp() << " ";
            assignment.GetStatement1()->Accept(*this);
        }
        EndStatement(out);
    }

    void Visit(const BinaryOp &binaryOp) override 
    {
        out.SyncComments(binaryOp);
        {
            Inline inln(out, true);
            OutputBrackets brackets(out);
            {
                ExplicitOrder order(out, true);
                binaryOp.GetStatement1()->Accept(*this);
                out.out << " " << ToCPP(binaryOp.GetOpName()) << " ";
                binaryOp.GetStatement2()->Accept(*this);
            }
        }
        EndStatement(out);
    }

    void Visit(const UnaryOp &unaryOp) override
    {
        out.SyncComments(unaryOp);
        {
            Inline inln(out, true);
            OutputBrackets brackets(out);
            std::string newOp = ToCPP(unaryOp.GetOpName());
            out.out << newOp;
            if (!IsNonAlphaOperator(newOp))
            {
                out.out << " ";
            }
            // Wrap the target in (), or else we could end up with something like
            // !x - 5
            // when we want
            // !(x - 5)
            out.out << "(";
            unaryOp.GetStatement1()->Accept(*this);
            out.out << ")";
        }
        EndStatement(out);
    }

    void Visit(const CppIfStatement &ifStatement) override
    {
        out.SyncComments(ifStatement);
        if (ifStatement._fTernary)
        {
            ifStatement.GetCondition()->Accept(*this);
            out.out << " ? ";
            ifStatement.GetStatement1()->Accept(*this);
            out.out << " : ";
            ifStatement.GetStatement2()->Accept(*this);
        }
        else
        {
            out.out << "if (";
            {
                Inline inln(out, true);
                ifStatement.GetCondition()->Accept(*this);
            }
            out.out << ")";
            // Now the code, indented
            {
                ifStatement.GetStatement1()->Accept(*this);
            }
            NewLine(out);

            if (ifStatement.GetStatement2() && ifStatement.GetStatement2()->GetSyntaxNode())
            {
                NewLine(out);
                out.out << "else";
                {
                    ifStatement.GetStatement2()->Accept(*this);
                }
                NewLine(out);
            }
        }
    }

    void Visit(const Asm &asmSection) override
    {
//        out.out << asmSection.GetInstructionName() << " " << asmSection._arguments;
        NewLine(out);
        EndStatement(out);
    }

    void Visit(const AsmBlock &asmSection) override
    {

    }

    void Visit(const ExportEntry &exportEntry) override
    {

    }

};

void OutputSourceCode_CPP(const sci::Script &script, sci::SourceCodeWriter &out)
{
    OutputCPPSourceCode output(out);
    output.Visit(script);
}
void OutputSourceCode_CPP(const sci::ClassDefinition &classDef, sci::SourceCodeWriter &out)
{
    OutputCPPSourceCode output(out);
    output.Visit(classDef);
}
void OutputSourceCode_CPP(const sci::MethodDefinition &classDef, sci::SourceCodeWriter &out)
{
    OutputCPPSourceCode output(out);
    output.Visit(classDef);
}