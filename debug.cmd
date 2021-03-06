@ECHO OFF

SETLOCAL

:: Ensure prerequisites are met.
IF [%ROOTDIR%] EQU [] (
    CALL setenv.cmd
)

:: Process command-line arguments passed to the script.
:Process_Argument
IF [%1] EQU [] GOTO Default_Arguments
IF /I "%1" == "target1" SET TARGET_OUTPUT=target1.exe
SHIFT
GOTO Process_Argument

:: Default any unspecified command-line arguments.
:Default_Arguments
IF [%TARGET_OUTPUT%] EQU [] (
    ECHO No target specified; expected "target1", ... Debugging "target1.exe"
    SET TARGET_OUTPUT=target1.exe
)
IF NOT EXIST "%OUTPUTDIR%" (
    ECHO Output directory "%OUTPUTDIR%" not found; building...
    CALL build.cmd debug
)
IF EXIST "%OUTPUTDIR%\%TARGET_OUTPUT%" (
    start devenv /debugexe "%OUTPUTDIR%\%TARGET_OUTPUT%"
    ENDLOCAL
    EXIT /b 0
) ELSE (
    ECHO Build failed; aborting debug session.
    GOTO Abort_Debug
)

:Abort_Debug
ENDLOCAL
EXIT /b 1

