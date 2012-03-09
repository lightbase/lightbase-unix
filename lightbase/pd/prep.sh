# /*** @(#) INFOCON *** %M% *** VERSAO %R%.%L% *** %E% ***/

# [ $# -ne 3 ] && echo "Sintaxe: $0 maquina midia lang" >&2 && exit 1

# LINGUA=$3
LINGUA=PORTUGUES

# remove o ic anterior
rm -f ic
cp ic.c ic
ed - ic <<!
/LANGPROD/s//$LINGUA/
w
q
!
