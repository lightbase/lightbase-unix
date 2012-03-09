#include <stdio.h>

main(  )
{
   int i= 1;
   unsigned int ui=2;
   short s=3;
   unsigned short us=4;
   char c=5;
   unsigned char uc=6;
   void xixi_funcao( int, unsigned int, short, unsigned short, char, unsigned char );

   xixi_funcao( i, ui, s, us, c, uc );

   exit(0);
}

void xixi_funcao( int i, unsigned int ui, short s, unsigned short us, char c, unsigned char uc )
{
	char *ptc;
	int indice;

	printf( "1:%d %d %d %d %d %d\n", i, ui, s, us, c, uc );
	getchar(  );

	ptc = (char *)&i;

	for( indice = 0; indice < 48; indice+=8 )
		printf( "2:%d %d %d %d %d %d %d %d\n", ptc[ indice ], ptc[ indice+1 ],
		      ptc[ indice+2 ], ptc[ indice+3 ], ptc[ indice+4 ],
		      ptc[ indice+5 ], ptc[ indice+6 ], ptc[ indice+7 ] );
	return;
}
