# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Pksedit - Win32 Release
!MESSAGE No configuration specified.  Defaulting to Pksedit - Win32 Release.
!ENDIF

!IF "$(CFG)" != "Pksedit - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE
!MESSAGE NMAKE /f "Pksedit.mak" CFG="Pksedit - Win32 Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Pksedit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE
!ERROR An invalid configuration is specified.
!ENDIF

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Pksedit - Win32 Release"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\Pksedit.exe"

CLEAN :
	-@erase ".\WinRel\Pksedit.exe"
	-@erase ".\WinRel\BLOCKS.OBJ"
	-@erase ".\WinRel\CRYPT.OBJ"
	-@erase ".\WinRel\FSEL.OBJ"
	-@erase ".\WinRel\BRACKETS.OBJ"
	-@erase ".\WinRel\PRMACS.OBJ"
	-@erase ".\WinRel\BLCOLUMN.OBJ"
	-@erase ".\WinRel\RESOURCE.OBJ"
	-@erase ".\WinRel\HISTORY.OBJ"
	-@erase ".\WinRel\FKEYS.OBJ"
	-@erase ".\WinRel\functab.obj"
	-@erase ".\WinRel\CUTPASTE.OBJ"
	-@erase ".\WinRel\CLIP.OBJ"
	-@erase ".\WinRel\CMP.OBJ"
	-@erase ".\WinRel\TEST.OBJ"
	-@erase ".\WinRel\ALIGN.OBJ"
	-@erase ".\WinRel\EXEC.OBJ"
	-@erase ".\WinRel\FUNPARSE.OBJ"
	-@erase ".\WinRel\ABORT.OBJ"
	-@erase ".\WinRel\KEYS.OBJ"
	-@erase ".\WinRel\UNDO.OBJ"
	-@erase ".\WinRel\CODEGEN.OBJ"
	-@erase ".\WinRel\STRINGS.OBJ"
	-@erase ".\WinRel\SCANNER.OBJ"
	-@erase ".\WinRel\TRANSCHR.OBJ"
	-@erase ".\WinRel\FONT.OBJ"
	-@erase ".\WinRel\CUSTOM.OBJ"
	-@erase ".\WinRel\RULER.OBJ"
	-@erase ".\WinRel\TAGS.OBJ"
	-@erase ".\WinRel\ICONS.OBJ"
	-@erase ".\WinRel\PROJECT.OBJ"
	-@erase ".\WinRel\FTW.OBJ"
	-@erase ".\WinRel\MATCH.OBJ"
	-@erase ".\WinRel\ARGS.OBJ"
	-@erase ".\WinRel\TODO.OBJ"
	-@erase ".\WinRel\XDIALOG.OBJ"
	-@erase ".\WinRel\EDIT.OBJ"
	-@erase ".\WinRel\toolbar.obj"
	-@erase ".\WinRel\MACLIB.OBJ"
	-@erase ".\WinRel\RUN.OBJ"
	-@erase ".\WinRel\USER.OBJ"
	-@erase ".\WinRel\FILEIO.OBJ"
	-@erase ".\WinRel\MOUSE.OBJ"
	-@erase ".\WinRel\PRINT.OBJ"
	-@erase ".\WinRel\REDRAW.OBJ"
	-@erase ".\WinRel\CRYPTSER.OBJ"
	-@erase ".\WinRel\properties.obj"
	-@erase ".\WinRel\port32.obj"
	-@erase ".\WinRel\RETREIVE.OBJ"
	-@erase ".\WinRel\REGEXP.OBJ"
	-@erase ".\WinRel\INIT.OBJ"
	-@erase ".\WinRel\GLOBMARK.OBJ"
	-@erase ".\WinRel\SLIDER.OBJ"
	-@erase ".\WinRel\STATUS.OBJ"
	-@erase ".\WinRel\MYSPRINT.OBJ"
	-@erase ".\WinRel\EWTERM.OBJ"
	-@erase ".\WinRel\OPTIONS.OBJ"
	-@erase ".\WinRel\FORM.OBJ"
	-@erase ".\WinRel\MARKS.OBJ"
	-@erase ".\WinRel\PARSER.OBJ"
	-@erase ".\WinRel\ATOL.OBJ"
	-@erase ".\WinRel\FORMAT.OBJ"
	-@erase ".\WinRel\BLFILL.OBJ"
	-@erase ".\WinRel\PRKEYS.OBJ"
	-@erase ".\WinRel\CMDFTAB.obj"
	-@erase ".\WinRel\SORT.OBJ"
	-@erase ".\WinRel\REDRAWAT.OBJ"
	-@erase ".\WinRel\WORKWIN.OBJ"
	-@erase ".\WinRel\FILES.OBJ"
	-@erase ".\WinRel\CCLASS.OBJ"
	-@erase ".\WinRel\OPEN.OBJ"
	-@erase ".\WinRel\SYM.OBJ"
	-@erase ".\WinRel\LABELS.OBJ"
	-@erase ".\WinRel\HELP.OBJ"
	-@erase ".\WinRel\DEBUG.OBJ"
	-@erase ".\WinRel\CURSOR.OBJ"
	-@erase ".\WinRel\LLIST.OBJ"
	-@erase ".\WinRel\MAKROC.OBJ"
	-@erase ".\WinRel\FINDREPL.OBJ"
	-@erase ".\WinRel\ERROR.OBJ"
	-@erase ".\WinRel\MAIN.OBJ"
	-@erase ".\WinRel\PROFILE.OBJ"
	-@erase ".\WinRel\LINELIST.OBJ"
	-@erase ".\WinRel\KEYBIND.OBJ"
	-@erase ".\WinRel\MACROS.OBJ"
	-@erase ".\WinRel\READLINE.OBJ"
	-@erase ".\WinRel\KEYFILE.OBJ"
	-@erase ".\WinRel\SOUND.OBJ"
	-@erase ".\WinRel\MENU.OBJ"
	-@erase ".\WinRel\STRTOK.OBJ"
	-@erase ".\WinRel\DIFF.OBJ"
	-@erase ".\WinRel\PKSEDIT.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "inc" /I "rc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /i "inc" /i "rc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Pksedit.bsc"
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /DEBUG /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Pksedit.pdb" /machine:I386 /def:".\PKSEDIT.DEF"\
 /out:"$(OUTDIR)/Pksedit.exe"
DEF_FILE= \
	".\PKSEDIT.DEF"
LINK32_OBJS= \
	"$(INTDIR)/BLOCKS.OBJ" \
	"$(INTDIR)/CRYPT.OBJ" \
	"$(INTDIR)/FSEL.OBJ" \
	"$(INTDIR)/BRACKETS.OBJ" \
	"$(INTDIR)/PRMACS.OBJ" \
	"$(INTDIR)/BLCOLUMN.OBJ" \
	"$(INTDIR)/RESOURCE.OBJ" \
	"$(INTDIR)/HISTORY.OBJ" \
	"$(INTDIR)/FKEYS.OBJ" \
	"$(INTDIR)/functab.obj" \
	"$(INTDIR)/CUTPASTE.OBJ" \
	"$(INTDIR)/CLIP.OBJ" \
	"$(INTDIR)/CMP.OBJ" \
	"$(INTDIR)/TEST.OBJ" \
	"$(INTDIR)/ALIGN.OBJ" \
	"$(INTDIR)/EXEC.OBJ" \
	"$(INTDIR)/FUNPARSE.OBJ" \
	"$(INTDIR)/ABORT.OBJ" \
	"$(INTDIR)/KEYS.OBJ" \
	"$(INTDIR)/UNDO.OBJ" \
	"$(INTDIR)/CODEGEN.OBJ" \
	"$(INTDIR)/STRINGS.OBJ" \
	"$(INTDIR)/SCANNER.OBJ" \
	"$(INTDIR)/TRANSCHR.OBJ" \
	"$(INTDIR)/FONT.OBJ" \
	"$(INTDIR)/CUSTOM.OBJ" \
	"$(INTDIR)/RULER.OBJ" \
	"$(INTDIR)/TAGS.OBJ" \
	"$(INTDIR)/ICONS.OBJ" \
	"$(INTDIR)/PROJECT.OBJ" \
	"$(INTDIR)/FTW.OBJ" \
	"$(INTDIR)/MATCH.OBJ" \
	"$(INTDIR)/ARGS.OBJ" \
	"$(INTDIR)/TODO.OBJ" \
	"$(INTDIR)/XDIALOG.OBJ" \
	"$(INTDIR)/EDIT.OBJ" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/MACLIB.OBJ" \
	"$(INTDIR)/RUN.OBJ" \
	"$(INTDIR)/USER.OBJ" \
	"$(INTDIR)/FILEIO.OBJ" \
	"$(INTDIR)/MOUSE.OBJ" \
	"$(INTDIR)/PRINT.OBJ" \
	"$(INTDIR)/REDRAW.OBJ" \
	"$(INTDIR)/CRYPTSER.OBJ" \
	"$(INTDIR)/properties.obj" \
	"$(INTDIR)/port32.obj" \
	"$(INTDIR)/RETREIVE.OBJ" \
	"$(INTDIR)/REGEXP.OBJ" \
	"$(INTDIR)/INIT.OBJ" \
	"$(INTDIR)/GLOBMARK.OBJ" \
	"$(INTDIR)/SLIDER.OBJ" \
	"$(INTDIR)/STATUS.OBJ" \
	"$(INTDIR)/MYSPRINT.OBJ" \
	"$(INTDIR)/EWTERM.OBJ" \
	"$(INTDIR)/OPTIONS.OBJ" \
	"$(INTDIR)/FORM.OBJ" \
	"$(INTDIR)/MARKS.OBJ" \
	"$(INTDIR)/PARSER.OBJ" \
	"$(INTDIR)/ATOL.OBJ" \
	"$(INTDIR)/FORMAT.OBJ" \
	"$(INTDIR)/BLFILL.OBJ" \
	"$(INTDIR)/PRKEYS.OBJ" \
	"$(INTDIR)/CMDFTAB.obj" \
	"$(INTDIR)/SORT.OBJ" \
	"$(INTDIR)/REDRAWAT.OBJ" \
	"$(INTDIR)/WORKWIN.OBJ" \
	"$(INTDIR)/FILES.OBJ" \
	"$(INTDIR)/CCLASS.OBJ" \
	"$(INTDIR)/OPEN.OBJ" \
	"$(INTDIR)/SYM.OBJ" \
	"$(INTDIR)/LABELS.OBJ" \
	"$(INTDIR)/HELP.OBJ" \
	"$(INTDIR)/DEBUG.OBJ" \
	"$(INTDIR)/CURSOR.OBJ" \
	"$(INTDIR)/LLIST.OBJ" \
	"$(INTDIR)/MAKROC.OBJ" \
	"$(INTDIR)/FINDREPL.OBJ" \
	"$(INTDIR)/ERROR.OBJ" \
	"$(INTDIR)/MAIN.OBJ" \
	"$(INTDIR)/PROFILE.OBJ" \
	"$(INTDIR)/LINELIST.OBJ" \
	"$(INTDIR)/KEYBIND.OBJ" \
	"$(INTDIR)/MACROS.OBJ" \
	"$(INTDIR)/READLINE.OBJ" \
	"$(INTDIR)/KEYFILE.OBJ" \
	"$(INTDIR)/SOUND.OBJ" \
	"$(INTDIR)/MENU.OBJ" \
	"$(INTDIR)/STRTOK.OBJ" \
	"$(INTDIR)/DIFF.OBJ" \
	"$(INTDIR)/PKSEDIT.res"

"$(OUTDIR)\Pksedit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

RSC_PROJ=/l 0x407 /fo"$(INTDIR)/PKSEDIT.res" /i "inc" /i "rc" /d "NDEBUG"
CPP_PROJ=/nologo /Od /Zi /ML /W3 /GX /D "_DEBUG" /I "inc" /I "rc" /D "WIN32" /D\
 "_WINDOWS" /Fp"$(INTDIR)/Pksedit.pch" /YX /Fo"$(INTDIR)/" /c
CPP_OBJS=.\WinRel/
CPP_SBRS=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<

MTL_PROJ=/nologo /D "NDEBUG" /win32
################################################################################
# Begin Target

# Name "Pksedit - Win32 Release"
################################################################################
# Begin Source File

SOURCE=.\INIT.C
DEP_CPP_INIT_=\
	".\inc\editab.h"\
	

"$(INTDIR)\INIT.OBJ" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ERROR.C
DEP_CPP_ERROR=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\rc\dial2.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\ERROR.OBJ" : $(SOURCE) $(DEP_CPP_ERROR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\PKSEDIT.DEF
# End Source File
################################################################################
# Begin Source File

SOURCE=.\CURSOR.C
DEP_CPP_CURSO=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\CURSOR.OBJ" : $(SOURCE) $(DEP_CPP_CURSO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\PROFILE.C
DEP_CPP_PROFI=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\inc\edifsel.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\PROFILE.OBJ" : $(SOURCE) $(DEP_CPP_PROFI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MOUSE.C
DEP_CPP_MOUSE=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\inc\edfuncs.h"\
	".\rc\pksedit.h"\
	".\inc\functab.h"\
	".\inc\iccall.h"\
	".\inc\resource.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\MOUSE.OBJ" : $(SOURCE) $(DEP_CPP_MOUSE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\WORKWIN.C
DEP_CPP_WORKW=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\rc\pksedit.h"\
	".\inc\context.h"\
	".\inc\iccall.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\WORKWIN.OBJ" : $(SOURCE) $(DEP_CPP_WORKW) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\PRINT.C
DEP_CPP_PRINT=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\rc\pksedit.h"\
	".\rc\dial2.h"\
	".\inc\prt.h"\
	".\inc\xdialog.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\PRINT.OBJ" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIFF.C
DEP_CPP_DIFF_=\
	".\inc\trace.h"\
	".\inc\winterf.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\DIFF.OBJ" : $(SOURCE) $(DEP_CPP_DIFF_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MATCH.C

"$(INTDIR)\MATCH.OBJ" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CLIP.C
DEP_CPP_CLIP_=\
	".\inc\winterf.h"\
	".\inc\editab.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\CLIP.OBJ" : $(SOURCE) $(DEP_CPP_CLIP_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\BLCOLUMN.C
DEP_CPP_BLCOL=\
	".\inc\editab.h"\
	".\rc\pksedit.h"\
	".\inc\edierror.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\BLCOLUMN.OBJ" : $(SOURCE) $(DEP_CPP_BLCOL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\HISTORY.C
DEP_CPP_HISTO=\
	".\inc\editab.h"\
	".\inc\edhist.h"\
	

"$(INTDIR)\HISTORY.OBJ" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TAGS.C
DEP_CPP_TAGS_=\
	".\inc\winterf.h"\
	".\inc\edctype.h"\
	".\inc\edifsel.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\rc\pksedit.h"\
	".\inc\edierror.h"\
	".\inc\iccall.h"\
	".\inc\xdialog.h"\
	".\rc\dial2.h"\
	".\inc\regcmp.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\TAGS.OBJ" : $(SOURCE) $(DEP_CPP_TAGS_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\KEYFILE.C
DEP_CPP_KEYFI=\
	".\rc\pksedit.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\edfuncs.h"\
	".\inc\edifsel.h"\
	".\inc\uc.h"\
	".\inc\winterf.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\KEYFILE.OBJ" : $(SOURCE) $(DEP_CPP_KEYFI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\EWTERM.C
DEP_CPP_EWTER=\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\inc\trace.h"\
	

"$(INTDIR)\EWTERM.OBJ" : $(SOURCE) $(DEP_CPP_EWTER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\LLIST.C

"$(INTDIR)\LLIST.OBJ" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\LINELIST.C
DEP_CPP_LINEL=\
	".\rc\pksedit.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\LINELIST.OBJ" : $(SOURCE) $(DEP_CPP_LINEL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FSEL.C
DEP_CPP_FSEL_=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\inc\tos.h"\
	".\inc\edhist.h"\
	".\inc\edfuncs.h"\
	".\inc\pathname.h"\
	".\inc\edifsel.h"\
	".\rc\fsel.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\FSEL.OBJ" : $(SOURCE) $(DEP_CPP_FSEL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\DEBUG.C
DEP_CPP_DEBUG=\
	".\inc\winterf.h"\
	".\inc\trace.h"\
	".\inc\editab.h"\
	

"$(INTDIR)\DEBUG.OBJ" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\STRINGS.C
DEP_CPP_STRIN=\
	".\inc\pathname.h"\
	

"$(INTDIR)\STRINGS.OBJ" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILES.C
DEP_CPP_FILES=\
	".\inc\tos.h"\
	".\inc\winterf.h"\
	".\inc\editab.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\FILES.OBJ" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CRYPT.C
DEP_CPP_CRYPT=\
	".\inc\des.h"\
	

"$(INTDIR)\CRYPT.OBJ" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\RESOURCE.C
DEP_CPP_RESOU=\
	".\inc\edtypes.h"\
	".\inc\tos.h"\
	".\inc\edfuncs.h"\
	".\inc\resource.h"\
	

"$(INTDIR)\RESOURCE.OBJ" : $(SOURCE) $(DEP_CPP_RESOU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\PKSEDIT.RC
DEP_RSC_PKSED=\
	".\rc\MOVE.CUR"\
	".\rc\COPY.CUR"\
	".\rc\PKS.ICO"\
	".\rc\EDIT.ICO"\
	".\rc\FILE.ICO"\
	".\rc\UNDO.ICO"\
	".\rc\TRASH.ICO"\
	".\rc\CLIP.ICO"\
	".\rc\PRINT.ICO"\
	".\rc\EXEC.ICO"\
	".\rc\FOLDER.ICO"\
	".\rc\CLIPEMPT.ICO"\
	".\rc\MACRO.ICO"\
	".\rc\HELP.ICO"\
	".\rc\PKS2.ICO"\
	".\rc\dial2.h"\
	".\rc\fsel.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\PKSEDIT.res" : $(SOURCE) $(DEP_RSC_PKSED) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\UNDO.C
DEP_CPP_UNDO_=\
	".\rc\pksedit.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\UNDO.OBJ" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FKEYS.C
DEP_CPP_FKEYS=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winterf.h"\
	".\inc\winfo.h"\
	".\rc\pksedit.h"\
	".\rc\dial2.h"\
	".\inc\edfuncs.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\FKEYS.OBJ" : $(SOURCE) $(DEP_CPP_FKEYS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MENU.C
DEP_CPP_MENU_=\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\inc\resource.h"\
	".\inc\edfuncs.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\MENU.OBJ" : $(SOURCE) $(DEP_CPP_MENU_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\XDIALOG.C
DEP_CPP_XDIAL=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winterf.h"\
	".\inc\winfo.h"\
	".\rc\pksedit.h"\
	".\rc\dial2.h"\
	".\inc\edfuncs.h"\
	".\inc\edhist.h"\
	".\inc\xdialog.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\XDIALOG.OBJ" : $(SOURCE) $(DEP_CPP_XDIAL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MAIN.C
DEP_CPP_MAIN_=\
	".\inc\trace.h"\
	".\inc\winterf.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\rc\pksedit.h"\
	".\inc\edierror.h"\
	".\inc\context.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\MAIN.OBJ" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\STATUS.C
DEP_CPP_STATU=\
	".\inc\trace.h"\
	".\inc\winterf.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\inc\edierror.h"\
	

"$(INTDIR)\STATUS.OBJ" : $(SOURCE) $(DEP_CPP_STATU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SLIDER.C
DEP_CPP_SLIDE=\
	".\inc\editab.h"\
	".\inc\trace.h"\
	".\inc\winfo.h"\
	".\inc\edierror.h"\
	

"$(INTDIR)\SLIDER.OBJ" : $(SOURCE) $(DEP_CPP_SLIDE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FONT.C
DEP_CPP_FONT_=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\rc\dial2.h"\
	

"$(INTDIR)\FONT.OBJ" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\EXEC.C
DEP_CPP_EXEC_=\
	".\rc\pksedit.h"\
	".\inc\edierror.h"\
	".\inc\editab.h"\
	".\inc\winterf.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\EXEC.OBJ" : $(SOURCE) $(DEP_CPP_EXEC_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\EDIT.C
DEP_CPP_EDIT_=\
	".\inc\trace.h"\
	".\inc\functab.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\inc\edierror.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\EDIT.OBJ" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\USER.C
DEP_CPP_USER_=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winterf.h"\
	".\inc\winfo.h"\
	".\rc\pksedit.h"\
	".\rc\dial2.h"\
	".\inc\edfuncs.h"\
	".\inc\edhist.h"\
	".\inc\xdialog.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\USER.OBJ" : $(SOURCE) $(DEP_CPP_USER_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ARGS.C
DEP_CPP_ARGS_=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	

"$(INTDIR)\ARGS.OBJ" : $(SOURCE) $(DEP_CPP_ARGS_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\REDRAWAT.C
DEP_CPP_REDRA=\
	".\inc\trace.h"\
	".\inc\edierror.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	

"$(INTDIR)\REDRAWAT.OBJ" : $(SOURCE) $(DEP_CPP_REDRA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\functab.c
DEP_CPP_FUNCT=\
	".\inc\edfuncs.h"\
	".\rc\pksedit.h"\
	".\inc\pksedith.h"\
	".\inc\edierror.h"\
	".\inc\iccall.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\functab.obj" : $(SOURCE) $(DEP_CPP_FUNCT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FTW.C
DEP_CPP_FTW_C=\
	".\inc\edtypes.h"\
	".\inc\ftw.h"\
	

"$(INTDIR)\FTW.OBJ" : $(SOURCE) $(DEP_CPP_FTW_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MARKS.C
DEP_CPP_MARKS=\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	

"$(INTDIR)\MARKS.OBJ" : $(SOURCE) $(DEP_CPP_MARKS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MACROS.C
DEP_CPP_MACRO=\
	".\inc\tos.h"\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\rc\pksedit.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\inc\edifsel.h"\
	".\inc\edfuncs.h"\
	".\inc\functab.h"\
	".\rc\dial2.h"\
	".\inc\iccall.h"\
	".\inc\resource.h"\
	".\inc\xdialog.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\MACROS.OBJ" : $(SOURCE) $(DEP_CPP_MACRO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FILEIO.C
DEP_CPP_FILEI=\
	".\inc\tos.h"\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\rc\pksedit.h"\
	".\inc\edexec.h"\
	".\inc\des.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\FILEIO.OBJ" : $(SOURCE) $(DEP_CPP_FILEI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FINDREPL.C
DEP_CPP_FINDR=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\rc\pksedit.h"\
	".\inc\edctype.h"\
	".\inc\regcmp.h"\
	".\inc\edhist.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\FINDREPL.OBJ" : $(SOURCE) $(DEP_CPP_FINDR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CUSTOM.C
DEP_CPP_CUSTO=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winterf.h"\
	".\inc\winfo.h"\
	

"$(INTDIR)\CUSTOM.OBJ" : $(SOURCE) $(DEP_CPP_CUSTO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMP.C
DEP_CPP_CMP_C=\
	".\inc\editab.h"\
	

"$(INTDIR)\CMP.OBJ" : $(SOURCE) $(DEP_CPP_CMP_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\BRACKETS.C
DEP_CPP_BRACK=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\inc\edierror.h"\
	".\inc\regcmp.h"\
	".\inc\uc.h"\
	".\inc\brackets.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\BRACKETS.OBJ" : $(SOURCE) $(DEP_CPP_BRACK) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FORMAT.C
DEP_CPP_FORMA=\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\FORMAT.OBJ" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ICONS.C
DEP_CPP_ICONS=\
	".\inc\trace.h"\
	".\rc\dial2.h"\
	".\inc\winterf.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\inc\iccall.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\ICONS.OBJ" : $(SOURCE) $(DEP_CPP_ICONS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\REGEXP.C
DEP_CPP_REGEX=\
	".\inc\edctype.h"\
	".\inc\tos.h"\
	".\inc\regcmp.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\REGEXP.OBJ" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ALIGN.C
DEP_CPP_ALIGN=\
	".\inc\editab.h"\
	".\inc\regcmp.h"\
	".\inc\edierror.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\ALIGN.OBJ" : $(SOURCE) $(DEP_CPP_ALIGN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\OPEN.C
DEP_CPP_OPEN_=\
	".\inc\tos.h"\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\inc\pathname.h"\
	".\inc\edifsel.h"\
	".\inc\edhist.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\OPEN.OBJ" : $(SOURCE) $(DEP_CPP_OPEN_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\BLFILL.C
DEP_CPP_BLFIL=\
	".\inc\editab.h"\
	

"$(INTDIR)\BLFILL.OBJ" : $(SOURCE) $(DEP_CPP_BLFIL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ABORT.C
DEP_CPP_ABORT=\
	".\inc\winterf.h"\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\rc\pksedit.h"\
	".\rc\dial2.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\ABORT.OBJ" : $(SOURCE) $(DEP_CPP_ABORT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TODO.C

"$(INTDIR)\TODO.OBJ" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CCLASS.C
DEP_CPP_CCLAS=\
	".\inc\editab.h"\
	".\inc\edctype.h"\
	

"$(INTDIR)\CCLASS.OBJ" : $(SOURCE) $(DEP_CPP_CCLAS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SOUND.C
DEP_CPP_SOUND=\
	".\inc\editab.h"\
	

"$(INTDIR)\SOUND.OBJ" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\READLINE.C
DEP_CPP_READL=\
	".\inc\editab.h"\
	

"$(INTDIR)\READLINE.OBJ" : $(SOURCE) $(DEP_CPP_READL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TRANSCHR.C
DEP_CPP_TRANS=\
	".\inc\edctype.h"\
	

"$(INTDIR)\TRANSCHR.OBJ" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GLOBMARK.C
DEP_CPP_GLOBM=\
	".\inc\editab.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\GLOBMARK.OBJ" : $(SOURCE) $(DEP_CPP_GLOBM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\BLOCKS.C
DEP_CPP_BLOCK=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\inc\tos.h"\
	".\rc\pksedit.h"\
	".\inc\iccall.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\BLOCKS.OBJ" : $(SOURCE) $(DEP_CPP_BLOCK) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ATOL.C
DEP_CPP_ATOL_=\
	".\inc\editab.h"\
	

"$(INTDIR)\ATOL.OBJ" : $(SOURCE) $(DEP_CPP_ATOL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MYSPRINT.C
DEP_CPP_MYSPR=\
	".\inc\winterf.h"\
	".\inc\edierror.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	

"$(INTDIR)\MYSPRINT.OBJ" : $(SOURCE) $(DEP_CPP_MYSPR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\FORM.C
DEP_CPP_FORM_=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winterf.h"\
	".\inc\winfo.h"\
	

"$(INTDIR)\FORM.OBJ" : $(SOURCE) $(DEP_CPP_FORM_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\RULER.C
DEP_CPP_RULER=\
	".\inc\tos.h"\
	".\inc\editab.h"\
	".\inc\project.h"\
	".\inc\edifsel.h"\
	".\inc\edierror.h"\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\RULER.OBJ" : $(SOURCE) $(DEP_CPP_RULER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\PROJECT.C
DEP_CPP_PROJE=\
	".\inc\editab.h"\
	".\inc\project.h"\
	".\inc\pathname.h"\
	

"$(INTDIR)\PROJECT.OBJ" : $(SOURCE) $(DEP_CPP_PROJE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\OPTIONS.C
DEP_CPP_OPTIO=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\rc\pksedit.h"\
	".\rc\dial2.h"\
	".\inc\functab.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\OPTIONS.OBJ" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MACLIB.C
DEP_CPP_MACLI=\
	".\inc\winterf.h"\
	".\inc\edfuncs.h"\
	

"$(INTDIR)\MACLIB.OBJ" : $(SOURCE) $(DEP_CPP_MACLI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CUTPASTE.C
DEP_CPP_CUTPA=\
	".\inc\tos.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\rc\pksedit.h"\
	".\inc\edierror.h"\
	".\inc\iccall.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\CUTPASTE.OBJ" : $(SOURCE) $(DEP_CPP_CUTPA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\STRINGS.RC
# PROP Exclude_From_Build 1
# End Source File
################################################################################
# Begin Source File

SOURCE=.\RUN.C
DEP_CPP_RUN_C=\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\inc\edierror.h"\
	".\rc\pksedit.h"\
	".\inc\edfuncs.h"\
	".\inc\functab.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\RUN.OBJ" : $(SOURCE) $(DEP_CPP_RUN_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\STRTOK.C

"$(INTDIR)\STRTOK.OBJ" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\KEYS.C
DEP_CPP_KEYS_=\
	".\inc\editab.h"\
	".\inc\edfuncs.h"\
	".\inc\resource.h"\
	

"$(INTDIR)\KEYS.OBJ" : $(SOURCE) $(DEP_CPP_KEYS_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\SORT.C
DEP_CPP_SORT_=\
	".\inc\edctype.h"\
	".\inc\editab.h"\
	".\rc\pksedit.h"\
	".\inc\edfuncs.h"\
	".\inc\edierror.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\SORT.OBJ" : $(SOURCE) $(DEP_CPP_SORT_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\RETREIVE.C
DEP_CPP_RETRE=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	".\rc\pksedit.h"\
	".\inc\ftw.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\RETREIVE.OBJ" : $(SOURCE) $(DEP_CPP_RETRE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\REDRAW.C
DEP_CPP_REDRAW=\
	".\inc\trace.h"\
	".\inc\editab.h"\
	".\inc\edierror.h"\
	".\inc\winfo.h"\
	".\inc\winterf.h"\
	

"$(INTDIR)\REDRAW.OBJ" : $(SOURCE) $(DEP_CPP_REDRAW) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\HELP.C
DEP_CPP_HELP_=\
	".\inc\winterf.h"\
	

"$(INTDIR)\HELP.OBJ" : $(SOURCE) $(DEP_CPP_HELP_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rc\dial2.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\rc\fsel.h
# End Source File
################################################################################
# Begin Source File

SOURCE=.\port32.c
DEP_CPP_PORT3=\
	".\rc\pksedit.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\port32.obj" : $(SOURCE) $(DEP_CPP_PORT3) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\PRKEYS.C
DEP_CPP_PRKEY=\
	".\inc\winterf.h"\
	".\inc\edfuncs.h"\
	".\inc\pksmod.h"\
	".\inc\pkscc.h"\
	".\inc\funcdef.h"\
	".\inc\uc.h"\
	".\inc\brackets.h"\
	".\inc\resource.h"\
	

"$(INTDIR)\PRKEYS.OBJ" : $(SOURCE) $(DEP_CPP_PRKEY) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\CODEGEN.C
DEP_CPP_CODEG=\
	".\inc\pkscc.h"\
	".\inc\edfuncs.h"\
	".\inc\scanner.h"\
	

"$(INTDIR)\CODEGEN.OBJ" : $(SOURCE) $(DEP_CPP_CODEG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\SCANNER.C
DEP_CPP_SCANN=\
	".\inc\sym.h"\
	".\inc\edfuncs.h"\
	".\inc\scanner.h"\
	".\inc\parser.h"\
	".\inc\editab.h"\
	".\inc\pkscc.h"\
	

"$(INTDIR)\SCANNER.OBJ" : $(SOURCE) $(DEP_CPP_SCANN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\PARSER.C
DEP_CPP_PARSE=\
	".\inc\edfuncs.h"\
	".\inc\scanner.h"\
	".\inc\test.h"\
	".\inc\binop.h"\
	".\inc\pkscc.h"\
	".\inc\sym.h"\
	

"$(INTDIR)\PARSER.OBJ" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\MAKROC.C
DEP_CPP_MAKRO=\
	".\inc\editab.h"\
	".\inc\pkscc.h"\
	

"$(INTDIR)\MAKROC.OBJ" : $(SOURCE) $(DEP_CPP_MAKRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\SYM.C
DEP_CPP_SYM_C=\
	".\inc\edfuncs.h"\
	".\inc\pkscc.h"\
	".\inc\sym.h"\
	

"$(INTDIR)\SYM.OBJ" : $(SOURCE) $(DEP_CPP_SYM_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\TEST.C
DEP_CPP_TEST_=\
	".\inc\edfuncs.h"\
	".\inc\regcmp.h"\
	".\inc\pksmod.h"\
	".\inc\test.h"\
	".\inc\binop.h"\
	".\inc\sym.h"\
	".\inc\pkscc.h"\
	

"$(INTDIR)\TEST.OBJ" : $(SOURCE) $(DEP_CPP_TEST_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\KEYBIND.C
DEP_CPP_KEYBI=\
	".\inc\edfuncs.h"\
	".\inc\resource.h"\
	".\inc\pkscc.h"\
	

"$(INTDIR)\KEYBIND.OBJ" : $(SOURCE) $(DEP_CPP_KEYBI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\FUNPARSE.C
DEP_CPP_FUNPA=\
	".\inc\edfuncs.h"\
	".\inc\funcdef.h"\
	".\inc\sym.h"\
	".\inc\pkscc.h"\
	

"$(INTDIR)\FUNPARSE.OBJ" : $(SOURCE) $(DEP_CPP_FUNPA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\PRMACS.C
DEP_CPP_PRMAC=\
	".\inc\edfuncs.h"\
	".\inc\pksmod.h"\
	".\inc\pkscc.h"\
	".\inc\test.h"\
	".\inc\funcdef.h"\
	".\inc\binop.h"\
	

"$(INTDIR)\PRMACS.OBJ" : $(SOURCE) $(DEP_CPP_PRMAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\CMDFTAB.c
DEP_CPP_CMDFT=\
	".\inc\funcdef.h"\
	

"$(INTDIR)\CMDFTAB.obj" : $(SOURCE) $(DEP_CPP_CMDFT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\makroc\LABELS.C
DEP_CPP_LABEL=\
	".\inc\edfuncs.h"\
	".\inc\pkscc.h"\
	

"$(INTDIR)\LABELS.OBJ" : $(SOURCE) $(DEP_CPP_LABEL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\brand\CRYPTSER.C

"$(INTDIR)\CRYPTSER.OBJ" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\toolbar.c
DEP_CPP_TOOLB=\
	".\inc\winterf.h"\
	".\rc\pksedit.h"\
	".\inc\edierror.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\properties.c
DEP_CPP_PROPE=\
	".\inc\winterf.h"\
	".\rc\pksedit.h"\
	".\rc\dial2.h"\
	".\inc\xdialog.h"\
	".\inc\edierror.h"\
	".\rc\helpitem.h"\
	

"$(INTDIR)\properties.obj" : $(SOURCE) $(DEP_CPP_PROPE) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
