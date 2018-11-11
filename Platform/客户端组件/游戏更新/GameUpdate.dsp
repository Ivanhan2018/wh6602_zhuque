# Microsoft Developer Studio Project File - Name="AutoUpdate" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AutoUpdate - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AutoUpdate.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AutoUpdate.mak" CFG="AutoUpdate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AutoUpdate - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AutoUpdate - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/1 KShareClient/1.1 ≈‰÷√ø‚/1.1.8 ¥˙¬Î/AutoUpdate", LEDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AutoUpdate - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wininet.lib Winmm.lib Version.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "AutoUpdate - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wininet.lib Winmm.lib Version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "AutoUpdate - Win32 Release"
# Name "AutoUpdate - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AutoUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoUpdate.rc
# End Source File
# Begin Source File

SOURCE=.\AutoUpdateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FileMD5.cpp
# End Source File
# Begin Source File

SOURCE=.\InternetGetFile.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\RButton.cpp
# End Source File
# Begin Source File

SOURCE=.\RDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\RMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\UISkinManager.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateThread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoUpdate.h
# End Source File
# Begin Source File

SOURCE=.\AutoUpdateDlg.h
# End Source File
# Begin Source File

SOURCE=.\InternetGetFile.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\MultiLanguage.h
# End Source File
# Begin Source File

SOURCE=.\RButton.h
# End Source File
# Begin Source File

SOURCE=.\RDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UISkinManager.h
# End Source File
# Begin Source File

SOURCE=.\UpdateThread.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AutoUpdate.ico
# End Source File
# Begin Source File

SOURCE=.\res\AutoUpdate.rc2
# End Source File
# Begin Source File

SOURCE=.\res\button01.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_close.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_close_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_maximize.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_maximize_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_minimize.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_minimize_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_restore.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_restore_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_system_control.bmp
# End Source File
# Begin Source File

SOURCE=.\res\title01.bmp
# End Source File
# Begin Source File

SOURCE=.\res\title02.bmp
# End Source File
# Begin Source File

SOURCE=.\res\title03.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
