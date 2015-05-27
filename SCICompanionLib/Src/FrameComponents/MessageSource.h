#pragma once

class MessageHeaderFile;
typedef std::pair<std::string, uint16_t> MessageDefine;

class MessageSource
{
public:
    MessageSource(MessageHeaderFile *file);

    const std::vector<MessageDefine> &GetDefines() const { return _defines; }
    void SetName(size_t index, const std::string &newName);
    void SetValue(size_t index, uint16_t newValue);
    int IndexOf(const std::string &name) const;
    int IndexOf(uint16_t value) const;
    size_t AddDefine(const std::string &newName, uint16_t newValue);
    void DeleteDefine(size_t index);
    void Commit();
    std::string ValueToName(uint16_t value) const;
    uint16_t NameToValue(const std::string &name) const;

    std::string MandatoryPrefix;
    std::string Name;   // Includes the prefix

private:
    std::vector<MessageDefine> _defines;
    MessageHeaderFile *_file;
    bool _dirty;
};