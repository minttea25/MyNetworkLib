#pragma once

NAMESPACE_OPEN(NetCore);

class Utils
{
public:
    /// <summary>
    /// Convert a wide Unicode string to an UTF8 string
    /// </summary>
    static inline std::string ToString(const std::wstring& wstr)
    {
        if (wstr.empty()) return std::string();

        int32 size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        
        std::string str(size_needed, 0);
        
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
        
        return str;
    }

    static inline std::string ToString(const wchar_t* wchar)
    {
        if (wchar == nullptr) return std::string();

        return ToString(std::wstring(wchar));
    }
};

NAMESPACE_CLOSE;

