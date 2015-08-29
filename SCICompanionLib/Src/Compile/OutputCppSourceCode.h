#pragma once

void OutputSourceCode_CPP(const sci::Script &script, sci::SourceCodeWriter &out);
void OutputSourceCode_CPP(const sci::ClassDefinition &classDef, sci::SourceCodeWriter &out);
void OutputSourceCode_CPP(const sci::MethodDefinition &classDef, sci::SourceCodeWriter &out);
void OutputSourceCode_CPP(const sci::ClassProperty &classDef, sci::SourceCodeWriter &out);