/*
 * Proj: nocrt0
 * Auth: matveyt
 * Desc: nostdlib entry point for console application (mainCRTStartup)
 * Note: Tested with GCC/MinGW-w64, Pelles C
 */


/** Build instructions:

    -D_UNICODE = compiles 'unicode' version instead of 'ansi'
    -DNODEFAULTLIBS = also compiles own implementation of _alloca and __chkstk
    -DARGV={own | msvcrt | shell32 | none} = selects underlying argv[] implementation:
        - own = built-in implementation (default)
        - msvcrt = import from msvcrt.dll
        - shell32 = import from shell32.dll (-D_UNICODE only)
        - none = intended for 'int main(void)' only, UB otherwise

**/


#if __STDC_VERSION__ < 199901L && !defined(__POCC__)
#error C99 compiler required.
#endif


#include <stddef.h>
#include <tchar.h>


// how to implement argv[]
#define ARGV_own        0   // own implementation (default)
#define ARGV_msvcrt     1   // __getmainargs() from msvcrt.dll
#define ARGV_shell32    2   // CommandLineToArgvW() from shell32.dll
#define ARGV_none       3   // no argv[] (non-standard, may cause UB!)

#ifndef ARGV
#define ARGV_type ARGV_own
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
extern void __declspec(noreturn) __stdcall ExitProcess(unsigned long);
extern char* __stdcall GetCommandLineA(void);
extern wchar_t* __stdcall GetCommandLineW(void);
extern wchar_t** __stdcall CommandLineToArgvW(const wchar_t*, int*);


#if (ARGV_type == ARGV_msvcrt)
typedef struct { int newmode; } _startupinfo;
extern void __getmainargs(int*, char***, char***, int, _startupinfo*);
extern void __wgetmainargs(int*, wchar_t***, wchar_t***, int, _startupinfo*);
extern void __set_app_type(int);
#ifndef __GNUC__
#pragma comment(lib, "msvcrt.lib")
#endif // __GNUC__
#endif // ARGV_msvcrt


#if (ARGV_type == ARGV_none)
extern int main(void);
extern int wmain(void);
#else
extern int main(int, char**, char**);
extern int wmain(int, wchar_t**, wchar_t**);
#endif // ARGV_none


#ifdef _UNICODE
#define GetCommandLine GetCommandLineW
#define CommandLineToArgv CommandLineToArgvW
#define _tgetmainargs __wgetmainargs
#ifdef __GNUC__
#define _tmainCRTStartup mainCRTStartup
#else
#define _tmainCRTStartup wmainCRTStartup
#endif // __GNUC__
#ifndef _tmain
#define _tmain wmain
#endif // _tmain
#else
#define GetCommandLine GetCommandLineA
#define _tgetmainargs __getmainargs
#define _tmainCRTStartup mainCRTStartup
#ifndef _tmain
#define _tmain main
#endif // _tmain
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


#if (ARGV_type == ARGV_own)
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
#endif // ARGV_own


__declspec(noreturn)
void _tmainCRTStartup(void)
{
#if (ARGV_type == ARGV_own)
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
    ExitProcess(_tmain(args.argc, args.argv, &(_TCHAR*){NULL}));

#elif (ARGV_type == ARGV_msvcrt)
    int argc;
    _TCHAR** argv;
    _TCHAR** envp;

    __set_app_type(1); // _CONSOLE_APP
    _tgetmainargs(&argc, &argv, &envp, 0, &(_startupinfo){0});
    ExitProcess(_tmain(argc, argv, envp));

#elif (ARGV_type == ARGV_shell32)
#ifndef _UNICODE
#error -DARGV=shell32 requires -D_UNICODE.
#endif // _UNICODE
    int argc;
    _TCHAR** argv = CommandLineToArgv(GetCommandLine(), &argc);
    ExitProcess(_tmain(argc, argv, &(_TCHAR*){NULL}));

#elif (ARGV_type == ARGV_none)
    ExitProcess(_tmain());

#else
#error Unknown ARGV_type.
#endif // ARGV_type
}
