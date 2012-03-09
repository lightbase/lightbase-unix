#include  "lb2all.h"


#include  <string.h>
#include  <ctype.h>

#ifdef     VAXVMS
#include  <types.h>
#include  <stat.h>
#else
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2entd.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "lb2trans.h"


#define   H_CFNOLOG           "CFNOLOG"   /* Erro na gravcao do log */
#define   H_FTLIBERA          "ARQLIBER"  /* Falta arquivo de liberacao comercial */
#define   H_NOLIBERA          "NOLIBERA"  /* Autorizacao nao ok p/ base comercial */


#define   E_FTLIBERA     loc_str( STR_GLB, 592)
#define   E_NOLIBERA     loc_str( STR_GLB, 593)


#define   TAM_SERIE    6


typedef struct {
   TEXT *cadeia;
   TEXT  sep;
   UCOUNT  achou;
} PT_TOKEN;


#ifdef MSDOS
   VOID chacoalha(TEXT *, TEXT *, UCOUNT, UCOUNT, UCOUNT);
   VOID roda_viva( TEXT * );
   VOID pega_strid( TEXT *, TEXT *);
   VOID xor_id( TEXT *, TEXT * );
   UCOUNT pega_tok( TEXT *, TEXT *, PT_TOKEN *);
   VOID xstr_nible( TEXT *, TEXT * );
   SC   xget_nible( SC, SI );
#else
   VOID chacoalha( );
   VOID roda_viva( );
   VOID pega_strid( );
   VOID xor_id( );
   UCOUNT pega_tok( );
   VOID xstr_nible( );
   SC   xget_nible( );
#endif



/*
** Dado o nome de uma base e o path da mesma, verifica a existencia da Base
** e tambem a possibilidade do usuario le/gravar a mesma. Alem disto,
** retorna em full_name da mesma (PATH+NOME+.TBF)
**
** info_base : retorna : -1 ----> se nome arquivo e improprio (esp / diret)
**                       0  ----> nao existe o arquivo
**                       1  ----> existe e nao pode ler/gravar
**                       2  ----> so pode ler
**                       4  ----> so pode gravar
**                       6  ----> pode ler // gravar
*/
F__GLB   COUNT info_base(nome_base, path, extensao, full_name)
TEXT *nome_base, *path, *extensao, *full_name;
{  COUNT ret, tam;
   struct stat status;
	TEXT *p, c;


   trim(nome_base);
   trim(path);

   p=full_name;

   do {
      c = *path++;
      if ( c == OUTRA_BARRA ) c = BARRA;
      *p++ = c;
   } while ( c != '\0' );

   if (full_name[0] != '\0' ) {
      strcat(full_name, STR_BARRA);
   }

   strcat(full_name, nome_base);  /* nome final do arquivo a ser aberto */

	strcat(full_name, extensao);

   /* retira BARRA final se a mesma existir */
   tam = (COUNT) strlen(full_name);
   if ( tam > 0 ) {
      if ( full_name[tam - 1] == BARRA )
         full_name[tam - 1] = '\0';
   }


   for ( p=full_name; *p != '\0' ; p++) {
      c = *p;
#ifdef MSDOS
      if ( c >= 'a' && c <= 'z' ) *p = c - ' '; /* tudo maiusculo */
#else
      if ( c >= 'A' && c <= 'Z' ) *p = c + ' '; /* tudo minusculo */
#endif
   };

   ret = stat(full_name, &status);

   if ( ret == -1 ) {
      ret = 0;
      goto fim;
   }

   if ( (status.st_mode & (unsigned)S_IFMT ) != (unsigned)S_IFREG ) {
      ret = -1;
   }
   else {
      ret = 0;

      if (status.st_mode & S_IREAD)  ret |= 2;
      if (status.st_mode & S_IWRITE) ret |= 4;
      if ( ret == 0 ) ret = 1; /* retorna OK sem poder ler/gravar */
   }

fim :

   return( ret );
}





COUNT look_regs(sele, total, ativos, deletados, to_index)
COUNT sele;
LONG *total, *ativos, *deletados, *to_index;
{  COUNT ret = 0;
   COUNT ret_trans;
   LONG conta = 0;
   LONG tot_regs;
   POINTER pos;

   if ( (ret_trans = BEG_TRANS(TR_CARREGAR, 0L, 0)) < 0 ) {
      ret = -1;
      goto fim;
   }


   tot_regs = LSTREC(AC_NUM(sele));

   /**/
   /*** conta numero de registros deletados */
   /**/

   pos = FRSKEY(AI_NUM(sele) + TREE_DELETADOS , ap_chave_ret);
   if ( uerr_cod != 0 ) {
      ret = -1;
      goto fim;
   }

   for ( conta = 0L; pos != (POINTER) 0; conta++) {
      pos = NXTKEY(AI_NUM(sele) + TREE_DELETADOS, ap_chave_ret);
      if ( uerr_cod != 0 ) {
         ret = -1;
         goto fim;
      }
   }

   if ( conta > 0 ) {  /* despreza ultimo registro se existe */
      if ( tot_regs == atol(ct_indkey)) {
         conta--;
         tot_regs--;
      }
   }

   if ( total     != NULL )  *total = tot_regs;
   if ( ativos    != NULL )  *ativos = tot_regs - conta;
   if ( deletados != NULL )  *deletados = conta;

   if ( to_index != NULL ) {
      pos = FRSKEY(AI_NUM(sele) + TREE_NAO_INDEX , ap_chave_ret);
      if ( uerr_cod != 0 ) {
         ret = -1;
         goto fim;
      }
      for ( conta=0L; pos != (POINTER) 0; conta++) {
         pos = NXTKEY(AI_NUM(sele) + TREE_NAO_INDEX, ap_chave_ret);
         if ( uerr_cod != 0 ) {
            ret = -1;
            goto fim;
         }
      }
      *to_index = conta;
   }

fim :

   if ( ret_trans >= 0 )
      END_TRANS(TR_CARREGAR);

   return(ret);
}



F__GLB   LONG pega_num()
{  LONG  nreg;
   WIN *jt;
   TEXT ch_num[20];
   DTela nr[2];
   COUNT a_sep_int, a_sep_dec;

   nr[0].destino = NULL;
   nr[1].destino = NULL;

   seta_dtela(&nr[0], ch_num, "999.999.999", M_REGSEL,
              1, 7, 0, E_VALOR, H_POSREG, NULL, NULL, NULL);

   if ( (jt = abre_janela(J_POSREG)) == NULL ) {
      nreg = -1;
      goto fim;
   }

   fw_title(jt, T_POSREG, jt->w_atrib);

	fwbox(jt);

   ch_num[0] = '\0';

   a_sep_int = sep_int;
   a_sep_dec = sep_dec;

   sep_int = '.';
   sep_dec = ',';

   ed_read(jt, &nr[0], 1, LIMPA_LE, "1 3 E");

   sep_int = a_sep_int;
   sep_dec = a_sep_dec;

   nreg = (LONG) str_to_db(ch_num, ',');

   if ( nreg < 0 )
      nreg = 0;

fim :

   fwkill(jt);

   return(nreg);
}





F__GLB   COUNT vl_log(entry)
d_entry *entry;
{  COUNT ret;
   COUNT tecla;
   TEXT  aux[PATH_MAX+1];

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP ||
		  tecla == K_HOME || tecla == K_PGUP) {
		ret = ACEITA;
		goto fim;
	};

   if ( entry->str[0] == '\0' ) {
      mens_erro(H_ARQLOG, E_IN_LOG, entry->str);
      ret = REJEITA;
      goto fim;
   }

   ret = info_base(entry->str, str_nada, str_nada, aux);

   if ( ret < 0 ) {
      mens_erro(H_NOREG, E_NOREG, entry->str);
      ret = REJEITA;
      goto fim;
   }

   if ( ret != 0  ) {
      if ( decida(TW_WL_D00, TW_WL_D99, "CF_RWLOG",
                  2, 36, "%s\n%s", TW_WL_D01, TW_WL_D02 ) != 1 ) {
         ret = REJEITA;
         goto fim;
      }
   }

  	ret = ACT_REIMP;

fim :

   return(ret);
}


F__GLB   VOID wr_log(adverte, mens)
COUNT adverte;
TEXT *mens;
{
   if ( adverte )
      ap_adverte++;

   if ( jl_glb != NULL ) {
      fwprintf(jl_glb, "%s\n", mens);
      fwrefresh(jl_glb);
   }

   if ( e_log_glb == 0 && fpo_glb != NULL) {
      if ( fprintf(fpo_glb, "%s\n", mens ) == EOF ) {
         if ( decida(TW_WL_T00, TW_WL_T99, H_CFNOLOG,
                     2, 43, "%s\n%s", TW_WL_T01, TW_WL_T02 ) != 1 ) {
            e_log_glb = 2;
         }
         else e_log_glb = 1;

      }
   }
}


F__GLB   COUNT pode_exportar(cod)
UCOUNT cod;
{  COUNT ret = 0;
   TEXT *senha, aux[20];


   senha = getenv("LBSENHA");
   if ( senha != NULL ) {
      sprintf(aux, "%06u", cod);
      if ( t_numse(aux, senha, 0L) == OK ) {
         ret = 'S';
      }
   }

   return(ret);
}

F__GLB   UCOUNT calc_bsxor(wbs)
Base *wbs;
{  LONG l;

   l=0;
   l += wbs->bs_idbase;
   l += wbs->bs_finger;
   l += wbs->bs_release;

   l  = l ^ wbs->bs_cr_data;

   if ( l == 0 )
      l = 1;

   return((UCOUNT)l);
}



F__GLB   COUNT base_liberada(wbs)
Base *wbs;
{  COUNT ret = 'N', linha = 0;
   LONG l;
   FILE *fp;
   TEXT aux[200], del = '\0', *p;
   TEXT token[60+1];
   TEXT senha[20], *psenha;

   psenha = getenv("LBSENHA");
   if ( psenha != NULL ) {
      sprintf(aux, "%06u", wbs->bs_idbase);
      if ( t_numse(aux, psenha, 0L) == OK ) {
         ret = 'S';
         goto fim;
      }
   }


   senha[0] = token[0] = '\0';

   info_base( wbs->bs_nome, wbs->bs_path, EXT_CONFIG, aux);

   if ( (fp = fopen(aux, "r")) == NULL ) {
      mens_erro(H_FTLIBERA, E_FTLIBERA);
      ret = -1;
      goto fim;
   }


   while ( fgets( aux, sizeof(aux) - 1, fp ) != NULL ) {

      strip_nl(aux);
      trim(aux);

      if (aux[0] == '\0')
         continue;


      for (p = strchr(aux, '\0'); *(p-1) != ' ' && p > aux; p-- )
         ;
      strncpy(senha, p, 15);
      *(p-1) = '\0';
      senha[16] = '\0';


      l = atol(aux);

      if ( ++linha == 1 && l == 0 ) {
         /* nome do usuario pode ser limitado usando [x] no inicio da string */
         if (aux[0] == '[' && aux[2] == ']') {
            del = aux[1];
            strncpy(token, aux+3, 60);
            p = strchr(token, del);
            if ( p != NULL )
               *p = '\0';
         }
         else {
            strncpy(token, aux, 60);
         }
         token[60] = '\0';
         break;
      }

      if ( l == ap_nserie ) {
         sprintf(token, "%6.6ld", l);
         break;
      }
   }

   fclose(fp);


   trim(token);
   chacoalha(aux, token, wbs->bs_finger, wbs->bs_release, wbs->bs_idbase);

   if ( stricmp(aux, senha) == 0 &&
        wbs->bs_xor == calc_bsxor(wbs) )
      ret = 'S';
   else
      mens_erro(H_NOLIBERA, E_NOLIBERA);


fim :

   return(ret);
}

F__LOC   VOID chacoalha(sai, str, n1, n2, n3)
TEXT *sai, *str;
UCOUNT n1, n2, n3;
{  TEXT strid[20], aux[30];
   LONG l;
   UTEXT c=0;
   COUNT i;

	memset( strid, 0, 20 );

	sprintf( aux, "%6.6u", n1 );
   roda_viva(aux);

   if ( (l = atol(str)) == 0 ) {
   	pega_strid( str, strid );
   }
   else {
      int x1,x2,x3;
      x1 = (int)l/10000;
      x2 = (int)((l%10000) / 100);
      x3 = (int)(l % 100);
      sprintf(str, "%2.2d %2.2d %2.2d", x1, x2, x3);
/*      sprintf(str, "%6.6ld %6.6ld %6.6ld", l, l/2, l/4); */
   	pega_strid( str, strid );
   }
	xor_id( aux, strid );

	sprintf( strid, "%6.6u", n2 );
   roda_viva(aux);
	xor_id( aux, strid );

	sprintf( strid, "%6.6u", n3 );
   roda_viva(aux);
	xor_id( aux, strid );


   for ( i = 0; i < 6 && str[i] != '\0'; i++) {
      c += str[i];
   }

   c = c & 0xf;

   for ( i = 0; sai[i] != '\0'; i++) {
      sai[i] = sai[i] + c;
   }

	xstr_nible(aux, sai);
}

VOID roda_viva(str)
TEXT *str;
{  COUNT i;

   for ( i = 0; str[i] != '\0'; i++) {
      str[i] += 1;
   }
}


VOID xor_id( serial, xorado )

SC *serial, *xorado;
{
	SI i,j,m;

	for ( i=0; serial[i]; ++i )
	{
	        m = i+1;

	        if( m > strlen(xorado) )
	           m = strlen(xorado);

		for ( j=m; xorado[j]; ++j )
		{
			serial[i] = serial[i] ^ xorado[j];
		}
	}

	return;
}


VOID pega_strid( linha, id )
SC *linha;
SC *id;
{
	PT_TOKEN Tok;
	SC aux[128];
	SI  i = 0, j = 0, k = 0 ;

	while( 1 )
	{
	 k = 0;
	 strcpy( aux, linha );
	 pega_tok( aux, " ", &Tok );
	 while ( *Tok.cadeia )
	 {
	    if ( j < strlen( Tok.cadeia ) )
	    {
		  id[i++] = toupper(Tok.cadeia[j]);
		  if ( i == 8 )
			goto fim;
		  k++;
	    }
	    pega_tok( NULL, " ", &Tok );
	 }
	 j++;
	 if ( !k )
	 {
	    while( i < 8 )
	    {
		  id[i] = 'A' + 8 - i - 1;
		  ++i;
	    }
	    goto fim;
	 }
	}
fim:
	return;
}


UCOUNT pega_tok( cadeia, separador, result )
TEXT *cadeia, *separador;
PT_TOKEN *result;
{
   /*
      Pega tudo de cadeia até encontrar separador. Quando encontrar
      coloca 'zero' na posicao e retorna 1. Se nao encontrar retorna 0.
   */

   static TEXT *pt_inicio, *pt_start;

   if( cadeia )   pt_inicio = cadeia;
   else           pt_inicio = pt_start;

   result->cadeia = pt_inicio;
   result->achou  = 0;

   for( ; *pt_inicio; ++pt_inicio )
   {
      if( strchr( separador, *pt_inicio ) )
      {
         result->achou = 1;
         result->sep   = *strchr( separador, *pt_inicio );
         *pt_inicio = 0;
         pt_start   = ++pt_inicio;
         goto fim;
      }
   }
   pt_start = pt_inicio;

fim:
   return result->achou;
}



VOID xstr_nible( chave, nibles )
SC *chave;
SC *nibles;
{
	SI i,j;

	i = 0;
	for ( j=0; j<TAM_SERIE; j++)
	{
	 nibles[i++] = xget_nible(chave[j],0);
	 nibles[i++] = xget_nible(chave[j],1);
	}

	nibles[ 2*TAM_SERIE ] = 0;
}


SC xget_nible( c, n )
SC c;
SI n;
{
	SC cc;

	if( n )      cc = c & 0xf;
	else         cc = c >> 4;

	if( cc < 10 ) cc+='0';
	else          cc+='A'-10;

	return cc;
}

