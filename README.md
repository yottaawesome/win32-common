# Win32 Common

## Intro

This will just be a static lib to hold some common baseline code I find (tediously) repeating frequently across my C++/Win32 repos. This will mainly be error reporting and handling, string utilities, RAII, common utility types and functions, module facades and some UI related types for Win32 windows. Possibly DirectX related stuff, if I can finally settle on a given structure when working on graphics applications.

The code is mostly cleaned up code extracted from my other repos.

## Building

You'll need Visual Studio 2026 with the _Desktop development with C++_ workload installed. I use the preview MSVC toolset, so the _MSVC Build Tools for x64/x86 (Preview)_ Visual Studio component is required, but this is not strictly necessary and can be disabled in the project settings.
