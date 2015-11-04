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

// This uses reStructuredText's js domain, as it is most similar to SCI.

std::string scriptFilenameSuffix = ".sc";

std::string indent = "    ";

// Adds an entry to generated files, returns the absolute path to the generated file.
std::string OutputRSTHelper(const std::string &rstFolder, const std::string &subFolder, const std::string &title, std::vector<std::string> &generatedFiles)
{
    std::string scriptsFolderPath = rstFolder + "\\" + subFolder;
    std::string relativeScriptsPath = subFolder + "\\" + title;
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
    std::string fullPath = OutputRSTHelper(rstFolder, "Scripts", script->GetTitle(), generatedFiles);

    fmt::MemoryWriter w;

    OutputPreamble(w, script->GetTitle() + scriptFilenameSuffix, script->GetTitle() + scriptFilenameSuffix);

    // Random text
    w << docScript.GetComment(script) << "\n\n";

    // Classes
    w << "Classes" << "\n";
    w << "==========" << "\n\n";
    auto &classes = script->GetClasses();
    size_t classCount = std::count_if(classes.begin(), classes.end(), [](const std::unique_ptr<sci::ClassDefinition> &theClass) { return !theClass->IsInstance(); });
    if (classCount == 0)
    {
        w << "There are no classes defined in this script.\n\n";
    }
    else
    {
        for (auto &theClass : classes)
        {
            if (!theClass->IsInstance())
            {
                w << ":class:`" << theClass->GetName() << "`";
                if (!theClass->GetSuperClass().empty()) // Happens in one case, with Obj
                {
                    w << " of " << theClass->GetSuperClass();
                }
                w << "\n\n";
            }
        }
    }

    // Procedures
    w << "Public Procedures" << "\n";
    w << "=================" << "\n\n";
    auto &procs = script->GetProcedures();
    size_t procCount = std::count_if(procs.begin(), procs.end(), [](const std::unique_ptr<sci::ProcedureDefinition> &proc) { return proc->IsPublic(); });
    if (procCount == 0)
    {
        w << "There are no public procedures in this script.\n\n";
    }
    else
    {
        for (auto &proc : procs)
        {
            if (proc->IsPublic())
            {
                w << ":func:`" << proc->GetName() << "`\n\n";
            }
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

void OutputClassRST(SCIClassBrowser &browser, DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    ClassBrowserLock lock(browser);
    lock.Lock();

    auto *script = docScript.GetScript();
    for (auto &theClass : script->GetClasses())
    {
        if (!theClass->IsInstance())
        {
            std::string fullPath = OutputRSTHelper(rstFolder, "Classes", theClass->GetName(), generatedFiles);
            fmt::MemoryWriter w;

            if (theClass->GetSuperClass().empty())
            {
                OutputPreamble(w, theClass->GetName(), fmt::format("Class: {0}", theClass->GetName()));
            }
            else
            {
                OutputPreamble(w, theClass->GetName(), fmt::format("Class: {0} (of :class:`{1}`)", theClass->GetName(), theClass->GetSuperClass()));
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

            // Find *all* the properties that are part of this class, and differentiate those that were newly
            // defined vs those that were inherited.
            std::vector<std::string> inheritedProperties;
            std::vector<std::string> newProperties;
            auto allProps = browser.CreatePropertyNameArray(theClass->GetName());
            if (theClass->GetSuperClass().empty())
            {
                newProperties = *allProps;
            }
            else
            {
                std::set<std::string> setAllProps(allProps->begin(), allProps->end());
                auto superProps = browser.CreatePropertyNameArray(theClass->GetSuperClass());
                std::set<std::string> setSuperProps(superProps->begin(), superProps->end());
                std::set_difference(setAllProps.begin(), setAllProps.end(), setSuperProps.begin(), setSuperProps.end(), std::back_inserter(newProperties));
                inheritedProperties = *superProps;
            }

            // Properties
            w << "Properties\n";
            w << "==========\n\n";
            for (auto &property : inheritedProperties)
            {
                w << indent << "- " << property << " (inherited)\n";
            }
            for (auto &property : newProperties)
            {
                w << indent << "- **" << property << "**\n";
            }

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
            std::string fullPath = OutputRSTHelper(rstFolder, "Procedures", proc->GetName(), generatedFiles);
            fmt::MemoryWriter w;

            OutputPreamble(w, proc->GetName(), fmt::format("Procedure: {0} ({1}{2})", proc->GetName(), script->GetTitle(), scriptFilenameSuffix));

            OutputFunctionRSTHelper(docScript, w, *proc);

            MakeTextFile(w.str().c_str(), fullPath);
        }
    }
}