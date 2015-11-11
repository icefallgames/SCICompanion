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
#include "OutputRST.h"
#include "ClassBrowser.h"
#include "DocScript.h"
#include "ScriptOMAll.h"
#include "format.h"
#include <filesystem>
#include <regex>
#include "StringUtil.h"

using namespace std::tr2::sys;

// This uses reStructuredText's js domain, as it is most similar to SCI.

std::string scriptFilenameSuffix = ".sc";

std::string indent = "    ";

std::string ClassesFolder = "Classes";
std::string ScriptsFolder = "Scripts";
std::string ProceduresFolder = "Procedures";
std::string KernelsFolder = "Kernels";
std::string NoDocTag = "nodoc";
std::string rstFunction = ".. function::";

// Adds an entry to generated files, returns the absolute path to the generated file.
std::string OutputRSTHelper(const std::string &rstFolder, const std::string &subFolder, const std::string &title, std::vector<std::string> &generatedFiles)
{
    std::string scriptsFolderPath = subFolder.empty() ? rstFolder : (rstFolder + "\\" + subFolder);
    std::string relativeScriptsPath = subFolder.empty() ? title : (subFolder + "\\" + title);
    generatedFiles.push_back(relativeScriptsPath);
    std::string fullScriptsPath = rstFolder + "\\" + relativeScriptsPath + ".rst";
    EnsureFolderExists(scriptsFolderPath, false);

    return fullScriptsPath;
}

void OutputPreamble(fmt::MemoryWriter &w, const std::string &title, const std::string &title2)
{
    w << ".. " << title << "\n\n";
    w << ".. default - domain::js\n\n"
        ".. include:: /includes/standard.rst\n\n";

    // Title
    std::string titleBar(title2.length(), '=');
    w << titleBar << "\n";
    w << title2 << "\n";
    w << titleBar << "\n\n";
}

std::string MarkFunctionAsNoIndex(const std::string &comment, bool skipFirst)
{
    std::string result;
    size_t offset = 0;
    size_t pos;
    bool first = true;
    while ((pos = comment.find(rstFunction, offset)) != std::string::npos)
    {
        // Advance to the next line
        pos = comment.find('\n', pos);
        if (pos != std::string::npos)
        {
            pos++; // Advance to after newline.
        }
        else
        {
            pos = comment.length();
        }
        std::copy(comment.begin() + offset, comment.begin() + pos, std::back_inserter(result));
        // Now add in our :noindex:
        if (!skipFirst || !first)
        {
            result += "\t:noindex:\n";
        }
        offset = pos;
        first = false;
    }
    // Copy the remainder:
    std::copy(comment.begin() + offset, comment.end(), std::back_inserter(result));
    return result;
}

bool iStartsWith(const std::string& a, const std::string& prefix)
{
    if (prefix.length() > a.length())
    {
        return false;
    }

    for (size_t i = 0; i < prefix.length(); i++)
    {
        if (toupper(a[i]) != (toupper(prefix[i])))
        {
            return false;
        }
    }
    return true;
}

bool ShouldDocumentScript(DocScript &script)
{
    return !iStartsWith(script.GetComment(script.GetScript()), NoDocTag);
}
bool ShouldDocument(DocScript &script, const sci::SyntaxNode *node)
{
    return !iStartsWith(script.GetComment(node), NoDocTag);
}

void OutputPropertyTableRSTWorker(fmt::MemoryWriter &w, std::vector<std::pair<std::string, std::string>> properties, const std::string &title)
{
    std::string propertyHeader = "Property";
    std::string descriptionHeader = "Description";
    size_t maxPropLength = propertyHeader.length();
    size_t maxDescriptionLength = descriptionHeader.length();
    for (auto &property : properties)
    {
        maxPropLength = max(maxPropLength, property.first.length());
        maxDescriptionLength = max(maxDescriptionLength, property.second.length());
    }

    size_t maxTotalLength = maxPropLength + maxDescriptionLength + 1;

    w << title << ":\n\n";

    std::string propertyLine(maxPropLength, '=');
    std::string descriptionLine(maxDescriptionLength, '=');
    w << propertyLine << " " << descriptionLine << "\n";

    std::string propSpecifier = fmt::format("{{: <{}s}}", maxPropLength); // Generates someting like "{: <13s}", which is a 13-wide specifier, with space fill characters.
    std::string descriptionSpecifier = fmt::format("{{: <{}s}}", maxDescriptionLength);
    w << fmt::format(propSpecifier, propertyHeader) << " " << fmt::format(descriptionSpecifier, descriptionHeader) << "\n";
    w << propertyLine << " " << descriptionLine << "\n";
    for (auto &property : properties)
    {
        w << fmt::format(propSpecifier, property.first) << " " << fmt::format(descriptionSpecifier, property.second) << "\n";
    }
    w << propertyLine << " " << descriptionLine << "\n";
    w << "\n";
}

void OutputScriptRST(DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    auto *script = docScript.GetScript();

    // Classes
    auto &classesAndInstances = script->GetClasses();
    sci::RawClassVector justClasses;
    for (auto &maybeClass : classesAndInstances)
    {
        if (!maybeClass->IsInstance() && ShouldDocument(docScript, maybeClass.get()))
        {
            justClasses.push_back(maybeClass.get());
        }
    }

    // Procedures
    auto &allProcs = script->GetProcedures();
    sci::RawProcedureVector publicProcs;
    for (auto &maybePublicProc : allProcs)
    {
        if (maybePublicProc->IsPublic() && ShouldDocument(docScript, maybePublicProc.get()))
        {
            publicProcs.push_back(maybePublicProc.get());
        }
    }

    bool isMain = (script->GetScriptNumber() == 0);

    // If there are no classes or public procedures, don't bother outputting anything for this script.
    // This filters out room scripts and such.
    if (!publicProcs.empty() || !justClasses.empty() || isMain)
    {
        if (ShouldDocumentScript(docScript))
        {
            std::string randomText = docScript.GetComment(script);

            std::string fullPath = OutputRSTHelper(rstFolder, ScriptsFolder, script->GetTitle(), generatedFiles);

            fmt::MemoryWriter w;

            OutputPreamble(w, script->GetTitle() + scriptFilenameSuffix, script->GetTitle() + scriptFilenameSuffix);

            // Random text
            w << randomText << "\n\n";

            if (!justClasses.empty())
            {
                w << "Classes" << "\n";
                w << "==========" << "\n\n";
                for (auto &theClass : justClasses)
                {
                    w << ":class:`" << theClass->GetName() << "`";
                    if (!theClass->GetSuperClass().empty()) // Happens in one case, with Obj
                    {
                        w << " of " << theClass->GetSuperClass();
                    }
                    w << "\n\n";
                }
            }

            if (!publicProcs.empty())
            {
                w << "Public Procedures" << "\n";
                w << "=================" << "\n\n";
                for (auto &proc : publicProcs)
                {
                    w << ":func:`" << proc->GetName() << "`\n\n";
                }
            }

            if (isMain)
            {
                w << "Global variables " << "\n";
                w << "=================" << "\n\n";

                std::vector<std::pair<std::string, std::string>> globalVaPairs;
                for (auto &globalVar : script->GetScriptVariables())
                {
                    globalVaPairs.emplace_back(globalVar->GetName(), docScript.GetComment(globalVar.get()));
                }
                OutputPropertyTableRSTWorker(w, globalVaPairs, "");
            }

            MakeTextFile(w.str().c_str(), fullPath);
        }
    }
}

void OutputFunctionRSTHelper(DocScript &docScript, fmt::MemoryWriter &w, sci::FunctionBase &function, bool isProcedure)
{
    std::string randomText = docScript.GetComment(&function);
    if (randomText.find(rstFunction) == std::string::npos)
    {
        int signatureCount = 0;
        for (auto &signature : function.GetSignatures())
        {
            // No function definition provided. Make one up.
            w << rstFunction << " " << function.GetName() << "(";
            bool first = true;
            bool startedOptional = false;
            int requiredParams = (int)signature->GetRequiredParameterCount();
            int paramCount = 0;
            for (auto &param : signature->GetParams())
            {
                if (!first)
                {
                    w << " ";
                }
                if (!startedOptional && (paramCount >= requiredParams))
                {
                    startedOptional = true;
                    w << "[";
                }
                w << param->GetName();
                first = false;
                paramCount++;
            }
            if (startedOptional)
            {
                w << "]";
            }
            w << ")\n";
            if (!isProcedure || (signatureCount > 0))
            {
                // Methods are always :noindex: since their names may not be unique..
                w << "\t:noindex:\n";
            }
            w << "\n";
            signatureCount++;
        }
        randomText = Indent(randomText);
    }
    else
    {
        // Warning if the function doesn't match.
        std::string sub = randomText;
        uint32_t pos;
        while ((pos = sub.find(rstFunction)) != std::string::npos)
        {
            sub = sub.substr(pos + rstFunction.length());
            ltrim(sub);
            uint32_t endPos = sub.find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
            if (endPos != std::string::npos)
            {
                std::string functionName = sub.substr(0, endPos);
                if (functionName != function.GetName())
                {
                    throw std::exception((functionName + " doesn't match " + function.GetName()).c_str());
                }
                sub = sub.substr(endPos);
            }
        }
        randomText = MarkFunctionAsNoIndex(randomText, isProcedure);
    }
    w << randomText << "\n\n";
}

void OutputKernelsRST(DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    auto *script = docScript.GetScript();
    for (auto &proc : script->GetProcedures())
    {
        fmt::MemoryWriter w;
        OutputPreamble(w, proc->GetName(), fmt::format("{0} (Kernel)", proc->GetName()));
        OutputFunctionRSTHelper(docScript, w, *proc, true);

        std::string fullPath = OutputRSTHelper(rstFolder, KernelsFolder, proc->GetName(), generatedFiles);
        MakeTextFile(w.str().c_str(), fullPath);
    }
}

// Must be a guarantee that name exists.
const sci::ClassProperty *FindProperty(const sci::ClassPropertyVector &properties, const std::string &name)
{
    auto it = std::find_if(properties.begin(), properties.end(), [name](const std::unique_ptr<sci::ClassProperty> &property) { return property->GetName() == name;  });
    return (it != properties.end()) ? it->get() : nullptr;
}

void OutputPropertyTableRST(SCIClassBrowser &browser, DocScript &docScript, fmt::MemoryWriter &w, sci::ClassDefinition &theClass)
{
    // Find *all* the properties that are part of this class, and differentiate those that were newly
    // defined vs those that were inherited.
    std::vector<std::string> inheritedProperties;
    std::vector<std::string> newProperties;
    auto allProps = browser.CreatePropertyNameArray(theClass.GetName());
    if (theClass.GetSuperClass().empty())
    {
        newProperties = *allProps;
    }
    else
    {
        // It's important to preserve declaration order here.
        auto superProps = browser.CreatePropertyNameArray(theClass.GetSuperClass());
        for (auto &prop : *allProps)
        {
            if (std::find(superProps->begin(), superProps->end(), prop) == superProps->end())
            {
                newProperties.push_back(prop);
            }
            else
            {
                inheritedProperties.push_back(prop);
            }
        }
    }

    // Properties
    w << "Properties\n";
    w << "==========\n\n";

    if (!theClass.GetSuperClass().empty())
    {
        std::vector<std::pair<std::string, std::string>> inheritedPropertyPairs;
        for (auto &property : inheritedProperties)
        {
            inheritedPropertyPairs.emplace_back(property, docScript.GetComment(FindProperty(theClass.GetProperties(), property)));
        }
        if (!inheritedProperties.empty())
        {
            OutputPropertyTableRSTWorker(w, inheritedPropertyPairs, fmt::format("Inherited from :class:`{0}`", theClass.GetSuperClass()));
        }
    }
    std::vector<std::pair<std::string, std::string>> newPropertyPairs;
    for (auto &property : newProperties)
    {
        newPropertyPairs.emplace_back(property, docScript.GetComment(FindProperty(theClass.GetProperties(), property)));
    }
    if (!newProperties.empty())
    {
        OutputPropertyTableRSTWorker(w, newPropertyPairs, fmt::format("Defined in {0}", theClass.GetName()));
    }
}

void OutputClassRST(SCIClassBrowser &browser, DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    if (ShouldDocumentScript(docScript))
    {
        ClassBrowserLock lock(browser);
        lock.Lock();

        auto *script = docScript.GetScript();
        for (auto &theClass : script->GetClasses())
        {
            if (!theClass->IsInstance() && ShouldDocument(docScript, theClass.get()))
            {
                std::string fullPath = OutputRSTHelper(rstFolder, ClassesFolder, theClass->GetName(), generatedFiles);
                fmt::MemoryWriter w;

                if (theClass->GetSuperClass().empty())
                {
                    OutputPreamble(w, theClass->GetName(), fmt::format("{0}", theClass->GetName()));
                }
                else
                {
                    OutputPreamble(w, theClass->GetName(), fmt::format("{0} (of :class:`{1}`)", theClass->GetName(), theClass->GetSuperClass()));
                }

                w << ".. class:: " << theClass->GetName() << "\n\n";

                w << "\tDefined in " << theClass->GetOwnerScript()->GetName() << ".\n\n";

                // Random text
                std::string indentedText = Indent(docScript.GetComment(theClass.get()));
                w << indentedText  << "\n\n";

                // Are there any subclasses? List them.
                std::vector<std::string> directSubclasses = browser.GetDirectSubclasses(theClass->GetName());
                if (!directSubclasses.empty())
                {
                    w << "Subclasses: ";
                    bool first = true;
                    for (auto &subClassName : directSubclasses)
                    {
                        if (!first)
                        {
                            w << ", ";
                        }
                        w << ":class:`" << subClassName << "`";
                        first = false;
                    }
                    w << ".\n\n";
                }

                OutputPropertyTableRST(browser, docScript, w, *theClass);

                // Methods - we'll do these inline instead of making new documents for them.
                w << "\n";
                w << "Methods\n";
                w << "==========\n\n";
                for (auto &method : theClass->GetMethods())
                {
                    OutputFunctionRSTHelper(docScript, w, *method, false);
                }

                MakeTextFile(w.str().c_str(), fullPath);
            }
        }
    }
}

void OutputProceduresRST(DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    if (ShouldDocumentScript(docScript))
    {
        auto *script = docScript.GetScript();
        for (auto &proc : script->GetProcedures())
        {
            if (proc->IsPublic() && ShouldDocument(docScript, proc.get()))
            {
                fmt::MemoryWriter w;
                OutputPreamble(w, proc->GetName(), fmt::format("{0} ({1}{2})", proc->GetName(), script->GetTitle(), scriptFilenameSuffix));
                OutputFunctionRSTHelper(docScript, w, *proc, true);

                std::string fullPath = OutputRSTHelper(rstFolder, ProceduresFolder, proc->GetName(), generatedFiles);
                MakeTextFile(w.str().c_str(), fullPath);
            }
        }
    }
}

void OutputIndexRSTHelper(const std::string &rstFolder, const std::string &indexFilename, const std::string &documentTitle, const std::string &subFolder, std::vector<std::string> &generatedFiles)
{
    std::string fullPath = OutputRSTHelper(rstFolder, "", indexFilename, generatedFiles);
    fmt::MemoryWriter w;
    OutputPreamble(w, documentTitle, documentTitle);

    path rootPath = rstFolder;
    path enumPath = rootPath / path(subFolder);
    std::vector<std::string> filenames;
    auto matchRSTRegex = std::regex("(\\w+)\\.rst");
    for (auto it = directory_iterator(enumPath); it != directory_iterator(); ++it)
    {
        const auto &file = it->path();
        std::smatch sm;
        std::string temp = file.filename();
        if (!is_directory(file) && std::regex_search(temp, sm, matchRSTRegex) && (sm.size() > 1))
        {
            std::string coreFilename = sm[1].str();
            filenames.push_back(coreFilename);
        }
    }

    w << ".. toctree::\n";
    w << "\t:maxdepth: 1\n\n";
    for (auto &filename : filenames)
    {
        w << "\t" << subFolder << "/" << filename << "\n";
    }
    w << "\n";

    MakeTextFile(w.str().c_str(), fullPath);
}

// For now, just classes to test.
void OutputIndexRST(const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    OutputIndexRSTHelper(rstFolder, "sci11_classes", "Classes", ClassesFolder, generatedFiles);
    OutputIndexRSTHelper(rstFolder, "sci11_scripts", "Script Files", ScriptsFolder, generatedFiles);
    OutputIndexRSTHelper(rstFolder, "sci11_procedures", "Public Procedures", ProceduresFolder, generatedFiles);
}