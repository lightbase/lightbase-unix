/*
#     Makefile for UNIX Compatible libraries
#     (c)1991 Sciencia Informatica
#     Todos Direitos Reservados
*/

EXTRA= -I$(BASE)/lib/infocon/mt -D$(SISTEMA)=1 -DXTOOLS
LDFLAGS= -lm
LIB=mtx.a
DEMO=demo
/*
#### C SOURCES ####
*/

OBJETOSCS = \
   etblock1.o \
   etblock2.o \
   etedit0.o \
   etedit1.o \
   etfind.o \
   etinter1.o \
   etinter2.o \
   etkbd1.o \
   etkbd2.o \
   etkbd3.o \
   etmain.o \
   etmemo.o \
   etmove0.o \
   etmove1.o \
   etmove2.o \
   etsr.o \
   etundo.o \
   etwrap.o \
   wbell.o \
   wblock0.o \
   wblock1.o \
   wcalc1.o \
   wcalc2.o \
   wcurmov.o \
   wcursor.o \
   wentry0.o \
   wentry1.o \
   wentry2.o \
   wentry3.o \
   wentry4.o \
   wget0.o \
   wget1.o \
   whelp.o \
   winit1.o \
   winit2.o \
   winit3.o \
   winter1.o \
   winter2.o \
   winter3.o \
   wmacro.o \
   wmain0.o \
   wmain1.o \
   wmake.o \
   wmemterm.o \
   wmenu0.o \
   wmenu1.o \
   wmove.o \
   wprint.o \
   wprotec.o \
   wprscn.o \
   wput.o \
   wscroll.o \
   wstr0.o \
   wstr1.o \
   wtabela.o \
   wupdate.o


all: $(LIB) $(DEMO) fimlib

ALVO:$(DEMO):wdemo.o:$(LIB):nao

ordena_o : $(OBJETOSCS)
	lorder $(OBJETOSCS) | tsort > ordena_o

$(LIB)  : $(OBJETOSCS) ordena_o
	ar  ru  $(LIB)  `cat ordena_o`
/*	ranlib  $(LIB) */

fimlib:
	@echo "Biblioteca $(LIB) esta pronta"

/*
#### Header Files ####
*/

H1 = window.h     winport.h
H2 = menu.h
H3 = entrada.h
H4 = mensagem.h   wstring.h
H5 = calcula.h
H6 = ethead.h
H7 = tabela.h
H8 = vm.h
H9 = wprotec.h

/*
#### Dependencies ####
*/

etblock1.o : etblock1.c $(H1)             $(H4)       $(H6)       $(H8)
etblock2.o : etblock2.c $(H1)             $(H4)       $(H6)       $(H8)
etedit0.o  : etedit0.c  $(H1)                         $(H6)       $(H8)
etedit1.o  : etedit1.c  $(H1)             $(H4)       $(H6)       $(H8)
etfind.o   : etfind.c   $(H1)       $(H3) $(H4)       $(H6)       $(H8)
etinter1.o : etinter1.c $(H1)             $(H4)       $(H6)       $(H8)
etinter2.o : etinter2.c $(H1)             $(H4)       $(H6)       $(H8)
etkbd1.o   : etkbd1.c   $(H1)                         $(H6)       $(H8)
etkbd2.o   : etkbd2.c   $(H2)                         $(H6)       $(H8)
etkbd3.o   : etkbd3.c   $(H2)                         $(H6)       $(H8)
etmain.o   : etmain.c   $(H1)             $(H4)       $(H6)       $(H8)
etmemo.o   : etmemo.c   $(H1)                         $(H6)       $(H8)
etmove0.o  : etmove0.c  $(H1)                         $(H6)       $(H8)
etmove1.o  : etmove1.c  $(H1)             $(H4)       $(H6)       $(H8)
etmove2.o  : etmove2.c  $(H1)             $(H4)       $(H6)       $(H8)
etsr.o     : etsr.c     $(H1)
etundo.o   : etundo.c   $(H1)                         $(H6)       $(H8)
etwrap.o   : etwrap.c   $(H1)                         $(H6)       $(H8)
wbell.o    : wbell.c    $(H1)
wblock0.o  : wblock0.c  $(H1)
wblock1.o  : wblock1.c  $(H1)
wcalc1.o   : wcalc1.c   $(H1)       $(H3) $(H4) $(H5)
wcalc2.o   : wcalc2.c   $(H1)       $(H3) $(H4) $(H5)
wcurmov.o  : wcurmov.c  $(H1)
wcursor.o  : wcursor.c  $(H1)
wentry0.o  : wentry0.c  $(H1)       $(H3) $(H4)
wentry1.o  : wentry1.c  $(H1)       $(H3) $(H4)             $(H7)
wentry2.o  : wentry2.c  $(H1)       $(H3)
wentry3.o  : wentry3.c  $(H1)       $(H3)
wentry4.o  : wentry4.c  $(H1)       $(H3)
wget0.o    : wget0.c    $(H1)             $(H4)
wget1.o    : wget1.c    $(H1)
whelp.o    : whelp.c    $(H1) $(H2)       $(H4)       $(H6)
winit1.o   : winit1.c   $(H1)             $(H4)                   $(H8)
winit2.o   : winit2.c   $(H1)             $(H4)
winit3.o   : winit3.c   $(H1)
winter1.o  : winter1.c  $(H1) $(H2)       $(H4)
winter2.o  : winter2.c  $(H1)       $(H3) $(H4)
winter3.o  : winter3.c  $(H1)       $(H3) $(H4)       $(H6)
wmacro.o   : wmacro.c   $(H1)             $(H4)
wmain0.o   : wmain0.c   $(H1)
wmain1.o   : wmain1.c   $(H1)
wmake.o    : wmake.c    $(H1)
wmemterm.o : wmemterm.c $(H1)
wmenu0.o   : wmenu0.c   $(H1) $(H2)
wmenu1.o   : wmenu1.c   $(H1) $(H2)
wmove.o    : wmove.c    $(H1)
wprint.o   : wprint.c   $(H1) $(H2)       $(H4)
wprotec.o  : wprotec.c                                                  $(H9)
wprscn.o   : wprscn.c   $(H1)
wput.o     : wput.c     $(H1)
wscroll.o  : wscroll.c  $(H1)
wstr0.o    : wstr0.c    $(H1)             $(H4)
wstr1.o    : wstr1.c    $(H1)             $(H4)
wtabela.o  : wtabela.c  $(H1)             $(H4)             $(H7)
wupdate.o  : wupdate.c  $(H1)
wdemo.o    : wdemo.c    $(H1) $(H2) $(H3) $(H4)       $(H6) $(H7)


