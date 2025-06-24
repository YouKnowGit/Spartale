#pragma once
#include <string>
#include <Windows.h>

namespace StringUtils
{
    // 인라인 함수
    // std::string(UTF-8)을 std::wstring(유니코드)으로 변환
    inline std::wstring ConvertToWstring(const std::string& str)
    {
        // 빈 문자열은 바로 반환
        if (str.empty())
        {
            return std::wstring();
        }

        // 필요한 wstring 버퍼의 크기를 계산
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);

        // 버퍼를 할당하고 변환을 수행
        std::wstring wstrTo(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], sizeNeeded);

        return wstrTo;
    }
}