#include  <stdio.h>
#include  <stdlib.h>

#ifdef    VAXVMS
#include  <file.h>
#else
#include  <fcntl.h>
#endif

#include  <string.h>
#include  <time.h>

#ifdef UNIX
#include  <unistd.h>
#else
#include  <io.h>
#endif


extern int errno;
#ifdef    VAXVMS
#include  <types.h>
#include  <stat.h>
#else
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

short ap_max_users;

#define   TR_TAM  ( sizeof(Tr_cabec) + (ap_max_users * sizeof(Tr_usu)))

#define   VOID void
#define   TEXT char
#define   UTEXT unsigned char
#define   TE   15             /* tamanho padrao de uma especificacao   */

typedef struct {
   long  trus_reg;      /* registro corrente do usuario */
   TEXT  trus_nome[15]; /* nome do usuario logado */
   UTEXT trus_zero;     /* fim do nome sempre NULL - Uso para alinhar word */
   TEXT  trus_modo;     /* modo utilizacao registro. E-exclusivo C-compart.*/
   UTEXT trus_fut1;     /* futuro 1 --- preocupacao com alinhamento de Words */
   UTEXT trus_fut2;     /* futuro 2 --- preocupacao com alinhamento de Words */
   UTEXT trus_fut3;     /* futuro 3 --- preocupacao com alinhamento de Words */
   long  trus_pid;      /* Ident processo p/ evitar conflito de nomes. Resto de time() */
} Tr_usu;

typedef struct {
   long  tr_ident;     /* Identificacao do arquivo de lock inicial */
   UTEXT tr_status;    /* status da base   T-travada   E-Modo exclusivo  0-Livre  C-Compart */
   UTEXT tr_logados;   /* num usuarios com a base aberta */
   UTEXT tr_pesquisa;  /* num usuarios efetuando pesquisa na base */
   TEXT  tr_nome[15];  /* nome em caso de travada/exclusiva */
   TEXT  tr_zero;      /* fim do nome sempre NULL - Uso para alinhar word */
   TEXT  tr_aguarde;   /* Aguardando vez para atualizar (gravar) */
   long  tr_pid;       /* Identificacao do processo em uso exclusivo */
   long  tr_time;      /* Time do Lock */
} Tr_cabec;



   Tr_cabec   *ap_trcabec;
   Tr_usu     *ap_trusu;
   short       ap_trfile;
   short       ap_trind;  /* indice do usuario no vetor de usuarios */
   long        ap_trpid;  /* pid do usuario na sessao LIGHT */
   TEXT        usu_usando[TE+1];
   long        ap_trident;  /* Numero magico do arquivo de controle */


#ifdef MSDOS
   short mostra_controle( VOID );
   short mostra_resto( VOID );
   void inicializa( char * );
#else
   short mostra_controle( );
   short mostra_resto( );
   void inicializa(  );
#endif

main ( argc, argv)
int argc;
char **argv;
{  char aux[30], file[80];
   
   if ( argc == 2 ) {
      strcpy(file, argv[1]);
   }
   else {
      printf("Nome do arquivo de lock (sem extensao) ? ");
      gets(file);
   }

   strcat(file, ".lck");

   if ( access(file, 0) != 0 ) { /* NAO existe arquivo de lock */
      fprintf(stderr, "Nao existe arquivo\n");
      exit(13);
   }

/*
   printf("Maximo de usuarios ");
   gets(aux);
   ap_max_users = atoi(aux);
*/

   inicializa( file );

   mostra_controle();
   mostra_resto();

   return(0);
}



short mostra_controle()
{  char aux[30];

   lseek(ap_trfile, 0L, 0);

   if ( read(ap_trfile, (TEXT *)ap_trcabec, TR_TAM) != TR_TAM ) {
      printf("Nao consegui todos os bytes do arquivo. (errno = %d)\n", errno);
      exit(1);
   }

   printf("Header do arquivo de lock\n");
   printf( "%-10ld %s", ap_trcabec->tr_ident   , "Identificacao do arquivo de lock inicial \n");
   printf( "%-10c %s",  ap_trcabec->tr_status  , "status T-travada  E-Modo exclusivo  0-Livre  C-Compart\n");
   printf( "%-10d %s",  ap_trcabec->tr_logados , "num usuarios com a base aberta \n");
   printf( "%-10d %s",  ap_trcabec->tr_pesquisa, "num usuarios efetuando pesquisa na base \n");

   if ( ap_trcabec->tr_nome[0] != '\0' )
      printf( "%10s %s",  ap_trcabec->tr_nome    , "nome em caso de travada/exclusiva \n");
   else
      printf( "%10s %s",  ""    , "nome em caso de travada/exclusiva \n");
   printf( "%-10d %s",  ap_trcabec->tr_aguarde , "Aguardando vez para atualizar (gravar) \n");
   printf( "%-10ld %s", ap_trcabec->tr_pid     , "Identificacao do processo em uso exclusivo \n");
   printf( "%-10ld %s", ap_trcabec->tr_time    , "Time do Lock \n");

   printf( "\nTecle algo para continuar ...");
   gets(aux);

   return(0);
}



short mostra_resto()
{  int i;

   if ( read(ap_trfile, (TEXT *)ap_trcabec, TR_TAM) < 0 ) {
      printf("Nao consegui todos os bytes do arquivo. (errno = %d)\n", errno);
      exit(1);
   }

   for ( i = 0 ; i < ap_max_users; i++ ) {
      printf( "%ld %s", ap_trusu[i].trus_reg     , "(Reg)  ");
      if ( ap_trusu[i].trus_nome[0] == '\0')
         printf( "%s %s", "[Nenhum]"    , "(nome)  ");
      else
         printf( "%s %s", ap_trusu[i].trus_nome    , "(nome)  ");
      printf( "%c %s", ap_trusu[i].trus_modo    , "(Modo E/C)  ");
      printf( "%ld %s\n", ap_trusu[i].trus_pid     , "(Pid)");
      printf("\n");
   }

   return(0);
}


void inicializa( file )
char *file;
{  long pos;
   FILE *fp;

   if ((ap_trcabec = malloc(TR_TAM)) == NULL ) {
      fprintf(stderr, "Nao consegui alocar\n");
      exit(13);
   }

   ap_trusu   = (Tr_usu *)(((TEXT *)ap_trcabec) + sizeof(Tr_cabec));

   ap_trfile  = -1;

#ifdef MSDOS
   fp = fopen(file, "rb");
#else
   fp = fopen(file, "r");
#endif

   if ( fp == NULL ) { /* NAO abriu arquivo */
      fprintf(stderr, "Nao abriu arquivo\n");
      exit(13);
   }

   fseek(fp,0,2);
   pos = ftell(fp);
   pos -= sizeof(Tr_cabec);
   ap_max_users = pos / sizeof(Tr_usu);
   printf("Max = %d\n", ap_max_users);
   fclose(fp);


#ifdef MSDOS
   ap_trfile = open(file, (O_BINARY | O_RDWR | O_DENYNONE) , (S_IREAD | S_IWRITE));
#else
   ap_trfile = open(file, O_RDWR);
#endif


}
