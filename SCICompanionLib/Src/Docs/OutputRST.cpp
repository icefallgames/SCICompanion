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

#include "DocScript.h"
#include "ScriptOMAll.h"
#include "format.h"

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
                w << ":class:`" << theClass->GetName() << "` of " << theClass->GetSuperClass() << "\n\n";
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


void OutputClassRST(DocScript &docScript, const std::string &rstFolder, std::vector<std::string> &generatedFiles)
{
    auto *script = docScript.GetScript();
    for (auto &theClass : script->GetClasses())
    {
        if (!theClass->IsInstance())
        {
            std::string fullPath = OutputRSTHelper(rstFolder, "Classes", theClass->GetName(), generatedFiles);
            fmt::MemoryWriter w;

            OutputPreamble(w, theClass->GetName(), fmt::format("Class: {0} (of :class:`{1}`)", theClass->GetName(), theClass->GetSuperClass()));

            w << ".. class:: " << theClass->GetName() << "\n\n";

            // Random text
            w << docScript.GetComment(theClass.get()) << "\n\n";

            // Properties
            w << "Properties\n";
            w << "==========\n\n";
            for (auto &property : theClass->GetProperties())
            {
                w << indent << "- " << property->GetName() << "\n";
            }

            // Methods - we'll do these inline instead of making new documents for them.
            w << "\n";
            w << "Methods\n";
            w << "==========\n\n";
            for (auto &method : theClass->GetMethods())
            {
                // TODO: make this better
                w << indent << "- " << method->GetName() << "\n";
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

            // TODO: Need :: function blah balh...
            // Search for ".. function::" in the random text. If none found, then generate one.

            // Random text
            w << docScript.GetComment(proc.get()) << "\n\n";

            // TODO: If no random text, then produce a function definition.

            MakeTextFile(w.str().c_str(), fullPath);
        }
    }
}