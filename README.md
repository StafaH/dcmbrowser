# dcmbrowser
Lightweight and fast file browser built for quick browsing and organizing of DICOM files on your computer. No more downloading large/complicated programs to locate/move/preview DICOM files.

## Features 
- Scan directories and sub-directories
- Automatically organize and group DICOM files
- Preview the DICOM image
- Copy all files into an organized directory
- Anonymize DICOM series for distribution

![alt text](https://github.com/StafaH/dcmbrowser/blob/master/misc/resources/screenshot_1.PNG "Application Screenshot")

## Build
Windows/MSVC: The project can be built using the build.bat file found in misc. This batch file uses the msvc compiler downloaded alongside visual studio to compile the project. All dependencies and files are included in the repository for simplified building.

Other: To build with another compiler, just compile all the source files in the code folder and include and link with the headers and lib files found in the dependencies folder.

## Dependencies
The following external projects were used in this software, and are acknowledged below
1. [https://dicom.offis.de/dcmtk.php.en](DCMTK) - Used for dicom file loading and manipulation
2. [https://github.com/ocornut/imgui](Dear ImGui) - Immediate Mode UI
3. [https://github.com/native-toolkit/tinyfiledialogs](tinyfiledialogs) - Cross-platform file dialogs

## Usage
Feel free to use this project freely inside your applications, and customize it to suit your needs. It can also be used as a standalone application.

A completed executable is found inside the build folder for standalone use.

## Contribution
Contributions to this project will always be welcome. 
