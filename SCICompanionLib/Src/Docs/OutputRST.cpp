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

using namespace std::tr2::sys;

// This uses reStructuredText's js domain, as it is most similar to SCI.

std::string scriptFilenameSuffix = ".sc";

std::string indent = "    ";

std::string ClassesFolder = "Classes";
std::string ScriptsFolder = "Scripts";
std::string ProceduresFolder = "Procedures";
std::string KernelsFolder = "Kernels";

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

void OutputScriptRST(DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    auto *script = docScript.GetScript();
    std::string fullPath = OutputRSTHelper(rstFolder, ScriptsFolder, script->GetTitle(), generatedFiles);

    fmt::MemoryWriter w;

    OutputPreamble(w, script->GetTitle() + scriptFilenameSuffix, script->GetTitle() + scriptFilenameSuffix);

    // Random text
    w << docScript.GetComment(script) << "\n\n";

    // Classes
    w << "Classes" << "\n";
    w << "==========" << "\n\n";
    auto &classesAndInstances = script->GetClasses();
    sci::RawClassVector justClasses;
    for (auto &maybeClass : classesAndInstances)
    {
        if (!maybeClass->IsInstance())
        {
            justClasses.push_back(maybeClass.get());
        }
    }
    if (justClasses.empty())
    {
        w << "There are no classes defined in this script.\n\n";
    }
    else
    {
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

    // Procedures
    w << "Public Procedures" << "\n";
    w << "=================" << "\n\n";
    auto &allProcs = script->GetProcedures();
    sci::RawProcedureVector publicProcs;
    for (auto &maybePublicProc : allProcs)
    {
        if (maybePublicProc->IsPublic())
        {
            publicProcs.push_back(maybePublicProc.get());
        }
    }
    if (publicProcs.empty())
    {
        w << "There are no public procedures in this script.\n\n";
    }
    else
    {
        for (auto &proc : publicProcs)
        {
            w << ":func:`" << proc->GetName() << "`\n\n";
        }
    }

    MakeTextFile(w.str().c_str(), fullPath);
}

void OutputFunctionRSTHelper(DocScript &docScript, fmt::MemoryWriter &w, sci::FunctionBase &function)
{
    std::string randomText = docScript.GetComment(&function);
    if (randomText.find(".. function::") == std::string::npos)
    {
        // No function definition provided. Make one up.
        w << ".. function:: " << function.GetName() << "(";
        bool first = true;
        for (auto &param : function.GetSignatures()[0]->GetParams())
        {
            if (!first)
            {
                w << " ";
            }
            w << param->GetName();
            first = false;
        }
        w << ")\n\n";
        randomText = Indent(randomText);
    }
    w << randomText << "\n\n";
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
        std::set<std::string> setAllProps(allProps->begin(), allProps->end());
        auto superProps = browser.CreatePropertyNameArray(theClass.GetSuperClass());
        std::set<std::string> setSuperProps(superProps->begin(), superProps->end());
        std::set_difference(setAllProps.begin(), setAllProps.end(), setSuperProps.begin(), setSuperProps.end(), std::back_inserter(newProperties));
        inheritedProperties = *superProps;
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
    OutputPropertyTableRSTWorker(w, newPropertyPairs, fmt::format("Defined in {0}", theClass.GetName()));
}

void OutputClassRST(SCIClassBrowser &browser, DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    ClassBrowserLock lock(browser);
    lock.Lock();

    auto *script = docScript.GetScript();
    for (auto &theClass : script->GetClasses())
    {
        if (!theClass->IsInstance())
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

            // Random text
            w << docScript.GetComment(theClass.get()) << "\n\n";

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
                OutputFunctionRSTHelper(docScript, w, *method);
            }

            MakeTextFile(w.str().c_str(), fullPath);
        }
    }
}

void OutputProceduresRST(DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    auto *script = docScript.GetScript();
    for (auto &proc : script->GetProcedures())
    {
        if (proc->IsPublic())
        {
            std::string fullPath = OutputRSTHelper(rstFolder, ProceduresFolder, proc->GetName(), generatedFiles);
            fmt::MemoryWriter w;

            OutputPreamble(w, proc->GetName(), fmt::format("{0} ({1}{2})", proc->GetName(), script->GetTitle(), scriptFilenameSuffix));

            OutputFunctionRSTHelper(docScript, w, *proc);

            MakeTextFile(w.str().c_str(), fullPath);
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