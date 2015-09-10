#pragma once

namespace sci
{
    //
    // Various things that might be interesting to outsiders.
    //
    enum NodeType
    {
        NodeTypeUnknown,
        NodeTypeValue,
        NodeTypeComplexValue,           // 2
        NodeTypeAssignment,
        NodeTypeVariableDeclaration,
        NodeTypeDefine,
        NodeTypeClassProperty,
        NodeTypeStatement,
        NodeTypeFunction,
        NodeTypeClassDefinition,
        NodeTypeSynonym,                // 10
        NodeTypeCodeBlock,
        NodeTypeSendParam,
        NodeTypeLValue,
        NodeTypeSendCall,
        NodeTypeProcedureCall,          // 15
        NodeTypeConditionalExpression,
        NodeTypeCase,
        NodeTypeSwitch,
        NodeTypeAsm,
        NodeTypeAsmBlock,               // 20
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
}