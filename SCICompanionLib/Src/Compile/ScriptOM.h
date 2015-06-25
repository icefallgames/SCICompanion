#pragma once

#include "CompileInterfaces.h"
#include "SCO.h"
#include "ScriptOMSmall.h"
#include "ScriptOMInterfaces.h"
class CompileContext;

//
//
// Contains object model for script files.
//
// Script
//      ClassDefinition[]
//          ClassProperty
//              PropertyValue
//          MethodDefinition
//              VariableDecl[]
//              SingleStatement[]
//      VariableDecl[]
//          VariableDecl
//          PropertyValue[]
//      ProcedureDefinition[]
//          VariableDecl[]
//          SingleStatement[]
//      Synonym[]
//      Define[]
//
// SingleStatement
//      CodeBlock
//          SingleStatement[]
//      SendCall
//          LValue/SingleStatement
//          SendParam[]
//      ProcedureCall
//          SingleStatement[]
//      SwitchStatement
//          SingleStatement[]
//          CaseStatement[]
//              SingleStatement
//              SingleStatement[]
//      ReturnStatement
//          SingleStatement
//      Assignment
//          LValue
//          SingleStatement
//      BinaryOp
//          SingleStatement
//          SingleStatement
//      UnaryOp
//          SingleStatement
//      ForLoop
//          SingleStatement
//          ConditionalExpression
//          SingleStatement
//          SingleStatement[]
//      WhileLoop
//          ConditionalExpression
//          SingleStatement
//      DoLoop
//          CodeBlock
//          SingleStatement - ???
//      BreakStatement
//      RestStatement
//          PropertyValue
//          
//      
// ConditionExpression
//      SingleStatement[]
//      
//      

class IOutputByteCode
{
public:
    // Generate the byte code.
    virtual CodeResult OutputByteCode(CompileContext &context) const { return 0; }
    // A prescan, used generally for "preprocessor" substitutions, and adding items to the symbol table
    virtual void PreScan(CompileContext &context) {}
    // Evaluate an expression at compile-time. Returns true if it can be evaluated.
    virtual bool Evaluate(CompileContext &context, uint16_t &result) { result = 0; return false; }
    virtual ~IOutputByteCode() {}
};

namespace sci
{
    // fwd decl
    class Script;
    class ClassDefinition;
    class FunctionParameter;
    class FunctionSignature;
    class PropertyValue;
    class ComplexPropertyValue;
    class Define;
    class ClassProperty;
    class SingleStatement;
    class VariableDecl;
    class FunctionParameter;
    class MethodDefinition;
    class ProcedureDefinition;
    class Synonym;
    class CodeBlock;
    class ConditionalExpression;
    class Comment;
    class SendParam;
    class LValue;
    class RestStatement;
    class Cast;
    class SendCall;
    class ProcedureCall;
    class ReturnStatement;
    class ForLoop;
    class WhileLoop;
    class DoLoop;
    class BreakStatement;
    class CaseStatement;
    class SwitchStatement;
    class Assignment;
    class BinaryOp;
    class UnaryOp;
    class CppIfStatement;
    class Asm;
    class AsmBlock;
    class ExportEntry;

    class ISyntaxNodeVisitor
    {
    public:
        virtual void Visit(const Script &script) = 0;
        virtual void Visit(const ClassDefinition &classDef) = 0;
        virtual void Visit(const FunctionParameter &param) = 0;
        virtual void Visit(const FunctionSignature &sig) = 0;
        virtual void Visit(const PropertyValue &prop) = 0;
        virtual void Visit(const ComplexPropertyValue &prop) = 0;
        virtual void Visit(const Define &define) = 0;
        virtual void Visit(const ClassProperty &classProp) = 0;
        virtual void Visit(const SingleStatement &statement) = 0;
        virtual void Visit(const VariableDecl &varDecl) = 0;
        virtual void Visit(const MethodDefinition &function) = 0;
        virtual void Visit(const ProcedureDefinition &function) = 0;
        virtual void Visit(const Synonym &syn) = 0;
        virtual void Visit(const CodeBlock &block) = 0;
        virtual void Visit(const ConditionalExpression &conditinoal) = 0;
        virtual void Visit(const Comment &comment) = 0;
        virtual void Visit(const SendParam &sendParam) = 0;
        virtual void Visit(const LValue &lValue) = 0;
        virtual void Visit(const RestStatement &rest) = 0;
        virtual void Visit(const Cast &cast) = 0;
        virtual void Visit(const SendCall &sendCall) = 0;
        virtual void Visit(const ProcedureCall &procCall) = 0;
        virtual void Visit(const ReturnStatement &ret) = 0;
        virtual void Visit(const ForLoop &forLoop) = 0;
        virtual void Visit(const WhileLoop &whileLoop) = 0;
        virtual void Visit(const DoLoop &doLoop) = 0;
        virtual void Visit(const BreakStatement &breakStatement) = 0;
        virtual void Visit(const CaseStatement &caseStatement) = 0;
        virtual void Visit(const SwitchStatement &switchStatement) = 0;
        virtual void Visit(const Assignment &assignment) = 0;
        virtual void Visit(const BinaryOp &binaryOp) = 0;
        virtual void Visit(const UnaryOp &unaryOp) = 0;
        virtual void Visit(const CppIfStatement &ifStatement) = 0;
        virtual void Visit(const Asm &asmSection) = 0;
        virtual void Visit(const AsmBlock &asmBlock) = 0;
        virtual void Visit(const ExportEntry &exportEntry) = 0;
    };

    class ScriptSite
    {
    public:
        const Script *GetOwnerScript() const { return _pScript; }
        Script *GetOwnerScript() { return _pScript; }
        void SetScript(Script *pScript) { _pScript = pScript; }
    protected:
        ScriptSite();
        ScriptSite(const ScriptSite &src) = delete;
        ScriptSite& operator=(const ScriptSite& src) = delete;
    private:
        Script *_pScript;
    };
	
    //
    // Various things that might be interesting to outsiders.
    //
    enum NodeType
    {
        NodeTypeUnknown,
        NodeTypeValue,
        NodeTypeComplexValue,
        NodeTypeAssignment,
        NodeTypeVariableDeclaration,
        NodeTypeDefine,
        NodeTypeClassProperty,
        NodeTypeStatement,
        NodeTypeFunction,
        NodeTypeClassDefinition,
        NodeTypeSynonym,
        NodeTypeCodeBlock,
        NodeTypeSendParam,
        NodeTypeLValue,
        NodeTypeSendCall,
        NodeTypeProcedureCall,
        NodeTypeConditionalExpression,
        NodeTypeCase,
        NodeTypeSwitch,
        NodeTypeAsm,
        NodeTypeAsmBlock,
        NodeTypeReturn,
        NodeTypeBinaryOperation,
        NodeTypeUnaryOperation,
        NodeTypeForLoop,
        NodeTypeWhileLoop,
        NodeTypeDoLoop,
        NodeTypeBreak,
        NodeTypeRest,
        NodeTypeScript,
        NodeTypeComment,
        NodeTypeFunctionParameter,
        NodeTypeFunctionSignature,
        NodeTypeCppIf,
        NodeTypeCast,
        NodeTypeConditionalExpressionSCIStudio,
        NodeTypeExport,
    };

    class Comment;
    class SyntaxNode;
    class Script;
    struct SourceCodeWriter;
    class CommentTracker
    {
    public:
        CommentTracker(Script &script);
        void Sync(const sci::SyntaxNode *pNode, SourceCodeWriter &out);
    private:
        size_t _commentIndex;
        const std::vector<std::unique_ptr<Comment>> &_comments;
    };

	class VariableUsageTracker
	{
	public:
		void DeclareUsage(const std::string &variableName, bool isIndexed = false)
		{
			OutputDebugString(variableName.c_str());

			// If the variable is not indexed, it's not part of an array.
			// If a variable is indexed, it is.
			// An indexed variable goes all the way up to the next mentioned variable.
		}

	private:
		// I guess each VUT can also contain the script local vars too

	};


    //
    // Used for generating a text representation of a script from its SyntaxNodes
	// I also use it to track variable indexing usage so we know what's an array and what's not.
    //
    struct SourceCodeWriter
    {
        SourceCodeWriter(std::stringstream &ss, LangSyntax syntax, Script *pScript = nullptr) : out(ss), lang(syntax),
            iIndent(0), fInline(false), fLast(false), fUseBrackets(false), fExpandCodeBlock(false), pszNewLine("\n"), lastNewLineLength(0), disallowedTokens(nullptr)
        { 
			if (pScript)
			{
				pComments = std::make_unique<CommentTracker>(*pScript);
			}
		}
        void SyncComments(const sci::SyntaxNode *pNode)
        {
            if (pComments)
            {
                pComments->Sync(pNode, *this);
            }
        }
        void SyncComments(const sci::SyntaxNode &node)
        {
            if (pComments)
            {
                pComments->Sync(&node, *this);
            }
        }
        const char *NewLineString() const
        {
            return fInline ? "" : pszNewLine;
        }
        void EnsureNewLine()
        {
            if (!fInline)
            {
                if (out.tellp() > lastNewLineLength)
                {
                    out << pszNewLine;
                    lastNewLineLength = out.tellp();
                }
                // Otherwise we had just added a line.
            }
        }
        std::stringstream::pos_type lastNewLineLength;
        LangSyntax lang;
        std::stringstream &out;
        int iIndent;
        bool fInline;
        bool fLast;
        bool fUseBrackets;
		bool fExpandCodeBlock;	// Don't treat codeblock as an inline thing surrounded by (). Instead expand it multi-line
        std::unique_ptr<CommentTracker> pComments;
        std::unordered_set<std::string> *disallowedTokens;

        const char *pszNewLine;
    };

    class SyntaxNode;

    class IExploreNodeContext
    {
    };

	enum class ExploreNodeState { Pre, Post };

    class IExploreNode
    {
    public:
		virtual void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) = 0;
    };

	class ExploreNodeBlock
	{
	public:
		ExploreNodeBlock(IExploreNode &en, IExploreNodeContext *pContext, SyntaxNode &node) :
			_en(en), _pContext(pContext), _node(node)
		{
			_en.ExploreNode(_pContext, _node, ExploreNodeState::Pre);
		}

		~ExploreNodeBlock()
		{
			_en.ExploreNode(_pContext, _node, ExploreNodeState::Post);
		}
	private:
		IExploreNode &_en;
		IExploreNodeContext *_pContext;
		SyntaxNode &_node;
	};

    //
    // Base class for all script objects
    //
    class SyntaxNode : public ISourceCodePosition, public IOutputByteCode
    {
    public:
        SyntaxNode() {}

        virtual ~SyntaxNode() {}
        virtual NodeType GetNodeType() const = 0;

        // A simple string describing the node, for error reporting.
        virtual std::string ToString() const { return ""; }

        virtual void Traverse(IExploreNodeContext *pContext, IExploreNode &en) {}

        // Determines whether or not one syntax node is equal to another.
        // Used for comparing two generated trees to make sure they are the same.
        virtual bool Equal(const SyntaxNode &right) const { return false; }

        // Visitor pattern for double-dispatch
        virtual void Accept(ISyntaxNodeVisitor &visitor) const = 0;
    };

    //
    // This allows syntax nodes to support both the virtual GetNodeType method, and have a
    // static MyNodeType member.  Thus, give a class name e.g. SendParam, we can verify that
    // a particular SyntaxNode is of that side
    //
#define DECLARE_NODE_TYPE(type)\
    public: \
    const static NodeType MyNodeType = type; \
    virtual NodeType GetNodeType() const { return type; }

    template <typename _T>
    const _T *SafeSyntaxNode(const SyntaxNode *pNode)
    {
        if (pNode->GetNodeType() == _T::MyNodeType)
        {
            return static_cast<const _T *>(pNode);
        }
        else
        {
            return nullptr;
        }
    }

    template <typename _T>
    _T *SafeSyntaxNode(SyntaxNode *pNode)
    {
        if (pNode->GetNodeType() == _T::MyNodeType)
        {
            return static_cast<_T *>(pNode);
        }
        else
        {
            return nullptr;
        }
    }


    //
    // Composition class used for script objects that have names.
    //
    class NamedNode
    {
    public:
        NamedNode(const std::string &name) : _innerName(name) {}
        NamedNode() {}
        void SetName(const std::string &name) { _innerName = name; }
        std::string GetName() const { return _innerName; }
    protected:
        std::string _innerName;
    };

    //
    // Composition class used for script objects that have an explicit data type.
    //
    class TypedNode
    {
    public:
        void SetDataType(const std::string &type) { ASSERT(_innerType.empty()); _innerType = type; }
        const std::string &GetDataType() const { return _innerType; }
    protected:
        std::string _innerType;
    };
	
    class SingleStatement; // fwd decl

    class PropertyValueBase : public SyntaxNode
    {
    public:
        PropertyValueBase(const PropertyValueBase& src);
        PropertyValueBase& operator=(const PropertyValueBase& src);
        bool operator==(const PropertyValueBase& value2) const;
        bool operator!=(const PropertyValueBase& value);
        std::string ToString() const;
        void SetValue(WORD wValue, bool fHexIn = false)
        {
            _numberValue = wValue; _type = sci::ValueType::Number; _fHex = fHexIn; _fNegate = false; _stringValue.clear();
        } // Used for compiled scripts
        void SetValue(int iValue, IntegerFlags flags);
        void SetStringValue(const std::string &value) { assert(value != "rest"); _stringValue = value; _type = sci::ValueType::String;  _fNegate = false; }
        void SetValue(const std::string &value, ValueType type)
        {
            assert(value != "rest");
            _stringValue = value; _type = type; _fNegate = false;
        }
        WORD GetNumberValue() const { return _numberValue; }
        std::string GetStringValue() const { return _stringValue; }
        bool IsEmpty() { return (_type == sci::ValueType::None); }
        void Negate() { _fNegate = true; _fHex = false; }
        ValueType GetType() const { return _type; }
        void SetType(ValueType type) { assert(_type == sci::ValueType::None); _type = type; }
        void Zero() { _type = sci::ValueType::Number; _numberValue = 0; _fHex = false; _fNegate = false; _stringValue.clear(); }
        bool IsZero() const { return (_type == sci::ValueType::Number) && (_numberValue == 0); }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const override;
        void PreScan(CompileContext &context) override;
        bool Evaluate(CompileContext &context, uint16_t &result) override;

        bool _fHex;     // Indicates that _numberValue was expressed in hex in the script.
        bool _fNegate;  // Indicates that _numberValue was the result of a negation.  _numberValue doesn't change.

        
    protected:
        virtual SingleStatement *GetIndexer() const { return nullptr; }
        // No one should construct this directly.
        PropertyValueBase()
        {
            _type = ValueType::None; _numberValue = 0; _fHex = false; _fNegate = false; 
        }

    private:
        ValueType _type;      // Can be of 3 types
        WORD _numberValue;
        std::string _stringValue;
    };

    class PropertyValue : public PropertyValueBase
    {
        DECLARE_NODE_TYPE(NodeTypeValue)
    public:
        PropertyValue() : PropertyValueBase() {}
        PropertyValue(const std::string &label, ValueType type) : PropertyValueBase()
        {
            SetValue(label, type);
        }
        PropertyValue(uint16_t number) : PropertyValueBase()
        {
            SetValue(number);
        }

		void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        void Accept(ISyntaxNodeVisitor &visitor) const override;
	};

    // PropertyValues are always passed by value
    typedef std::vector<PropertyValue> PropertyValueVector;

    //
    // Different from a regular property value, because it can have an array indexer.
    //
    class ComplexPropertyValue : public PropertyValueBase
    {
        DECLARE_NODE_TYPE(NodeTypeComplexValue)
    public:
		ComplexPropertyValue() : PropertyValueBase() {}
        ComplexPropertyValue(ComplexPropertyValue& src);
        ComplexPropertyValue& operator=(ComplexPropertyValue& src);
        void SetIndexer(SingleStatement *pIndexer) { _pArrayInternal.reset(pIndexer); }
		void SetIndexer(std::unique_ptr<SingleStatement> pIndexer) { _pArrayInternal = std::move(pIndexer); }
        virtual SingleStatement *GetIndexer() const { return _pArrayInternal.get(); }
        std::unique_ptr<SingleStatement> StealIndexer() { return move(_pArrayInternal); }
        
        bool Evaluate(CompileContext &context, uint16_t &result) override;

        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        void Accept(ISyntaxNodeVisitor &visitor) const override;
    private:
        std::unique_ptr<SingleStatement> _pArrayInternal;
    };

    class Define : public SyntaxNode, public ScriptSite
    {
        DECLARE_NODE_TYPE(NodeTypeDefine)
    public:
        const std::string &GetLabel() const { return _label; }
        const std::string &GetName() const { return _label; }
        uint16_t GetValue() const { ASSERT(_strValue.empty()); return _wValue; }
        bool Match(const std::string &label) { return label == _label; }

        IntegerFlags GetFlags() const { return _flags; }
        void SetLabel(const std::string &label) { _label = label; }
        void SetValue(uint16_t w) { _wValue = w; }
        void SetValue(int iNumber, IntegerFlags flags) { ASSERT(iNumber <= 0xFFFF); _wValue = static_cast<uint16_t>(iNumber); _flags = flags; }
        void SetValue(const std::string &value) { _strValue = value; };
        

        // IOutputByteCode
        void PreScan(CompileContext &context);

        void Accept(ISyntaxNodeVisitor &visitor) const override;
    private:
        std::string _label;

        // Either _strValue is non-empty, or _wValue is valid.
        uint16_t _wValue;
        IntegerFlags _flags;
        std::string _strValue;
    };

    typedef Define* DefinePtr;
    typedef std::vector<std::unique_ptr<Define>> DefineVector;

} // namespace sci


namespace sci
{
    //
    // Code segments
    //
    class SingleStatement : public SyntaxNode
    {
        DECLARE_NODE_TYPE(NodeTypeStatement)
    public:
        SingleStatement() { _type = NodeTypeUnknown; }

        SingleStatement(const SingleStatement &src) = delete;
		SingleStatement& operator=(const SingleStatement& src) = delete;

        const SyntaxNode *GetSegment() const { return _pThing.get(); }
        SyntaxNode *GetSegment() { return _pThing.get(); }
        NodeType GetType() const { return _type; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const override;
        void PreScan(CompileContext &context) override;
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en) override;
        bool Evaluate(CompileContext &context, uint16_t &result) override;

        template<typename _statementT>
        const typename _statementT *CastSyntaxNode() const
        {
            if (_statementT::MyNodeType == _type)
            {
                return static_cast<const _statementT*>(_pThing.get());
            }
            else
            {
                return nullptr;
            }
        }
        template<typename _statementT>
        typename _statementT *CastSyntaxNode()
        {
            if (_statementT::MyNodeType == _type)
            {
                return static_cast<_statementT*>(_pThing.get());
            }
            else
            {
                return nullptr;
            }
        }

        void SetSyntaxNode(std::unique_ptr<SyntaxNode> pThing)
		{
			_pThing = std::move(pThing);
			_type = _pThing->GetNodeType();
		}
        const SyntaxNode *GetSyntaxNode() const { return _pThing.get(); }
        SyntaxNode *GetSyntaxNode() { return _pThing.get(); }
        SyntaxNode *ReleaseSyntaxNode() { return _pThing.release(); }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    private:
        bool _ContainsStatement(NodeType) const;
        std::unique_ptr<SyntaxNode> _pThing;
        NodeType _type;
    };

    typedef SingleStatement* SingleStatementPtr;
    typedef std::vector<std::unique_ptr<SingleStatement>> SingleStatementVector;
	typedef std::vector<SingleStatementPtr> RawSingleStatementVector;

    //
    // Composition class for nodes that have a series of statements
    // These help objects adhere to a generic "API" so that template functions that
    //  construct these objects can be used in parsing.
    //
    class StatementsNode
    {
    public:
		StatementsNode() {} 
		StatementsNode(const StatementsNode &src) = delete;
		StatementsNode& operator=(StatementsNode& src) = delete;

		void AddStatement(std::unique_ptr<SingleStatement> pStatement) { _segments.push_back(std::move(pStatement)); }
        const SingleStatementVector &GetStatements() const { return _segments; }
        SingleStatementVector &GetStatements() { return _segments; }
    protected:
		SingleStatementVector _segments;
	};

    //
    // Composition class for nodes that have an statement.
    //
    class OneStatementNode
    {
    public:
		OneStatementNode() {}
		OneStatementNode(const OneStatementNode &src) = delete;
		OneStatementNode& operator=(OneStatementNode& src) = delete;

		void SetStatement1(std::unique_ptr<SingleStatement> statement) { _statement1 = std::move(statement); }
        const SingleStatement* GetStatement1() const { return _statement1.get(); }
        SingleStatement* GetStatement1() { return _statement1.get(); }

        std::unique_ptr<SingleStatement> &GetStatement1Internal() { return _statement1; }
    protected:
        std::unique_ptr<SingleStatement> _statement1;
    };

    //
    // Composition class for nodes that have a second statement.
    //
    class TwoStatementNode
    {
    public:
		TwoStatementNode() {}
		TwoStatementNode(const TwoStatementNode &src) = delete;
		TwoStatementNode& operator=(TwoStatementNode& src) = delete;

		void SetStatement2(std::unique_ptr<SingleStatement> statement) { _statement2 = std::move(statement); }
        const SingleStatement* GetStatement2() const { return _statement2.get(); }
        SingleStatement* GetStatement2() { return _statement2.get(); }

        std::unique_ptr<SingleStatement> &GetStatement2Internal() { return _statement2; }
    protected:
        std::unique_ptr<SingleStatement> _statement2;
    };

    // A class or instance property declaration and value. The value can be complex in version 2 of the syntax.
    class ClassProperty : public SyntaxNode, public NamedNode, public TypedNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeClassProperty)
    public:
        ClassProperty(const ClassProperty &cp) = default;
        ClassProperty& operator=(const ClassProperty& src) = default;

        ClassProperty() : NamedNode(), TypedNode() {};
        ClassProperty(const std::string &str, WORD wValue);
        ClassProperty(const std::string &str, const std::string &value);
        ClassProperty(const std::string &str, const PropertyValue &value);
        //const PropertyValue GetValue() const;
        std::string GetValueUnimplemented() const { return "Unimplemented"; }
        const PropertyValue *TryGetValue() const;
        void SetValue(const PropertyValue &value);

        // IOutputByteCode
        void PreScan(CompileContext &context);

        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;
    };

    typedef std::vector<std::unique_ptr<ClassProperty>> ClassPropertyVector;
    typedef std::vector<ClassProperty*> RawClassPropertyVector;

    //
    // Represents a variable decl that could be an array (includes name and size)
    //
    class VariableDecl : public SyntaxNode, public StatementsNode, public TypedNode, public ScriptSite
    {
        DECLARE_NODE_TYPE(NodeTypeVariableDeclaration)
    public:
        VariableDecl();
        VariableDecl(const VariableDecl &src) = delete;
		VariableDecl& operator=(const VariableDecl& src) = delete;

        const std::string &GetName() const { return _name; }
        uint16_t GetSize() const { return _wSize; }
        const SingleStatementVector &GetInitializers() const { return _segments; }

        void AddSimpleInitializer(const PropertyValue &value);
        std::vector<uint16_t> GetSimpleValues() const;

        void SetName(const std::string &name) { _name = name; }
        void SetSize(uint16_t wSize) { _wSize = wSize; _unspecifiedSize = false; }
        void SetIsUnspecifiedSize(bool unspecified) { _unspecifiedSize = unspecified; }
        bool IsUnspecifiedSize() { return _unspecifiedSize; }
        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const override;
        void PreScan(CompileContext &context) override;
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en) override;
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    private:
        std::string _name;
        uint16_t _wSize;
        bool _unspecifiedSize;  // For script string declarations
    };

    typedef VariableDecl* VariableDeclPtr;
    typedef std::vector<std::unique_ptr<VariableDecl>> VariableDeclVector;

    class ClassDefinition; // fwd decl

    class FunctionParameter : public SyntaxNode, public NamedNode, public TypedNode
    {
        DECLARE_NODE_TYPE(NodeTypeFunctionParameter)
    public:
        FunctionParameter();
        FunctionParameter(const std::string &str);

        FunctionParameter(FunctionParameter &src) = delete;
        FunctionParameter& operator=(FunctionParameter& src) = delete;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        // IOutputByteCode
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en) override;
    };
    typedef FunctionParameter* FunctionParameterPtr;
    typedef std::vector<std::unique_ptr<FunctionParameter>> FunctionParameterVector;

    class FunctionSignature : public SyntaxNode, public ScriptSite, public IVariableLookupContext, public TypedNode
    {
        DECLARE_NODE_TYPE(NodeTypeFunctionSignature)
    public:
        FunctionSignature();
        FunctionSignature(FunctionSignature &src) = delete;
        FunctionSignature& operator=(FunctionSignature& src) = delete;

        const FunctionParameterVector &GetParams() const { return _params; }
        FunctionParameterVector &GetParams() { return _params; }
        void AddParam(const std::string &name);
        void AddParam(std::unique_ptr<FunctionParameter> pParam, bool fOptional);
        void SetMoreParametersAllowed(bool fOpt) { _fMoreParameters = fOpt; }
        bool GetMoreParametersAllowed() const { return _fMoreParameters; }
        size_t GetRequiredParameterCount() const { return (_iOptional == NoOptional) ? _params.size() : _iOptional; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const  override { assert(false); return CodeResult(); }
        void PreScan(CompileContext &context) override;
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en) override;

        // IVariableLookupContext
        ResolvedToken LookupVariableName(CompileContext &context, const std::string &str, WORD &wIndex, SpeciesIndex &dataType) const;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    private:
        static const size_t NoOptional = ((size_t)-1);
        FunctionParameterVector _params;
        bool _fMoreParameters;              // Does this have more parameters? (relevant to CPP only)
        size_t _iOptional;                  // Index at which the parameters become optional
    };
    typedef FunctionSignature* FunctionSignaturePtr;
    typedef std::vector<FunctionSignaturePtr> RawFunctionSignatureVector;
	typedef std::vector<std::unique_ptr<FunctionSignature>> FunctionSignatureVector;

    class FunctionBase : public SyntaxNode, public ScriptSite, public IVariableLookupContext, public NamedNode, public StatementsNode
    {
        DECLARE_NODE_TYPE(NodeTypeFunction)
    public:
        FunctionBase();
        FunctionBase(const std::string &str);

		FunctionBase(const FunctionBase &src) = delete;
		FunctionBase& operator=(const FunctionBase& src) = delete;

        const FunctionSignatureVector &GetSignatures() const { return _signatures; }
        FunctionSignatureVector &GetSignaturesNC() { return _signatures; }
		void AddSignature(std::unique_ptr<FunctionSignature> pSig) { _signatures.push_back(std::move(pSig)); }
		void AddVariable(std::unique_ptr<VariableDecl> pVar, PropertyValue value);
        void AddVariable(std::unique_ptr<VariableDecl> pVar) { _tempVars.push_back(std::move(pVar)); }
        std::string ToString() const;
        const VariableDeclVector &GetVariables() const { return _tempVars; }
        const ClassDefinition *GetOwnerClass() const { return _pOwnerClass; }
        void SetOwnerClass(const ClassDefinition *pOwnerClass) { _pOwnerClass = pOwnerClass; }
        const SingleStatementVector &GetCodeSegments() const { return _segments; }

        void AddParam(const std::string &param);

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        // IVariableLookupContext
        ResolvedToken LookupVariableName(CompileContext &context, const std::string &str, WORD &wIndex, SpeciesIndex &dataType) const;
        

        void PruneExtraneousReturn();

    private:
        FunctionSignatureVector _signatures;
        VariableDeclVector _tempVars;           // Temp variables
        const ClassDefinition *_pOwnerClass;    // Our owner class (weak ref)

        // Documentation
        std::string _docDescription;            // Description of the function
    };

    typedef FunctionBase* FunctionPtr;
    typedef std::vector<FunctionPtr> FunctionVector;

    class MethodDefinition : public FunctionBase
    {
    public:
        MethodDefinition();
        MethodDefinition(MethodDefinition &src) = delete;
        MethodDefinition& operator=(MethodDefinition& src) = delete;

        void SetPrivate(bool f) { _fPrivate = f; }
        bool SetPrivate() const { return _fPrivate; }
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    private:
        bool _fPrivate;
    };

    typedef MethodDefinition* MethodPtr;
    typedef std::vector<MethodPtr> RawMethodVector;
	typedef std::vector<std::unique_ptr<MethodDefinition>> MethodVector;
	
    class ClassDefinition; // fwd decl
    class ProcedureDefinition : public FunctionBase
    {
    public:
        ProcedureDefinition();

        ProcedureDefinition(ProcedureDefinition &src) = delete;
        ProcedureDefinition& operator=(ProcedureDefinition& src) = delete;

        bool IsPublic() const { return _public; }
        void SetPublic(bool f) { _public = f; }
        const std::string &GetClass() const { return _class; }
        void SetClass(const std::string className) { _class = className; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        bool _public;
        std::string _class;                 // for class procedures.
    };

    typedef ProcedureDefinition* ProcedurePtr;
    typedef std::vector<std::unique_ptr<ProcedureDefinition>> ProcedureVector;
	typedef std::vector<ProcedureDefinition*> RawProcedureVector;

    struct SelectorValuePair
    {
        WORD wSelector;
        WORD wValue;
    };

    class ClassDefinition : public SyntaxNode, public ScriptSite, public ISCIPropertyBag, public IVariableLookupContext, public NamedNode
    {
        DECLARE_NODE_TYPE(NodeTypeClassDefinition)
    public:
        ClassDefinition();
		// Disable copy/assignment, since other objects hold weak references to us.
		ClassDefinition(ClassDefinition &src) = delete;
		ClassDefinition& operator=(ClassDefinition& src) = delete;

        void CreateNew(const Script *pScript, PCTSTR pszName, PCTSTR pszSpecies);

        // Methods out of which to extract data.
        std::string GetSuperClass() const { return _superClass; }
        bool IsInstance() const { return _fInstance; }
        bool IsPublic() const { return _fPublic; }
        const ClassPropertyVector &GetProperties() const { return _properties; }
        ClassPropertyVector &GetPropertiesNC() { return _properties; }
        const MethodVector &GetMethods() const { return _methods; }
        MethodVector &GetMethodsNC() { return _methods; }

        bool GetPropertyConst(PCTSTR pszName, PropertyValue &value) const;

        // ISCIPropertyBag
        bool SetProperty(PCTSTR pszName, PropertyValue value);
        bool GetProperty(PCTSTR pszName, PropertyValue &value);
        void SetBagName(PCTSTR pszName) { _innerName = pszName; }
		const std::string GetBagName() { return _innerName; }
        void SetSpecies(PCTSTR pszName) { _superClass = pszName; }
        const std::string GetSpecies() const { return _superClass; }
        const ClassPropertyVector &GetProperties() { return _properties; }

        void SetSuperClass(const std::string &superClass) { _superClass = superClass; }
        void SetPublic(bool fPublic) { _fPublic = fPublic; }
        void SetInstance(bool fInstance) { _fInstance = fInstance; }
        void AddProperty(std::unique_ptr<ClassProperty> classProp) { _properties.push_back(move(classProp)); }
        void AddProperty(const std::string &name, uint16_t value);
		void AddMethod(std::unique_ptr<MethodDefinition> method) { _methods.push_back(std::move(method)); }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        // IVariableLookupContext
        virtual ResolvedToken LookupVariableName(CompileContext &context, const std::string &str, WORD &wIndex, SpeciesIndex &dataType) const;

        void OutputSourceCode(SourceCodeWriter &out) const;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    private:
        std::string _superClass;
        bool _fPublic;
        bool _fInstance; // Instance or class.
        ClassPropertyVector _properties;
        MethodVector _methods;
    };

    typedef ClassDefinition* ClassPtr;
	typedef std::vector<std::unique_ptr<ClassDefinition>> ClassVector;
	typedef std::vector<ClassDefinition*> RawClassVector;

    class Synonym : public SyntaxNode
    {
        DECLARE_NODE_TYPE(NodeTypeSynonym)
    public:
        std::string MainWord;
        std::string Replacement;

        void Accept(ISyntaxNodeVisitor &visitor) const override;
    };

    typedef Synonym* SynonymPtr;
    typedef std::vector<SynonymPtr> RawSynonymVector;
	typedef std::vector<std::unique_ptr<Synonym>> SynonymVector;



    //
    // Series of code segments, surrounded by ()
    //
    class CodeBlock : public SyntaxNode, public StatementsNode
    {
        DECLARE_NODE_TYPE(NodeTypeCodeBlock)
    public:
		CodeBlock() {}
		CodeBlock(const CodeBlock &src) = delete;
		CodeBlock& operator=(const CodeBlock& src) = delete;

		const SingleStatementVector &GetList() const { return _segments; }
        

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const override;
        void PreScan(CompileContext &context) override;
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en) override;
        bool Evaluate(CompileContext &context, uint16_t &result) override;

        template<typename _T>
        _T *ReduceBlock()
        {
            _T *pReduced = nullptr;
			if ((_segments.size() == 1) && (_segments[0]->CastSyntaxNode<_T>()))
            {
				pReduced = static_cast<_T*>(_segments[0]->ReleaseSyntaxNode());
            }
            return pReduced;
        }

        void Accept(ISyntaxNodeVisitor &visitor) const override;
    };

    //
    // Helper for nodes with code blocks
    //
    class CodeBlockNode
    {
    public:
        void SetCodeBlock(std::unique_ptr<CodeBlock> list) { _list = std::move(list); }
    protected:
		std::unique_ptr<CodeBlock> _list;
    };

    class ConditionalExpression : public SyntaxNode, public StatementsNode
    {
        DECLARE_NODE_TYPE(NodeTypeConditionalExpression)
    public:
        ConditionalExpression() {}
		ConditionalExpression(const ConditionalExpression &src) = delete;
		ConditionalExpression& operator=(const ConditionalExpression& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;
    };

    //
    // Helper class for things with conditional expressions
    //
    class ConditionNode
    {
    public:
        void SetCondition(std::unique_ptr<ConditionalExpression> cond) { _innerCondition = std::move(cond); }
        const std::unique_ptr<ConditionalExpression>& GetCondition() const { return _innerCondition; }
    protected:
        std::unique_ptr<ConditionalExpression> _innerCondition;
    };

    //
    // This represents a comment.
    // Currently, it can be used just as any other syntax node, OR
    // it can be added specially to the comment list for the script.
    //
    class Comment : public SyntaxNode, public NamedNode
    {
        DECLARE_NODE_TYPE(NodeTypeComment)
    public:
        Comment() : NamedNode() {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const { return CodeResult(); }
        void PreScan(CompileContext &context) {}
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en) {}
        
        std::string GetSanitizedText() const;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        bool IsMultiline() const;
    private:
        //std::string _comment;
    };
    typedef Comment* CommentPtr;
    typedef std::vector<std::unique_ptr<Comment>> CommentVector;
	typedef std::vector<CommentPtr> RawCommentVector;

    class ExportEntry : public SyntaxNode
    {
        DECLARE_NODE_TYPE(NodeTypeExport)
    public:
        ExportEntry() {}
        ExportEntry(int slot, const std::string &name) : Slot(slot), Name(name) {}
        ExportEntry(const ExportEntry &src) = delete;
        ExportEntry& operator=(const ExportEntry& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const { return CodeResult(); }
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en) {}
        void Accept(ISyntaxNodeVisitor &visitor) const override;

        std::string Name;
        int Slot;
    };

    typedef std::vector<std::unique_ptr<ExportEntry>> ExportEntryVector;
    
    //
    // This represents an entire script
    //
    class Script : public SyntaxNode, public IVariableLookupContext
    {
        DECLARE_NODE_TYPE(NodeTypeScript)
    public:
        Script(PCTSTR pszFilePath, PCTSTR pszFileName);
        Script(ScriptId script);
        Script();

        // Methods to retrieve information from a Loaded script:
        const ClassVector &GetClasses() const { return _classes; }
        const ProcedureVector &GetProcedures() const { return _procedures; }
        ClassVector &GetClassesNC() { return _classes; }
        ProcedureVector &GetProceduresNC() { return _procedures; }
        WORD GetScriptNumber() const { return _scriptId.GetResourceNumber(); }
        void SetScriptNumber(WORD wNumber);
        void SetScriptNumberDefine(const std::string &define) { _scriptDefine = define; }
        const std::string &GetScriptNumberDefine() const { return _scriptDefine; }
        std::string GetTitle() const { return _scriptId.GetTitle(); }
        std::string GetName() const { return _scriptId.GetFileName(); }
        std::string GetPath() const { return _scriptId.GetFullPath(); }
        const VariableDeclVector &GetScriptVariables() const { return _scriptVariables; }
        const VariableDeclVector &GetScriptStringsDeclarations() const { return _scriptStringDeclarations; }
        const DefineVector &GetDefines() const { return _defines; }
        const ExportEntryVector &GetExports() const { return _exports; }
        ExportEntryVector &GetExports() { return _exports; }
        const SynonymVector &GetSynonyms() const { return _synonyms; }
        const CommentVector &GetComments() const { return _comments; }
        const std::vector<std::string> &GetUses() const { return _uses; }
        const std::vector<std::string> &GetIncludes() const { return _includes; }

        // IVariableLookupContext
        ResolvedToken LookupVariableName(CompileContext &context, const std::string &str, WORD &wIndex, SpeciesIndex &dataType) const;

        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        void AddUse(const std::string &use) { _uses.push_back(use); }
        void AddInclude(const std::string &include) { _includes.push_back(include); }
		void AddSynonym(std::unique_ptr<Synonym> pSynonym) { _synonyms.push_back(std::move(pSynonym)); }
		void AddDefine(std::unique_ptr<Define> pDefine) { _defines.push_back(std::move(pDefine)); }
		void AddClass(std::unique_ptr<ClassDefinition> pClass) { _classes.push_back(std::move(pClass)); }
		void AddVariable(std::unique_ptr<VariableDecl> pVar) { _scriptVariables.push_back(std::move(pVar)); }
        void AddStringDeclaration(std::unique_ptr<VariableDecl> pVar) { _scriptStringDeclarations.push_back(std::move(pVar)); }
        void AddProcedure(std::unique_ptr<ProcedureDefinition> pProc) { _procedures.push_back(std::move(pProc)); }
		void AddComment(std::unique_ptr<Comment> pComment) { _comments.push_back(std::move(pComment)); }

        int SyntaxVersion;

        void SetScriptId(ScriptId scriptId) { _scriptId = scriptId; }
        LangSyntax Language() const { return _scriptId.Language(); }
        bool IsHeader() const { return _scriptId.IsHeader(); }

        void OutputSourceCode(SourceCodeWriter &out) const;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        ScriptId GetScriptId() const { return _scriptId; }

    private:
        void _PreScanStringDeclaration(CompileContext &context, VariableDecl &stringDecl);

        // Disable copy/assignment, since other classes hold weak references to us.
		Script(Script &src) = delete;
		Script& operator=(Script& src) = delete;

        // Script number
        std::string _scriptDefine; // This is used if _scriptId.GetResourceNumber() is InvalidResourceNumber, but it needs to use the classbrowser to resolve.

        std::vector<std::string> _uses;
        std::vector<std::string> _includes;
        VariableDeclVector _scriptVariables;
        VariableDeclVector _scriptStringDeclarations;
        ClassVector _classes;
        ProcedureVector _procedures;
        SynonymVector _synonyms;
        DefineVector _defines;
        ExportEntryVector _exports;

        // Since comments can be anywhere in the script, including the middle of statements,
        // we don't generally store comments as distinct nodes (though it is supported).
        // Instead, comments are added at the script level.  The position property of the comments
        // can be used to place it in the right spot.
        CommentVector _comments;

        // These are not serialized:
        ScriptId _scriptId;
        LangSyntax _language;
    };

}; // namespace sci	

//
// Generate a unique name for a method on an object
//
std::string GetMethodTrackingName(const sci::ClassDefinition *pClass, const sci::FunctionBase &method, bool makeValidFilename = false);

SpeciesIndex MatchParameters(const std::string &name,
                             CompileContext &context,
                             const ISourceCodePosition *pPos,
                             const std::vector<CSCOFunctionSignature> &signatures, 
                             const std::vector<SpeciesIndex> &parameterTypes, 
                             const sci::SingleStatementVector &parameterStatements);
