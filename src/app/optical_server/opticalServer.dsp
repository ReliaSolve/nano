# Microsoft Developer Studio Project File - Name="opticalServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=opticalServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "opticalServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "opticalServer.mak" CFG="opticalServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "opticalServer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "opticalServer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "opticalServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "C:\nsrg\external\pc_win32\include" /I "..\..\..\..\vrpn" /I "..\..\lib\nmBase" /I "..\..\lib\nmMP" /I "..\nano\lib\nmSEM" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c /Tp
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 tk83.lib tcl83.lib vrpn.lib wsock32.lib spotCamVC.lib SC2_Cam.lib glui32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\..\..\vrpn\pc_win32\Release" /libpath:"C:\nsrg\external\pc_win32\lib"

!ELSEIF  "$(CFG)" == "opticalServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\nsrg\external\pc_win32\include" /I "..\..\..\..\vrpn" /I "..\..\lib\nmBase" /I "..\..\lib\nmMP" /I "..\nano\lib\nmSEM" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c /Tp
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 tk83.lib tcl83.lib vrpn.lib wsock32.lib spotCamVC.lib SC2_Cam.lib glui32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd" /pdbtype:sept /libpath:"..\..\..\..\vrpn\pc_win32\Debug" /libpath:"C:\nsrg\external\pc_win32\lib"

!ENDIF 

# Begin Target

# Name "opticalServer - Win32 Release"
# Name "opticalServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\edax_defs.cpp
# End Source File
# Begin Source File

SOURCE=.\Logging.cpp
# End Source File
# Begin Source File

SOURCE=.\nmm_Microscope_SEM_cooke.cpp
# End Source File
# Begin Source File

SOURCE=.\nmm_Microscope_SEM_diaginc.cpp
# End Source File
# Begin Source File

SOURCE=.\nmm_Microscope_SEM_optical.cpp
# End Source File
# Begin Source File

SOURCE=.\opticalServer.cpp
# End Source File
# Begin Source File

SOURCE=.\OpticalServerInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Tcl_Linkvar.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\edax_defs.h
# End Source File
# Begin Source File

SOURCE=.\Logging.h
# End Source File
# Begin Source File

SOURCE=.\nmm_Microscope_SEM_cooke.h
# End Source File
# Begin Source File

SOURCE=.\nmm_Microscope_SEM_diaginc.h
# End Source File
# Begin Source File

SOURCE=.\nmm_Microscope_SEM_optical.h
# End Source File
# Begin Source File

SOURCE=.\optical_server.h
# End Source File
# Begin Source File

SOURCE=.\OpticalServerInterface.h
# End Source File
# Begin Source File

SOURCE=.\Tcl_Linkvar.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "ImageMagick libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_bzlib_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_coders_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_filters_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_jbig_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_jp2_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_jpeg_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_lcms_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_libxml_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_magick_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_Magick++_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_png_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_tiff_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_ttf_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_wand_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_wmf_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_xlib_.lib"
# End Source File
# Begin Source File

SOURCE="C:\nsrg\external\pc_win32\ImageMagick-6.2.3_staticDLL\lib\CORE_RL_zlib_.lib"
# End Source File
# End Group
# End Target
# End Project
