/*
 * Proj: nocrt0
 * Auth: matveyt
 * Desc: nostdlib entry point for GUI application (WinMainCRTStartup)
 * Note: Tested with GCC/MinGW-w64, Pelles C
 */


/** Build instructions:

    -D_UNICODE = compiles 'unicode' version instead of 'ansi'
    -DNODEFAULTLIBS = also compiles own implementation of _alloca and __chkstk

**/


#include <stddef.h>


//
// external function prototypes
//
extern void __declspec(noreturn) __stdcall ExitProcess(unsigned long);
extern char* __stdcall GetCommandLineA(void);
extern wchar_t* __stdcall GetCommandLineW(void);
extern void* __stdcall GetModuleHandleA(char*);
extern void* __stdcall GetModuleHandleW(wchar_t*);
extern int __stdcall WinMain(void*, void*, char*, int);
extern int __stdcall wWinMain(void*, void*, wchar_t*, int);


#ifdef _UNICODE
#define GetCommandLine GetCommandLineW
#define GetModuleHandle GetModuleHandleW
#define _tWinMainCRTStartup wWinMainCRTStartup
#define _tWinMain wWinMain
#else
#define GetCommandLine GetCommandLineA
#define GetModuleHandle GetModuleHandleA
#define _tWinMainCRTStartup WinMainCRTStartup
#define _tWinMain WinMain
#endif // _UNICODE


#ifdef NODEFAULTLIBS
#ifdef __GNUC__
// reference implementation of _alloca() etc.
#if defined(__amd64__)
__asm__(
    ".global ___chkstk_ms, __alloca, ___chkstk, ___main\n"
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
    "___main:ret\n"
);
#elif defined(__i386__)
__asm__(
    ".global ___chkstk_ms, __alloca, ___chkstk, ___main\n"
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
    "___main:ret\n"
);
#endif
#endif // __GNUC__
#endif // NODEFAULTLIBS


__declspec(noreturn)
void _tWinMainCRTStartup(void)
{
    ExitProcess(_tWinMain(GetModuleHandle(NULL), NULL, GetCommandLine(),
        1 /*SW_SHOWNORMAL*/));
}
