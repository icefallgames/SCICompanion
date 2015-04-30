#pragma once

enum class DecompilerResultType
{
    Update, // A minor update
    Important, // Important status
    Warning,
    Error
};

class IDecompilerResults
{
public:
    virtual void AddResult(DecompilerResultType type, const std::string &message) = 0;
    virtual bool IsAborted() = 0;
    virtual void InformStats(bool functionSuccessful, int byteCount) = 0;
};