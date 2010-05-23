# Microsoft Developer Studio Project File - Name="pcs2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=pcs2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pcs2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pcs2.mak" CFG="pcs2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pcs2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "pcs2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pcs2 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob0 /I "C:\libraries\wxWidgets\lib\msw" /I "C:\libraries\wxWidgets\include" /I "C:\libraries\DevIL\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib devil.lib ilu.lib ilut.lib opengl32.lib glu32.lib comctl32.lib wsock32.lib RPCRT4.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"C:\libraries\wxWidgets\lib" /libpath:"C:\libraries\DevIL\lib"

!ELSEIF  "$(CFG)" == "pcs2 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "C:\libraries\wxWidgets\lib\mswd" /I "C:\libraries\wxWidgets\include" /I "C:\libraries\DevIL\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WXDEBUG__" /D WXDEBUG=1 /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib devild.lib ilu-d.lib ilutd.lib opengl32.lib glu32.lib comctl32.lib wsock32.lib RPCRT4.LIB kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"C:\libraries\wxWidgets\lib" /libpath:"C:\libraries\DevIL\lib\debug\\"

!ENDIF 

# Begin Target

# Name "pcs2 - Win32 Release"
# Name "pcs2 - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\apply.xpm
# End Source File
# Begin Source File

SOURCE=.\bullet.xpm
# End Source File
# Begin Source File

SOURCE=.\dock.xpm
# End Source File
# Begin Source File

SOURCE=.\down.xpm
# End Source File
# Begin Source File

SOURCE=.\fuel.xpm
# End Source File
# Begin Source File

SOURCE=.\glow.xpm
# End Source File
# Begin Source File

SOURCE=.\header.xpm
# End Source File
# Begin Source File

SOURCE=.\left.xpm
# End Source File
# Begin Source File

SOURCE=.\new.xpm
# End Source File
# Begin Source File

SOURCE=.\open.xpm
# End Source File
# Begin Source File

SOURCE=.\paths.xpm
# End Source File
# Begin Source File

SOURCE=.\pcs2.rc
# End Source File
# Begin Source File

SOURCE=.\pcsico.xpm
# End Source File
# Begin Source File

SOURCE=.\placeholder.xpm
# End Source File
# Begin Source File

SOURCE=.\revert.xpm
# End Source File
# Begin Source File

SOURCE=.\right.xpm
# End Source File
# Begin Source File

SOURCE=.\save.xpm
# End Source File
# Begin Source File

SOURCE=.\solid.xpm
# End Source File
# Begin Source File

SOURCE=.\spcl.xpm
# End Source File
# Begin Source File

SOURCE=.\subobjects.xpm
# End Source File
# Begin Source File

SOURCE=.\textured.xpm
# End Source File
# Begin Source File

SOURCE=.\textures.xpm
# End Source File
# Begin Source File

SOURCE=.\turrets.xpm
# End Source File
# Begin Source File

SOURCE=.\up.xpm
# End Source File
# Begin Source File

SOURCE=.\weap.xpm
# End Source File
# Begin Source File

SOURCE=.\wireframe.xpm
# End Source File
# Begin Source File

SOURCE=.\xleft.xpm
# End Source File
# Begin Source File

SOURCE=.\xright.xpm
# End Source File
# Begin Source File

SOURCE=.\yleft.xpm
# End Source File
# Begin Source File

SOURCE=.\yright.xpm
# End Source File
# Begin Source File

SOURCE=.\zin.xpm
# End Source File
# Begin Source File

SOURCE=.\zleft.xpm
# End Source File
# Begin Source File

SOURCE=.\zout.xpm
# End Source File
# Begin Source File

SOURCE=.\zright.xpm
# End Source File
# End Group
# Begin Group "POF"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\POFDataStructs.h
# End Source File
# Begin Source File

SOURCE=.\POFHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\POFHandler.h
# End Source File
# End Group
# Begin Group "BSP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BSPDataStructs.cpp
# End Source File
# Begin Source File

SOURCE=.\BSPDataStructs.h
# End Source File
# Begin Source File

SOURCE=.\BSPHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\BSPHandler.h
# End Source File
# End Group
# Begin Group "COB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\COBDataStructs.h
# End Source File
# Begin Source File

SOURCE=.\COBHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\COBHandler.h
# End Source File
# End Group
# Begin Group "PMF"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pcs_file.cpp
# End Source File
# Begin Source File

SOURCE=.\pcs_file.h
# End Source File
# Begin Source File

SOURCE=.\pcs_file_dstructs.cpp
# End Source File
# Begin Source File

SOURCE=.\pcs_file_dstructs.h
# End Source File
# Begin Source File

SOURCE=.\pcs_pmf_cob.cpp
# End Source File
# Begin Source File

SOURCE=.\pcs_pmf_pof.cpp
# End Source File
# Begin Source File

SOURCE=.\pcs_pof_bspfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\pcs_pof_bspfuncs.h
# End Source File
# End Group
# Begin Group "util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FileList.cpp
# End Source File
# Begin Source File

SOURCE=.\FileList.h
# End Source File
# Begin Source File

SOURCE=.\kaz_templates.cpp
# End Source File
# Begin Source File

SOURCE=.\kaz_templates.h
# End Source File
# Begin Source File

SOURCE=.\tex_ctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\tex_ctrl.h
# End Source File
# Begin Source File

SOURCE=.\vector3d.cpp
# End Source File
# Begin Source File

SOURCE=.\vector3d.h
# End Source File
# Begin Source File

SOURCE=.\VPReader.cpp
# End Source File
# Begin Source File

SOURCE=.\VPReader.h
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Editor Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pe_base.h
# End Source File
# Begin Source File

SOURCE=.\pe_docking.h
# End Source File
# Begin Source File

SOURCE=.\pe_glows.h
# End Source File
# Begin Source File

SOURCE=.\pe_header.h
# End Source File
# Begin Source File

SOURCE=.\pe_paths.h
# End Source File
# Begin Source File

SOURCE=.\pe_subobjects.h
# End Source File
# Begin Source File

SOURCE=.\pe_subsystems.h
# End Source File
# Begin Source File

SOURCE=.\pe_textures.h
# End Source File
# Begin Source File

SOURCE=.\pe_thrusters.h
# End Source File
# Begin Source File

SOURCE=.\pe_turrets.h
# End Source File
# Begin Source File

SOURCE=.\pe_weapons.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\pcs2.h
# End Source File
# Begin Source File

SOURCE=.\pcs2_CIDs.h
# End Source File
# Begin Source File

SOURCE=.\pcs2_filethread.h
# End Source File
# Begin Source File

SOURCE=.\wxCTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\wxPCSPref.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Editor Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pe_docking.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_glows.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_header.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_paths.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_subobjects.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_subsystems.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_textures.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_thrusters.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_turrets.cpp
# End Source File
# Begin Source File

SOURCE=.\pe_weapons.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\pcs2.cpp
# End Source File
# Begin Source File

SOURCE=.\pcs2_filethread.cpp
# End Source File
# Begin Source File

SOURCE=.\PCS2_MainPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\wxCTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\wxGL_PMFCanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\wxPCSPref.cpp
# End Source File
# End Group
# End Group
# Begin Group "compat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\compat\filelength.cpp
# End Source File
# Begin Source File

SOURCE=.\compat\filelength.h
# End Source File
# Begin Source File

SOURCE=.\compat\strncmp.cpp
# End Source File
# Begin Source File

SOURCE=.\compat\strncmp.h
# End Source File
# End Group
# End Target
# End Project
