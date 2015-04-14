#include "stdafx.h"
#include "SummarizeScript.h"

using namespace std;

PCSTR el(bool fRN) { return  fRN ? "\r\n" : "\n"; }

const char c_szDivider[] = "------------------------------";

const uint16_t bytesPerLine = 16;


void DebugOut(const CompiledObjectBase &object, std::ostream &out, bool fRN)
{
    if (object.IsPublic)
    {
        out << "public ";
    }
    out << (object.IsInstance() ? "instance" : "class") << "@$" << object.GetPosInResource() << el(fRN);

    const vector<uint16_t> &propertySelectors = object.GetProperties();
    const vector<uint16_t> &propertyValues = object.GetPropertyValues();

    out << "properties:" << el(fRN);
    out << "    species: " << object.GetSpeciesIfClass() << el(fRN);
    out << "    super: " << object.GetSuperClass() << el(fRN);
    out << "    name: " << object.GetName() << el(fRN);
    out << "    -info-: " << object.GetInfo() << el(fRN);

    vector<uint16_t>::const_iterator selIt = propertySelectors.begin();
    vector<uint16_t>::const_iterator valueIt = propertyValues.begin();
    for (; selIt != propertySelectors.end() && valueIt != propertyValues.end(); ++selIt, ++valueIt)
    {
        out << "    " << *selIt << " : " << *valueIt << el(fRN);
    }

    const vector<uint16_t> &functionSelectors = object.GetMethods();
    const vector<uint16_t> &functionOffsetsTO = object.GetMethodCodePointersTO();

    out << el(fRN) << "methods:" << el(fRN);
    vector<uint16_t>::const_iterator funcIt = functionSelectors.begin();
    vector<uint16_t>::const_iterator offIt = functionOffsetsTO.begin();
    for (; funcIt != functionSelectors.end() && offIt != functionOffsetsTO.end(); ++funcIt, ++offIt)
    {
        out << "    " << *funcIt << " @" << *offIt << el(fRN);
    }
    out << el(fRN);
}


void DebugOut(const CompiledScript &script, std::ostream &out, bool fRN)
{
    out << "Script: " << script.GetScriptNumber() << el(fRN) << el(fRN);

    out << (uint16_t)script._localVars.size() << " script variables." << el(fRN) << c_szDivider << el(fRN);

    out << (uint16_t)script._exportsTO.size() << " exports." << el(fRN) << c_szDivider << el(fRN);
    vector<uint16_t>::const_iterator it = script._exportsTO.begin();
    out << hex;
    for (; it != script._exportsTO.end(); ++it)
    {
        out << "    $" << (*it) << el(fRN);
    }
    out << el(fRN);
    out << dec;
    out << (uint16_t)script._objects.size() << " objects." << el(fRN) << c_szDivider << el(fRN);
    out << hex;

    for (auto &object : script._objects)
    {
        DebugOut(*object, out, fRN);
    }

    // More raw stuff
    out << el(fRN) << c_szDivider << el(fRN);
    out << hex;
    for (auto ss : script._rawScriptSections)
    {
        out << el(fRN);
        out << "[" << setw(4) << setfill('0') << ss.offset << "]: [" << setw(4) << setfill('0') << ss.type << "] [" << setw(4) << setfill('0') << ss.length << "]" << el(fRN);

        // Now print out the raw bytes
        uint16_t wStart = ss.offset + 4; // 4 bytes taken up by header
        uint16_t wLength = ss.length - 4;
        uint16_t wEnd = wStart + wLength;
        for (uint16_t wOffset = wStart; wOffset < wEnd; wOffset += bytesPerLine)
        {
            for (uint16_t i = 0; (i < 16) && (wOffset < wEnd); i++, wOffset++)
            {
                out << setw(2) << setfill('0') << static_cast<int>(script.GetRawBytes()[wOffset]) << " ";
            }
            out << el(fRN);
        }
    }
}