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
#include <numeric>
#include "CompileInterfaces.h"
#include "CompileContext.h"
#include "SCO.h"
#include "ScriptOMAll.h"
#include "Text.h"
#include "AppState.h"
#include "PMachine.h"

using namespace std;
using namespace sci;

void ReportKeywordError(CompileContext &context, const ISourceCodePosition *pPos, const string &text, const string &use);

// Ensure w is an even number.  Return true if it rounded up.
bool make_even(WORD &w)
{
    if (w % 2)
    {
        ++w;
        return true;
    }
    return false;
}
void zero_pad(vector<BYTE> &output, bool fMadeEven)
{
    if (fMadeEven)
    {
        output.push_back(0);
    }
}

void push_word(vector<BYTE> &output, WORD w)
{
    // big-endian
    output.push_back(w & 0xff);
    output.push_back(w >> 8);
}

uint16_t read_word(const vector<uint8_t> &output, size_t index)
{
    return (output[index + 1] << 8) | output[index];
}

//
// Push a temporary WORD value (0), and return the offset.
//
WORD push_temp(vector<BYTE> &output)
{
    WORD wOffset = (WORD)output.size();
    push_word(output, 0);
    return wOffset;
}

void write_word(vector<BYTE> &output, size_t index, WORD w)
{
    // big-endian
    output[index] = (w & 0xff);
    output[index + 1] = (w >> 8);
}

// Run length encoded string (no nullptr terminator)
void push_string_rle(vector<BYTE> &output, const string &str)
{
    push_word(output, (WORD)str.length());
    copy(str.begin(), str.end(), back_inserter(output));
}

// WORD size, followed by nullptr terminated string
void push_string(vector<BYTE> &output, const string &str)
{
    push_word(output, (WORD)(str.size() + 1)); // + 1 for terminating nullptr
    copy(str.begin(), str.end(), back_inserter(output));
    output.push_back(0); // Terminating nullptr.
}

// Plain old nullptr terminated string...
void push_string_nt(vector<BYTE> &output, const string &str)
{
    copy(str.begin(), str.end(), back_inserter(output));
    output.push_back(0); // Terminating nullptr.
}


bool IsPublicProcedure(const unique_ptr<ProcedureDefinition> &proc)
{
    return proc->IsPublic();
}
bool IsPublicInstance(const unique_ptr<ClassDefinition> &theClass)
{
    return theClass->IsPublic() && theClass->IsInstance();
}

bool IsClass(const ClassPtr theClass)
{
    return !theClass->IsInstance();
}

//
// Compares two classes for the purposes of dependencies between them.
// Returns true if a is less than b (meaning b inherits from a)
//
bool CompareClassDefinition(const ClassPtr &a, const ClassPtr &b)
{
    if (b->GetSuperClass() == a->GetName())
    {
        return true;
    }
    return false;
}


typedef struct
{
    vector<BYTE> *pOutput;
    CompileContext *pContext;
} OUTPUTANDCONTEXT;

typedef std::vector<species_property> property_vector;

struct MatchSelector : public std::binary_function<species_property, WORD, bool>
{
    bool operator()(const species_property &prop, WORD wSelector) const
    {
        return (prop.wSelector == wSelector);
    }
};

class WordToByteThingy
{
public:
    WordToByteThingy(vector<BYTE> &output) : _output(output) {}
    void operator()(WORD w) const
    {
        push_word(_output, w);
    }
private:
    vector<BYTE> &_output;
};

// Procedure/class (true/false), index
enum class ExportType
{
    Empty,
    Procedure,
    Object,
};

struct ExportTableInfo
{
    ExportTableInfo() : Type(ExportType::Empty), ReferenceIndex(-1), SyntaxNodeWeak(nullptr) {}
    ExportTableInfo(ExportType type, int refIndex, SyntaxNode *syntaxNode, const std::string &nameIfInstance) : Type(type), ReferenceIndex(refIndex), SyntaxNodeWeak(syntaxNode), NameIfInstance(nameIfInstance) {}
    ExportType Type;
    int ReferenceIndex;
    SyntaxNode *SyntaxNodeWeak;
    std::string NameIfInstance;
};

void _AddToTable(vector<ExportTableInfo> &table, const ExportTableInfo &entry, const string &name, map<string, set<int>> nameToSlots, set<int> &freeSlots)
{
    bool inserted = false;
    for (int slot : nameToSlots[name])
    {
        inserted = true;
        table[slot] = entry;
    }
    if (!inserted)
    {
        if (freeSlots.empty())
        {
            table.push_back(entry);
        }
        else
        {
            table[*freeSlots.begin()] = entry;
            freeSlots.erase(*freeSlots.begin());
        }
    }
}

// See EnsurePublicsInExports 
vector<ExportTableInfo> GetExportTableOrder(CompileContext *contextOptional, const Script &script)
{
    // There are two types of exports:
    // 1) public procedures (common)
    // 2) public instances or objects
    // For < SCI1.1, public instances need to come first - as the interpreter looks for the first export, and may assume its
    // a room/locale/rgn, or in the case of script 0, the Game instance.
    // For SCI1.1, things can be in any order (with the exception of main perhaps?), and export slots can be empty too.

    // We'll put public objects first, followed by public procedures. But this may be overriden using the "exports" keywords
    // in script (version 2). Some Sierra games have the same export in multiple slots, even. So we need to handle that.
    map<string, set<int>> nameToSlots;
    set<int> explicitSlots;
    int maxSlot = -1;
    for (const auto &exportEntry : script.GetExports())
    {
        maxSlot = max(maxSlot, exportEntry->Slot);
        explicitSlots.insert(exportEntry->Slot);
        nameToSlots[exportEntry->Name].insert(exportEntry->Slot);
    }
    // Make a list of empty slots to use for exports that don't have an explicit slot.
    set<int> freeSlots;
    for (int i = 0; i < maxSlot; i++)
    {
        if (explicitSlots.find(i) == explicitSlots.end())
        {
            freeSlots.insert(i);
        }
    }

    // Make the table big enough
    vector<ExportTableInfo> table;
    table.assign(maxSlot + 1, ExportTableInfo());

    int classRefIndex = 0;
    for (const auto &theClass : script.GetClasses())
    {
        if (theClass->IsPublic())
        {
            ExportTableInfo entry(ExportType::Object, classRefIndex, theClass.get(), theClass->GetName());
            _AddToTable(table, entry, theClass->GetName(), nameToSlots, freeSlots);
            classRefIndex++;
        }
    }

    int procRefIndex = 0;
    for (const auto &theProc : script.GetProcedures())
    {
        if (theProc->IsPublic())
        {
            ExportTableInfo entry(ExportType::Procedure, procRefIndex, theProc.get(), theProc->GetName());
            _AddToTable(table, entry, theProc->GetName(), nameToSlots, freeSlots);
            procRefIndex++;
        }
    }

    return table;
}

//
// Writes the class or instance described by object, into the script resource output stream.
//
void _WriteClassOrInstance(const CSCOObjectClass &object, bool fInstance, vector<BYTE> &output, CompileContext *pContext)
{
    size_t sizeBeforeOutput = output.size();
    // Section header and magic number
    push_word(output, fInstance ? 0x0001 : 0x0006); // 1 = instance, 6 = class
    size_t iPlaceToWriteSize = output.size();
    push_word(output, 0);   // A temporary value where we'll write the size
    push_word(output, 0x1234);  // Magic value

    // WORD: local variable offset (filled in at runtime)
    push_word(output, 0x0000);

    const vector<CSCOObjectProperty> &properties = object.GetProperties();
    WORD wNumProps = (WORD)properties.size();
    // WORD: offset to function selectors
    push_word(output, 2 + (fInstance ? 2 : 4) * wNumProps); // classes have both selectors and values, instances just have values
    // WORD: number of property values
    push_word(output, wNumProps);

    // This is where code that has offsets to this instance should point to (for some reason)
    WORD wObjectPointer = (WORD)output.size();
    // So tell that code to point here.
    pContext->WroteSource(pContext->GetTempToken(ValueType::Token, object.GetName()), wObjectPointer);
    if (object.IsPublic())
    {
        // If it's public, we need to put this in the export table.
        pContext->TrackPublicInstance(wObjectPointer);
    }

    // Property selectors and values
    for (const auto &objectProperty : properties)
    {
        WORD wValue = objectProperty.GetValue();
        if (objectProperty.NeedsReloc())
        {
            pContext->WroteSink(wValue, (uint16_t)output.size());
        }
        push_word(output, wValue);
    }

    if (!fInstance)
    {
        // Only classes specify their property selectors (defined implicitly for instances by their species)
        for (const auto &objectProperty : properties)
        {
            // (Don't mess with for_each in here.)
            push_word(output, objectProperty.GetSelector());
        }
    }

    // Method pointers
    const vector<uint16_t> &methods = object.GetMethods();
    // WORD: the number of methods
    push_word(output, (WORD)methods.size());
    // n*WORD: the method selectors
    for (uint16_t scoMethodSelector : methods)
    {
        push_word(output, scoMethodSelector);
    }

    // WORD: zero
    push_word(output, 0x0000);
    // n*WORD: the method function pointers
    string className = object.GetName();
    for (uint16_t scoMethod : methods)
    {
        string methodName = pContext->LookupSelectorName(scoMethod);
        assert(!methodName.empty()); // Means we have a bug.
        code_pos methodPos = pContext->GetLocalProcPos(className + "::" + methodName);
        // Then from the code_pos, we get the offset at which it was written.
        push_word(output, methodPos->get_final_offset());
    }

    // Finally write the size of this section
    write_word(output, iPlaceToWriteSize, (WORD)(output.size() - sizeBeforeOutput));
}


//
// Streams a said word to the output.
//
void ParseSaidWord(CompileContext *contextOpt, ILookupSaids &context, string &word, const std::string &stringCode, vector<uint8_t> *output, const ISourceCodePosition *pos, std::vector<string> *wordsOptional)
{
    size_t nLen = word.length();
    if (nLen > 0)
    {
        // Trim whitespace from the ends.
        string::size_type notwhite = word.find_first_not_of(" \t\n");
        word.erase(0, notwhite);
        notwhite = word.find_last_not_of(" \t\n");
        word.erase(notwhite + 1);
    }
    if (!word.empty())
    {
        WORD wWordGroup = 0;
        if (context.LookupWord(word, wWordGroup))
        {
            assert((wWordGroup >> 8) < 0xf0); // Otherwise it's treated as a said token
            if (output)
            {
                // Note, we can't use push_word, because this must be in big-endian notation.
                output->push_back(wWordGroup >> 8);
                output->push_back(wWordGroup & 0xff);
            }
        }
        else
        {
            if (contextOpt)
            {
                if (output)
                {
                    assert(contextOpt->HasErrors() && "Should have already validated said string and made an error.");
                }
                else
                {
                    contextOpt->ReportError(pos, "'%s' is not in the vocabulary.", word.c_str());
                }
            }
        }
        if (wordsOptional)
        {
            wordsOptional->push_back(word);
        }
    }
    word.clear();
}

//
// Streams said tokens described by stringCode, to the output.
//
void ParseSaidString(CompileContext *contextOpt, ILookupSaids &context, const std::string &stringCode, vector<uint8_t> *output, const ISourceCodePosition *pos, std::vector<string> *wordsOptional)
{
    static string c_saidTokens = ",&/()[]#<>"; // 0xf0 to 0xf9
    string::const_iterator saidIt = stringCode.begin();
    string word; // We build up this word as we scan the string passed in.
    while (saidIt != stringCode.end())
    {
        size_t x = c_saidTokens.find(*saidIt);
        if (x < c_saidTokens.size())
        {
            // It was a token - if we've got a word ready, look it up now, and add its vocab index
            ParseSaidWord(contextOpt, context, word, stringCode, output, pos, wordsOptional);
            if (output)
            {
                // Then add the token
                output->push_back(0xf0 + (BYTE)x);
            }
        }
        else
        {
            // Just a regular letter (or non token)... add it to our word.
            word.push_back(*saidIt);
        }
        saidIt++;
    }
    // In case we had something in "word", parse it now...
    ParseSaidWord(contextOpt, context, word, stringCode, output, pos, wordsOptional);
    if (output)
    {
        output->push_back(0xff); // terminator
    }
}



//
// Write all class and instance sections to the script resource output stream
//
void _Section1And6_ClassesAndInstances(vector<BYTE> &output, CompileContext &context, CompileResults &results)
{
    size_t beginning = output.size();
    CSCOFile &sco = context.GetScriptSCO();
    for (CSCOObjectClass &oClass : sco.GetObjects())
    {
        _WriteClassOrInstance(oClass, false, output, &context);
    }

    // Instances
    for (CSCOObjectClass &iClass : context.GetInstanceSCOs())
    {
        _WriteClassOrInstance(iClass, true, output, &context);
    }

    results.Stats.Objects += (int)(output.size() - beginning);
}

void _Section10_LocalVariables(Script &script, CompileContext &context, vector<BYTE> &output, bool separateHeapResource, CompileResults &results)
{
    size_t beginning = output.size();

    const VariableDeclVector &scriptVars = script.GetScriptVariables();
    if (!scriptVars.empty())
    {
        if (!separateHeapResource)
        {
            // Section 10 for SCI0
            push_word(output, 10);
        }
        size_t localVarSizeIndex = output.size();
        push_word(output, 0); // Temporary value.

        for (const auto &var : scriptVars)
        {
            int size = 0;
            for (auto &value : var->GetStatements())
            {
                const PropertyValueBase *pValue = SafeSyntaxNode<PropertyValue>(value.get());
                if (!pValue)
                {
                    pValue = SafeSyntaxNode<ComplexPropertyValue>(value.get());
                    assert(SafeSyntaxNode<ComplexPropertyValue>(value.get())->GetIndexer() == nullptr);
                }
                assert(pValue); // Must be a property value.

                switch (pValue->GetType())
                {
                    case ValueType::Said:
                    case ValueType::String:
                    case ValueType::ResourceString:
                        context.WroteSink(context.GetTempToken(pValue->GetType(), pValue->GetStringValue()), (uint16_t)output.size());
                        break;
                    case ValueType::Number:
                        // We're good.
                        break;
                    default: // Token, etc...
                        context.ReportError(pValue, "%s is not a valid token for an array initializer.", pValue->GetStringValue().c_str());
                        break;
                }
                push_word(output, pValue->GetNumberValue());
                size++;
            }
            if ((int)var->GetSize() < size)
            {
                context.ReportError(var.get(), "Initializer too large (%d) for array size (%d).", size, var->GetSize());
            }
            else
            {
                // Fill any remaining spots with 0
                WORD wZeroFill = (var->GetSize() - (WORD)size);
                output.insert(output.end(), wZeroFill * 2, 0); // WORD = BYTE * 2
            }
        }

        if (separateHeapResource)
        {
            // word count: i.e. number of local vars.
            write_word(output, localVarSizeIndex, (WORD)((output.size() - (localVarSizeIndex + 2)) / 2));
        }
        else
        {
            // But here it's a section size byte count. i.e. 4 bytes plus byte count of local vars
            write_word(output, localVarSizeIndex, (WORD)(output.size() - localVarSizeIndex + 2));
        }
    }
    else
    {
        // For SCI0, we aren't writing the section at all. But for SCI1.1+ it's always there.
        // So we need to write a var count.
        if (separateHeapResource)
        {
            push_word(output, 0);
        }
    }

    results.Stats.Locals += (int)(output.size() - beginning);
}

void _Section2_Code(Script &script, CompileContext &context, vector<BYTE> &output, WORD &wStartOfCode, bool separateHeapResources, CompileResults &results)
{
    size_t beginning = output.size();

    context.PushVariableLookupContext(&script); // Don't really need to pop this, ever.
    const ProcedureVector &procs = script.GetProcedures();
    for_each(procs.begin(), procs.end(), GenericOutputByteCode2<ProcedureDefinition>(context));
    // b) classes, methods
    const ClassVector &classes = script.GetClasses();
    for_each(classes.begin(), classes.end(), GenericOutputByteCode2<ClassDefinition>(context));
    // Now some dirty work...
    context.FixupLocalCalls();
    context.FixupAsmLabelBranches();

    uint16_t codeSizeBase = context.code().calc_size();
    bool fRoundUp = make_even(codeSizeBase);

    WORD wCodeSize = codeSizeBase + 4;

    if (!separateHeapResources)
    {
        // Write the section header and size.
        push_word(output, 2);
        push_word(output, wCodeSize);
    }

    wStartOfCode = (uint16_t)output.size(); // Store where the code begins

    if (context.GenerateDebugInfo)
    {
        push_string(results.GetDebugInfo(), script.GetScriptId().GetFileName());
    }
    context.code().write_code(context, output, context.GenerateDebugInfo ? &results.GetDebugInfo() : nullptr);
    zero_pad(output, fRoundUp);

    uint16_t after = (uint16_t)output.size();
    if ((after - wStartOfCode) != codeSizeBase)
    {
        context.ReportError(&script, "There was an error generating the script resource. Calculated code size didn't match output code size.");
    }

    results.Stats.Code += (int)(output.size() - beginning);
}

void _Section3_Synonyms(Script &script, CompileContext &context, vector<BYTE> &output, CompileResults &results)
{
    size_t beginning = output.size();

    const SynonymVector &synonyms = script.GetSynonyms();
    if (!synonyms.empty())
    {
        push_word(output, 3);           // 3 = synonyms
        uint16_t totalSize = 4 + 2;     // header plus terminator
        for (auto &synonymClause : synonyms)
        {
            totalSize += 4 * (uint16_t)synonymClause->Synonyms.size();  // 4 bytes per synonym entry
        }
        push_word(output, totalSize);

        for (auto &synonymClause : synonyms)
        {
            for (auto &synonym : synonymClause->Synonyms)
            {
                WORD wGroup = 0;
                if (!context.LookupWord(synonym, wGroup))
                {
                    context.ReportError(synonymClause.get(), "'%s' is not in the vocabulary.", synonym.c_str());
                }
                push_word(output, wGroup); // Write anyway, so we can continue...
                if (!context.LookupWord(synonymClause->MainWord, wGroup))
                {
                    context.ReportError(synonymClause.get(), "'%s' is not in the vocabulary.", synonymClause->MainWord.c_str());
                }
                push_word(output, wGroup);
            }
        }

        push_word(output, 0xffff);
    }

    results.Stats.Saids += (int)(output.size() - beginning);
}

void _Section4_Saids(CompileContext &context, vector<BYTE> &output, CompileResults &results)
{
    size_t beginning = output.size();

    auto saids = context.GetSaids();
    if (!saids.empty())
    {
        push_word(output, 4);
        size_t saidSizeIndex = output.size();
        push_word(output, 0); // Temporary size...
        // Now actually write out the saids into the stream.
        for (auto &said : saids)
        {
            // a) Get the index to which we'll write the said stream.
            uint16_t wAbsolute = (uint16_t)output.size();
            context.WroteSource(context.GetTempToken(ValueType::Said, said), wAbsolute);

            // b) Parse the said stream
            ParseSaidString(&context, context, said, &output, nullptr);
            // c) update the offset...
        }

        // How much room did that take?
        WORD wSaidSectionSize = (WORD)(output.size() - saidSizeIndex) + 2;
        // Round it up to a WORD boundary and pad if necessary:
        zero_pad(output, make_even(wSaidSectionSize));
        // Go back and write the size.
        write_word(output, saidSizeIndex, wSaidSectionSize);
    }

    results.Stats.Saids += (int)(output.size() - beginning);
}

void _Section5_Strings(CompileContext &context, vector<BYTE> &outputScr, vector<BYTE> &outputHeap, bool writeSCI0SectionHeader, CompileResults &results)
{
    size_t beginning = outputHeap.size();

    // This requires all strings to have been pre-scanned up to this point.
    auto strings = context.GetStringsThatWereWritten();
    if (!strings.empty())
    {
        // Compute the length of all the strings.
        WORD wStringSectionSize = accumulate(strings.begin(), strings.end(), 0, [](std::string::size_type sumSoFar, const std::string &theString) { return sumSoFar + theString.length() + 1; });

        // Round it up to a WORD boundary:
        bool fRoundUp = make_even(wStringSectionSize);

        if (writeSCI0SectionHeader)
        {
            push_word(outputHeap, 5);
            // Indicate the section size:
            push_word(outputHeap, wStringSectionSize + 4);
        }

        for (auto &theString : strings)
        {
            // Get the index to which we'll write the string.
            uint16_t wAbsolute = (uint16_t)outputHeap.size();
            context.WroteSource(context.GetTempToken(ValueType::String, theString), wAbsolute);
            outputHeap.insert(outputHeap.end(), theString.begin(), theString.end());
            outputHeap.push_back(0);
        }

        zero_pad(outputHeap, fRoundUp);
    }

    results.Stats.Strings += (int)(outputHeap.size() - beginning);
}

void _Section7_Exports_Part1(Script &script, CompileContext &context, vector<BYTE> &output, const vector<ExportTableInfo> &exportTableOrder, size_t &offsetOfExports)
{
    push_word(output, 7); // 7 = exports
    uint16_t exportWidth = context.GetVersion().IsExportWide ? 4 : 2;
    push_word(output, (uint16_t)(exportTableOrder.size() * exportWidth) + 4 + 2); // exports + header + exportCount.
    push_word(output, (uint16_t)exportTableOrder.size()); // the number of exports
    offsetOfExports = output.size(); // Store where the ptrs will go.
    // Fill with zeroes for now:
    output.insert(output.end(), exportTableOrder.size() * exportWidth, 0);
}

const uint16_t ExportTempMarker = 0x5845;   // "EX" backwards

void _Exports_SCI11(Script &script, CompileContext &context, vector<BYTE> &outputScr, const vector<ExportTableInfo> &exportTableOrder, size_t &offsetOfExports)
{
    assert(!context.GetVersion().IsExportWide);

    push_word(outputScr, (WORD)exportTableOrder.size()); // the number of exports
    offsetOfExports = outputScr.size(); // Store where the ptrs will go.
    // Fill with dumb markers for now:
    for (size_t i = 0; i < exportTableOrder.size(); i++)
    {
        uint16_t token = ExportTempMarker;
        if (exportTableOrder[i].Type == ExportType::Object)
        {
            token = context.GetTempToken(ValueType::Token, exportTableOrder[i].NameIfInstance);
            context.WroteScrSink(token, (uint16_t)outputScr.size());
        }
        push_word(outputScr, token);
    }
}

void _Section7_Exports_Part2(CompileContext &context, vector<BYTE> &output, WORD wStartOfCode, const vector<ExportTableInfo> &exportTableOrder, size_t offsetOfExports)
{
    // Now code  and classes have been written to the stream.  It's time to set up the exports table, which starts back at
    // indexOfExports.
    // In here, we write the offset from the begining of the resource to the beginning of the code or object.
    vector<code_pos> &procExports = context.GetExports();
    vector<WORD> &instanceOffsets = context.GetPublicInstanceOffsets();
    uint16_t exportWidth = context.GetVersion().IsExportWide ? 4 : 2;

    // We already filled the thing with zeroes, so we can just write a uint16_t, and increment by 4.
    for (const auto &exportOrderEnty : exportTableOrder)
    {
        switch (exportOrderEnty.Type)
        {
            case ExportType::Empty:
                write_word(output, offsetOfExports, 0);
                break;
            case ExportType::Procedure:
            {
                WORD wCodeOffset = context.code().offset_of(procExports[exportOrderEnty.ReferenceIndex]);
                write_word(output, offsetOfExports, wStartOfCode + wCodeOffset);
            }
                break;
            case ExportType::Object:
                write_word(output, offsetOfExports, instanceOffsets[exportOrderEnty.ReferenceIndex]);
                break;
        }
        offsetOfExports += exportWidth;
    }
}

void _Section8_RelocationTable(CompileContext &context, vector<BYTE> &output)
{
    bool isEntryCount32bit = context.GetVersion().lofsaOpcodeIsAbsolute;
    vector<WORD> relocations = context.GetRelocations();
    push_word(output, 8);
    push_word(output, (isEntryCount32bit ? 8 : 6) + (WORD)relocations.size() * 2);  // section size
    push_word(output, (WORD)relocations.size());    // # of relocation entries
    if (isEntryCount32bit)
    {
        // The "upper" 16 bits, 0:
        push_word(output, 0);
    }
    for_each(relocations.begin(), relocations.end(), WordToByteThingy(output));
}

class FixCaseStatements : public IExploreNode
{
public:
    FixCaseStatements(CompileContext &context) : _context(context) {}

private:
    //
    // Problems with parsing the SCIStudio syntax means we may need to make some adjustments in the
    // syntax tree.
    //
    void ExploreNode(SyntaxNode &node, ExploreNodeState state)
    {
        if (state == ExploreNodeState::Pre)
        {
            // 1)
            // Look for case statements of the form
            // (case foo(stuff))    // foo is a procedure call, and stuff is parameters
            // and turn them into
            // (case foo (stuff))   // foo is a value, and stuff is code
            if (node.GetNodeType() == NodeTypeCase)
            {
                CaseStatement &caseStatement = static_cast<CaseStatement&>(node);
                SyntaxNode *caseValue = caseStatement.GetCaseValue();
                ProcedureCall *maybeProcedureCall = SafeSyntaxNode<ProcedureCall>(caseValue);
                if (caseValue && (caseValue->GetNodeType() == NodeTypeProcedureCall))
                {
                    if (ProcedureUnknown == _context.LookupProc(maybeProcedureCall->GetName()))
                    {
                        // This isn't a procedure call.  "Undo it"
                        // The name becomes a simple token value - let's construct that.
                        unique_ptr<ComplexPropertyValue> pTokenValue = std::make_unique<ComplexPropertyValue>();
                        pTokenValue->SetValue(maybeProcedureCall->GetName(), ValueType::Token);
                        // ...and the parameters become the code statements. So steal the procedures parameters.
                        SyntaxNodeVector formerParams;
                        maybeProcedureCall->StealParams(formerParams);
                        // Replace the casevalue's procedure, with our "simple token value"
                        // whose parameters we have now stolen.
                        caseStatement.SetCaseValue(std::move(pTokenValue));
                        // Finally, add back the statements.  In case there is already some code in the
                        // case statement, we'll make sure to insert ours at the beginning.
                        SyntaxNodeVector &existingCode = caseStatement.GetCodeSegments();
                        existingCode.insert(existingCode.begin(), std::make_move_iterator(formerParams.begin()), std::make_move_iterator(formerParams.end()));
                    }
                }
            }
        }
    }

    CompileContext &_context;
};

//
// Adds the script's public instances and procedures to the sco file being compiled by the context
//
void GenerateSCOPublics(CompileContext &context, const Script &script)
{
    vector<ExportTableInfo> exportTableOrder = GetExportTableOrder(&context, script);
    const ClassVector &classes = script.GetClasses();
    const ProcedureVector &procs = script.GetProcedures();

    uint16_t wIndex = 0;
    for (auto &exportTableInfo : exportTableOrder)
    {
        switch (exportTableInfo.Type)
        {
            case ExportType::Procedure:
            {
                auto pProc = static_cast<sci::ProcedureDefinition*>(exportTableInfo.SyntaxNodeWeak);
                CSCOPublicExport procExport(pProc->GetName(), wIndex);
                context.AddSCOPublics(procExport);
            }
                break;

            case ExportType::Object:
            {
                auto classDef = static_cast<sci::ClassDefinition*>(exportTableInfo.SyntaxNodeWeak);
                CSCOPublicExport pe(classDef->GetName(), wIndex);
                SpeciesIndex si = DataTypeAny;;
                if (!context.LookupTypeSpeciesIndex(classDef->GetSuperClass(), si))
                {
                    context.ReportError(classDef, "Unknown superclass: %s.", classDef->GetSuperClass().c_str());
                }
                context.AddSCOPublics(pe);
            }
                break;
        }

        wIndex++;
    }
}

//
// Adds the script's procedures to the sco file being compiled by the context
//
void GenerateSCOVariables(CompileContext &context, const Script &script)
{
    const VariableDeclVector &vars = script.GetScriptVariables();
    WORD wIndex = 0;
    for (auto &pVar : vars)
    {
        WORD wSize = pVar->GetSize();
        assert(wSize != 0);

        // Determine the species number of the type.
        SpeciesIndex wTypeSpecies = 0;
        if (!context.LookupTypeSpeciesIndex(pVar->GetDataType(), wTypeSpecies))
        {
            context.ReportError(pVar.get(), "Unknown type for %s: %s.", pVar->GetName().c_str(), pVar->GetDataType().c_str());
        }

        context.AddSCOVariable(CSCOLocalVariable(pVar->GetName()));
        --wSize;
        // Fill up arrays with empty var names... this is important so the index remains correct
        for (WORD w = 0; w < wSize; w++)
        {
            context.AddSCOVariable(CSCOLocalVariable());
        }
        ++wIndex;
    }
}


//
// Comes up with a resolved list of properties that were specified in the class or instance definition.
// (does not include implicit properties: species, superclass, info, name (except when overridden)
// Does not check the super class - this is just the (properties) declaration made in script.
//
std::vector<species_property> GetOverriddenProperties(CompileContext &context, const ClassPtr pClass)
{
    std::vector<species_property> propRet;
    for (auto &classProperty : pClass->GetProperties())
    {
        WORD wSelectorIndex = 0;
        WORD wValue = 0;
        SpeciesIndex wType = DataTypeNone;
        // First the selector
        string selectorName = classProperty->GetName();
        if (!context.LookupSelector(selectorName, wSelectorIndex))
        {
            if (pClass->IsInstance())
            {
                // This error is not comprehensive - the user might have added a new property
                // on an instance, and there is already a selector for this property.  Hence, the caller
                // of this function needs to compare against the species props too.
                context.ReportError(classProperty.get(), "Unknown property '%s'.  Instances can not define new properties.", selectorName.c_str());
            }
            else
            {
                // We'll add a new selector for this.
                context.DefineNewSelector(selectorName, wSelectorIndex);
            }
        }
        // Then the value - defines should already be resolved.
        bool fTrackRelocation = false;
        const PropertyValueBase *value = classProperty->TryGetValue2();
        if (value)
        {
            switch (value->GetType())
            {
                case ValueType::Number:
                    wValue = value->GetNumberValue();
                    break;
                case ValueType::String: // For now, strings are ok in property lists
                case ValueType::ResourceString: // For now, strings are ok in property lists
                    wValue = context.GetTempToken(ValueType::String, value->GetStringValue());
                    fTrackRelocation = true;
                    break;
                case ValueType::Said:
                    wValue = context.GetTempToken(ValueType::Said, value->GetStringValue());
                    fTrackRelocation = true;
                    break;
                case ValueType::Selector:
                    if (!context.LookupSelector(value->GetStringValue(), wValue))
                    {
                        context.ReportError(value, "Unknown selector '%s'.", value->GetStringValue().c_str());
                    }
                    break;
                case ValueType::Pointer:
                    context.ReportError(value, "Pointers are not allowed as property values.", value->ToString().c_str());
                    break;
                default:
                    context.ReportError(value, "Uknown token %s is not allowed as a property value.", value->ToString().c_str());
                    break;
            }
        }
        else
        {
            if (classProperty->GetStatement1()->Evaluate(context, wValue, &context))    // report errors
            {
                // We evaluated an expression
            }
            else
            {
                context.ReportError(classProperty.get(), "The value for %s is not a constant expression.", classProperty->GetName().c_str());
            }
        }

        // For classes, see if the property has a type, and make sure it makes sense.
        if (!pClass->IsInstance())
        {
            std::string type = classProperty->GetDataType();
            if (!type.empty())
            {
                if (!context.LookupTypeSpeciesIndex(type, wType))
                {
                    context.ReportError(classProperty.get(), "Unknown type '%s' for property '%s'.", type.c_str(), selectorName.c_str());
                }
            }
            else
            {
                // REVIEW, I added this. SCISyntax
                wType = DataTypeAny;
            }
        }

        species_property newProp = { wSelectorIndex, wValue, wType, fTrackRelocation };
        propRet.push_back(newProp);
    }
    return propRet;
}

//
// Generates SCO objects for a Script and adds them to the context.
// These are used so that other parts of the script can reference
// them, just like there were in another script that we included that was already compiled.
//
// This is also where we potentially generate new species indexes
//
void GenerateSCOObjects(CompileContext &context, const Script &script)
{
    // First, ensure that we have an entry in the species table for each class that is defined.
    // We do it here (instead of sorted, below), because they should appear in the order that
    // they were defined in the script.
    // scriptClassIndexToSpeciesIndex will make the index of each in class in this script,
    // to a global species index.
    vector<WORD> scriptClassIndexToSpeciesIndex;
    for (auto &classDef : script.GetClasses())
    {
        if (!classDef->IsInstance())
        {
            // This is a class.  Ensure it has a spot in the species table.
            WORD wSpeciesIndex = context.EnsureSpeciesTableEntry((WORD)scriptClassIndexToSpeciesIndex.size());
            scriptClassIndexToSpeciesIndex.push_back(wSpeciesIndex);

            // This is sort of a hack.  We need to have the CSCOObjectClass for all the classes in this file
            // around, prior to executing the rest of this function.  For example, if the Script class has
            // a property of type Script (which it does), we need this data type to resolve propertly.
            // That won't happen unless we have added the CSCOObjectClass to the "this" CSCOFile in the
            // CompileContext.  So add dummy ones now.  We'll replace with the real ones later.
            CSCOObjectClass scoDummy;
            scoDummy.SetName(classDef->GetName());
            context.AddSCOClass(scoDummy, false);
        }
    }

    // NOTE: if the user moves classes around in a file, things will get corrupt.  The species indicies will
    // be wrong.  We need to figure this out now (look in the SCO file?) - and tell the species table that
    // it needs to dirty itself, even though the number of classes didn't change... hmm. Actually - that won't
    // change the species table at all.  But it does mean that any file that references one of these classes
    // will need to recompile.

    // Now that we're sure we have an entry in the species table for each class, we
    // can construct the SCO object for each.
    WORD wClassIndexInScript = 0;
    for (auto &classDef : script.GetClasses())
    {
        CSCOObjectClass sco;
        sco.SetPublic(classDef->IsPublic());
        // Make SCO objects for instances too, but only as a convenience for writing in the script resource.
        WORD wSuperClass; // This must already exist elsewhere, since we just scanned through the classes.
        if (classDef->GetSuperClass().empty())
        {
            // This is the game where there is no super class.  0xffff is a sentinel value that the
            // interpreter uses for this.  We want to put this logic here, instead of in LookupSpeciesIndex,
            // since we only want to allow empty superclasses right here (and not other places where we
            // call LookupSpeciesIndex).
            wSuperClass = DataTypeNone; // 0xffff
        }
        else
        {
            SpeciesIndex sindex;
            if (context.LookupSpeciesIndex(classDef->GetSuperClass(), sindex))
            {
                wSuperClass = sindex.Type();
            }
            else
            {
                wSuperClass = 0x50FE; // sentinel value for debugging
                ErrorHelper(context, classDef.get(), "Unknown class", classDef->GetSuperClass());
            }
        }
        // Set superclass
        sco.SetSuperClass(wSuperClass);
        // set name
        sco.SetName(classDef->GetName()); // TODO: check conflicts?
        // set species
        if (classDef->IsInstance())
        {
            sco.SetSpecies(wSuperClass);
        }
        else
        {
            sco.SetSpecies(scriptClassIndexToSpeciesIndex[wClassIndexInScript]);
            wClassIndexInScript++;
        }
        // The --info-- is implied... although we are making temporary scos for instances.

        vector<CSCOObjectProperty> scoProperties;
        // Now get the properties for our super class
        property_vector speciesProps = context.GetSpeciesProperties(classDef->GetSuperClass());
        // And get the properties that we declared
        property_vector newProps = GetOverriddenProperties(context, classDef.get());

        int nameIndex;
        // But first, provide some of our own overrides.
        if (context.GetVersion().SeparateHeapResources)
        {
            assert(speciesProps.size() >= 9);
            for (int i = 0; i < 5; i++)
            {
                speciesProps[i].wValue = 0;
            }
            speciesProps[5].wValue = sco.GetSpecies();
            speciesProps[6].wValue = wSuperClass; // superclass index
            speciesProps[7].wValue = classDef->IsInstance() ? 0x0000 : 0x8000; // --info--
            speciesProps[8].wValue = context.GetTempToken(ValueType::String, classDef->GetName()); // name (can be overridden explicitly too)
            nameIndex = 8;
        }
        else
        {
            assert(speciesProps.size() >= 4);
            speciesProps[0].wValue = sco.GetSpecies();
            speciesProps[1].wValue = wSuperClass; // superclass index
            speciesProps[2].wValue = classDef->IsInstance() ? 0x0000 : 0x8000; // --info--
            speciesProps[3].wValue = context.GetTempToken(ValueType::String, classDef->GetName()); // name (can be overridden explicitly too)
            nameIndex = 3;
        }

        // Replace the species default values, with any that the user specified.
        int iIndex = 0;
        for (species_property &speciesProp : speciesProps)
        {
            WORD wValue = speciesProp.wValue;
            // name is tracked by default (since we populate with a string by default)
            // Other species props are not tracked by default.
            bool fTrackRelocation = (iIndex == nameIndex);
            property_vector::const_iterator overriddenIt = find_if(newProps.begin(), newProps.end(), bind2nd(MatchSelector(), speciesProp.wSelector));
            if (overriddenIt != newProps.end())
            {
                wValue = overriddenIt->wValue;
                fTrackRelocation = overriddenIt->fTrackRelocation;
                // We overrode it... class property syntax allows specifying a type.
            }
            scoProperties.push_back(CSCOObjectProperty(speciesProp.wSelector, wValue, fTrackRelocation));
            ++iIndex;
        }
        // Now scan the new props for any new properties!
        for (species_property &newProp : newProps)
        {
            // Is this a new property, not defined by the superclass?
            property_vector::const_iterator speciesIt = find_if(speciesProps.begin(), speciesProps.end(), bind2nd(MatchSelector(), newProp.wSelector));
            if (speciesIt == speciesProps.end())
            {
                // Must be - we didn't find it in the species props.
                // Make sure it's not a default property either
                if (context.IsDefaultSelector(newProp.wSelector))
                {
                    // Someone tried to override a default property
                    context.ReportError(classDef.get(), "Can't override default selectors.");
                }
                else
                {
                    if (classDef->IsInstance())
                    {
                        // User tried to declare a new property on an instance.
                        string propName = context.LookupSelectorName(newProp.wSelector);
                        context.ReportError(classDef.get(), "Instances can not declare new properties: '%s'.", propName.c_str());
                    }
                    else
                    {
                        // This is a new property for this class.  The selector should have already been defined, if its new.
                        scoProperties.push_back(CSCOObjectProperty(newProp.wSelector, newProp.wValue, newProp.fTrackRelocation));
                    }
                }
            }
        }
        sco.SetProperties(scoProperties);

        // Now methods
        vector<uint16_t> methodsOut;
        for (auto &method : classDef->GetMethods())
        {
            const string &methodName = method->GetName();
            methodsOut.push_back(context.LookupSelectorAndAdd(methodName)); // Add it if it doesn't exist.
        }
        sco.SetMethods(methodsOut);
        if (classDef->IsInstance())
        {
            context.AddSCOClass(sco, classDef->IsInstance());
        }
        else
        {
            context.ReplaceSCOClass(sco); // Since we had to already add it.
        }
    }
}


void CommonScriptPrep(Script &script, CompileContext &context, CompileResults &results)
{
    // Load the include files
    context.LoadIncludes();
    // Set the script number now (might have relied on defines)
    context.SetScriptNumber();
    results.SetScriptNumber(context.GetScriptNumber());

	// Process the defines before doing a TrackArraySizes and PreScane
	for (auto &theDefine : script.GetDefines())
	{
		const string &defineName = theDefine->GetName();
		if (IsSCIKeyword(context.GetLanguage(), defineName))
		{
			ReportKeywordError(context, theDefine.get(), defineName, "define");
		}
		else
		{
			context.AddDefine(theDefine.get());
		}
	}

    // Some stuff needs to be done even before this
    // Note: TrackArraySizes has to do some PreScan too, since an array could use
    // a define as its size.
    TrackArraySizes(context, script);
    // Do some prescans (script number must already be set!)
    script.PreScan(context);

    EvaluateConstantExpressions(context, script);
    // Ok, now we should have been told about all the saids and strings.
    if (script.Language() == LangSyntaxStudio)
    {
        // Fix up case statements we may have mis-interpreted.
        FixCaseStatements hack(context);
        script.Traverse(hack);
    }
}

bool GenerateScriptResource_SCI0(Script &script, PrecompiledHeaders &headers, CompileTables &tables, CompileResults &results, bool generateDebugInfo)
{
    vector<BYTE> &output = results.GetScriptResource();

    // Create our "CompileContext", which holds state during the compilation.
    CompileContext context(appState->GetVersion(), script, headers, tables, results.GetLog(), generateDebugInfo);

    _Section3_Synonyms(script, context, output, results);

    CommonScriptPrep(script, context, results);

    // To figure out how many exports we have, let's look at the public procedures and public instances
    size_t offsetOfExports = 0;
    vector<ExportTableInfo> exportTableOrder = GetExportTableOrder(nullptr, script);
    // Christmas Card Demo 1990 VGA will not work properly if we output empty exports tables.
    if (!exportTableOrder.empty())
    {
        _Section7_Exports_Part1(script, context, output, exportTableOrder, offsetOfExports);
    }

    // Generate SCO objects for the classes and instances in the script.  We want to do this before generating any code,
    // since some code relies on it.
    GenerateSCOObjects(context, script);
    GenerateSCOPublics(context, script);
    GenerateSCOVariables(context, script);

    // It would be nice to put the code right after the saids and strings,
    // in the hope that we can use some 8bit opcodes instead of 16bit ones.  However, it
    // turns out it's easier to put them after, with the instances.  It makes it easier to fix up
    // the references to saids, strings and instances that are in the code.
    WORD wStartOfCode = 0;
    _Section2_Code(script, context, output, wStartOfCode, false, results);

    _Section4_Saids(context, output, results);

    _Section1And6_ClassesAndInstances(output, context, results);

    if (!exportTableOrder.empty())
    {
        _Section7_Exports_Part2(context, output, wStartOfCode, exportTableOrder, offsetOfExports);
    }

    _Section10_LocalVariables(script, context, output, false, results);

    // NOTE: Strings must come after all strings have been pre-scaned. Note that we no longer
    // pre-scan string property values by default - but only when they are actually used in code 
    // (in order to support resource strings)
    // The parser ensures that local vars (which follow) do not have ValueType::ResourceString.
    // Also note: The strings section should come after all sinks have been written - because we
    // won't bother writing a string if it isn't referenced by anyone.
    _Section5_Strings(context, output, output, true, results);

    _Section8_RelocationTable(context, output);

    // Finish off by writing 0x0000
    push_word(output, 0);

    // Get the .sco file produced.
    results.GetSCO() = context.GetScriptSCO();

    // Fill the text resource.
    uint16_t autoTextNumber;
    if (context.IsAutoText(autoTextNumber))
    {
        for (auto &text : context.GetResourceStrings())
        {
            TextEntry entry = { 0 };
            entry.Text = text;
            results.GetTextComponent().Texts.push_back(entry);
        }
        results.SetAutoTextNumber(autoTextNumber);
    }

    context.FixupSinksAndSources(output, output);

    return !context.HasErrors();
}

const uint16_t MethodCodeOffsetMarker = 0x454d; // "ME" backwards
const uint16_t OffsetToAfterCodePointer = 0x0;

void WriteMethodSelectors(const CSCOObjectClass &oClass, vector<uint8_t> &outputScr, vector<uint16_t> &trackMethodCodePointerOffsets)
{
    // Then the method selectors
    push_word(outputScr, (uint16_t)oClass.GetMethods().size());
    for (uint16_t method : oClass.GetMethods())
    {
        push_word(outputScr, method);
        trackMethodCodePointerOffsets.push_back((uint16_t)outputScr.size());
        push_word(outputScr, MethodCodeOffsetMarker);   // Something temporary for now
    }
}

void WriteMethodCodePointers(const CSCOObjectClass &oClass, vector<uint8_t> &outputScr, CompileContext &context, const vector<uint16_t> &trackMethodCodePointerOffsets, int &index)
{
    for (uint16_t method : oClass.GetMethods())
    {
        string methodName = context.LookupSelectorName(method);
        assert(!methodName.empty()); // Means we have a bug.
        code_pos methodPos = context.GetLocalProcPos(oClass.GetName() + "::" + methodName);
        // Then from the code_pos, we get the offset at which it was written.
        uint16_t offsetOfMethodPointer = trackMethodCodePointerOffsets[index];
        write_word(outputScr, offsetOfMethodPointer, methodPos->get_final_offset());
        index++;
    }
}

const uint16_t SpeciesSelector_SCI1 = 0x1005;

void WriteClassToHeap(const CSCOObjectClass &oClass, bool isInstance, vector<uint8_t> &outputHeap, vector<uint8_t> &outputScr, CompileContext &context, uint16_t &objectSelectorOffsetInScr)
{
    // This is where code that has offsets to this instance should point to
    WORD wObjectPointer = (WORD)outputHeap.size();
    // Indicate where it is so anyone who needs to reference it can:
    context.WroteSource(context.GetTempToken(ValueType::Token, oClass.GetName()), wObjectPointer);

    if (oClass.IsPublic())
    {
        // If it's public, we need to put this in the export table.
        context.TrackPublicInstance(wObjectPointer);
    }

    // Now write out the object
    push_word(outputHeap, 0x1234); // object marker

    // This number includes all the props (include the 9 standard ones)
    uint16_t numProps = (uint16_t)oClass.GetProperties().size();
    push_word(outputHeap, numProps);

    push_word(outputHeap, objectSelectorOffsetInScr);

    objectSelectorOffsetInScr += (isInstance ? 0 : (numProps * 2));
    push_word(outputHeap, objectSelectorOffsetInScr);

    // Increment our offset in the scr file
    // num Methods, plus 4 for each method (see WriteMethodSelectors)
    objectSelectorOffsetInScr += (uint16_t)(2 + 4 * oClass.GetMethods().size());

    push_word(outputHeap, 0);   // REVIEW: Always zero??

    // Now write the prop values. These should start with: species/super/-info-/name, so
    // we'll start writing from property index 5.
    for (uint16_t i = 5; i < numProps; i++)
    {
        auto &prop = oClass.GetProperties()[i];
        WORD value = prop.GetValue();

        // In SCI1, instances always have the species (-script- selector) as 0xffff
        if (prop.GetSelector() == SpeciesSelector_SCI1)
        {
            assert(i == 5);
            if (isInstance)
            {
                value = 0xffff;
            }
        }

        if (prop.NeedsReloc())
        {
            context.WroteSink(value, (uint16_t)outputHeap.size());
            // Add this to the string offsets we're tracking.
        }
        push_word(outputHeap, value);
    }
}

bool GenerateScriptResource_SCI11(Script &script, PrecompiledHeaders &headers, CompileTables &tables, CompileResults &results, bool generateDebugInfo)
{
    vector<BYTE> &outputScr = results.GetScriptResource();
    vector<BYTE> &outputHeap = results.GetHeapResource();

    string gameClassName;
    if (script.GetScriptNumber() == 0)
    {
        for (const auto &classDef : script.GetClasses())
        {
            if (classDef->GetSuperClass() == "Game")
            {
                gameClassName = classDef->GetName();
            }
        }
    }

    // These track the offsets that store pointers to method code.
    vector<uint16_t> trackMethodCodePointerOffsets;

    // Create our "CompileContext", which holds state during the compilation.
    CompileContext context(appState->GetVersion(), script, headers, tables, results.GetLog(), generateDebugInfo);

    CommonScriptPrep(script, context, results);
    // Errors above could mean crashes below. Bail out now.
    if (!context.HasErrors())
    {
        push_word(outputScr, 0);    // This is where "after end of code" goes.
        push_word(outputScr, 0);    // Always zero
        bool preloadText = false;
        push_word(outputScr, preloadText ? 0x1 : 0x0);  // Preload flag (not supported)

        // Now the exports
        // To figure out how many exports we have, let's look at the public procedures and public instances
        // TODO: Retain gaps from old SCO file? If we don't all other files will need to recompile.
        size_t numExports = 0;
        size_t offsetOfExports = 0;
        vector<ExportTableInfo> exportTableOrder = GetExportTableOrder(nullptr, script);
        _Exports_SCI11(script, context, outputScr, exportTableOrder, offsetOfExports);

        // Generate SCO objects for the classes and instances in the script.  We want to do this before generating any code,
        // since some code relies on it.
        GenerateSCOObjects(context, script);
        GenerateSCOPublics(context, script);
        GenerateSCOVariables(context, script);

        // Next, we write out the objects. They consist of parts in both .hep and .scr
        // In .scr come the property and method selectors for the objects
        // Instances have method selectors only.
        // Classes have both.
        // In .hep we have some more basic info about the object. Instances and classes are all smushed together.
        CSCOFile &sco = context.GetScriptSCO();
        sco.SetVersion(SCOVersion::SeparateHeap);

        // Script 21, KQ6-CD, ends up being a corrupt script resource (according to Sierra and SV.exe, although I can't
        // determine why, other than it reads props/methods from the wrong offset in the scr resource)
        // unless instances/objects are written out in the order they appear in the script file.
        // To that end, we'll store our SCO objects in a map, and then enumerate them in the order that the corresponding
        // class def appears in the script.
        // I suspect there is still a bug somewhere that this is simply hiding, as there is no reason why the original
        // way (classes then instances) shouldn't work.
        std::unordered_map<std::string, const CSCOObjectClass*> orderedObjects;
        for (const CSCOObjectClass &oClass : sco.GetObjects())
        {
            orderedObjects[oClass.GetName()] = &oClass;
        }
        for (const CSCOObjectClass &oClass : context.GetInstanceSCOs())
        {
            orderedObjects[oClass.GetName()] = &oClass;
        }

        // We're write the objects' property and method selectors to the scr file first,
        // since the code will follow it, and we need to write the code early.
        uint16_t objectSelectorOffsetInScr = (uint16_t)outputScr.size();
        for (auto &classDef : script.GetClasses())
        {
            size_t beginning = outputScr.size();

            const CSCOObjectClass &oClass = *orderedObjects[classDef->GetName()];
            if (!classDef->IsInstance())
            {
                // Classes have all their prop selectors written out (instances do not)
                for (auto &prop : oClass.GetProperties()) // VERIFY THIS - this DOES include all 9 basic selectors (must match number we wrote to heap)
                {
                    push_word(outputScr, prop.GetSelector());
                }
            }
            WriteMethodSelectors(oClass, outputScr, trackMethodCodePointerOffsets);

            results.Stats.Objects += (int)(outputScr.size() - beginning);
        }

        // Now comes the code:
        WORD wStartOfCode = 0;
        _Section2_Code(script, context, outputScr, wStartOfCode, true, results);
        // Now we know how much code was written, so let's write the offset to that.
        assert(outputScr.size() < 0xffff);
        write_word(outputScr, OffsetToAfterCodePointer, (uint16_t)outputScr.size());

        // Now we know where the method code is, so go back and write in those pointers
        int index = 0;
        for (auto &classDef : script.GetClasses())
        {
            WriteMethodCodePointers(*orderedObjects[classDef->GetName()], outputScr, context, trackMethodCodePointerOffsets, index);
        }

        // Now let's start writing the hep file
        push_word(outputHeap, 0);   // This will point to "after strings" 
        // Next come the local var values
        _Section10_LocalVariables(script, context, outputHeap, true, results);

        for (auto &classDef : script.GetClasses())
        {
            size_t beginning = outputHeap.size();
            WriteClassToHeap(*orderedObjects[classDef->GetName()], classDef->IsInstance(), outputHeap, outputScr, context, objectSelectorOffsetInScr);
            results.Stats.Objects += (int)(outputHeap.size() - beginning);
        }

        // Now it appears there is a zero marker in the heap, after the objects
        push_word(outputHeap, 0);

        _Section5_Strings(context, outputScr, outputHeap, false, results);

        // At the beginning of the file, write the offset to after string table:
        write_word(outputHeap, 0, (uint16_t)outputHeap.size());

        // Now write the string relocation table
        context.WriteOutOffsetsOfHepPointersInHep(outputHeap);
        // The .hep file is done

        // Now back to the scr:
        // Now let's write the "after code" stuff. 
        // This is a list of offsets (in .scr) that pointer to offsets in .hep.
        // It will consist first of any exports that are classes/instances, then any lofsa/lofss offsets in the code
        context.WriteOutOffsetsOfHepPointersInScr(outputScr);
        // We're now done with .scr

        _Section7_Exports_Part2(context, outputScr, wStartOfCode, exportTableOrder, offsetOfExports);

        // Fixups time!
        context.FixupSinksAndSources(outputScr, outputHeap);

        // Get the .sco file produced.
        results.GetSCO() = context.GetScriptSCO();

        // Fill the text resource.
        uint16_t autoTextNumber;
        if (context.IsAutoText(autoTextNumber))
        {
            for (auto &text : context.GetResourceStrings())
            {
                TextEntry entry = { 0 };
                entry.Text = text;
                results.GetTextComponent().Texts.push_back(entry);
            }
            results.SetAutoTextNumber(autoTextNumber);
        }
    }

    return !context.HasErrors();
}

bool GenerateScriptResource(SCIVersion version, sci::Script &script, PrecompiledHeaders &headers, CompileTables &tables, CompileResults &results, bool generateDebugInfo)
{
    if (version.SeparateHeapResources)
    {
        return GenerateScriptResource_SCI11(script, headers, tables, results, generateDebugInfo);
    }
    else
    {
        return GenerateScriptResource_SCI0(script, headers, tables, results, generateDebugInfo);
    }
}