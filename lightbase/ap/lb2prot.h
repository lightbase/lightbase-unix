/* Arquivo de definicao de tipo de protecao */

#define   REDE 1 /* caso queira protecao everlock retirar esta linha */

#ifdef MSDOS
#	ifdef ESPANHA
#		undef    VIAS_IMPLICITAS
#	else
#		ifndef REDE
#			define   VIAS_IMPLICITAS
#		endif
#	endif
#else
#	undef    VIAS_IMPLICITAS
#endif

#ifdef LITE
#ifndef RUNTIME
#	undef    VIAS_IMPLICITAS
#endif
#endif

