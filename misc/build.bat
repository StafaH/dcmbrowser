@echo off

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
:: /MD Which runtime library to use (Static/Dynamic)
set CommonCompilerFlags=/O2 /nologo /Zi /FC /EHsc /I"..\deps\include" /W4 /MD -DIMGUI_IMPL_OPENGL_LOADER_GLEW 

:: libs ws2_32.lib iphlpapi, ws2_32, netapi32 and wsock32 are needed for dcmtk (as well as others)
:: The order of compilation is also important
set CommonLinkerFlags=user32.lib gdi32.lib winmm.lib opengl32.lib kernel32.lib shell32.lib glu32.lib winspool.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib netapi32.lib wsock32.lib ws2_32.Lib iphlpapi.lib
set GLFWLibPath=/LIBPATH:..\deps\lib\GLFW\lib-vc2017 glfw3.lib
set GLEWLibPath=..\deps\lib\GLEW\glew32s.lib ..\deps\lib\GLEW\glew32.lib
set dcmtkPath=/LIBPATH:..\deps\lib\dcmtk ofstd.lib oflog.lib dcmdata.lib dcmimgle.lib 

:: Code and libraries directory path
set CodePath=..\code\*.cpp
set ImGuiPath=..\code\imgui\*.cpp

cl %CommonCompilerFlags% %CodePath% %ImGuiPath% /link %CommonLinkerFlags% %dcmtkPath% %GLEWLibPath% %GLFWLibpath% /out:"dcmbrowser.exe"
 
:: Copy dependencies if needed
 
popd