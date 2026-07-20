/*
 * nocrt0
 * C entry point for GUI application (WinMainCRTStartup)
 * Tested with GCC/MinGW, Pelles C
 */


/** Build instructions:

    -D_UNICODE = compile 'unicode' version instead of 'ansi'
    -DNOSTDLIB = also compile internal implementation of _alloca and __chkstk

**/


#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#endif // UNICODE

#include <stddef.h>
#include <tchar.h>


//
// external function prototypes
//
#if defined(STRICT)
#define _hinstance  HINSTANCE
#define _hmodule    HMODULE
#else
#define _hinstance  void*
#define _hmodule    void*
#endif // STRICT
extern int __stdcall WinMain(_hinstance, _hinstance, char*, int);
extern int __stdcall wWinMain(_hinstance, _hinstance, wchar_t*, int);
__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned);
__declspec(dllimport) char* __stdcall GetCommandLineA(void);
__declspec(dllimport) wchar_t* __stdcall GetCommandLineW(void);
__declspec(dllimport) _hmodule __stdcall GetModuleHandleA(const char*);
__declspec(dllimport) _hmodule __stdcall GetModuleHandleW(const wchar_t*);

#if defined(_UNICODE)
#define MANGLE_w(name)      w##name
#define MANGLE_uw(name)     _##w##name
#define MANGLE_uuw(name)    __##w##name
#define MANGLE_AW(name)     name##W
#else
#define MANGLE_w(name)      name
#define MANGLE_uw(name)     _##name
#define MANGLE_uuw(name)    __##name
#define MANGLE_AW(name)     name##A
#endif // _UNICODE

#if !defined(_tWinMainCRTStartup)
#if defined(__TINYC__)
#define _tWinMainSRTStartup MANGLE_uw(winstart)
#else
#define _tWinMainCRTStartup MANGLE_w(WinMainCRTStartup)
#endif // __TINYC__
#endif // _tWinMainCRTStartup
#if !defined(_tWinMain)
#define _tWinMain           MANGLE_w(WinMain)
#endif // _tWinMain
#if !defined(GetCommandLine)
#define GetCommandLine      MANGLE_AW(GetCommandLine)
#endif // GetCommandLine
#if !defined(GetModuleHandle)
#define GetModuleHandle     MANGLE_AW(GetModuleHandle)
#endif // GetModuleHandle


#if defined(NOSTDLIB)
#if defined(__GNUC__)
// reference implementation of _alloca() etc.
#if defined(__amd64__)
__asm__(
    ".intel_syntax noprefix     \n"
    ".global ___chkstk_ms       \n"
    ".global __alloca, ___chkstk\n"
    "___chkstk_ms:              \n"
    "push rcx                   \n"
    "push rax                   \n"
    "cmp rax, 0x1000            \n"
    "lea rcx, [rsp + 24]        \n"
    "jb 2f                      \n"
    "1: sub rcx, 0x1000         \n"
    "or dword ptr [rcx], 0      \n"
    "sub rax, 0x1000            \n"
    "cmp rax, 0x1000            \n"
    "ja 1b                      \n"
    "2: sub rcx, rax            \n"
    "or dword ptr [rcx], 0      \n"
    "pop rax                    \n"
    "pop rcx                    \n"
    "ret                        \n"
    "__alloca:                  \n"
    "mov rax, rcx               \n"
    "___chkstk:                 \n"
    "pop r11                    \n"
    "mov r10, rsp               \n"
    "cmp rax, 0x1000            \n"
    "jb 2f                      \n"
    "1: sub r10, 0x1000         \n"
    "or dword ptr [r10], 0      \n"
    "sub rax, 0x1000            \n"
    "cmp rax, 0x1000            \n"
    "ja 1b                      \n"
    "2: sub r10, rax            \n"
    "or dword ptr [r10], 0      \n"
    "mov rax, rsp               \n"
    "mov rsp, r10               \n"
    "push r11                   \n"
    "ret                        \n"
    ".att_syntax prefix"
);
#elif defined(__i386__)
__asm__(
    ".intel_syntax noprefix     \n"
    ".global ___chkstk_ms       \n"
    ".global __alloca, ___chkstk\n"
    "___chkstk_ms:              \n"
    "push ecx                   \n"
    "push eax                   \n"
    "cmp eax, 0x1000            \n"
    "lea ecx, [esp + 12]        \n"
    "jb 2f                      \n"
    "1: sub ecx, 0x1000         \n"
    "or dword ptr [ecx], 0      \n"
    "sub eax, 0x1000            \n"
    "cmp eax, 0x1000            \n"
    "ja 1b                      \n"
    "2: sub ecx, eax            \n"
    "or dword ptr [ecx], 0      \n"
    "pop eax                    \n"
    "pop ecx                    \n"
    "ret                        \n"
    "__alloca:                  \n"
    "___chkstk:                 \n"
    "push ecx                   \n"
    "lea ecx, [esp + 8]         \n"
    "cmp eax, 0x1000            \n"
    "jb 2f                      \n"
    "1: sub ecx, 0x1000         \n"
    "or dword ptr [ecx], 0      \n"
    "sub eax, 0x1000            \n"
    "cmp eax, 0x1000            \n"
    "ja 1b                      \n"
    "2: sub ecx, eax            \n"
    "or dword ptr [ecx], 0      \n"
    "mov eax, esp               \n"
    "mov esp, ecx               \n"
    "mov ecx, [eax]             \n"
    "push [eax + 4]             \n"
    "ret                        \n"
    ".att_syntax prefix"
);
#endif
#endif // __GNUC__
#endif // NOSTDLIB


__declspec(noreturn)
void _tWinMainCRTStartup(void)
{
    ExitProcess(_tWinMain(GetModuleHandle(NULL), NULL, GetCommandLine(),
        1 /*SW_SHOWNORMAL*/));
}


#if defined(__GNUC__)
void __main(void) {}
int atexit(void (*func)(void)) { (void)func; return -1; }
#endif // __GNUC__
