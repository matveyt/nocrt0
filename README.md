nocrt0
======

Summary
-------

Custom C startup code (mainCRTStartup) implementation for GCC/MinGW.

Description
-----------

*Nocrt0* is a custom C startup code without any dependencies of the standard C runtime
libraries. It is intended for building very small executable files under OS Windows(R).
*Nocrt0* was successfully tested with *GCC/MinGW* and *Pelles C*. It is also possible
that *nocrt0* could be adapted for other C compilers, including Microsoft Visual C, as
well, however, the source code requires some of C99 language features, so it cannot be
compiled with *msvc* directly.

Using nocrt0
------------

There are two modules: *nocrt0w* for GUI applications, and *nocrt0c* for console
applications. Simply add the correct file into your project and build it without linking
to the default C startup code (hint: for GCC you will need `-nostartfiles` or `-nostdlib`
option). Alternatively, you can #include it right into any of your other C source files.

The process of building *nocrt0* can also be controlled with the following DEFINEs:

    -D_UNICODE
    -DNOSTDLIB
    -DARGV={builtin | msvcrt | shell32 | none}

`-D_UNICODE` builds "wide" version of the startup code (`wmain` or `wWinMain`).

`-DNOSTDLIB` when defined compiles built-in implementation of `_alloca()` and
`__chkstk()`. You might need it if you don't link with *libgcc.a*.

`-DARGV` is used for a console application (nocrt0c) only. It selects the type of the
underlying implementation of the standard command-line parser (argc/argv). `builtin`
(default) is the built-in implementation; `msvcrt` makes use of an external function
`__getmainargs()` (which is normally imported from *msvcrt.dll*); `shell32` relies on
`CommandLineToArgvW()` function from *shell32.dll*; `none` is for applications that do
not need any command-line processing (`int main(void)`).
