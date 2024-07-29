/*
 * Proj: nocrt0
 * Auth: matveyt
 * Desc: C entry point for console application (mainCRTStartup)
 * Note: Tested with GCC/MinGW, Pelles C
 */


/** Build instructions:

    -D_UNICODE = compiles 'unicode' version instead of 'ansi'
    -DNOSTDLIB = also compiles internal implementation of _alloca and __chkstk
    -DARGV={builtin | msvcrt | shell32 | none} = selects argv[] implementation:
        - builtin = internal implementation (default)
        - msvcrt = import from msvcrt.dll
        - shell32 = import from shell32.dll (-D_UNICODE only)
        - none = 'int main(void)' only

**/


#if __STDC_VERSION__ < 199901L && !defined(__POCC__)
#error C99 compiler required.
#endif

#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#endif // UNICODE

#include <stddef.h>
#include <tchar.h>


// how to implement argv[]
#define ARGV_builtin    0   // builtin implementation (default)
#define ARGV_msvcrt     1   // __getmainargs() from msvcrt.dll
#define ARGV_shell32    2   // CommandLineToArgvW() from shell32.dll
#define ARGV_none       3   // no argv[]

#ifndef ARGV
#define ARGV_type ARGV_builtin
#else
#ifndef _CONCAT
#define _CONCAT2(_Token1, _Token2)  _Token1 ## _Token2
#define _CONCAT(_Token1, _Token2)   _CONCAT2(_Token1, _Token2)
#endif // _CONCAT
#define ARGV_type _CONCAT(ARGV_, ARGV)
#endif // ARGV


//
// external function prototypes
//
#if (ARGV_type != ARGV_none)
extern int __cdecl main(int, char**);
extern int __cdecl wmain(int, wchar_t**);
#else
extern int __cdecl main(void);
extern int __cdecl wmain(void);
#endif // ARGV_none
typedef struct { int newmode; } _startupinfo;
__declspec(dllimport) int __cdecl __getmainargs(int*, char***, char***, int,
    _startupinfo*);
__declspec(dllimport) int __cdecl __wgetmainargs(int*, wchar_t***, wchar_t***, int,
    _startupinfo*);
__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned);
__declspec(dllimport) char* __stdcall GetCommandLineA(void);
__declspec(dllimport) wchar_t* __stdcall GetCommandLineW(void);
__declspec(dllimport) wchar_t** __stdcall CommandLineToArgvW(const wchar_t*, int*);

#if defined(_UNICODE)
#define MANGLE_w(name) w##name
#define MANGLE_uuw(name) __##w##name
#define MANGLE_AW(name) name##W
#else
#define MANGLE_w(name) name
#define MANGLE_uuw(name) __##name
#define MANGLE_AW(name) name##A
#endif // _UNICODE

#if !defined(_tgetmainargs)
#define _tgetmainargs MANGLE_uuw(getmainargs)
#endif // _tgetmainargs
#if !defined(_tmainCRTStartup)
#if defined(__GNUC__)
#define _tmainCRTStartup mainCRTStartup
#else
#define _tmainCRTStartup MANGLE_w(mainCRTStartup)
#endif // __GNUC__
#endif // _tmainCRTStartup
#if !defined(_tmain)
#define _tmain MANGLE_w(main)
#endif // _tmain
#if !defined(GetCommandLine)
#define GetCommandLine MANGLE_AW(GetCommandLine)
#endif // GetCommandLine


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


#if (ARGV_type == ARGV_builtin)
typedef struct {
    int argc;       // inout; if argc <= 0 then argv[] and pchBuf[] are unused
    _TCHAR** argv;  // pointers to arguments
    size_t cchBuf;  // inout
    _TCHAR* pchBuf; // buffer for arguments
} ARGS;

static void parse_args(const _TCHAR* pszCmdLine, ARGS* pArgs)
{
    int argc = 0;
    size_t cchBuf = 0;
    _TCHAR* pcout = pArgs->pchBuf;

    for (const _TCHAR* pcin = pszCmdLine; *pcin; ++argc) {
        // skip spaces
        while (*pcin == _T(' ')) ++pcin;
        if (!*pcin)
            break;

        // get arg
        const _TCHAR* cp_start;
        size_t cnt = 0;
        if (*pcin == _T('"') || *pcin == _T('\'')) {
            // quoted arg
            _TCHAR delim = *pcin++;
            cp_start = pcin;
            while (*pcin && *pcin != delim) ++pcin, ++cnt;
            if (*pcin == delim)
                ++pcin;
        } else {
            // unquoted arg
            cp_start = pcin;
            while (*pcin && *pcin != _T(' ')) ++pcin, ++cnt;
        }

        // need (cnt + 1) chars more
        cchBuf += cnt + 1;

        // should we store it?
        if (pArgs->argc > 0) {
            // check for buffer overflow
            if (argc >= pArgs->argc || cchBuf > pArgs->cchBuf) {
                cchBuf -= cnt + 1; // failed to store the last arg
                break;
            }

            // store arg
            // note: do not use memcpy() to prevent import from msvcrt.dll
            pArgs->argv[argc] = pcout;
            if (cnt) do {
                *pcout++ = *cp_start++;
            } while (--cnt);
            *pcout++ = _T('\0');
        }
    }

    // output counters
    pArgs->argc = argc;
    pArgs->cchBuf = cchBuf;
}
#endif // ARGV_builtin


__declspec(noreturn)
void _tmainCRTStartup(void)
{
#if (ARGV_type == ARGV_builtin)
    // get command line
    _TCHAR* pszCmdLine = GetCommandLine();

    // count args
    ARGS args;
    args.argc = 0;
    parse_args(pszCmdLine, &args);

    //assert(args.argc > 0);
    //assert(args.cchBuf > 0);

    // reserve space (VLA)
    _TCHAR* ptr_buf[args.argc + 1];
    _TCHAR char_buf[args.cchBuf];

    // get args
    args.argv = ptr_buf;
    args.pchBuf = char_buf;
    parse_args(pszCmdLine, &args);

    // append NULL
    args.argv[args.argc] = NULL;

    // invoke main and exit
    ExitProcess(_tmain(args.argc, args.argv));

#elif (ARGV_type == ARGV_msvcrt)
    int argc;
    _TCHAR** argv;
    _TCHAR** envp;

    _tgetmainargs(&argc, &argv, &envp, 0, &(_startupinfo){0});
    ExitProcess(_tmain(argc, argv));

#elif (ARGV_type == ARGV_shell32)
#if !defined(_UNICODE)
#error -DARGV=shell32 requires -D_UNICODE.
#endif // _UNICODE
    int argc;
    _TCHAR** argv = CommandLineToArgvW(GetCommandLine(), &argc);
    ExitProcess(_tmain(argc, argv));

#elif (ARGV_type == ARGV_none)
    ExitProcess(_tmain());

#else
#error Unknown ARGV_type.
#endif // ARGV_type
}


#if defined(__GNUC__)
void __main(void) {}
#else
#if (ARGV_type == ARGV_msvcrt)
#pragma comment(lib, "msvcrt.lib")
#elif (ARGV_type == ARGV_shell32)
#pragma comment(lib, "shell32.lib")
#endif // ARGV_type
#endif // __GNUC__
