#include  "lb2all.h"

#include  <stdlib.h>
#include  <string.h>

#ifdef    VAXVMS
#include  <file.h>
#include  <types.h>
#include  <stat.h>
#else
#include  <fcntl.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

#include  HD_IO

#ifdef UNIX
#include  <unistd.h>
#endif

/*
#include  <ctype.h>
#include  <time.h>
#include  HD_MEMO
#include  HD_DIR
#include  HD_IO
#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
*/

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */


#define   GRP_OPCLS  18

#define   E_AUTONUM3   loc_str(GRP_OPCLS, 518)
#define   E_AUTONUM4   loc_str(GRP_OPCLS, 519)
#define   E_AUTONUM5   loc_str(GRP_OPCLS, 520)



/******************************

Implementacao do campo de Incremento Automatico.

Jarbas, em 20/05/94. Madrid

A ideia aqui e':

	1) Define-se um campo como sendo de qq tipo.

	2) Assim que a base for aberta, procuro um arquivo
	   com o mesmo nome da base, com a extensao .CAI
	   (campo auto-incrementado).

	3) Este arquivo pode ter uma mascara qualquer a ser usada
	   em sprintf para formatar o valor auto-incrementado, juntamente
	   com o numero atual a ser incrementado.

	4) Assim que o usuario entrar no campo a ser autoincrementado
	   ele pode fazer uma de tres coisas.

	   	a) Deixar o campo em branco, e neste caso o light ira' preen-
	   	   che-lo no momento em que for pressionado F2. O numero nao
	   	   sera' visivel para o operador. Neste caso o campo nao podera'
	   	   ter sido definido como obrigatorio.

	   	b) Pressionar F10 para receber o valor do campo, e neste caso
	   	   o resultado recebido será gravado naquele estado, podendo ocorrer
	   	   inconsistencia se o registro for abandonado antes de
	   	   ser gravado.

	   	c) Preencher com o conteudo desejado, caso em que o light nao
	   	   modificara o seu conteudo.

   O arquivo deve conter apenas uma llinha com o seguinte formato:

   "mascara para sprintf",valor atual

   Exemplo:

   "%05d/94",35

   Naturalmente o numero sera' atuallizado para que na proxima
   secao de entrada de dados tudo esteja de acordo.

   Esta implementacao so' admite um campo de autoincremento por
   base.
******************************/

/********* variaveis estaticas para este ambiente *********/

static TEXT an_cpnome[20];
static TEXT *mascara;
static TEXT *an_filename;
TEXT auto_last[81];
static int fdesc = -1;
static COUNT n_campo, an_padrao, an_cpind;
static LONG next;
COUNT ap_flag_auto = 0;


#ifdef MSDOS
   VOID auto_close( VOID );
   COUNT auto_init( COUNT, COUNT, TEXT *, TEXT *);
   LONG auto_getnext( VOID );
   LONG auto_incre( TEXT *, COUNT );
   COUNT auto_getfield( VOID );
   COUNT ver_campo(COUNT, TEXT *, COUNT *);
   COUNT abre_fdesc(TEXT *);
#else
   VOID auto_close( );
   COUNT auto_init( );
   LONG auto_getnext( );
   LONG auto_incre( );
   COUNT auto_getfield( );
   COUNT ver_campo( );
   COUNT abre_fdesc( );
#endif


VOID auto_close( )
{
	/*
		Encerra o modo autoincremento, salvando no arquivo .aut
		o número do proximo incremento a ser reallizado.
	   Util quando muda a base ativa
	*/

	if ( ! ap_flag_auto )
      return;

   if ( mascara != NULL )
      free(mascara);
   if ( an_filename != NULL )
      free(an_filename);

	close(fdesc);

	ap_flag_auto = 0;
	fdesc = -1;
   mascara = NULL;
   an_filename = NULL;
   an_cpnome[0] = '\0';
   an_cpind = -1;

	return;

}

COUNT auto_init( sele, campo, nome, path )
COUNT sele, campo;
TEXT *nome, *path;
{  COUNT ret=0;
	TEXT *ptc;
   Campo *pc;

	ap_flag_auto = 0;
	n_campo = campo;

	pc = (Campo *)v_campos[sele]->vi_vtit[campo];

	/**** preparar o arquivo de controle do autoincremento ****/
   /* a mascara esta na descricao do campo apos uma , ou o campo todo */
	ptc = pc->cp_param.tab_auto.autonum;

   if ( strchr(ptc, '%') == NULL ) {
      pc->cp_critica = 'N';
      ret = -1;
      debug(E_AUTONUM5);
      goto fim;
   }

   /* para respeitar compatibilidade com versoes anteriores */
   /* so vou mudar a forma de fazer as coisas caso se use nova sintaxe */

   an_padrao = 1;

   /* caso comece por @ a sintaxe eh a sequinte: @nomecp,mascara */
   if ( ptc[0] == '@') {
      TEXT *p;
      COUNT num;
      p = strchr(ptc, ',');
      if ( p == NULL ) {
         pc->cp_critica = 'N';
         ret = -1;
         debug(E_AUTONUM5);
         goto fim;
      }
      else {
         *p = '\0';
         strcpy(an_cpnome, ptc+1);
         *p = ',';
         ptc = p + 1;
      }
      an_padrao = 2;
      if ( (an_cpind = ver_campo(sele, an_cpnome, &num)) == -1 ) {
         pc->cp_critica = 'N';
         ret = -1;
         debug(E_AUTONUM5);
         goto fim;
      }

   }

   mascara = malloc(strlen(ptc) + 1);
	strcpy( mascara, ptc );

   an_filename = malloc(strlen(path) + 2);
	strcpy( an_filename, path );
   strcat( an_filename, STR_BARRA);


   /* se padrao for antigo abre arquivo agora */

   if ( an_padrao == 1 ) {
      TEXT aux[255];

	   /* abrir arquivo "base.aut" */
	   strcpy( aux, an_filename );
	   strcat( aux, nome );
	   strcat( aux, ".aut" );

      if ( abre_fdesc(aux) < 0 ) {
         ret = -1;
      }
   }

	ap_flag_auto = 1;

fim :

   return(ret);
}

LONG auto_getnext( )
{
	/* Devolve o proximo numero sequencial */
   COUNT rl, leu;
   TEXT buf[30];

	if ( ! ap_flag_auto )
      return -1;

   if ( an_padrao == 2 ) {
      TEXT aux[255], *p;
      Campo *pc;
   	pc = (Campo *)v_campos[BASE_EDIT]->vi_vtit[an_cpind];
   	p = pc->cp_newdata;

      if ( strlen(p) > 8 || *p == '\0' ) {
         debug("Nao e possivel gerar numero para campo sem informacao ou maior que 8");
         next = 0;
         goto fim;
      }

      strcpy(aux, an_filename);
      strcat(aux, p);
      strcat(aux, ".aut");


      if ( abre_fdesc(aux) < 0 ) {
         debug("Erro na geracao de autonum para arquivo %s", aux);
         next = 0;
         goto fim;
      }
   }

	/* dar um lock no arquivo */
	lseek( fdesc, SEEK_SET, 0 );

#ifdef MSDOS
	rl = lock( fdesc, 0L, 99 );
#else
#ifndef VAXVMS
	rl = lockf( fdesc, F_LOCK, 0 );
#endif
#endif

   if ( rl < 0 ) {
      debug( E_AUTONUM3 );
   }

	/* ler o numero do atual sequencial */
   buf[0] = '\0';

   leu = read(fdesc, buf, sizeof(buf) - 1);

   if ( leu < 1 ) {
      debug( E_AUTONUM4 );
      next = -1;
   }
   else {
      next = atol(buf);
      next++;
   	/* salvar o numero a ser usado */
   	lseek( fdesc, SEEK_SET, 0 );

      sprintf(buf, "%ld \n", next);
      write(fdesc, buf, strlen(buf) + 1);

      /* flush( fdesc ); */
   }

	/* liberar o lock */
	lseek( fdesc, SEEK_SET, 0 );
#ifdef MSDOS
	rl = unlock( fdesc, 0L, 99 );
#else
#ifndef VAXVMS
	rl = lockf( fdesc, F_ULOCK, 0 );
#endif
#endif

   if ( rl < 0 ) {
      debug( E_AUTONUM3 );
   }

   if ( an_padrao == 2 && fdesc != -1 ) {
      close(fdesc);
      fdesc = -1;
   }

fim :

	return next;
}

LONG auto_incre( dest, tam )
TEXT *dest;
COUNT tam;
{  LONG ret;
   TEXT s[81];
	/* insere na string s o resultado da mascara com a acao do
	   auto-incremento
	*/

	if ( ! ap_flag_auto )
      return -1;

   ret = auto_getnext();

   if ( ret > 0 )
      sprintf( s, mascara, ret );
   else
      s[0] = '\0';

   s[tam] = '\0';

   strcpy( dest, s);
   strcpy( auto_last, s);

	return ret;
}

COUNT auto_getfield(  )
{
	/* Retorna o numero do campo que tem auto_incremento */

	if ( ! ap_flag_auto ) return -1;
	else               return n_campo;
}



COUNT abre_fdesc(filename)
TEXT *filename;
{  COUNT ret = 0, rl;

abre :

#ifdef MSDOS
   fdesc = open(filename, (O_BINARY | O_RDWR | O_DENYNONE) , (S_IREAD | S_IWRITE));
#else
   strlwr(filename);

   fdesc = open(filename, O_RDWR);
#endif

	if ( fdesc == -1 ) {
#ifdef MSDOS
   	fdesc = open(filename, (O_CREAT | O_TRUNC | O_BINARY | O_RDWR),
                       (S_IREAD | S_IWRITE));
#else
      fdesc = creat(filename, 0666);
#endif

      if ( fdesc == -1 ) {
         ret = -1;
         goto fim;
      }
      else {
		   /* dar um lock no arquivo */
#ifdef MSDOS
   		rl = lock( fdesc, 0L, 99 );
#else
#ifndef VAXVMS
   		rl = lockf( fdesc, F_LOCK, 0 );
#endif
#endif
         if ( rl < 0 ) {
            debug( E_AUTONUM3 );
         }

		   /* gravar o novo valor */

         /*        sprintf(buf, "%ld \n", 0);  */
         /*        write(fdesc, buf, strlen(buf) + 1); */

         write(fdesc, "0\n", 3);

		   /* liberar o lock */
   		lseek( fdesc, SEEK_SET, 0 );
#ifdef MSDOS
	   	rl = unlock( fdesc, 0L, 99 );
#else
#ifndef VAXVMS
   		rl = lockf( fdesc, F_ULOCK, 0 );
#endif
#endif
         if ( rl < 0 ) {
            debug( E_AUTONUM3 );
         }

         close(fdesc);
         goto abre;
      }
	}

fim : 

   return(ret);
}
