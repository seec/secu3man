# SECU-3  - An open source, free engine control unit
#
# Makefile for building of Chart2D DLL. Borland C Bulder is required.
# Created by Alexey A. Shabelnikov, UA, Kiev. http://secu-3.org

# ----------------------------------------------------------------------------
VERSION = BCB.01
# ----------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ----------------------------------------------------------------------------
PROJECT  = ..\..\binaries\Chart2D.dll
OBJFILES = Form2D.obj Chart2D.obj TFloatUpDown.obj TChartEx.obj
RESDEPEN = $(RESFILES) Form2D.dfm
RESFILES = resource.res
DEFFILE  = Chart2D.def
LIBFILES =
# ----------------------------------------------------------------------------
CFLAG1 = -Od -Hc -w -k -r- -y -v -vi- -c -a4 -b- -w-par -w-inl -Vx -Ve -x
CFLAG2 = -I$(BCB)\include;$(BCB)\include\vcl -H=$(BCB)\lib\vcld.csm
PFLAGS = -U$(BCB)\lib\obj;$(BCB)\lib \
   -I$(BCB)\include;$(BCB)\include\vcl \
   -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE -v \
   -$Y -$W -$O- -JPHNV -M
RFLAGS = -i$(BCB)\include;$(BCB)\include\vcl 
LFLAGS = -L$(BCB)\lib\obj;$(BCB)\lib -aa -Tpd -x -Gn -Gi -l..\..\libraries
IFLAGS =
LINKER = ilink32
# ----------------------------------------------------------------------------
ALLOBJ = c0d32.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) vcl.lib import32.lib cp32mt.lib Tee50.lib import32.lib cp32mt.lib
# ----------------------------------------------------------------------------
.autodepend

$(PROJECT): $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES)
!

.pas.hpp:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.pas.obj:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.cpp.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $*

.c.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $**

.rc.res:
    $(BCB)\BIN\brcc32 $(RFLAGS) $<
#------------------------------------------------------------------------------

clean:
    @if exist ..\..\binaries\Chart2D.dll ( DEL /Q ..\..\binaries\Chart2D.dll )
    @if exist ..\..\binaries\Chart2D.tds ( DEL /Q ..\..\binaries\Chart2D.tds )
    @if exist ..\..\libraries\Chart2D.lib ( DEL /Q ..\..\libraries\Chart2D.lib )
    @if exist *.obj ( DEL /Q *.obj )
    @if exist *.res ( DEL /Q *.res )
