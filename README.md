nocrt0
======

Summary
-------
Custom C startup code (mainCRTStartup) implementation for GCC/MinGW.

Description
-----------
_Nocrt0_ is a custom C startup code without any dependencies of the standard
C runtime libraries. It is intended for building very small executable files
under OS Windows(R). _Nocrt0_ was successfully tested with _GCC/MinGW 7.2.0_
and _Pelles C 8.0_. It is also possible that _nocrt0_ could be adapted for
other C compilers, including Microsoft Visual C, as well, however, the source
code requires some of C99 language features, so it cannot be compiled with
_msvc_ directly.

Using nocrt0
============

There are two modules: _nocrt0w_ for GUI applications, and _nocrt0c_ for
console applications. Simply add the correct source into your project and build
it without linking to the default C startup code (hint: for GCC you will need
`nostdlib` or `nostartfiles` option).

The process of building _nocrt0_ can also be controlled with the following DEFINEs:

    -D_UNICODE
    -DARGV={own | msvcrt | none}

`_UNICODE` builds "wide" version of the startup code (`wmain` or `wWinMain`).

`ARGV` is used for a console application (nocrt0c) only. It selects the type of
the underlying implementation of the standard command-line parser (argc/argv).
`own` (default) is the built-in implementation (note: `envp` is not supported and
is always set to NULL); `msvcrt` makes use of an external function `__getmainargs()`
(which is normally imported from _msvcrt.dll_); `none` is intended for applications
which do not need any command-line processing (`int main(void)`).


GCC note: _nocrt0_ includes the internal assembly (both x86 and x86-64) implementation
of `__alloca` function. That means you shouldn't link your application with _libgcc.a_.
If it's undesirable you have to patch the source code first.
