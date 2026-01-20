@echo off
REM Modern C++ Practices Runner Script for Windows
REM Usage: run.bat <sample_number>
REM Examples: run.bat 1, run.bat 01

if "%~1"=="" (
    echo Usage: %0 ^<sample_number^>
    echo Available samples:
    echo   1 or 01: RAII - Resource Acquisition Is Initialization
    echo   2 or 02: SFINAE - Substitution Failure Is Not An Error
    echo   3 or 03: CRTP - Curiously Recurring Template Pattern
    echo   4 or 04: PIMPL - Pointer to Implementation
    echo   5 or 05: RuleOfFive - Rule of Five
    echo   6 or 06: TypeErasure - Type Erasure
    echo   7 or 07: VariantVisitor - Variant and Visitor pattern
    echo   8 or 08: SmartPointers - Smart Pointers usage
    echo   9 or 09: ExceptionSafety - Exception Safety guarantees
    echo   10: MoveSemantics - Move Semantics
    echo.
    echo For name-based running, use the bash script: bash run.sh ^<name^>
    goto :eof
)

REM Remove leading zeros
set "num=%~1"
:remove_zeros
if "%num:~0,1%"=="0" (
    set "num=%num:~1%"
    goto remove_zeros
)

REM Check if it's a valid number
echo %num%| findstr /r "^[0-9][0-9]*$" >nul
if errorlevel 1 (
    echo Error: Invalid sample '%~1'
    goto :eof
)

REM Check if executable exists
if not exist "build\main" if not exist "build\Debug\main.exe" (
    echo Error: Executable not found. Please build the project first:
    echo   vcpkg install
    echo   cmake -B build -S .
    echo   cmake --build build
    goto :eof
)

REM Run the sample
echo Running sample %num%...
if exist "build\main" (
    build\main %num%
) else if exist "build\Debug\main.exe" (
    build\Debug\main.exe %num%
) else (
    echo Error: Could not find executable
)