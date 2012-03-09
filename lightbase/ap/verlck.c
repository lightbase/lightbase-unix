#include  "lb2all.h"

#ifdef    VAXVMS
#include  <file.h>
#else
#include  <fcntl.h>
#endif

#include  <string.h>
#include  <time.h>

#ifdef UNIX
#include  <unistd.h>
#endif

#include  HD_IO

#ifdef    VAXVMS
#include  <types.h>
#include  <stat.h>
#else
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

#define   TR_DEFINE

#include  "lb2files.h"
#include  "lb2stru.h"
#include  "lb2trans.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2prot1.h"


short ap_max_users;

main(argc, argv)
int argc;
char **argv;
{
   TEXT nx[PATH_MAX+1];

   if ((ap_trcabec = malloc(TR_TAM)) == NULL ) {
      fim_lb(SEM_MEMO);
      exit(13);
   }


   ap_trusu   = (Tr_usu *)(((TEXT *)ap_trcabec) + sizeof(Tr_cabec));
   ap_trfile  = -1;

   printf("Nome do arquivo de lock ?");
   gets(nx);


   if ( access(nx, 0) != 0 ) {
      printf("Arquivo de lock nao existe\n");
      exit(1);
   }

#ifdef MSDOS
   ap_trfile = open(nx, (O_BINARY | O_RDWR | O_DENYNONE) , (S_IREAD | S_IWRITE));
#else
   ap_trfile = open(nx, O_RDWR);
#endif

   mostra_1();
}

mostra_1()
{

   if ( read(ap_trfile, (TEXT *)ap_trcabec, TR_TAM) < 0 ) {
      printf("Erro na leitura do header\n");
      return(-1);
   }


   printf("Status da Base :");
   switch(ap_trcabec->tr_status) {
      case LIVRE	  :
               printf("Livre\n");
               break;
      case COMPARTILHADO :
               printf("Compartilhado\n");
               break;
      case EXCLUSIVO :
               printf("Exclusivo\n");
               break;
      case TRAVADO   :
               printf("Travado\n");
               printf("Pid = %d\n", ap_trcabec->tr_pid);
               break;
      dafault        :
               printf("Erro no arquivo LCK. Status da base desconhecido\n");
               break;
   }
}

void fim_lb(x)
char *x;
{

exit(1);

}
