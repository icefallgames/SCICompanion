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
        NodeTypeCondClause,
        NodeTypeCond,
        NodeTypeBreakIf,
        NodeTypeRepeat,
    };
}