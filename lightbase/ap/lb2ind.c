#include  "lb2all.h"

#include <ctype.h>
#include <string.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */


#include  "lb2bases.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "lb2trans.h"



#define   MAX_SEQ   500
#define   MAX_FRA   1000

#define  READ_BUFFER  2000


#define   MAX_VET    1000
#define   DEC_VET    (MAX_VET / 10)


static COUNT uso_ind;    /* qtd   de elementos usados no vetor */
static COUNT tot_ind;    /* total de ponteiros no vetor */
static TEXT **vet_ind;   /* endereco do vetor de ponteiros */
static TEXT  *vet_pal;   /* endereco do vetor de palavras */


static  COUNT     sele_glb;
static  COUNT     inde_glb;
static  COUNT     op_glb;

static  COUNT     idx_fonetica;


#define   TML  255            /*** Tamanho Maximo da LINHA   ***/


#define   H_ADDKEY    "addkeyer"  /* Erros na indexacao. Reindexe */

#define   E_ADDKEY    loc_str(STR_GLB, 549)

#ifdef MSDOS
   SS    file_palavra( US, UL, UL, S_INDEX *, S_DELIMIT *, SS ( * )( ) );
   COUNT make_vet( VOID );
   COUNT f_push( S_INDEX *);
   COUNT flush( VOID );
   SI    bq_testa(TEXT **, TEXT **);
   SI    bqsp_comp(TEXT **, TEXT **);
   TEXT  *fonema(TEXT*);
   COUNT poe_file(Campo *, LONG);
#else
   SS    file_palavra( );
   COUNT make_vet( );
   COUNT f_push( );
   COUNT flush( );
   SI    bq_testa( );
   SI    bqsp_comp( );
   TEXT  *fonema( );
   COUNT poe_file( );
#endif


/*
**#  ---------------------------------------------------------------------
**#   Funcao...: reg_index()
**#   Objetivo.: Indexar um registro
**#
**/
F__GLB   COUNT reg_index(sele, operacao, flag, num_rec, acumula_sort)
COUNT sele, operacao, flag;
LONG num_rec;
COUNT acumula_sort;
{  COUNT num_cps, tam, j, ret, vf_modflag;
   ARG *adoc;
   Campo **vet_cmp, *pc;
   UTEXT *pe, salva;
   LONG tam_ct;
   TEXT *pi;
   POINTER pos;
   S_INDEX si;

   switch (operacao ) {
      case INCLUSAO : flag = '+';
                      vf_modflag = 0;
                      break;

      case ALTERACAO:
                      vf_modflag = 1;
                      break;

      case EXCLUSAO : flag = '-';
                      vf_modflag = 0;
                      break;
   }

   if ( make_vet() < 0 ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -2;
      goto fim;
   }

   vet_cmp  = pos_vetcmp(sele);
   num_cps  = qtd_vetcmp(sele);

   sele_glb = sele;
   op_glb   = flag;
   inde_glb = TREE_PALAVRAS;

   for ( j = 0; j < num_cps; j++ ) {

      pc = vet_cmp[j];
      idx_fonetica = 0;

      if ( vf_modflag && pc->cp_mod_flag == 0 ) continue;

      if (flag == '+' ) {
         pi  = pc->cp_newdata;
         tam = pc->cp_newtam;
      }
      else {
         pi  = pc->cp_olddata;
         tam = pc->cp_oldtam;
      }

      /* .... verifica se indexa em branco ..... */
      if ( pc->cp_branco == 'S' ) {
         if ( cp_vazio(pc, (flag == '+') ? 'N' : 'O') == 0 ) {
            inde_glb = TREE_PALAVRAS;
            prep_indice( &si, num_rec, pc->cp_num, 0, 0, 0);
            si.palavra[0]= 1;
            si.palavra[1]= 1;
            si.palavra[2]= pc->cp_num + 1;
            si.palavra[3]= '\0';
            f_push(&si);
         }
         else {
            /* indexacao de campos CHEIOS */
            inde_glb = TREE_PALAVRAS;
            prep_indice( &si, num_rec, pc->cp_num, 0, 0, 0);
            si.palavra[0]= 1;
            si.palavra[1]= 3;
            si.palavra[2]= pc->cp_num + 1;
            si.palavra[3]= '\0';
            f_push(&si);
         }
      }

      if ( pc->cp_indexavel != 'S' ) continue;

#ifdef CP_FLAG
      /* vai ter sua indexacao gerada externamente */
      if ( pc->cp_1futuro == 'F' )  /* Campo Flag */
         continue;
#endif

      if ( pc->cp_tipo == CP_TEXTO) {
         if ( pi  == NULL ) continue;
         if (flag == '+' )  {
            if ( get_postexto('N', pi) == (POINTER) 0L) continue;
         }
         else
            if ( get_postexto('O', pi) == (POINTER) 0L) continue;

      }
      else {
         if ( pi == NULL ) continue;
      }

      pe = (UTEXT *)pi + tam;

      prep_indice( &si, num_rec, pc->cp_num, 0, 0, 0);

      switch(pc->cp_tipo ) {

         case  'D' :
         case  'd' : inde_glb = TREE_DATAS;
                     break;

         case  'H' :
         case  'h' : inde_glb = TREE_HORAS;
                     break;

         case  'F' :
         case  'N' : inde_glb = TREE_NUMEROS;
                     break;

         default   : inde_glb = TREE_PALAVRAS;
                     break;
      }


      if ( inde_glb != TREE_PALAVRAS ) { /*indexa em duas arvores */

         salva = *pe;
         *pe = '\0';

         ret = sep_palavra( &si, (TEXT *) pi, &ap_icomun, f_push);
         *pe = salva;
         if ( ret != 0 )  goto fim;
      }


      if ( pc->cp_fonetica == 'S' )
         idx_fonetica = 1;

      inde_glb = TREE_PALAVRAS;  /* indexa tambem em tree palavras */
      prep_indice( &si, num_rec, pc->cp_num, 0, 0, 0);

      if ( pc->cp_tipo != CP_TEXTO ) {
         salva = *pe;
         *pe = '\0';
         ret = sep_palavra( &si, (TEXT *) pi, &ap_icomun, f_push);
         *pe = salva;
      }
      else {

         adoc = pc->cp_adoc;

         if ( flag == '-' ||  adoc == NULL ) { /* Nao existe documento aberto */

            if ( pi == NULL ) continue;

            pos = get_postexto('O', pi);

            if ( pos == (POINTER) 0L ) continue;

            /* carrega texto para passar para lb_palavra */

            adoc = txt_load(AD_NUM(sele), pos, &tam_ct, pc->cp_mhor, WCB->cb_numrec);

            if ( adoc == NULL ){
               mens_erro(H_LDDOC, E_LDDOC);
               ret = -1;
               goto fim;
            }

            ret = lb_palavra( adoc, 0, &si, &ap_itexto, f_push);
            free_buffer( adoc );
            adoc = NULL;
         }
         else { /*************************************  Texto ja aberto ***/
            ret = lb_palavra( adoc, 0, &si, &ap_itexto, f_push);
         }
      }

      if ( ret != 0 )  goto fim;
   };

   ret = 0;

fim :

   idx_fonetica = 0;

   if ( ! acumula_sort )  {
      ret = fim_index();
   }

   return( ret );

}




/*
**#  ---------------------------------------------------------------------
**#   Funcao...: touch_unica
**#   Objetivo.: verificar chave unica
**#
**/
F__GLB   COUNT touch_unica(pc, reg, str, flag)
Campo *pc;
LONG *reg;
TEXT *str;
COUNT flag;   /* < retorna numero do registro   ||  */
{  COUNT ret, tam_chave, tam_tot;
   UTEXT chave[LB_KEYCT+1];

   ret = 0;  /* a principio nao existe chave */

   if ( pc->cp_chave_unica != 'S' ) {
      goto fim;
   }


   strncpy((TEXT *)chave, str, LB_KEYCT - 6);
   chave[LB_KEYCT - 5 - 1] = '\0';
   prep_palavra((TEXT *)chave);
   tam_chave = strlen((TEXT *)chave) + 1;
   chave[tam_chave] = pc->cp_num;
   memcpy((TEXT *)&chave[tam_chave+1], reg, sizeof(POINTER));
/* tam_tot   = tam_chave + 5; ==> estava assim */
  tam_tot   = tam_chave + 1 + sizeof(POINTER); /* Ananias 11/03/97 */

   if ( EQLKEY(AI_NUM(BASE_EDIT) + TREE_UNIQ, (TEXT *)chave, tam_chave + 1) != 0L ) {

      if ( flag == '<' ) { /* so retorna numero do registro que possui a chave */
         memcpy((TEXT *) reg, (TEXT *)&ct_indkey[tam_chave+1], sizeof(POINTER));
         ret = 1;
      }

      else if ( flag == '-' ) {
         if ( DELKEY(AI_NUM(BASE_EDIT) + TREE_UNIQ, (TEXT *) chave, tam_tot) != 0 ) {
            ret = -2;  /* erro de exclusao da chave */
         }
         else ret = 1;
      }

      else if ( memcmp((TEXT *)&ct_indkey[tam_chave+1], reg,
                                   sizeof(POINTER)) != 0 ) {
            ret = -1;  /* nao e' o mesmo registro */
      }
      else {
         ret = 1;   /* registro existe */
      }
   }
   else {
      if ( flag == '<' ) {
         *reg = 0L;
         ret = 1;
      }
      else if ( flag == '+' ) {
         if ( ADDKEY(AI_NUM(BASE_EDIT) + TREE_UNIQ, (TEXT *) chave,
                         (POINTER) 0 , REGADD, tam_tot)  !=  0 ) {
            ret = -2;  /* erro de inclusao da chave */
         }
      }
   }

fim :

   return(ret);
}



/*
**#  ---------------------------------------------------------------------
**#   Funcao...: salva_unica
**#   Objetivo.: salvar chave unica
**#
**/
F__GLB   TEXT *salva_unica(sele, operacao, flag)
COUNT sele, operacao, flag;
{  COUNT num_cps, j, ret, vf_modflag, tam;
   COUNT historia[MAX_CAMPOS];
   TEXT chave[LB_KEYCT+1];
   Campo **vet_cmp, *pc;
   TEXT *pi;
   static TEXT  nome_cp_unica[TE+1];  /* nome do campo invalido em chave unica */

   n_cp_unica = -1;  /* indice de campo com chave unica duplicada */


   switch (operacao ) {
      case INCLUSAO : flag = '+';
                      vf_modflag = 0;
                      break;

      case ALTERACAO:
                      vf_modflag = 1;
                      break;

      case EXCLUSAO : flag = '-';
                      vf_modflag = 0;
                      break;
   }

   vet_cmp  = pos_vetcmp(sele);
   num_cps  = qtd_vetcmp(sele);

   for ( j = 0; j < num_cps; j++ )
      historia[j] = 0;



   for ( j = 0; j < num_cps; j++ ) {

      pc = vet_cmp[j];

      if ( pc->cp_chave_unica != 'S' ) continue;

      if ( vf_modflag && pc->cp_mod_flag == 0 ) continue;

      if ( cp_vazio(pc, (flag == '+') ? 'N' : 'O') == 0 ) continue;

      /* Garantia de que dado mudou.  pois em algumas situacoes estou */
      /* marcando como campo modificado mais o mesmo se mantem inalterado */
      /* Erro quando 1o elemento era chave unica e F2 sem alterar nada. */
      /* Min.Justica. Out/92 */
      if ( operacao == ALTERACAO &&
           pc->cp_newtam == pc->cp_oldtam &&
           strncmp(pc->cp_newdata, pc->cp_olddata , pc->cp_newtam) == 0 )
          continue;


      if (flag == '+' ) { pi  = pc->cp_newdata; tam = pc->cp_newtam; }
      else              { pi  = pc->cp_olddata; tam = pc->cp_oldtam; }



      strncpy(chave, pi, tam);
      chave[tam]='\0';

      if ( touch_unica(pc, (LONG *) &(c_bases[sele]->cb_numrec), chave, flag) < 0 ) {
         strcpy(nome_cp_unica, pc->cp_nome);
         n_cp_unica = j;
         ret = -1;
         goto fim;
      }

      historia[j]=1;

   };

   ret = 0;

fim :

   if ( ret < 0 && flag == '+' ) { /* se nao inclui todos, libera os incluidos */
      for ( j = 0; j < num_cps; j++ ) {
         if ( historia[j] != 0 ) {
            if (flag == '+' ) { pi  = pc->cp_newdata; tam = pc->cp_newtam; }
            else              { pi  = pc->cp_olddata; tam = pc->cp_oldtam; }

            strncpy(chave, pi, tam);
            chave[tam]='\0';

            touch_unica(pc, (LONG *) &(c_bases[sele]->cb_numrec), chave, '-');
         }
      }
   }

   if ( ret == 0 ) return( NULL );
   else            return(nome_cp_unica);
}




F__GLB   US to_bits( I, bits )
S_INDEX   *I;
UC        *bits;
{
   US   tipo;

   if (      I->sequencia < 16  &&      /* 4  BITS */
             I->frase     < 2   &&      /* 1  BITS */
             I->paragrafo < 2   &&      /* 1  BITS */
             I->campo     < 16  &&      /* 4  BITS */
             I->registro  < 4096L  )    /* 12 BITS */
   {  /***   MINIMO   ***/
      tipo = 0;
      bits[ 0 ] = ((tipo & 3) << 6) +
                  ((I->sequencia & 0x0F) << 2) +
                  ((I->frase & 1) << 1) +
                  ((I->paragrafo & 1));
      bits[ 1 ] = ((I->campo & 0x0F) << 4) +
                  ((I->registro & 0xF00) >> 8);
      bits[ 2 ] = ((I->registro & 0xFF));

      return ( 3 );
   }

   else if ( I->sequencia < 16  &&      /* 4  BITS */
             I->frase     < 16  &&      /* 4  BITS */
             I->paragrafo < 8   &&      /* 3  BITS */
             I->campo     < 32  &&      /* 5  BITS */
             I->registro  < 16384L )    /* 14 BITS */
   {  /***   MEDIO   ***/
      tipo = 1;
      bits[ 0 ] = ((tipo & 3) << 6) +
                  ((I->sequencia & 0x0F) << 2) +
                  ((I->frase & 0x0C) >> 2);
      bits[ 1 ] = ((I->frase & 0x03) << 6) +
                  ((I->paragrafo & 0x7) << 3) +
                  ((I->campo & 0x1C) >> 2);
      bits[ 2 ] = ((I->campo & 0x03) << 6) +
                  ((I->registro & 0x3F00) >> 8);
      bits[ 3 ] = ((I->registro & 0xFF));

      return ( 4 );
   }

   else if ( I->sequencia < 32  &&      /* 5  BITS */
             I->frase     < 64  &&      /* 6  BITS */
             I->paragrafo < 64  &&      /* 6  BITS */
             I->campo     < 32  &&      /* 5  BITS */
             I->registro  < 65536L )    /* 16 BITS */
   {  /***   MAXIMO   ***/
      tipo = 2;
      bits[ 0 ] = ((tipo & 3) << 6) +
                  ((I->sequencia & 0x1F) << 1) +
                  ((I->frase & 0x20) >> 5);
      bits[ 1 ] = ((I->frase & 0x1F) << 3) +
                  ((I->paragrafo & 0x38) >> 3);
      bits[ 2 ] = ((I->paragrafo & 0x07) << 5) +
                  ((I->campo & 0x1F));
      bits[ 3 ] = ((I->registro & 0xFF00) >> 8);
      bits[ 4 ] = ((I->registro & 0xFF));

      return ( 5 );
   }
#ifdef ERA_ASSIM
...   else if ( I->sequencia < 512  &&      /* 9  BITS */
...             I->frase     < 1024 &&      /* 10 BITS */
...             I->paragrafo < 2048 &&      /* 11 BITS */
...             I->campo     < 256  &&      /* 8  BITS */
...             I->registro  < 16777216L )  /* 24 BITS */
...   {  /***   ABSURDO   ***/
...      tipo = 3;
...      bits[ 0 ] = ((tipo & 3) << 6) +
...                  ((I->sequencia & 0x1F8) >> 3);
...      bits[ 1 ] = ((I->sequencia & 0x07) << 5) +
...                  ((I->frase & 0x3E0) >> 5);
...      bits[ 2 ] = ((I->frase & 0x1F) << 3) +
...                  ((I->paragrafo & 0x700) >> 8);
...      bits[ 3 ] = ((I->paragrafo & 0xFF));
...      bits[ 4 ] = ((I->campo & 0xFF));
...      bits[ 5 ] = ((I->registro & 0xFF0000L) >> 16);
...      bits[ 6 ] = ((I->registro & 0xFF00) >> 8);
...      bits[ 7 ] = ((I->registro & 0xFF));
...
...      return ( 8 );
...   }
#endif

   else if ( I->sequencia < 1024  &&      /* 10  BITS */
             I->frase     < 2048  &&      /* 11 BITS */
             I->paragrafo < 16384 &&      /* 14 BITS */
             I->campo     < 256   &&      /* 8  BITS */
             I->registro  < 134217728L )  /* 27 BITS */
   {  /***   ABSURDO   ***/
      tipo = 3;
      bits[ 0 ] = (tipo << 6) +                   /* tt...... */
                  (I->sequencia >> 4);            /* pegou 1111110000 de seq ttssssss */
      bits[ 1 ] = ((I->sequencia & 0x0f) << 4) +  /* ssss.... */
                  (I->frase >> 7);                /* pegou 111 1000 0000 de frase  ssssffff*/
      bits[ 2 ] = ((I->frase & 0x7F) << 1) +      /* fffffff. */
                  (I->paragrafo >> 13);           /* pegou 10 0000 0000 0000 de paragrafo  fffffffp */
      bits[ 3 ] = (I->paragrafo & 0x1FE0) >> 5;   /* pppppppp */
      bits[ 4 ] = ((I->paragrafo & 0x1F) << 3) +  /* ppppp... */
                  (I->campo >> 5);                /* pegou 0001 1111 de campo pppccccc */
      bits[ 5 ] = ((I->campo & 0x01F) << 3) +     /* ccccc... */
                  (I->registro >> 24);            /* pegou 111000000000000000000000000 registro  cccccrrr */
      bits[ 6 ] = ((I->registro & 0xFF0000L) >> 16);
      bits[ 7 ] = ((I->registro & 0xFF00) >> 8);
      bits[ 8 ] = ((I->registro & 0xFF));

      return ( 9 );
   }

   else {
      debug("WWW %l %d %d %d %d",
         I->registro, I->campo, I->paragrafo, I->frase, I->sequencia);
      return( 0 );
   }
}



F__GLB   COUNT fim_index()
{  COUNT ret;

   ret = 0;

   if ( vet_ind != NULL ) {
      ret = flush( );
      free((TEXT *)vet_ind);
   }

   uso_ind = 0;    /* qtd   de elementos usados no vetor */
   tot_ind = 0;    /* total de ponteiros no vetor */
   vet_ind = NULL;   /* endereco do vetor de ponteiros */
   vet_pal = NULL;   /* endereco do vetor de palavras  */

   return(ret);
}


/* WWW */
F__LOC   SS file_palavra( arq, inicio, tam, I, D, f_exe )
US         arq;
UL         inicio;
UL         tam;
S_INDEX    *I;
S_DELIMIT  *D;
SS         ( *f_exe )( );
{
   SS    ret = 0, lido, lido_ant;
   SC    *string;
   UL    quant = 0;

   lido = MIN( READ_BUFFER, tam );

   string = malloc( (UCOUNT)lido + 1 );

   if ( string == NULL )   return( -1 );

   while ( quant  <  tam  )
   {
      lseek( arq, inicio + quant, 0 );

      lido_ant = lido = read( arq, string, lido );

      if ( lido  <   0 )  {  ret = -1;   goto  fim;  }

      if ( lido  ==  0 )  {  goto  fim;  }

      /*** Enquanto for diferente de ESPACOS - RECUE ***/

      while ( --lido  >  0        &&
              string[ lido ] != 0   &&
              string[ lido ] != ' '   &&
              string[ lido ] != '\n' )  continue;

      if ( lido > 0 )  {

         string[ ++lido ] = '\0';

         ret = sep_palavra( I, string, D, f_exe );
      }
      else {
         lido = lido_ant;
      }

      quant += lido;

      lido = MIN( READ_BUFFER, tam - quant );
   }

fim:

   free( string );

   return ( ret );
}







F__LOC   COUNT make_vet()
{  COUNT n, i, ret;
   UCOUNT tam;
   TEXT *p, *new;

   if ( vet_ind != NULL ) {
      ret = 0;
      goto fim;
   }

   uso_ind = 0;    /* qtd   de elementos usados no vetor */
   tot_ind = 0;    /* total de ponteiros no vetor */
   vet_ind = NULL;   /* endereco do vetor de ponteiros */
   vet_pal = NULL;   /* endereco do vetor de palavras  */



   n = MAX_VET;

   while ( n > 1 ) {

      /* LB_MAX + 1 pois no final da palavra armazenaremos o tamanho */

      tam = ( (sizeof(TEXT *) + LB_KEYMAX + 1 ) * n) + sizeof(TEXT *);

      new = calloc( tam, sizeof(TEXT));

      if ( new != NULL ) break;

      else {
         n -= DEC_VET;
      }
   }

   if ( new != NULL ) {
      uso_ind = 0;
      tot_ind = n;
      vet_ind = (TEXT **)new; /* nao mudar sem mudar o free equivalente */
      vet_pal = new + ( n * sizeof( TEXT *)) + sizeof(TEXT *);
      for ( p = vet_pal, i = 0 ; i < n ; i++) {
         vet_ind[i] = p;
         p += (LB_KEYMAX + 1);
      }

      ret = 0;

   }
   else {
      ret = -1;
   }

fim :

   return(ret);

}



F__LOC   COUNT f_push( si)
S_INDEX *si;
{  FAST TEXT *p, i;
   COUNT tam, ret;
   DOUBLE dub;



   ret = 0;
   p   = vet_ind[uso_ind];

   switch ( inde_glb ) {
      case TREE_PALAVRAS :
            strcpy( p,  si->palavra);
            break;

      case TREE_NUMEROS :
            dub = str_to_db( si->palavra, sep_dec);
            db_to_str(dub, si->palavra, TAM_DECIMAIS, 0);
            memset(p, '0', TAM_CPNUM);
            for ( i= 0; si->palavra[i] != '.' && si->palavra[i] != '\0'; i++)
               ;
            strcpy( p + (TAM_CPNUM - i),  si->palavra);

            break;

      case TREE_DATAS   :
            strcpy(p, conv_data(si->palavra));
            break;

      case TREE_HORAS   :
            strcpy(p, conv_hora(si->palavra));
            break;

      default           :
            strcpy( p,  si->palavra);
            break;
   }



   tam = strlen(p) + 1;
   tam += to_bits( si, (UTEXT *) p + tam);

   vet_ind[uso_ind][LB_KEYMAX] = ((inde_glb & 0x7) << 5) +  tam;

   uso_ind++;
   if ( uso_ind >= tot_ind ) ret = flush();

   if ( idx_fonetica ) {
      p = vet_ind[uso_ind];
      /* o token 0x2 0x2 vai indicar que o que tem indexado em TREE_HORAS e'fonetico */
      *p     = 2;    /* Indicador de palavra FONETICA */
      *(p+1) = 2;

      strcpy( p+2,  fonema(si->palavra));

      tam = strlen(p) + 1;  /* ANANIAS @ este 1 deve sumir */

      if ( tam > 3) {
         tam += to_bits( si, (UTEXT *) p + tam);
         vet_ind[uso_ind][LB_KEYMAX] = ((TREE_HORAS & 0x7) << 5) +  tam;
         uso_ind++;
         if ( uso_ind >= tot_ind ) ret = flush();
      }
   }

   return(ret);
}



F__LOC   COUNT flush()
{  COUNT ret, tam, ind, i, tipo_add;
   TEXT mens_w[80];
   COUNT ret_trans = 0, erros;
   TEXT *pv;
   TEXT p;
   WIN *jm;

   tipo_add = REGADD;
   erros = 0;


   if ( (pv = getenv("LBCIND")) != NULL )
      tipo_add = atoi(pv);

   sprintf( mens_w, "%s (%ld)            ",
             M_AGUARDE, c_bases[sele_glb]->cb_numrec);

   jm = abre_mensagem(mens_w);

   qsort( (TEXT *) vet_ind, uso_ind, sizeof( TEXT * ), (SI (* )()) bq_testa );

   ret = 0;

   if ( (ret_trans = BEG_TRANS(TR_FLUSHIND, 0, 0)) < 0 ) {
      ret = -1;
      goto fim;
   }

   for ( i = 0 ;  i < uso_ind  && ret == 0 ; i++ ){

      p = vet_ind[i][LB_KEYMAX];

      tam = p & 0x1F;
      ind = ( p & 0xE0 ) >> 5;

      if ( op_glb == '+' )
         ret = ADDKEY(AI_NUM(BASE_EDIT) + ind, vet_ind[i],
                         (POINTER) 0 , tipo_add, tam);
      else
         ret = DELKEY(AI_NUM(BASE_EDIT) + ind, vet_ind[i], tam);

      if (ret != 0)
         erros++;

   }

   if ( ret_trans >= 0 )
      END_TRANS(TR_FLUSHIND);

   if (ret != 0) {
      if (op_glb == '+' ) {
         if ( uerr_cod == WRITE_ERR ) {
            fim_lb(E_NOSPACE);
         }
         else

            mens_erro(H_ADDKEY, E_ADDKEY, erros);

            /* debug("Erro em ADDKEY erro : %d (palavra : [%s])\n i=%d uso_ind=%d   tam=%d  ind=%d",
               ret, vet_ind[i], i, uso_ind, tam, ind);
            */
      }
      ret = 0;
   }

   uso_ind = 0;

fim:


	fecha_mensagem(jm);
   return(ret);
}




F__LOC   SI bq_testa(p1, p2)
PFAST TEXT **p1,**p2;
{
   return(memcmp(*p1, *p2, LB_KEYMAX));
}

