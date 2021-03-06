#     Make para UNIX
#     (c)1991 Light Software
#     Todos Direitos Reservados
#
#SISTEMA = MSDOS
#SISTEMA = XENIX
#SISTEMA = IX386
SISTEMA = SCOUNIX
#SISTEMA = PA800R
#SISTEMA = ULTRIX
#SISTEMA = VAXVMS
#SISTEMA = SUN
#SISTEMA = AIX
#SISTEMA = U600
#SISTEMA = DECALFA

#CROSS    = -x2.3
#MODELO   = -M3
#WARNING  = -W 3

IPATH  = -I. -I..
CFLAGS = $(WARNING) $(CROSS) $(MODELO) -D$(SISTEMA) -DUNIX
#AIX CFLAGS = -qchars=signed $(WARNING) $(CROSS) $(MODELO) -D$(SISTEMA) -DUNIX
LDFLAGS= -lm -lx
LIB=ctl.a

COMPILER=cc -c $(CFLAGS) $(IPATH)

#
#### OBJETOS
#
CS =	cttrace.o ctvrc2.o ctdelk.o ctaddk.o ctupdt.o \
      ctsrch.o ctdatf.o ctkeyf.o ctkrnl.o ctcomp.o \
      ctvrcu.o ctinit.o ctclb2.o ctclib.o ctclb3.o

#
#### Header Files ####
#
H1 = ctcmpl.h ctport.h ctoptn.h ctstdr.h ctstrc.h ctgvar.h cterrc.h ctiproto.h cteproto.h

#
# Regras Gerais
#

.c.o :
	$(COMPILER) $<

#### Dependencies ####

library     : $(CS)  $(LIB)

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

$(LIB)	: $(CS)
	lorder $(CS) | tsort > ordena
	ar  rv  $(LIB)  `cat ordena`
#	ranlib $(LIB)
#  no HPUX comentar deixar so a linha abaixo
#	ar rv $(LIB) $(CS)
#  no ULTRIX deixar a linha abaixo
#	ar ts $(LIB)

ordena : $(CS)
	lorder $(CS) | tsort > ordena

teste	: teste.c $(LIB)
	cc $(CFLAGS) $(LDFLAGS) -o teste teste.c $(LIB) -lx


testemin	: testemin.c $(LIB)
	cc $(CFLAGS) $(LDFLAGS) -o testemin testemin.c $(LIB) -lx

limpa	:
	rm -f *.o *.a *.bak ordena*

