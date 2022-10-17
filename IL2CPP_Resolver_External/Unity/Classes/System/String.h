#pragma once

namespace Unity
{
    /*
        Convert To String, Unity String
    */
    std::string Make_String(System_String* m_str)
    {
        System_String m_String;

        um::ReadMemory(m_str, &m_String);

        std::wstring str(m_String.m_wString);

        // 65001 CP_UTF8
        int ANSIStringLength = WideCharToMultiByte(65001, 0, str.c_str(), m_String.m_iLength + 1, 0, 0, 0, 0);

        char* m_Buffer = (char*)malloc(ANSIStringLength);

        WideCharToMultiByte(65001, 0, str.c_str(), m_String.m_iLength + 1, m_Buffer, ANSIStringLength, 0, 0);

        std::string nstr(m_Buffer);

        free(m_Buffer);

        return nstr;
    }
}