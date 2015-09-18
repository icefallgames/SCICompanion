#pragma once

struct ToolTipResult
{
    // Is the tooltip empty (e.g. no result)
    bool empty() { return strTip.empty(); }

    // The tip
    std::string strTip;

    // Line number, text, and scriptid for "goto" information
    int iLineNumber;
    ScriptId scriptId;
    std::string strBaseText;

    std::string OriginalText;
};
