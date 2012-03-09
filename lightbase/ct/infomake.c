
/* #SISTEMA = MSDOS
#SISTEMA = XENIX
#SISTEMA = IX386
SISTEMA = SCOUNIX  Este sistemas estao cadastrados no metamake.c
#SISTEMA = PA800R  da biblioteca Infocon (Ve como eh feito neste arquivo)
#SISTEMA = ULTRIX
#SISTEMA = VAXVMS
#SISTEMA = SUN
#SISTEMA = AIX
#SISTEMA = U600
#SISTEMA = DECALFA
*/
/* 
CROSS    = -x2.3   Como na sei para qual sistema operacional
MODELO   = -M3     deixei para voce fazer. eh so colocar no ifdef 
WARNING  = -W 3    adequado do arquivo /u/gms/lib/infocon/metamake.c
*/

EXTRA  = -I. -I..
LDFLAGS= -lm -lx
LIB=ctl.a

TESTE = teste
TESTEMIN = testemin

/*
#
#### OBJETOS
#
*/
OBJETOSCS =	cttrace.o ctvrc2.o ctdelk.o ctaddk.o ctupdt.o \
      ctsrch.o ctdatf.o ctkeyf.o ctkrnl.o ctcomp.o \
      ctvrcu.o ctinit.o ctclb2.o ctclib.o ctclb3.o


all: $(LIB) fimlib

$(LIB)	: $(OBJETOSCS) ordena
	rm -f $(LIB)
	ar  rv $(LIB) `cat ordena`


/*
#	ranlib $(LIB)
#  no HPUX comentar deixar so a linha abaixo
#	ar rv $(LIB) $(CS)
#  no ULTRIX deixar a linha abaixo
#	ar ts $(LIB)
*/
ordena : $(OBJETOSCS)
	lorder $(OBJETOSCS) | tsort > ordena

ALVO:$(TESTE):teste.o:$(LIB):nao
ALVO:$(TESTE):testemin.o:$(LIB):nao


fimlib: 
	@echo "Biblioteca $(LIB) esta pornta"
/*
#
#### Header Files ####
#
*/
H1 = ctcmpl.h ctport.h ctoptn.h ctstdr.h ctstrc.h ctgvar.h cterrc.h ctiproto.h cteproto.h

/*
#
# Regras Gerais
#



#### Dependencies ####
*/

ctaddk.o     : ctaddk.c  $(H1)
ctclb2.o     : ctclb2.c  $(H1)
ctclb3.o     : ctclb3.c  $(H1)
ctclib.o     : ctclib.c  $(H1)
ctcomp.o     : ctcomp.c  $(H1)
ctdelk.o     : ctdelk.c  $(H1)
ctinit.o     : ctinit.c  $(H1)
ctkeyf.o     : ctkeyf.c  $(H1)
ctkrnl.o     : ctkrnl.c  $(H1)
ctsrch.o     : ctsrch.c  $(H1)
ctdatf.o     : ctdatf.c  $(H1)
ctupdt.o     : ctupdt.c  $(H1)
ctvrc2.o     : ctvrc2.c  $(H1)
cttrace.o    : cttrace.c $(H1)
ctvrcu.o     : ctvrcu.c  $(H1)
