/*
 * Proj: nocrt0
 * Auth: matveyt
 * Desc: C entry point for GUI application (WinMainCRTStartup)
 * Note: Tested with GCC/MinGW, Pelles C
 */


/** Build instructions:

    -D_UNICODE = compiles 'unicode' version instead of 'ansi'
    -DNOSTDLIB = also compiles internal implementation of _alloca and __chkstk

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
#define _hinstance HINSTANCE
#define _hmodule HMODULE
#else
#define _hinstance void*
#define _hmodule void*
#endif // STRICT
extern int __stdcall WinMain(_hinstance, _hinstance, char*, int);
extern int __stdcall wWinMain(_hinstance, _hinstance, wchar_t*, int);
__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned);
__declspec(dllimport) char* __stdcall GetCommandLineA(void);
__declspec(dllimport) wchar_t* __stdcall GetCommandLineW(void);
__declspec(dllimport) _hmodule __stdcall GetModuleHandleA(const char*);
__declspec(dllimport) _hmodule __stdcall GetModuleHandleW(const wchar_t*);

#if defined(_UNICODE)
#define MANGLE_w(name) w##name
#define MANGLE_uw(name) _##w##name
#define MANGLE_uuw(name) __##w##name
#define MANGLE_AW(name) name##W
#else
#define MANGLE_w(name) name
#define MANGLE_uw(name) _##name
#define MANGLE_uuw(name) __##name
#define MANGLE_AW(name) name##A
#endif // _UNICODE

#if !defined(_tWinMainCRTStartup)
#if defined(__TINYC__)
#define _tWinMainSRTStartup MANGLE_uw(winstart)
#else
#define _tWinMainCRTStartup MANGLE_w(WinMainCRTStartup)
#endif // __TINYC__
#endif // _tWinMainCRTStartup
#if !defined(_tWinMain)
#define _tWinMain MANGLE_w(WinMain)
#endif // _tWinMain
#if !defined(GetCommandLine)
#define GetCommandLine MANGLE_AW(GetCommandLine)
#endif // GetCommandLine
#if !defined(GetModuleHandle)
#define GetModuleHandle MANGLE_AW(GetModuleHandle)
#endif // GetModuleHandle


#if defined(NOSTDLIB)
#if defined(__GNUC__)
// reference implementation of _alloca() etc.
#if defined(__amd64__)
__asm__(
    ".global ___chkstk_ms, __alloca, ___chkstk\n"
    "___chkstk_ms:pushq %rcx\n"
    "pushq %rax\n"
    "cmpq $0x1000, %rax\n"
    "leaq 24(%rsp), %rcx\n"
    "jb 2f\n"
    "1:subq $0x1000, %rcx\n"
    "orl $0, (%rcx)\n"
    "subq $0x1000, %rax\n"
    "cmpq $0x1000, %rax\n"
    "ja 1b\n"
    "2:subq %rax, %rcx\n"
    "orl $0, (%rcx)\n"
    "popq %rax\n"
    "popq %rcx\n"
    "ret\n"
    "__alloca:movq %rcx, %rax\n"
    "___chkstk:popq %r11\n"
    "movq %rsp, %r10\n"
    "cmpq $0x1000, %rax\n"
    "jb 2f\n"
    "1:subq $0x1000, %r10\n"
    "orl $0, (%r10)\n"
    "subq $0x1000, %rax\n"
    "cmpq $0x1000, %rax\n"
    "ja 1b\n"
    "2:subq %rax, %r10\n"
    "orl $0, (%r10)\n"
    "movq %rsp, %rax\n"
    "movq %r10, %rsp\n"
    "pushq %r11\n"
    "ret\n"
);
#elif defined(__i386__)
__asm__(
    ".global ___chkstk_ms, __alloca, ___chkstk\n"
    "___chkstk_ms:pushl %ecx\n"
    "pushl %eax\n"
    "cmpl $0x1000, %eax\n"
    "leal 12(%esp), %ecx\n"
    "jb 2f\n"
    "1:subl $0x1000, %ecx\n"
    "orl $0, (%ecx)\n"
    "subl $0x1000, %eax\n"
    "cmpl $0x1000, %eax\n"
    "ja 1b\n"
    "2:subl %eax, %ecx\n"
    "orl $0, (%ecx)\n"
    "popl %eax\n"
    "popl %ecx\n"
    "ret\n"
    "__alloca:\n"
    "___chkstk:pushl %ecx\n"
    "leal 8(%esp), %ecx\n"
    "cmpl $0x1000, %eax\n"
    "jb 2f\n"
    "1:subl $0x1000, %ecx\n"
    "orl $0, (%ecx)\n"
    "subl $0x1000, %eax\n"
    "cmpl $0x1000, %eax\n"
    "ja 1b\n"
    "2:subl %eax, %ecx\n"
    "orl $0, (%ecx)\n"
    "movl %esp, %eax\n"
    "movl %ecx, %esp\n"
    "movl (%eax), %ecx\n"
    "pushl 4(%eax)\n"
    "ret\n"
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
