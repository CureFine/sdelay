TARGET=sdelay.exe
ASM=WASM
CC	= WCC386
RC	= WRC
HC	= HC31
DISASM	= WDIS
LNK	= WLINK

WC=E:\WC

ODIR=OBJ\

INCLUDE=$(WC)\H;$(WC)\H\NT

COPTS = -zk0 -bt=nt -mf

DEFINES	=
RESFLAGS=  -r
OBJS	= &
	$(ODIR)base.obj &
	$(ODIR)misc.obj &
	$(ODIR)wave.obj &
	$(ODIR)wnd.obj

LINK_OBJS = &
	$(ODIR)base, &
	$(ODIR)misc, &
	$(ODIR)wave, &
	$(ODIR)wnd

RES = res.res

LINK_FLAGS= NAME $(TARGET) &
	LIBP $(WC)\lib386;$(WC)\lib386\nt &
	LANG JA &
	FORM WINDOWS NT RU WIN=4.0 &
	SORT COMMIT HEAP=1M COMMIT STACK=4K &
	OP STACK=4M,HEAPSIZE=4M,Map=MAPFILE.TXT,EL,&
	ALIGNMENT=512,OBJA=4096, &
	RES=RES.RES,DE DRAG,VERS=1.0, &
	DE NOTEXIT,VERS=1.0, &
	MODN=sdelay,VERBOSE,STATICS

LINK_LIBS = LIB kernel32,user32,gdi32,winmm

OPTION =

!ifdef D
CFLAGS = -D_DEBUG -3r -d2 -db -hd -s -w=4
LINK_OPTS= DEBUG DWARF
DLL_LINK_OPTS= DEBUG DWARF
!else
CFLAGS = -DRELEASE -5r -oabhkrs -s -w=4 -d0
LINK_OPTS = OP NOD,NOR,START='Entry_'
DLL_LINK_OPTS = OP NOD,START='_DllEntryPoint@12'
!endif

.EXTENSIONS:
.EXTENSIONS: .exe .lib .obj .asm .c .cpp .res .rc .h

.c.obj:
	$(CC) -i=$(INCLUDE) $(COPTS) $(CFLAGS) $(OPTION) -fo$@ $[@

.asm.obj:
	$(ASM) $(AFLAGS) -fo$@ $[@

.rc.res:
	$(RC) $(RCDEFINES) $(RESFLAGS) -i=$(INCLUDE) -fo$@ $[@

all :: $(TARGET) $(TARGET_DLL)


$(TARGET): $(OBJS) $(RES)
	SET WATCOM=$(WC)
	-del $(TARGET)
	$(LNK) @<<
	$(LINK_FLAGS)
	$(LINK_OPTS)
	FILE $(LINK_OBJS)
	$(LINK_LIBS)
<<
	SET WATCOM=

