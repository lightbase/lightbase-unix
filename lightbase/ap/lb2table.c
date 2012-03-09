#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>
#include  <errno.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2prot1.h"


#define   TAM_TAB   ap_maxitens
#define   TAM_LINHA 100


#define   DEFINE     'D'

extern    SI    errno;

#ifdef MSDOS
   COUNT car_table(TEXT *);
   VOID  limpa_str( UTEXT * );
#else
   COUNT car_table();
   VOID  limpa_str();
#endif


F__GLB   COUNT ld_matriz(sele)
COUNT sele;
{  COUNT ret = 0, n, nc;
   TEXT *p, nome_tab[TB+1];
   COUNT num_campos;
   Campo **vet_cmp, *pc;

   lib_matriz();

   num_campos = qtd_vetcmp(sele);
   vet_cmp    = pos_vetcmp(sele);

   for ( nc = 0 ; nc < num_campos ; nc++) {

      pc = vet_cmp[nc];

      if ( pc->cp_tipo == 'F' || pc->cp_rodape[0] == '@') {
         if ( pc->cp_tipo == 'F')
            p = pc->cp_mascara;
         else 
            p = pc->cp_rodape;
         while (*p) {
            if ( *p == '#' ) {
               /* Teremos dois tipos de tabelas. */
               /* tabelas com numero sao associadas a tabelas indexadas */
               /* tabelas com nome sao  sequencias */
               if ( atoi(p+1) == 0 ) {
                  n = tab_number(p+1, nome_tab);
                  if ( nome_tab[0] != '\0' &&   n < 0   &&
                     car_table(nome_tab) < 0 ) {
                     mens_erro(H_OPTAB, E_OPTAB, nome_tab);
                     ret = -1;
                     goto fim;
                  }
               }
               else {
                  /* a principio nao faz nada */
               }
            }
            p++;
         }
      }
   }

fim :

   return(ret);
}


F__GLB   VOID lib_matriz()
{  COUNT i;
   if ( ap_vetmatriz != NULL ) {
      for (i=0; i < ap_maxmatriz; i++) {
         if ( ap_vetmatriz[i].tb_inicio != NULL )
            free(ap_vetmatriz[i].tb_inicio);
         ap_vetmatriz[i].tb_inicio = NULL;
         ap_vetmatriz[i].tb_qtd = 0;
         ap_vetmatriz[i].tb_nome[0] = '\0';
      }
   }
}


F__LOC   COUNT car_table(nome_arq)
TEXT *nome_arq;
{  COUNT ret, so_contabiliza, num_table, i;
   UCOUNT tam, quant, elem;
   FILE *fp = NULL;
   TEXT aux[250];
   UTEXT *pl;
   TEXT *ps;

   num_table = -1;

   for ( i = 0; i < ap_maxmatriz; i++ ) {
      if ( ap_vetmatriz[i].tb_nome[0] == '\0' ) {
         strcpy(ap_vetmatriz[i].tb_nome, nome_arq);
         num_table = i;
         break;
      }
   }

   if ( num_table < 0 ) {
      ret = -1;
      goto fim;
   }


   so_contabiliza = 1;
   quant = elem = 0;

   ret = info_base(nome_arq, WBS->bs_path, EXT_TABELA, aux);

   if ( (ret <= 0 ) ||
        ((ret & PODE_LER) == 0 ) ||
        ( fp = fopen(aux, "r")) == NULL ) {
      ret = -1;
      goto fim;
   }

   pl = (UTEXT *)aux;  /* so tira warnings */

volta :

   while ( ! feof(fp) ) {

      if ( fgets(aux, sizeof(aux) - 1, fp ) == NULL ) break;

      strip_nl(aux);

      if ( aux[0] == '#' ) continue;   /* linhas de comentario */

      tam = strlen(aux);  /* fim + tamanho */
      
      if ( tam == 0 ) continue;

      tam+=2;

      if ( so_contabiliza ) {
         elem++;
         quant += tam;
         continue;
      }
      else {
         *pl++ = tam;
         memcpy((TEXT *)pl, aux, tam-1);
         /* prep_palavra((TEXT *)pl);  @@@ ANANIAS vai diferenciar acentos */
         pl += (tam - 1);
      }
   }

   ret = 0;

   if ( so_contabiliza ) {
      /* aloca area para strings */
      if ( (ps = calloc(quant, sizeof(TEXT))) == NULL ){
         ret = -3;
         goto fim;
      }

      if ( ap_vetmatriz[num_table].tb_inicio != NULL )
         free(ap_vetmatriz[num_table].tb_inicio);

      pl = (UTEXT *)ps;
      ap_vetmatriz[num_table].tb_inicio = ps;
      ap_vetmatriz[num_table].tb_qtd = elem;
      so_contabiliza = 0;
      fseek(fp, 0L, 0);
      goto volta ;
   }


fim :

   if ( fp != NULL ) fclose ( fp );

   return(ret);
}



F__GLB   COUNT file_array( tab_nome, opcoes, tamanho, operacao )
SC     *tab_nome;
SC    **opcoes;
SS      tamanho;
SS      operacao;
{
   SS      ret, I, J;
   SC      linha[ TAM_LINHA + 1 ];
   SC      arquivo[PATH_MAX + 1];
   FILE   *f1;

   if ( tab_nome == NULL  ||  tab_nome[ 0 ] == '\0' ) {
      ret = -1;
      goto fim;
   }

   if ( WBS != NULL ) {
      ret = info_base(tab_nome, WBS->bs_path, EXT_TABELA, arquivo);
      if ( ret & PODE_LER )
         goto abre;
   }

   ret = info_base(tab_nome, ap_dirbase, EXT_TABELA, arquivo);

   if ( (ret & PODE_LER) == 0 ) {
      if ( errno == ENOENT && operacao == DEFINE ) ret = 0;
      else ret = -1;
      goto fim;
   }

abre:

   if (( f1 = fopen( arquivo, "r" ))  ==  NULL ) {
      ret = -1;
      goto fim;
   }
      
   I = 0;

   while ( I < TAM_TAB ) {

      if ( fgets( linha, TAM_LINHA, f1 ) == NULL ) break;

      strip_nl(linha);

      limpa_str( (UTEXT*) linha );

      J = MIN( tamanho, strlen( linha ) );

      if ( J == 0 ) continue;

      if (( opcoes[ I ] = malloc( J + 1 ) )  == NULL ) {
         ret = -1;
         goto fim;
      }

      strncpy( opcoes[ I ], linha, J );

      opcoes[ I ][ J ] = '\0';
      
      I++;
   }

   fclose( f1 );

   ret = I;

fim :

   return ( ret );
}




F__GLB   VOID free_array( opcoes )
SC   **opcoes;
{
   SS    I;

   if ( opcoes != NULL ) {
      for ( I = 0;   opcoes[ I ]  !=  NULL;   I++ )
      {
         free( opcoes[ I ] );
      }
   }
}



F__GLB   COUNT write_array( tab_nome, opcoes)
SC     *tab_nome;
SC    **opcoes;
{
   SS    ret, I;
   SS    e1, e2;
   FILE *f1;
   SC      arquivo[PATH_MAX + 1];

   /* ANANIAS devera recarregar arquivo para sistemas multi-usuarios */


   if ( tab_nome == NULL  ||  tab_nome[ 0 ] == '\0' ) {
      ret = -1;
      goto fim;
   }

   e1 = info_base(tab_nome, ap_home_lb, EXT_TABELA, arquivo);

   if ( WBS != NULL ) 
      e2 = info_base(tab_nome, WBS->bs_path, EXT_TABELA, arquivo);

   if ( e2 == 0 && (e1 & PODE_LER)) {
      /* so grava no home se existir em home e nao existir na base */
      info_base(tab_nome, ap_home_lb, EXT_TABELA, arquivo);
   }

abre:

   if (( f1 = fopen( arquivo, MODO_WR ))  ==  NULL ) {
      ret = -2;
      goto fim;
   }

      
   for ( I = 0;   opcoes[ I ]  !=  NULL;   I++ )
   {
      fprintf( f1, "%s\n", opcoes[ I ] );
   }

   fclose( f1 );

   ret = I;

fim :

   return ( ret );
}



F__LOC   VOID limpa_str( linha )
UTEXT *linha;
{
   UTEXT *P1, *P2;

   for ( P1 = P2 = (UTEXT *)linha;   *P1 != '\0';   P1++ )
      if (e_print( *P1 ))
         *P2++ = *P1;

   *P2++ = *P1;
}


