.SALVARQS
.REGCOMP : cc -c -g -M3 -I./ -I../mt -DSCOUNIX -DXTOOLS %s.c
.LERRO   : \s:\swarning\s|\s:\serror\s|\s:\sfatal\serror\s
.NLERRO  : \($\)
.TRUNC   : [0-9]:\s
.NOMEARQ : $\([0-9]+\)

