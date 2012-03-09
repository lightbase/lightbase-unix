#include  "lb2all.h"


#include  <ctype.h>
#include  <string.h>


#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"  /* strings comus a todos os modulos */
#include  "lb2prot1.h"



#define   H_NOSOUND    "NOSOUD"       /* soudex.lb nao pode ser lido */


#define   FILE_SOUND   loc_str(STR_GLB,  6)

#define   E_NOSOUND    loc_str(STR_GLB,  590)


#define  MAIOR_FONEMA   3        /* Maior tamanho para divisao da palavra em fonemas */
#define  MAX_FONEMA     (MAIOR_FONEMA+1) /* Tamanho maximo do fonema */
#define  MAIOR_PALAVRA  25        /* Tamanho da Maior Palavra */
#define  SEP_FONEMA   ","
#define  SEP_NUMERO   "|"
#define  MAX_LINHA    250        /* Tamanho maximo da linha  */

#define  FONEMA(I)    tab_fonema[(I*MAX_FONEMA)+1]  /* Posiciona no fonema I */
#define  N_FONEMA(I)  tab_fonema[I*MAX_FONEMA]      /* Posiciona no Nro. do fonema I */

#define   CONTANDO   'C'
#define   SETANDO    'S'

static UTEXT *tab_fonema;
static COUNT num_fonemas;

#ifdef MSDOS
   TEXT   *fonema( TEXT * );
   COUNT   achou_fonema( TEXT *, COUNT *, COUNT  );
   VOID    carrega_vetor( VOID );
   SI      fsort( );
   SI      xsort( );
   TEXT   *tira_branco( TEXT * );
#else
   TEXT   *fonema( );
   COUNT   achou_fonema( );
   VOID    carrega_vetor( );
   SI      fsort( );
   SI      xsort( );
   TEXT   *tira_branco( );
#endif


F__GLB   COUNT load_soundex()
{
   COUNT ret, ind, status, num_fonema;
   TEXT aux[MAX_LINHA+1], *p;
   FILE *fp=NULL;


   ind = 0;
   status = CONTANDO;

volta :

   ret = info_base(FILE_SOUND, ap_home_lb, str_nada, aux);

   if ( ret < 0 ||
         (ret & PODE_LER) == 0 ||
         (fp = fopen(aux, "r" )) == NULL ) {
      mens_erro(H_NOSOUND, E_NOSOUND);
      ret = -1;
      goto fim;
   }



   ind = 0;  /* Controla posicao no vetor de fonemas */

   while( fgets( aux, MAX_LINHA, fp ) ) {
      strip_nl( strupr(aux) );

      p = strtok( aux, SEP_NUMERO);

      if ( p == NULL )
         continue;

      num_fonema = atoi(p);

      if ( num_fonema <= 32 && num_fonema >= 254 )
         continue;

      while ( p != NULL ) {
         if ( (p = strtok(NULL, SEP_FONEMA)) != NULL) {
            if ( status == CONTANDO ) {
               ind++;
            }
            else {
               N_FONEMA(ind) = num_fonema;
               memcpy( &FONEMA(ind), p, strlen(p) );
               ind++;
            }
         }
      }
   }

   fclose( fp );
   fp = NULL;

   if ( status == CONTANDO ) {
      num_fonemas = ind;
      if ( (tab_fonema = calloc(ind+1, MAX_FONEMA)) == NULL ) {
         mens_erro(H_NOMEMO, SEM_MEMO);
         ret = -1;
         goto fim;
      }

      memset( tab_fonema, ' ', (ind+1) * MAX_FONEMA );

      status = SETANDO;
      goto volta;
   }

   N_FONEMA(ind) = 0;


   qsort( tab_fonema, num_fonemas, MAX_FONEMA, fsort );

   ret = 0;

fim :

   if ( fp != NULL )
      fclose( fp );

   return(ret);
}



/*---- converte palavra em simbolo fonema. retorna vet_fonema que e global */

F__GLB   TEXT *fonema( str_ori )
TEXT *str_ori;
{
   static UTEXT vet_fonema[MAIOR_PALAVRA + 1];
   COUNT t, num, d, i ;
   TEXT str[MAIOR_PALAVRA+1];
   TEXT *p;

   strncpy(str, str_ori, MAIOR_PALAVRA);
   str[MAIOR_PALAVRA] = '\0';
   strupr( str );

   t = strlen( str );

   i = 0;
   p = str;

   while( t )
   {
      d = achou_fonema( p, &num, t );

      if( num != 0 )
      {
         vet_fonema[i] = num;
         i++;
      }

      p += d;
      t -= d;
   }

   vet_fonema[i] = '\0';

   return( (TEXT *)vet_fonema );
}

/*---- localiza o fonema ----*/

F__LOC   COUNT achou_fonema( p, num, t )
TEXT  *p;
COUNT *num, t;
{
   UTEXT *p_item, c;
   COUNT m, ret, i;

   ret  = 1;
   *num = 0;

   m = MIN(MAIOR_FONEMA, t );

   for( i = m; i > 0; i-- )
   {
      c      = p[i];
      p[i] = '\0';
      p_item = (UTEXT*) bsearch( p, tab_fonema, num_fonemas, MAX_FONEMA,
                                 xsort );
      p[i] = c;
      if( p_item != NULL )
      {
         *num = ( COUNT ) *p_item;
         ret = i;
         break;
      }
   }

   return(ret);
}




/*---- Funcao utilizada por bsearch para ordenar fonema ----*/

SI fsort( s1, s2 )
TEXT *s1, *s2;
{
   return( strcmp((TEXT *) s1+1, (TEXT *) s2+1) );
}

/*---- Funcao utilizada por bsearch para localizar fonema ----*/

SI xsort( s1, s2 )
TEXT *s1, *s2;
{
   TEXT p[MAX_FONEMA+1];

   memset(p, 0, MAX_FONEMA+1);
   strncpy( p, (TEXT *) s2, MAX_FONEMA );

   tira_branco( p );

   return( strcmp( (TEXT *) s1, p+1) );
}



/*---- tira branco do final de uma cadeia de caracteres. ----*/

TEXT *tira_branco( buf )
TEXT *buf;
{
   TEXT *p;

   p = strchr( buf, 0 );

   if ( p != buf ) {
      p--;
      while( *p == ' ' )
         p--;
      p++;
      *p = '\0';
   }

   return(buf);
}

