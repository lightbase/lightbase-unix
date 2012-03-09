#ifdef VAXVMS

FILE *w_fopen ( arquivo, modo )
char *arquivo;
char *modo;
{
	return ( fopen ( arquivo, modo, "ctx=stm" ) );
}

#endif

