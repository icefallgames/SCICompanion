#include "stdafx.h"
#include <numeric>
#include "CompileInterfaces.h"
#include "CompileContext.h"
#include "SCO.h"
#include "ScriptOMAll.h"
#include "Text.h"
#include "AppState.h"

using namespace std;
using namespace sci;

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

string::size_type ComputeStringLengthFirst(string::size_type sumSoFar, const stringcode_map::value_type &thePair)
{
    // Return the length of the string, including a nullptr terminator
    return sumSoFar + (thePair.first.size() + 1);
}

string::size_type ComputeStringLengthSecond(string::size_type sumSoFar, const std::map<std::string, std::string>::value_type &thePair)
{
    // Return the length of the string, including a nullptr terminator
    return sumSoFar + (thePair.second.size() + 1);
}

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

//
// Final fixups for saids, strings and instance references.
//
void _FixupReferencesHelper(SCIVersion version, CompileContext &context, vector<BYTE> &output, const ref_and_index_multimap &references, const string &name, WORD wPosInResource)
{
    // For all "name"s in references, ask the code_pos where its final position was, and get its post instruction
    // position.  Calc the diff of the wPosInResource with that post instruction position, and, then change the value in output.
    ref_and_index_multimap::const_iterator refIt = references.find(name);
    while (refIt != references.end())
    {
        // It's a multimap, so there could be multiple keys of "name"
        if (refIt->first == name)
        {
            const code_pos &instruction = refIt->second.first;
            WORD wIndex = refIt->second.second;
            assert((instruction->get_opcode() == Opcode::LOFSA) || (instruction->get_opcode() == Opcode::LOFSS));
            context.AddHeapPointerOffset(instruction->get_final_offset() + 1);  // +1 to skip the opcode

            WORD wValue;
            if (version.lofsaOpcodeIsAbsolute)
            {
                wValue = wPosInResource; // This is the number we write to the resource.
            }
            else
            {
                WORD wFinalPostOp = instruction->get_final_postop_offset();
                wValue = wPosInResource - wFinalPostOp; // This is the number we write to the resource.
            }
            wValue += wIndex;                            // Oh, and the index too...
            // Where do we write it?  One byte after the start of the instruction (opcode is 1 byte)
            WORD wWhere = instruction->get_final_offset() + 1;
            write_word(output, wWhere, wValue);
        }
        ++refIt;
    }
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
    if (fInstance)
    {
        // So tell that code to point here.
        _FixupReferencesHelper(pContext->GetVersion(), *pContext, output, pContext->GetInstanceReferences(), object.GetName(), wObjectPointer);
        if (object.IsPublic())
        {
            // If it's public, we need to put this in the export table.
            pContext->TrackPublicInstance(wObjectPointer);
        }
    }

    // Property selectors and values
    for (const auto &objectProperty : properties)
    {
        WORD wValue = objectProperty.GetValue();
        if (objectProperty.NeedsReloc())
        {
            pContext->TrackRelocation((WORD)output.size());
            // If it needs a relocation, it's an offset, for which we current only have a temp
            // value.  Get the real value.
            wValue = pContext->LookupFinalStringOrSaidOffset(wValue);
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
    const vector<CSCOMethod> &methods = object.GetMethods();
    // WORD: the number of methods
    push_word(output, (WORD)methods.size());
    // n*WORD: the method selectors
    for (const CSCOMethod &scoMethod : methods)
    {
        push_word(output, scoMethod.GetSelector());
    }

    // WORD: zero
    push_word(output, 0x0000);
    // n*WORD: the method function pointers
    string className = object.GetName();
    for (const CSCOMethod &scoMethod : methods)
    {
        string methodName = pContext->LookupSelectorName(scoMethod.GetSelector());
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
void ParseSaidWord(CompileContext *pContext, string &word, const stringcode_map::value_type &stringCode, vector<BYTE> &output)
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
        if (pContext->LookupWord(word, wWordGroup))
        {
            assert((wWordGroup >> 8) < 0xf0); // Otherwise it's treated as a said token
            // Note, we can't use push_word, because this must be in big-endian notation.
            output.push_back(wWordGroup >> 8);
            output.push_back(wWordGroup & 0xff);
        }
        else
        {
            pContext->ReportError(stringCode.second, "'%s' is not in the vocabulary.", word.c_str());
        }
    }
    word.clear();
}

//
// Streams said tokens described by stringCode, to the output.
//
void ParseSaidString(CompileContext *pContext, const stringcode_map::value_type &stringCode, vector<BYTE> &output)
{
    static string c_saidTokens = ",&/()[]#<>"; // 0xf0 to 0xf9
    string::const_iterator saidIt = stringCode.first.begin();
    string word; // We build up this word as we scan the string passed in.
    while (saidIt != stringCode.first.end())
    {
        size_t x = c_saidTokens.find(*saidIt);
        if (x < c_saidTokens.size())
        {
            // It was a token - if we've got a word ready, look it up now, and add its vocab index
            ParseSaidWord(pContext, word, stringCode, output);
            // Then add the token
            output.push_back(0xf0 + (BYTE)x);
        }
        else
        {
            // Just a regular letter (or non token)... add it to our word.
            word.push_back(*saidIt);
        }
        saidIt++;
    }
    // In case we had something in "word", parse it now...
    ParseSaidWord(pContext, word, stringCode, output);
    output.push_back(0xff); // terminator
}



//
// Write all class and instance sections to the script resource output stream
//
void _Section1And6_ClassesAndInstances(vector<BYTE> &output, CompileContext *pContext)
{
    CSCOFile &sco = pContext->GetScriptSCO();
    for (CSCOObjectClass &oClass : sco.GetObjects())
    {
        _WriteClassOrInstance(oClass, false, output, pContext);
    }

    // Instances
    for (CSCOObjectClass &iClass : pContext->GetInstanceSCOs())
    {
        _WriteClassOrInstance(iClass, true, output, pContext);
    }
}

void _ResolveLocalVariables(Script &script, CompileContext &context, bool resolveNow)
{
    for (auto &scriptVar : script.GetScriptVariables())
    {
        for (auto &segment : scriptVar->GetStatements())
        {
            PropertyValue *pValue = SafeSyntaxNode<PropertyValue>(segment->GetSyntaxNode());
            if (pValue && pValue->GetType() == ValueType::String)
            {
                uint16_t temp = context.GetStringTempOffset(pValue->GetStringValue());
                if (resolveNow)
                {
                    uint16_t value = context.LookupFinalStringOrSaidOffset(temp);
                    pValue->SetValue(value);
                }
                else
                {
                    pValue->SetValue(temp);
                }
                context.ScriptVariableValueNeedsReloc.insert(pValue);
            }
        }
    }
}

void _Section10_LocalVariables(Script &script, CompileContext &context, vector<BYTE> &output, bool separateHeapResource, vector<uint16_t> *trackHeapStringOffsets = nullptr)
{
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
                const PropertyValue *pValue = SafeSyntaxNode<PropertyValue>(value->GetSyntaxNode());
                assert(pValue); // Must be a property value.
                if (context.ScriptVariableValueNeedsReloc.find(pValue) != context.ScriptVariableValueNeedsReloc.end())
                {
                    context.TrackRelocation((uint16_t)output.size());
                    if (trackHeapStringOffsets)
                    {
                        // SCI1 uses a different mechanism to track these (need to differentiate heap vs scr relocations)
                        trackHeapStringOffsets->push_back((uint16_t)output.size());
                    }
                }
                push_word(output, pValue->GetNumberValue());
                size++;
            }
            // Fill any remaining spots with 0
            WORD wZeroFill = (var->GetSize() - (WORD)size);
            output.insert(output.end(), wZeroFill * 2, 0); // WORD = BYTE * 2
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
}

void _Section2_Code(Script &script, CompileContext &context, vector<BYTE> &output, WORD &wStartOfCode, bool separateHeapResources)
{
    context.PushVariableLookupContext(&script); // Don't really need to pop this, ever.
    const ProcedureVector &procs = script.GetProcedures();
    for_each(procs.begin(), procs.end(), GenericOutputByteCode2<ProcedureDefinition>(context));
    // b) classes, methods
    const ClassVector &classes = script.GetClasses();
    for_each(classes.begin(), classes.end(), GenericOutputByteCode2<ClassDefinition>(context));
    // Now some dirty work...
    context.code().fixup_offsets(context.GetOffsetFixups());
    context.FixupLocalCalls();
    context.FixupAsmLabelBranches();

    WORD wCodeSize = context.code().calc_size() + 4;
    bool fRoundUp = make_even(wCodeSize);
    if (!separateHeapResources)
    {
        // Write the section header and size.
        push_word(output, 2);
        push_word(output, wCodeSize);
    }

    wStartOfCode = (WORD)output.size(); // Store where the code begins
    context.code().write_code(output);
    zero_pad(output, fRoundUp);
}

void _Section3_Synonyms(Script &script, CompileContext &context, vector<BYTE> &output)
{
    const SynonymVector &synonyms = script.GetSynonyms();
    if (!synonyms.empty())
    {
        push_word(output, 3); // 3 = synonyms
        push_word(output, (WORD)(synonyms.size() * 4) + 4); // 4 bytes per synonym entry

        for (auto &synonym : synonyms)
        {
            WORD wGroup = 0;
            if (!context.LookupWord(synonym->MainWord, wGroup))
            {
                context.ReportError(synonym.get(), "'%s' is not in the vocabulary.", synonym->MainWord.c_str());
            }
            push_word(output, wGroup); // Write anyway, so we can continue...
            if (!context.LookupWord(synonym->Replacement, wGroup))
            {
                context.ReportError(synonym.get(), "'%s' is not in the vocabulary.", synonym->Replacement.c_str());
            }
            push_word(output, wGroup);
        }
    }
}

void _Section4_Saids(CompileContext &context, vector<BYTE> &output)
{
    stringcode_map &saids = context.GetSaids();
    if (!saids.empty())
    {
        push_word(output, 4);
        size_t saidSizeIndex = output.size();
        push_word(output, 0); // Temporary size...
        // Now actually write out the saids into the stream.
        stringcode_map::iterator saidIt = saids.begin();
        while (saidIt != saids.end())
        {
            // a) Get the index to which we'll write the said stream.
            WORD wAbsolute = (WORD)output.size();
            context.SpecifyFinalSaidOffset(saidIt->first, wAbsolute);
            _FixupReferencesHelper(context.GetVersion(), context, output, context.GetSaidReferences(), saidIt->first, wAbsolute);
            // b) Parse the said stream
            ParseSaidString(&context, *saidIt, output);
            // c) update the offset...
            ++saidIt;
        }
        // How much room did that take?
        WORD wSaidSectionSize = (WORD)(output.size() - saidSizeIndex) + 2;
        // Round it up to a WORD boundary and pad if necessary:
        zero_pad(output, make_even(wSaidSectionSize));
        // Go back and write the size.
        write_word(output, saidSizeIndex, wSaidSectionSize);
    }
}

void _Section5_Strings(CompileContext &context, vector<BYTE> &outputScr, vector<BYTE> &outputHeap, bool writeSCI0SectionHeader)
{
    // In code strings:
    stringcode_map &inCodestrings = context.GetInCodeStrings();
    // Declared strings:
    map<string, string> &declaredTokenToString = context.GetDeclaredStrings();
    const ref_and_index_multimap &declaredTokenToCodePos = context.GetStringTokenReferences();
    if (!inCodestrings.empty() || !declaredTokenToString.empty())
    {
        // Compute the length of all the strings.
        WORD wStringSectionSize = accumulate(inCodestrings.begin(), inCodestrings.end(), 0, ComputeStringLengthFirst);
        // For declaredStrings, it's the second of the pair that we're concerned with.
        wStringSectionSize += accumulate(declaredTokenToString.begin(), declaredTokenToString.end(), 0, ComputeStringLengthSecond);

        // Round it up to a WORD boundary:
        bool fRoundUp = make_even(wStringSectionSize);

        if (writeSCI0SectionHeader)
        {
            push_word(outputHeap, 5);
            // Indicate the section size:
            push_word(outputHeap, wStringSectionSize + 4);
        }

        // Now actually write the strings, and update the parts of the code with the absolute positions
        // of each string (not yet relative pos's).
        // REVIEW: does this need to be a pointer?  References aren't allowed... does that mean it's already a ref?
        // Can't get an algorithm to work for this.

        // First, do it for the in-code strings:
        {
            stringcode_map::iterator stringIt = inCodestrings.begin();
            while (stringIt != inCodestrings.end())
            {
                // Get the index to which we'll write the string.
                WORD wAbsolute = (WORD)outputHeap.size();
                context.SpecifyFinalStringOffset(stringIt->first, wAbsolute);
                _FixupReferencesHelper(context.GetVersion(), context, outputScr, context.GetStringReferences(), stringIt->first, wAbsolute);
                outputHeap.insert(outputHeap.end(), stringIt->first.begin(), stringIt->first.end());
                outputHeap.push_back(0);
                stringIt++;
            }
        }

        // Finish off with the declared strings.
        {
            auto stringIt = declaredTokenToString.begin();
            while (stringIt != declaredTokenToString.end())
            {
                // Get the index to which we'll write the string.
                WORD wAbsolute = (WORD)outputHeap.size();
                context.SpecifyFinalStringTokenOffset(stringIt->first, wAbsolute);
                _FixupReferencesHelper(context.GetVersion(), context, outputScr, declaredTokenToCodePos, stringIt->first, wAbsolute);
                outputHeap.insert(outputHeap.end(), stringIt->second.begin(), stringIt->second.end());
                outputHeap.push_back(0);
                stringIt++;
            }
        }

        zero_pad(outputHeap, fRoundUp);
    }
}

void _Section7_Exports_Part1(Script &script, CompileContext &context, vector<BYTE> &output, size_t &numExports, size_t &indexOfExports)
{
    numExports = 0;
    const ClassVector &classes = script.GetClasses();
    numExports += count_if(classes.begin(), classes.end(), IsPublicInstance);   // public instances
    const ProcedureVector &procs = script.GetProcedures();
    numExports += count_if(procs.begin(), procs.end(), IsPublicProcedure);      // public procedures
    push_word(output, 7); // 7 = exports
    uint16_t exportWidth = context.GetVersion().IsExportWide() ? 4 : 2;
    push_word(output, (WORD)(numExports * exportWidth) + 4 + 2); // exports + header + exportCount.
    push_word(output, (WORD)numExports); // the number of exports
    indexOfExports = output.size(); // Store where the ptrs will go.
    // Fill with zeroes for now:
    output.insert(output.end(), numExports * exportWidth, 0);
}

const uint16_t ExportTempMarker = 0x5845;   // "EX" backwards

void _Exports_SCI11(Script &script, CompileContext &context, vector<BYTE> &output, size_t &numExports, size_t &indexOfExports)
{
    numExports = 0;
    for (const auto &theClass : script.GetClasses())
    {
        bool isGameClass = ((script.GetScriptNumber() == 0) && (theClass->GetSuperClass() == "Game"));
        if (isGameClass || (theClass->IsPublic() && theClass->IsInstance()))
        {
            // Track his heap pointer:
            context.AddHeapPointerOffset(2 + (numExports * 2) + (uint16_t)output.size());
            numExports++;
        }
    }

    const ProcedureVector &procs = script.GetProcedures();
    numExports += count_if(procs.begin(), procs.end(), IsPublicProcedure);      // public procedures
    assert(!context.GetVersion().IsExportWide());
    push_word(output, (WORD)numExports); // the number of exports
    indexOfExports = output.size(); // Store where the ptrs will go.
    // Fill with dumb markers for now:
    for (size_t i = 0; i < numExports; i++)
    {
        push_word(output, ExportTempMarker);
    }
}

void _Section7_Exports_Part2(CompileContext &context, vector<BYTE> &output, WORD wStartOfCode, size_t numExports, size_t indexOfExports)
{
    // Now code  and classes have been written to the stream.  It's time to set up the exports table, which starts back at
    // indexOfExports.  We need to take one additional step for the main script.
    // There are two types of exports:
    // 1) public procedures (common)
    // 2) public instances (generally 1 per script).
    // public instances need to come first - as the interpreter looks for the first export, and may assume its
    // a room/locale/rgn, or in the case of script 0, the Game instance.
    // In here, we write the offset from the begining of the resource to the beginning of the code.
    vector<code_pos> &exports = context.GetExports();
    vector<WORD> &instanceOffsets = context.GetPublicInstanceOffsets();
    uint16_t exportWidth = context.GetVersion().IsExportWide() ? 4 : 2;
    // We already filled the thing with zeroes, so we can just write a uint16_t, and increment by 4.
    if (numExports == (exports.size() + instanceOffsets.size())) // Otherwise we generate a corrupt resource!
    {
        // Public instances first.
        for (WORD instanceOffset : instanceOffsets)
        {
            write_word(output, indexOfExports, instanceOffset);
            indexOfExports += exportWidth;
        }
        // Then procedures.
        for (code_pos procEntry : exports)
        {
            WORD wCodeOffset = context.code().offset_of(procEntry);
            write_word(output, indexOfExports, wStartOfCode + wCodeOffset);
            indexOfExports += exportWidth;
        }
    }
    else
    {
        // We're not putting the exports in, so it's an incomplete scritp resource - we'd better have some errors.
        assert(context.HasErrors());
    }
}

void _Section8_RelocationTable(CompileContext &context, vector<BYTE> &output)
{
    const vector<WORD> &relocations = context.GetRelocations();
    push_word(output, 8);
    push_word(output, 6 + (WORD)relocations.size() * 2);  // section size
    push_word(output, (WORD)relocations.size());    // # of relocation entries
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
    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state)
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
                SingleStatement *caseValue = caseStatement.GetCaseValue();
                if (caseValue && (caseValue->GetType() == NodeTypeProcedureCall))
                {
                    ProcedureCall *pProc = static_cast<ProcedureCall*>(caseValue->GetSegment());
                    if (ProcedureUnknown == _context.LookupProc(pProc->GetName()))
                    {
                        // This isn't a procedure call.  "Undo it"
                        // The name becomes a simple token value - let's construct that.
                        unique_ptr<ComplexPropertyValue> pTokenValue = std::make_unique<ComplexPropertyValue>();
                        pTokenValue->SetValue(pProc->GetName(), ValueType::Token);
                        // ...and the parameters become the code statements. So steal the procedures parameters.
                        SingleStatementVector formerParams;
                        pProc->StealParams(formerParams);
                        // Replace the casevalue's procedure, with our "simple token value"
                        // whose parameters we have now stolen.
                        caseValue->SetSyntaxNode(std::move(pTokenValue)); // This will delete pProc.
                        // Finally, add back the statements.  In case there is already some code in the
                        // case statement, we'll make sure to insert ours at the beginning.
                        SingleStatementVector &existingCode = caseStatement.GetCodeSegments();
                        existingCode.insert(existingCode.begin(), std::make_move_iterator(formerParams.begin()), std::make_move_iterator(formerParams.end()));
                    }
                }
            }
        }
    }

    CompileContext &_context;
};


//
// Given an array of FunctionSignatures, put them in the SCO function signature object.
//
void _CreateSCOFunctionSignature(CompileContext &context, std::vector<CSCOFunctionSignature> &scoSigs, const FunctionSignatureVector &signatures)
{
    for (auto &signature : signatures)
    {
        // Return type
        CSCOFunctionSignature scoSig;
        SpeciesIndex wReturnType = DataTypeAny;
#if ENABLE_TYPED
        SpeciesIndex wReturnType = 0;
        if (!context.LookupTypeSpeciesIndex(signature->GetDataType(), wReturnType))
        {
            context.ReportError(signature.get(), "Unknown return type: %s.", signature->GetDataType().c_str());
        }
#endif
        scoSig.SetReturnType(wReturnType.Type());

        auto &params = signature->GetParams();
        for_each(params.begin(), params.end(),
            [&context, &scoSig, &signature](const unique_ptr<FunctionParameter> &param)
        {
            SpeciesIndex wParamType = DataTypeAny;
#if ENABLE_TYPED
            SpeciesIndex wParamType;
            if (!context.LookupTypeSpeciesIndex(param->GetDataType(), wParamType))
            {
                context.ReportError(signature.get(), "Unknown type for parameter %s: %s.", param->GetName().c_str(), param->GetDataType().c_str());
            }
#endif
            scoSig.AddParameterType(wParamType.Type());
        }
        );

        scoSig.SetAdditionalParametersOk(signature->GetMoreParametersAllowed()); // Additional params
        scoSig.SetRequiredParameterCount(static_cast<WORD>(signature->GetRequiredParameterCount()));
        scoSigs.push_back(scoSig);
    }
}


//
// Adds the script's public instances and procedures to the sco file being compiled by the context
//
void GenerateSCOPublics(CompileContext &context, const Script &script)
{
    WORD wIndex = 0;
    const ClassVector &classes = script.GetClasses();
    for (auto &classDef : script.GetClasses())
    {
        if (classDef->IsPublic() && classDef->IsInstance())
        {
            CSCOPublicExport pe(classDef->GetName(), wIndex);
            SpeciesIndex si = DataTypeAny;;
            if (!context.LookupTypeSpeciesIndex(classDef->GetSuperClass(), si))
            {
                context.ReportError(classDef.get(), "Unknown superclass: %s.", classDef->GetSuperClass().c_str());
            }
            pe.SetInstanceSpecies(si);
            context.AddSCOPublics(pe);
            ++wIndex;
        }
    }
    const ProcedureVector &procs = script.GetProcedures();
    for (auto &pProc : procs)
    {
        CSCOPublicExport procExport(pProc->GetName(), wIndex);
        // Store the function signature(s).
        std::vector<CSCOFunctionSignature> signatures;
        _CreateSCOFunctionSignature(context, signatures, pProc->GetSignatures());
        procExport.SetSignatures(signatures);
        if (pProc->IsPublic())
        {
            context.AddSCOPublics(procExport);
            ++wIndex;
        }
        // Also add a fake one, even if it's not public - we'll use this for function signature matching.
        context.AddFakeSCOPublic(procExport);
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

        context.AddSCOVariable(CSCOLocalVariable(pVar->GetName(), wTypeSpecies.Type()));
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
        string selectorName = classProperty.GetName();
        if (!context.LookupSelector(selectorName, wSelectorIndex))
        {
            if (pClass->IsInstance())
            {
                // This error is not comprehensive - the user might have added a new property
                // on an instance, and there is already a selector for this property.  Hence, the caller
                // of this function needs to compare against the species props too.
                context.ReportError(&classProperty, "Unknown property '%s'.  Instances can not define new properties.", selectorName.c_str());
            }
            else
            {
                // We'll add a new selector for this.
                context.DefineNewSelector(selectorName, wSelectorIndex);
            }
        }
        // Then the value - defines should already be resolved.
        bool fTrackRelocation = false;
        PropertyValue value = classProperty.GetValue();
        switch (value.GetType())
        {
            case ValueType::Number:
                wValue = value.GetNumberValue();
                break;
            case ValueType::String: // For now, strings are ok in property lists
                wValue = context.GetStringTempOffset(value.GetStringValue());
                fTrackRelocation = true;
                break;
            case ValueType::Said:
                wValue = context.GetSaidTempOffset(value.GetStringValue());
                fTrackRelocation = true;
                break;
            case ValueType::Selector:
                if (!context.LookupSelector(value.GetStringValue(), wValue))
                {
                    context.ReportError(&value, "Unknown selector '%s'.", value.GetStringValue().c_str());
                }
                break;
                // FEATURE: possibly support pointers here.
            default:
                context.ReportError(&value, "%s is not allowed as a property value.", value.ToString().c_str());
                break;
        }

        // For classes, see if the property has a type, and make sure it makes sense.
        if (!pClass->IsInstance())
        {
            std::string type = classProperty.GetDataType();
            if (!type.empty())
            {
                if (!context.LookupTypeSpeciesIndex(type, wType))
                {
                    context.ReportError(&classProperty, "Unknown type '%s' for property '%s'.", type.c_str(), selectorName.c_str());
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
            speciesProps[8].wValue = context.GetStringTempOffset(classDef->GetName()); // name (can be overridden explicitly too)
            nameIndex = 8;
        }
        else
        {
            assert(speciesProps.size() >= 4);
            speciesProps[0].wValue = sco.GetSpecies();
            speciesProps[1].wValue = wSuperClass; // superclass index
            speciesProps[2].wValue = classDef->IsInstance() ? 0x0000 : 0x8000; // --info--
            speciesProps[3].wValue = context.GetStringTempOffset(classDef->GetName()); // name (can be overridden explicitly too)
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

                // REVIEW: we could also avoid the error if DataTypeAny, perhaps? Is DataTypeAny ever used in cpp?
                if (context.GetLanguage() == LangSyntaxCpp)
                {
                    // Make sure the user didn't do this, since the base class already specified the type.
                    if (overriddenIt->wType != DataTypeNone) // (Assumed that it's always DataTypeNone for instances.)
                    {
                        assert(!classDef->IsInstance()); // Had better be a class if the user specified a data type.
                        string propName = context.LookupSelectorName(overriddenIt->wSelector);
                        context.ReportError(classDef.get(), "The base class '%s' already has a type for '%s'.", classDef->GetSuperClass().c_str(), propName.c_str());
                    }
                }
            }

            if (classDef->IsInstance())
            {
                // Don't care about type for instances
                scoProperties.push_back(CSCOObjectProperty(speciesProp.wSelector, wValue, fTrackRelocation));
            }
            else
            {
                // For classes, make sure no type was specified.  BUT!!!! Re-use the type we know from
                // the super class (just for convenience).
                scoProperties.push_back(CSCOObjectProperty(speciesProp.wSelector, wValue, speciesProp.wType, fTrackRelocation));
            }
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
                if (newProp.wSelector < 3)
                {
                    // Someone tried to override a default property
                    context.ReportError(classDef.get(), "species, superclass, and -info- may not be overridden.");
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
                        // Also, this had better have a type.
                        if (newProp.wType == DataTypeNone)
                        {
                            // Get some info for the error.
                            string propName = context.LookupSelectorName(newProp.wSelector);
                            ClassPropertyVector::const_iterator propIt = match_name2(classDef->GetProperties().begin(), classDef->GetProperties().end(), propName);
                            // propIt should always be valid.
                            context.ReportError(&(*propIt), "Property '%s' is not defined on the base class.  A data type must be supplied.", propName.c_str());
                        }
                        scoProperties.push_back(CSCOObjectProperty(newProp.wSelector, newProp.wValue, newProp.wType, newProp.fTrackRelocation));
                    }
                }
            }
        }
        sco.SetProperties(scoProperties);

        // Now methods
        vector<CSCOMethod> methodsOut;
        for (auto &method : classDef->GetMethods())
        {
            const string &methodName = method->GetName();
            CSCOMethod theMethod;
            theMethod.SetSelector(context.LookupSelectorAndAdd(methodName)); // Add it if it doesn't exist.

            // Add the signatures:
            std::vector<CSCOFunctionSignature> signatures;
            _CreateSCOFunctionSignature(context, signatures, method->GetSignatures());
            theMethod.SetSignatures(signatures);

            methodsOut.push_back(theMethod);
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
    // Do some prescans (script number must already be set!)
    script.PreScan(context);
    // Ok, now we should have been told about all the saids and strings.
    if (script.Language() == LangSyntaxSCIStudio)
    {
        // Fix up case statements we may have mis-interpreted.
        FixCaseStatements hack(context);
        script.Traverse(nullptr, hack);
    }
}

bool GenerateScriptResource_SCI0(Script &script, PrecompiledHeaders &headers, CompileTables &tables, CompileResults &results)
{
    vector<BYTE> &output = results.GetScriptResource();

    // Create our "CompileContext", which holds state during the compilation.
    CompileContext context(appState->GetVersion(), script, headers, tables, results.GetLog());

    _Section3_Synonyms(script, context, output);

    CommonScriptPrep(script, context, results);

    // To figure out how many exports we have, let's look at the public procedures and public instances
    size_t numExports = 0;
    size_t indexOfExports = 0;
    _Section7_Exports_Part1(script, context, output, numExports, indexOfExports);

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
    _Section2_Code(script, context, output, wStartOfCode, false);

    _Section5_Strings(context, output, output, true);

    _Section4_Saids(context, output);

    _Section1And6_ClassesAndInstances(output, &context);

    _Section7_Exports_Part2(context, output, wStartOfCode, numExports, indexOfExports);

    _ResolveLocalVariables(script, context, true);
    _Section10_LocalVariables(script, context, output, false);

    _Section8_RelocationTable(context, output);

    // Finish off by writing 0x0000
    push_word(output, 0);

    // Get the .sco file produced.
    results.GetSCO() = context.GetScriptSCO();

    // Fill the text resource.
    const vector<string> &resourceStrings = context.GetResourceStrings();
    for (auto &text : resourceStrings)
    {
        TextEntry entry = { 0 };
        entry.Text = text;
        results.GetTextComponent().Texts.push_back(entry);
    }

    return !context.HasErrors();
}

const uint16_t MethodCodeOffsetMarker = 0x454d; // "ME" backwards
const uint16_t OffsetToAfterCodePointer = 0x0;

void WriteMethodSelectors(const CSCOObjectClass &oClass, vector<uint8_t> &outputScr, vector<uint16_t> &trackMethodCodePointerOffsets)
{
    // Then the method selectors
    push_word(outputScr, (uint16_t)oClass.GetMethods().size());
    for (auto &method : oClass.GetMethods())
    {
        push_word(outputScr, method.GetSelector());
        trackMethodCodePointerOffsets.push_back((uint16_t)outputScr.size());
        push_word(outputScr, MethodCodeOffsetMarker);   // Something temporary for now
    }
}

void WriteMethodCodePointers(const CSCOObjectClass &oClass, vector<uint8_t> &outputScr, CompileContext &context, const vector<uint16_t> &trackMethodCodePointerOffsets, int &index)
{
    for (auto &method : oClass.GetMethods())
    {
        string methodName = context.LookupSelectorName(method.GetSelector());
        assert(!methodName.empty()); // Means we have a bug.
        code_pos methodPos = context.GetLocalProcPos(oClass.GetName() + "::" + methodName);
        // Then from the code_pos, we get the offset at which it was written.
        uint16_t offsetOfMethodPointer = trackMethodCodePointerOffsets[index];
        write_word(outputScr, offsetOfMethodPointer, methodPos->get_final_offset());
        index++;
    }
}

const uint16_t SpeciesSelector_SCI1 = 0x1005;

void WriteClassToHeap(const CSCOObjectClass &oClass, bool isInstance, bool isGameClass, vector<uint8_t> &outputHeap, vector<uint8_t> &outputScr, CompileContext &context, vector<uint16_t> &trackHeapStringOffsets, uint16_t &objectSelectorOffsetInScr)
{
    // This is where code that has offsets to this instance should point to
    WORD wObjectPointer = (WORD)outputHeap.size();
    if (isInstance)
    {
        // So tell that code to point here.
        _FixupReferencesHelper(context.GetVersion(), context, outputScr, context.GetInstanceReferences(), oClass.GetName(), wObjectPointer);
    }

    if ((isInstance && oClass.IsPublic()) || isGameClass)
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
            context.TrackRelocation((uint16_t)outputHeap.size());
            // We haven't written out strings yet, so we don't know the final value.
            // value = context.LookupFinalStringOrSaidOffset(value);
            // But add this to the string offsets we're tracking.
            trackHeapStringOffsets.push_back((uint16_t)outputHeap.size());
            // Afterwards, we will look at these places, suck out the value, and then replace with the final offset.
        }
        push_word(outputHeap, value);
    }
}

bool GenerateScriptResource_SCI11(Script &script, PrecompiledHeaders &headers, CompileTables &tables, CompileResults &results)
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

    // These contain the offsets in hep at which there are string pointers.
    vector<uint16_t> trackHeapStringOffsets;

    // These track the offsets that store pointers to method code.
    vector<uint16_t> trackMethodCodePointerOffsets;

    // Create our "CompileContext", which holds state during the compilation.
    CompileContext context(appState->GetVersion(), script, headers, tables, results.GetLog());

    CommonScriptPrep(script, context, results);

    push_word(outputScr, 0);    // This is where "after end of code" goes.
    push_word(outputScr, 0);    // Always zero
    bool preloadText = false;
    push_word(outputScr, preloadText ? 0x1 : 0x0);  // Preload flag (not supported)

    // Now the exports
    // To figure out how many exports we have, let's look at the public procedures and public instances
    // TODO: Retain gaps from old SCO file? If we don't all other files will need to recompile.
    size_t numExports = 0;
    size_t indexOfExports = 0;
    _Exports_SCI11(script, context, outputScr, numExports, indexOfExports);

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

    // We're write the objects' property and method selectors to the scr file first,
    // since the code will follow it, and we need to write the code early.
    uint16_t objectSelectorOffsetInScr = (uint16_t)outputScr.size();
    for (const CSCOObjectClass &oClass : sco.GetObjects())
    {
        // Classes have all their prop selectors written out (instances do not)
        for (auto &prop : oClass.GetProperties()) // VERIFY THIS - this DOES include all 9 basic selectors (must match number we wrote to heap)
        {
            push_word(outputScr, prop.GetSelector());
        }
        WriteMethodSelectors(oClass, outputScr, trackMethodCodePointerOffsets);
    }
    // Now instances
    for (const CSCOObjectClass &oClass : context.GetInstanceSCOs())
    {
        WriteMethodSelectors(oClass, outputScr, trackMethodCodePointerOffsets);
    }

    // Now comes the code:
    WORD wStartOfCode = 0;
    _Section2_Code(script, context, outputScr, wStartOfCode, true);
    // Now we know how much code was written, so let's write the offset to that.
    assert(outputScr.size() < 0xffff);
    write_word(outputScr, OffsetToAfterCodePointer, (uint16_t)outputScr.size());

    // Now we know where the method code is, so go back and write in those pointers
    int index = 0;
    for (const CSCOObjectClass &oClass : sco.GetObjects())
    {
        WriteMethodCodePointers(oClass, outputScr, context, trackMethodCodePointerOffsets, index);
    }
    for (const CSCOObjectClass &oClass : context.GetInstanceSCOs())
    {
        WriteMethodCodePointers(oClass, outputScr, context, trackMethodCodePointerOffsets, index);
    }

    // Now let's start writing the hep file
    push_word(outputHeap, 0);   // This will point to "after strings" 
    // Next come the local var values
    _ResolveLocalVariables(script, context, false);
    _Section10_LocalVariables(script, context, outputHeap, true, &trackHeapStringOffsets);

    for (const CSCOObjectClass &oClass : sco.GetObjects())
    {
        // The class that inherits from Game is special, it goes in the export table too.
        bool isGameClass = !gameClassName.empty() && (oClass.GetName() == gameClassName);
        WriteClassToHeap(oClass, false, isGameClass, outputHeap, outputScr, context, trackHeapStringOffsets, objectSelectorOffsetInScr);
    }
    for (const CSCOObjectClass &oClass : context.GetInstanceSCOs())
    {
        WriteClassToHeap(oClass, true, false, outputHeap, outputScr, context, trackHeapStringOffsets, objectSelectorOffsetInScr);
    }

    
    // Now it appears there is a zero marker in the heap, after the objects
    push_word(outputHeap, 0);

    // TODO: What about the isntance and class names? Are they tracked?
    _Section5_Strings(context, outputScr, outputHeap, false);

    // At the beginning of the file, write the offset to after string table:
    write_word(outputHeap, 0, (uint16_t)outputHeap.size());

    // Now write the string relocation table
    // TODO: How do we know about in-code strings? Do we care?
    push_word(outputHeap, (uint16_t)trackHeapStringOffsets.size());
    for (uint16_t stringPointerOffset : trackHeapStringOffsets)
    {
        push_word(outputHeap, stringPointerOffset);
        // We also need to fill in the values (which are temporaries) with actual pointers to the strings.
        write_word(outputHeap, stringPointerOffset, context.LookupFinalStringOrSaidOffset(read_word(outputHeap, stringPointerOffset)));
    }
    // The .hep file is done

    // Now back to the scr:
    // Now let's write the "after code" stuff. 
    // This is a list of offsets (in .scr) that pointer to offsets in .hep.
    // It will consist first of any exports that are classes/instances, then any lofsa/lofss offsets in the code
    const vector<uint16_t> &heapPointerOffsets = context.GetHeapPointerOffsets();
    push_word(outputScr, (uint16_t)heapPointerOffsets.size());
    for (uint16_t heapPointerOffset : heapPointerOffsets)
    {
        push_word(outputScr, heapPointerOffset);
    }
    // We're now done with .scr

    // TODO: Write our prop relocs
    // TODO GenerateSCOObjects ... this is where CSCOpropertys are marked as needing relocation. name is hard-coded, we'll need others too. Like "up" and "down" in Gauge.sc
    // TODO: keep exports in sync with previous SCO file iteration
    _Section7_Exports_Part2(context, outputScr, wStartOfCode, numExports, indexOfExports);

    // Get the .sco file produced.
    results.GetSCO() = context.GetScriptSCO();

    // Fill the text resource.
    const vector<string> &resourceStrings = context.GetResourceStrings();
    for (auto &text : resourceStrings)
    {
        TextEntry entry = { 0 };
        entry.Text = text;
        results.GetTextComponent().Texts.push_back(entry);
    }

    return !context.HasErrors();
}


bool GenerateScriptResource(SCIVersion version, sci::Script &script, PrecompiledHeaders &headers, CompileTables &tables, CompileResults &results)
{
    if (version.SeparateHeapResources)
    {
        return GenerateScriptResource_SCI11(script, headers, tables, results);
    }
    else
    {
        return GenerateScriptResource_SCI0(script, headers, tables, results);
    }
}