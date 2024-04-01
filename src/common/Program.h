#pragma once

#include <vector>
#include <string>
#include <iostream>

extern int Main(const std::vector<std::string>& args);

#ifdef _WIN32
    #include <Windows.h>

    LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs)
    {
        int retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, nullptr, 0);
        if (!SUCCEEDED(retval))
            return nullptr;

        auto* lpWideCharStr = new WCHAR[retval];// (LPWSTR)malloc(retval * sizeof(WCHAR));

        retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
        if (!SUCCEEDED(retval))
        {
            free(lpWideCharStr);
            return nullptr;
        }

        int numArgs;
        auto* args = CommandLineToArgvW(lpWideCharStr, &numArgs);
        delete[] lpWideCharStr;
        if (!args)
            return nullptr;

        int storage = numArgs * sizeof(LPSTR);
        for (int i = 0; i < numArgs; ++ i)
        {
            auto lpUsedDefaultChar = FALSE;
            retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, nullptr, 0, nullptr, &lpUsedDefaultChar);
            if (!SUCCEEDED(retval))
            {
                LocalFree(args);
                return nullptr;
            }

            storage += retval;
        }

        auto* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
        if (!result)
        {
            LocalFree(args);
            return nullptr;
        }

        int bufLen = storage - numArgs * sizeof(LPSTR);
        LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
        for (int i = 0; i < numArgs; ++ i)
        {
            BOOL lpUsedDefaultChar = FALSE;
            retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, nullptr, &lpUsedDefaultChar);
            if (!SUCCEEDED(retval))
            {
                LocalFree(result);
                LocalFree(args);
                return nullptr;
            }

            result[i] = buffer;
            buffer += retval;
            bufLen -= retval;
        }

        LocalFree(args);

        *pNumArgs = numArgs;
        return result;
    }

    #define NewMain() int CALLBACK WinMain(                             \
            HINSTANCE hInstance, HINSTANCE hPrevInstance,               \
            LPSTR lpCmdLine, int nCmdShow                               \
        )                                                               \
    {                                                                   \
        int argc;                                                       \
        const auto* argv = CommandLineToArgvA(GetCommandLine(), &argc); \
        std::vector<std::string> args;                                  \
        for (int i = 0; i < argc; i++)                                  \
        {                                                               \
            args.emplace_back(std::string(argv[i]));                    \
        }                                                               \
        return Main(args);                                              \
    }

#else

    #define TremaMainNewMain() int main(int argc, char* argv[])         \
    {                                                                   \
        std::vector<std::string> args;                                  \
        for (int i = 0; i < argc; i ++)                                 \
        {                                                               \
            args.emplace_back(std::string(argv[i]));                    \
        }                                                               \
        return Main(args);                                              \
    }

#endif

NewMain();
