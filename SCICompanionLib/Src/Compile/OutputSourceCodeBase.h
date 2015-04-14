#pragma once

class OutputSourceCodeBase : public sci::ISyntaxNodeVisitor
{
public:
    OutputSourceCodeBase(sci::SourceCodeWriter &out) : out(out) {}

protected:
    sci::SourceCodeWriter &out;

    template<typename _T>
    void Forward(_T &things)
    {
        for (const auto &thing : things)
        {
            thing->Accept(*this);
            if (out.fInline)
            {
                // If these are all inline, add whitespace between them.
                out.out << " ";
            }
        }
    }

    template<typename _T>
    void Forward(_T &things, const char *pszSeparator)
    {
        bool first = true;
        for (const auto &thing : things)
        {
            if (!first && out.fInline)
            {
                out.out << pszSeparator;
            }
            thing->Accept(*this);
            first = false;
        }
    }

    template<typename _T>
    void Forward2(_T &things)
    {
        for (const auto &thing : things)
        {
            thing.Accept(*this);
            if (out.fInline)
            {
                // If these are all inline, add whitespace between them.
                out.out << " ";
            }
        }
    }

    template<typename _T>
    void ForwardOptionalSection(const char *sectionName, _T &things)
    {
        if (!things.empty())
        {
            out.out << "(" << sectionName;
            {
                DebugIndent indent(out);
                out.out << endl;
                for (const auto &thing : things)
                {
                    thing->Accept(*this);
                    if (out.fInline)
                    {
                        // If these are all inline, add whitespace between them.
                        out.out << " ";
                    }
                }
                out.out << endl;
            }
            out.out << ")";
        }
    }
};