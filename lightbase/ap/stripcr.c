#include   <stdio.h>
#include   <strings.h>
#include   <errno.h>

main( argc, argv )
int argc;
char *argv[];
{
   while (--argc)
      striplf( *++argv );
}

striplf( arq )
char *arq;
{
   FILE *in, *out;
   char linha[2048], *p;
   char tmpname[200];

   strcpy( tmpname, "@@##XXXXXX" );
   mktemp( tmpname );
   in = fopen( arq, "r" );
   out = fopen( tmpname, "w" );
   if ( in && out )
   {
      fprintf(stderr, "Tratando : %s ", arq);
      while ((fgets( linha, 2047, in )) != NULL) {
         p = strchr( linha, 0x1a);
         if (p)
            *p = '\0';
         p = strchr( linha, '\r' );
         if (p != NULL) {
            p[0] = '\n';
            p[1] = '\0';
         }
         fputs( linha, out );
      }
      fclose( in );
      fclose( out );
      unlink( arq );
      link( tmpname, arq );
      unlink( tmpname );
      fprintf(stderr, " OK.\n");
   }
   else
      fprintf( stderr, "Problema[%d] com arquivo %s \n", errno, arq );
}
