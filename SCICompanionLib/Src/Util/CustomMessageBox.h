#pragma once

class MessageBoxCustomization
{
public:
    MessageBoxCustomization() = delete;
    MessageBoxCustomization(const MessageBoxCustomization &src) = delete;
    MessageBoxCustomization(const std::vector<std::pair<int, std::string>> &subs);
    ~MessageBoxCustomization();

    std::vector<std::pair<int, std::string>> Substitutions;
    
    // TODO: Add more as needed
    static const int Yes = 6;
    static const int No = 7;

private:
    bool _unhook;
};