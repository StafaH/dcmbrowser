::@echo off

:: Uncomment to debug the vsvarsall call
set VSCMD_DEBUG=1
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

 :: %~dp0 Cool way of getting the d = drive and p = path name of the current batch file.
set BuildDir=%~dp0..\build
 
if not exist %BuildDir% mkdir %BuildDir%
 
pushd %BuildDir%

:: delete pdb file and get rid of output errors
:: del *.pdb > NUL 2> NUL
 
:: compiler flags:
:: /Zi enable debugging information
:: /FC use full path in diagnostics
:: /I look for other include directories
:: /W# Warnings
set CommonCompilerFlags=/O2 /nologo /Zi /FC /I"..\deps\include" /W4 /MD -DIMGUI_IMPL_OPENGL_LOADER_GLEW
set CommonLinkerFlags=user32.lib gdi32.lib winmm.lib opengl32.lib kernel32.lib shell32.lib glu32.lib
set GLFWLibPath=/LIBPATH:..\deps\lib\GLFW\lib-vc2017 glfw3.lib
set GLEWLibPath=..\deps\lib\GLEW\glew32s.lib ..\deps\lib\GLEW\glew32.lib

:: Code and libraries directory path
set CodePath=..\code\*.cpp
set ImGuiPath=..\code\imgui\*.cpp

cl %CommonCompilerFlags% %CodePath% %ImGuiPath% /link %CommonLinkerFlags% %GLEWLibPath% %GLFWLibpath% /Fe:"dcmbrowser"
 
:: Copy dependencies if needed
 
popd