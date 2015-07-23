#pragma once

void OutputSourceCode_SCIStudio(const sci::Script &script, sci::SourceCodeWriter &out);
void OutputSourceCode_SCIStudio(const sci::ClassDefinition &classDef, sci::SourceCodeWriter &out);
void OutputSourceCode_SCIStudio(const sci::MethodDefinition &classDef, sci::SourceCodeWriter &out);