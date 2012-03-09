.REGCOMP : cc -c -g -DSCOUNIX -DUNIX -DBRASIL -DAUTO_INCREMENTO -DPESQUISA_RECURSIVA -I./ -I../mt -I../ct %s.c
.LERRO : warning|error
.NLERRO : line\s$:
.TRUNC  : :

