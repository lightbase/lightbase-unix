

SUBDIR=sco
DG=../pacotes


VERSAO= 1.0
ALVO = LIXO
LIGHT = light
TOOLS = tools
LBCONV = lbconv
FILTRO = filtro

/*  informar diretorio raiz do "porte" do light base e
  subdiretorio onde deve ser colocado o resultado do porte
  (abaixo do diretorio @LIGHT_HOME/pacotes/subdir)
*/

PERSON= person
BINARIOS = \
light \
lbconv \
converte/convlb4 \
tools

DEFS   = -DBRASIL
EXTRA  = -I$(BASE)/deslight/ap -I$(BASE)/lib/infocon/mt -I$(BASE)/deslight/ct -I$(BASE)/include $(DEFS) -DPROT_INFOCON

LIBX = -lm -lc

LIGHT_HOME = $(BASE)/deslight

CT=$(LIGHT_HOME)/ct
MT=$(BASE)/lib/infocon/mt
AP=$(LIGHT_HOME)/ap
PS=$(BASE)/lib/instalado

LIB_CT=$(CT)/ctl.a
LIB_MT =$(MT)/mtl.a

LIBS = $(LIB_CT) $(LIB_MT) $(LIBPERSON)
LIBSCT = $(LIB_CT)

/*
#### OBJETOS
*/
OBJETOSCS = \
   lb2auto.o   \
   lb2base0.o   \
   lb2base1.o   \
   lb2bkrs.o    \
   lb2but1.o    \
   lb2but1a.o    \
   lb2cad1.o    \
   lb2cad2.o    \
   lb2cad3.o    \
   lb2cadge.o   \
   lb2carel.o   \
   lb2crit.o    \
   lb2db00.o    \
   lb2db01.o    \
   lb2dbac.o    \
   lb2dbcb.o    \
   lb2dbib.o    \
   lb2dbcp0.o   \
   lb2dbcp1.o   \
   lb2dbrl0.o   \
   lb2dbrl1.o   \
   lb2dbrl2.o   \
   lb2dbvs0.o   \
   lb2dbvs1.o   \
   lb2dbsw.o    \
   lb2div1.o    \
   lb2dummy.o   \
   lb2edind.o   \
   lb2ent1.o    \
   lb2ent2.o    \
   lb2ent3.o    \
   lb2ent4.o    \
   lb2ent4a.o    \
   lb2ent4b.o    \
   lb2ent5.o    \
   lb2expo.o    \
   lb2fergn.o   \
   lb2ferrd.o   \
   lb2ferri.o   \
   lb2ferrm.o   \
   lb2ger4.o    \
   lb2gntxt.o   \
   lb2gntx2.o   \
   lb2grp.o     \
   lb2hmenu.o   \
   lb2ind.o     \
   lb2ind2.o    \
   lb2main.o    \
   lb2main2.o   \
   lb2menu.o    \
   lb2musgr.o   \
   lb2perso.o   \
   lb2pesq0.o   \
   lb2pesqa.o   \
   lb2pesq1.o   \
   lb2pesq2.o   \
   lb2pesq3.o   \
   lb2pesq4.o   \
   lb2pesq5.o   \
   lb2pesq6.o   \
   lb2prot1.o   \
   lb2rdisp.o   \
   lb2reind.o   \
   lb2rel0.o    \
   lb2rel1.o    \
   lb2rel2.o    \
   lb2rel3.o    \
   lb2rel4.o    \
   lb2rel5.o    \
   lb2sbase.o   \
   lb2sound.o   \
   lb2tab.o     \
   lb2table.o   \
   lb2tela.o    \
   lb2tr0.o     \
   lb2tr1.o     \
   lb2usu0.o    \
   lb2usu1.o    \
   lb2usu2.o    \
   lb2vetdf.o   \
   lb2vetut.o

OBJETOSTS =   \
   tools_0.o   \
   tools_1.o   \
   tools_2.o   \
   tools_3.o   \
   tools_4.o   \
   tools_5.o   \
   tools_6.o   \
   tools_7.o   \
   tools_9.o   \
   tools_a.o   \
   tools_b.o



all: $(LIGHT) $(TOOLS) $(LBCONV) $(FILTRO) fimlight

ALVO:$(LIGHT):$(OBJETOSCS):$(LIBS):sim
ALVO:$(TOOLS):$(OBJETOSTS):$(LIB_CT):nao
ALVO:$(LBCONV):lbconv.o::nao
ALVO:$(FILTRO):filtro.o::nao

ordena_o : $(OBJETOSCS)
	lorder $(OBJETOSCS) | tsort > ordena_o

espanha:
	touch `grep -l ESPANHA *.[ch]`
	sed s/BRASIL$$/ESPANHA/ < makefile > makefile.tmp; cp makefile.tmp makefile
	make
	make restart
	make gerachav
	make serializ
	make le_dados
	make tools
	./serializa -CInterno -S1
	cp serializa ${DG}
	cp gerachav ${DG}/gerachav.esp
	cp le_dados ${DG}/le_dados.br
	strip light
	strip restart
	strip tools
	cp light    ${DG}/${SUBDIR}/light.esp
	chmod 711   ${DG}/${SUBDIR}/light.esp
	cp restart  ${DG}/${SUBDIR}/restart.esp
	chmod 711   ${DG}/${SUBDIR}/restart.esp
	cp tools  ${DG}/${SUBDIR}/tools.esp
	chmod 711   ${DG}/${SUBDIR}/tools.esp
	cp pos.dat  ${DG}/${SUBDIR}/pos.dat.esp

pkt:
	sh geraconv
	make restart
	make gerachav
	make serializ
	make le_dados
	make tools
	./serializa -Cinterno -S1
	cp serializa ${DG}
	cp gerachav ${DG}/gerachav
	cp le_dados ${DG}/le_dados
	strip light
	strip restart
	strip tools
	strip converte/convlb4
	cp light    ${DG}/${SUBDIR}/light
	chmod 711   ${DG}/${SUBDIR}/light
	cp restart  ${DG}/${SUBDIR}/restart
	chmod 711   ${DG}/${SUBDIR}/restart
	cp tools  ${DG}/${SUBDIR}/tools
	chmod 711   ${DG}/${SUBDIR}/tools
	cp pos.dat  ${DG}/${SUBDIR}/pos.dat
	cp converte/convlb4 ${DG}/${SUBDIR}/convlb4
	chmod 711   ${DG}/${SUBDIR}/convlb4
	cp portugue.lb ${DG}/${SUBDIR}


fimlight:
	@echo $(LIGHT) esta pronto


/* HEADERS DOS ARQUIVOS */

HMA   = $(MA)/ctdefs.h     $(MA)/cteproto.h  $(MA)/cterrc.h
MTGLB  = $(MT)/window.h    $(MT)/winport.h
MTENT  = $(MT)/entrada.h
MTMENU = $(MT)/menu.h
MTTAB  = $(MT)/tabela.h
GER   = $(MTGLB)           $(AP)/lb2stru.h   $(AP)/lb2vars.h   $(AP)/lb2text.h
ENTRA = $(AP)/lb2entra.h   $(MTENT)
FILES = $(AP)/lb2files.h
HEAD  = $(AP)/lb2head.h
LOCS  = $(AP)/lb2locs.h
MACES = $(AP)/lb2maces.h
PERSO = $(AP)/lb2perso.h
TRANS = $(AP)/lb2trans.h
VET   = $(AP)/lb2vet.h
DEFBAS= $(AP)/lb2db.h
VISTA = $(AP)/lb2vista.h
ATIVA = $(AP)/restart.h
SERIAL= $(AP)/serial.h

/* DEPENDECIAS */

lb2auto.o    :  lb2auto.c   $(GER)
lb2base0.o   :  lb2base0.c  $(GER)   $(LOCS)     $(MTMENU)
lb2base1.o   :  lb2base1.c  $(GER)
lb2bkrs.o    :  lb2bkrs.c   $(GER)   $(BASES)    $(FILES)    $(LOCS)  $(MACES)    $(MTMENU)
lb2but1.o    :  lb2but1.c   $(GER)   $(FILES)    $(VET)
lb2but1a.o   :  lb2but1a.c  $(GER)   $(FILES)    $(VET)
lb2cad1.o    :  lb2cad1.c   $(GER)   $(BASES)    $(FILES)    $(MACES)
lb2cad2.o    :  lb2cad2.c   $(GER)   $(BASES)    $(FILES)    $(VET)   $(MTMENU)
lb2cad3.o    :  lb2cad3.c   $(GER)   $(BASES)    $(FILES)    $(VET)   $(MTMENU)
lb2carel.o   :  lb2carel.c  $(GER)   $(ENTRA)    $(FILES)    $(LOCS)  $(DEFBAS)
lb2cadge.o   :  lb2cadge.c  $(GER)   $(FILES)    $(MACES)
lb2crit.o    :  lb2crit.c   $(GER)   $(ENTRA)    $(MTTAB)
lb2db00.o    :  lb2db00.c   $(GER)   $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(MTMENU)
lb2db1.o     :  lb2db01.c   $(GER)   $(FILES)    $(MACES)
lb2dbac.o    :  lb2dbac.c   $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(VET)      $(DEFBAS)
lb2dbcb.o    :  lb2dbcb.c   $(GER)   $(FILES)    $(LOCS)     $(MACES)
lb2dbib.o    :  lb2dbib.c   $(GER)   $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(DEFBAS)
lb2dbcp0.o   :  lb2dbcp0.c  $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(VET)      $(DEFBAS)   $(MTTAB)
lb2dbcp1.o   :  lb2dbcp1.c  $(GER)   $(BASES)    $(ENTRA)    $(DEFBAS)
lb2dbrl0.o   :  lb2dbrl0.c  $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(VET)      $(DEFBAS)   $(MTTAB)    $(MTMENU)
lb2dbrl1.o   :  lb2dbrl1.c  $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(VET)      $(DEFBAS)   $(MTTAB)    $(VISTA)
lb2dbrl2.o   :  lb2dbrl2.c  $(GER)   $(ENTRA)    $(FILES)    $(LOCS)     $(VET)      $(DEFBAS)   $(VISTA)    $(MTTAB)    $(MTMENU)
lb2dbvs0.o   :  lb2dbvs0.c  $(GER)   $(BASES)    $(FILES)    $(LOCS)     $(MACES)    $(VET)      $(DEFBAS)   $(MTTAB)    $(VISTA)
lb2dbvs1.o   :  lb2dbvs1.c  $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(VET)      $(DEFBAS)   $(VISTA)    $(MTTAB)
lb2dbsw.o    :  lb2dbsw.c   $(GER)   $(FILES)    $(LOCS)     $(MACES)
lb2div1.o    :  lb2div1.c   $(GER)
lb2dummy.o   :  lb2dummy.c  $(GER)
lb2edind.o   :  lb2edind.c  $(ENTRA)
lb2ent1.o    :  lb2ent1.c   $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)
lb2ent2.o    :  lb2ent2.c   $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)
lb2ent3.o    :  lb2ent3.c   $(GER)   $(BASES)    $(FILES)    $(MACES)    $(VISTA)
lb2ent4.o    :  lb2ent4.c   $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(VISTA)
lb2ent4a.o   :  lb2ent4a.c  $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(VISTA)
lb2ent4b.o   :  lb2ent4b.c  $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(VISTA)
lb2ent5.o    :  lb2ent5.c   $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)
lb2expo.o    :  lb2expo.c   $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)
lb2fergn.o   :  lb2fergn.c  $(GER)   $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)    $(MTMENU)
lb2ferrd.o   :  lb2ferrd.c  $(GER)   $(BASES)    $(FILES)    $(MACES)    $(VET)      $(VISTA)
lb2ferri.o   :  lb2ferri.c  $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(MACES)    $(VET)
lb2ferrm.o   :  lb2ferrm.c  $(GER)   $(FILES)    $(MACES)
lb2ger4.o    :  lb2ger4.c   $(GER)   $(FILES)    $(MACES)
lb2gntxt.o   :  lb2gntxt.c  $(GER)   $(MACES)
lb2gntx2.o   :  lb2gntx2.c  $(GER)   $(MACES)
lb2grp.o     :  lb2grp.c    $(GER)   $(ENTRA)    $(LOCS)     $(MACES)
lb2hmenu.o   :  lb2hmenu.c  $(GER)
lb2ind.o     :  lb2ind.c    $(GER)   $(BASES)    $(MACES)    $(TRANS)
lb2ind2.o    :  lb2ind2.c   $(GER)   $(BASES)    $(MACES)    $(TRANS)
lb2main.o    :  lb2main.c   $(GER)   $(FILES)    $(HEAD)     $(LOCS)     $(MACES)    $(PERSO)
lb2main2.o   :  lb2main2.c  $(GER)   $(FILES)    $(HEAD)
lb2menu.o    :  lb2menu.c   $(MTMENU)
lb2musgr.o   :  lb2musgr.c  $(GER)   $(LOCS)     $(MTMENU)
lb2perso.o   :  lb2perso.c  $(GER)   $(LOCS)     $(PERSO)    $(MTMENU)
lb2pesq0.o   :  lb2pesq0.c  $(GER)   $(BASES)    $(ENTRA)    $(LOCS)     $(MACES)    $(PERSO)    $(PESQ)  $(MTTAB)
lb2pesqa.o   :  lb2pesqa.c  $(GER)   $(BASES)    $(ENTRA)    $(LOCS)     $(MACES)    $(PERSO)    $(PESQ)  $(MTTAB)
lb2pesq1.o   :  lb2pesq1.c  $(GER)   $(BASES)    $(ENTRA)    $(PESQ)     $(MTTAB)    $(MTMENU)
lb2pesq2.o   :  lb2pesq2.c  $(GER)   $(BASES)    $(ENTRA)    $(PERSO)    $(PESQ)     $(MTTAB)
lb2pesq3.o   :  lb2pesq3.c  $(GER)   $(PESQ)     $(MTTAB)
lb2pesq4.o   :  lb2pesq4.c  $(GER)   $(BASES)    $(LOCS)     $(PESQ)     $(VISTA)    $(MTTAB)
lb2pesq5.o   :  lb2pesq5.c  $(GER)   $(PESQ)     $(MTTAB)    $(MTMENU)
lb2pesq6.o   :  lb2pesq6.c  $(GER)   $(PESQ)     $(BASES)    $(ENTRA)    $(LOCS)  $(VISTA)
lb2prot1.o   :  lb2prot1.c  $(GER)   $(ATIVA)    $(SERIAL)
lb2rdisp.o   :  lb2rdisp.c  $(GER)   $(BASES)    $(VISTA)    $(MTTAB)
lb2reind.o   :  lb2reind.c  $(GER)   $(BASES)    $(FILES)    $(LOCS)     $(MACES)
lb2rel0.o    :  lb2rel0.c   $(GER)   $(ENTRA)    $(FILES)    $(REL)
lb2rel1.o    :  lb2rel1.c   $(GER)   $(BASES)    $(ENTRA)    $(REL)
lb2rel2.o    :  lb2rel2.c   $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(LOCS)     $(REL)
lb2rel3.o    :  lb2rel3.c   $(GER)   $(FILES)    $(REL)
lb2rel4.o    :  lb2rel4.c   $(GER)   $(ENTRA)    $(FILES)    $(LOCS)   $(REL)
lb2rel5.o    :  lb2rel5.c   $(GER)   $(ENTRA)    $(LOCS)     $(REL)
lb2sbase.o   :  lb2sbase.c  $(GER)   $(FILES)    $(LOCS)     $(MTMENU)
lb2sound.o   :  lb2sound.c  $(GER)   $(FILES)
lb2tab.o     :  lb2tab.c    $(GER)   $(FILES)
lb2table.o   :  lb2table.c  $(GER)   $(FILES)
lb2tela.o    :  lb2tela.c   $(GER)   $(BASES)    $(ENTRA)    $(FILES)    $(VET)   $(VISTA)
lb2tr0.o     :  lb2tr0.c    $(GER)   $(FILES)    $(TRANS)
lb2tr1.o     :  lb2tr1.c    $(GER)   $(FILES)    $(TRANS)
lb2usu0.o    :  lb2usu0.c   $(GER)   $(ENTRA)    $(LOCS)
lb2usu1.o    :  lb2usu1.c   $(GER)   $(ENTRA)    $(FILES)    $(LOCS)     $(MACES)
lb2usu2.o    :  lb2usu2.c   $(GER)   $(ENTRA)    $(LOCS)     $(MACES)
lb2vetdf.o   :  lb2vetdf.c  $(GER)   $(VET)
lb2vetut.o   :  lb2vetut.c  $(GER)   $(VET)
