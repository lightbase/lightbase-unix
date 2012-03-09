#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2prot1.h"

#include  "lb2maces.h"

#define   T80     80

#ifdef MSDOS
   VOID anl_arg(TEXT *, COUNT *, COUNT *, COUNT *, TEXT *, COUNT *, COUNT *);
   TEXT *find_table(COUNT, COUNT, COUNT, TEXT *, COUNT);
   TEXT *ind_tables(COUNT, COUNT, TEXT *, COUNT);
   COUNT av_expr(TEXT *, Campo *);
#else
   VOID anl_arg( );
   TEXT *find_table( );
   TEXT *ind_tables( );
   COUNT av_expr( );
#endif



F__GLB   COUNT trata_indireto( str, pc, reprocessando )
TEXT *str;
Campo *pc;
COUNT reprocessando;
{  COUNT ret, param, ind, num, val_inter, sele;
   COUNT piece_find, piece_ret, num_table, tab_numerica;
   TEXT *p, *masc;
   TEXT str_proc[TD+1];


   sele = WCB->cb_fordem - 1;
   ret = 0;
   p = NULL;

   if ( str == NULL )
      goto fim;

   if ( str[0] == '@' )
      str++;

   switch ( *str ) {
      case '#' :
            anl_arg(str, &num_table, &piece_find, &piece_ret, str_proc, &val_inter, &tab_numerica);

            if ( piece_find == 0 ) {
               p = str_proc;
            }
            else  {
               if ( tab_numerica == 0 ) 
                  p = find_table(num_table, piece_find, piece_ret, str_proc, val_inter);
               else
                  p = ind_tables(tab_numerica, piece_ret, str_proc, val_inter);
            }
            break;

      case '!' :
            av_expr(++str, pc);
            p = pc->cp_newdata;
            break;

      case 'U' :
            if ( reprocessando == 1 ) 
               goto fim;

            p = ap_nome;
            break;

      case 'L' :
            if ( reprocessando == 1 ) 
               goto fim;

            p = ap_local;
            break;

      case 'u' :
            if ( reprocessando == 1 ) 
               goto fim;
            if ( pc->cp_newdata[0] != '\0' )
               goto fim;
            else
               p = ap_nome;
            break;

      case 'l' :
            if ( reprocessando == 1 ) 
               goto fim;
            if ( pc->cp_newdata[0] != '\0' ) 
               goto fim;
            else
               p = ap_local;
            break;

      default  :
            p = NULL;
            break;
   }

   if ( p != NULL ) {

      if ( p[0] == '@' ) {
         ind = ver_campo(sele, p+1, &num);
         if ( ind >= 0 && c_bases[sele]->cb_numrec != 0L )
            p =  pos_vetcmp(sele)[ind]->cp_newdata;
      }

      if ( pc->cp_tipo == 'N' ) param = E_VALOR;
      else                      param = E_ALFA;
      if ( pc->cp_tipo == 'F' )
         masc = pc->cp_param.cp_msc_form;
      else
         masc = pc->cp_mascara;
      formata_campo(pc->cp_newdata, p, masc, param);
      ret = 1;
   }
   else {
      pc->cp_newdata[0] = '\0';
      ret = 1;
   }

fim :

   return(ret);
}





F__GLB   COUNT tab_number(str, nome)
TEXT *str;
TEXT *nome;
{  COUNT ret, i;
   TEXT aux[TB+1];

   ret = -1;

   if ( str[0] == '#' )
      str++;

   for ( i=0; i < TB && str[i] != '\0' && str[i] != '['; i++ )
      aux[i] = str[i];

   aux[i] = '\0';

   prep_palavra(aux);

   if ( nome != NULL )
      strcpy(nome, aux);
   
   if ( aux[0] != '\0' ) {
      for ( i = 0; i < ap_maxmatriz; i++ ) {
         if ( strcmp(aux, ap_vetmatriz[i].tb_nome) == 0 ) {
            ret = i;
            break;
         }
      }
   }

   return(ret);
}


F__GLB   VOID trata_formula( sele )
COUNT sele;
{  COUNT i, num_cps;
   TEXT result[81];
   Campo **vet_cmp, *pc;
   DOUBLE dub;

   num_cps = qtd_vetcmp(sele);
   vet_cmp = pos_vetcmp(sele);

   for ( i = 0 ; i < num_cps; i++ ) {

      pc = vet_cmp[i];

      if ( pc->cp_tipo != 'F' ) continue;

      if ( calc_avalia(pc->cp_mascara, &dub, NULL, calc_token, 'N') >= 0 ) {

         db_to_str(dub, result, pc->cp_decimais, 1);

         if ( strcmp(pc->cp_newdata, result) != 0 ) {
            /* estava invadindo quando um campo destino era menor que o resultado */
            /* em 1.8 rel 4.0 em 12/05/94 */
            COUNT t;
            TEXT aux[60];
            t = strlen(result);
            if ( t > pc->cp_mhor ) {
               strcpy(pc->cp_newdata, result + (t - pc->cp_mhor));
               sprintf(aux, M_OVFFOR, c_bases[sele]->cb_numrec, pc->cp_nome, result);
               if ( fpo_glb != NULL )
                  wr_log(1, aux);
               else
                  debug(aux);
            }
            else {
               strcpy(pc->cp_newdata, result);
            }

            pc->cp_newtam = strlen(pc->cp_newdata);
            pc->cp_mod_flag = 1;
            pc->cp_crt_flag = 1;
         }

      }
      else {
         /* valor indefinido. Exemplo : divisao por zero */
            pc->cp_newdata[0] = '\0';
            pc->cp_newtam = 0;
            pc->cp_mod_flag = 1;
            pc->cp_crt_flag = 1;
      }
   }
}



F__GLB   COUNT calc_token( str, val)
TEXT *str;
DOUBLE *val;
{  COUNT ret, ind, num, sele, val_inter;
   COUNT piece_find, piece_ret, num_table, tab_numerica;
   TEXT *p;
   TEXT str_proc[TD+1];

   sele = BASE_EDIT;
   ret = -1;
   *val = 0;

   if ( str != NULL && str[0] != '\0' ) {
      if ( str[0] != '#' ) {
         ind = ver_campo(sele, str, &num);
         if ( ind >= 0 ) {
            ret = 0;
            if ( WCB != NULL && c_bases[sele]->cb_numrec != 0L ) {
               p =  pos_vetcmp(sele)[ind]->cp_newdata;
               if ( p != NULL ) {
                  *val = str_to_db(p, sep_dec);
                  ret = ind;
               }
            }
         }
      }
      else {
         p = str;      
         anl_arg(str, &num_table, &piece_find, &piece_ret, str_proc, &val_inter, &tab_numerica);
         if ( tab_numerica == 0 ) 
            p = find_table(num_table, piece_find, piece_ret, str_proc, val_inter);
         else
            p = ind_tables(tab_numerica, piece_ret, str_proc, val_inter);
         if ( p != NULL ) {
            if ( p[0] == '@' ) {
               ind = ver_campo(BASE_EDIT, p+1, &num);
               if ( ind >= 0 &&  WCB != NULL  &&  WCB->cb_numrec != 0L )
                  p = pos_vetcmp(BASE_EDIT)[ind]->cp_newdata;
            }
            *val = str_to_db(p, sep_dec);
         }
         ret = 0;
      }
   }

   return(ret);
}


F__LOC   TEXT *find_table(num_table, piece_find, piece_ret, str_proc, val_inter)
COUNT num_table, piece_find, piece_ret;
TEXT *str_proc;
COUNT val_inter;
{  COUNT i;
   UTEXT *pl, *p_str, *p_ret = NULL;
   TEXT aux[TD+1], proc[TD+1];
   static TEXT achou[TD+1];

   if ( num_table < 0 || ap_vetmatriz[num_table].tb_inicio == NULL ) {
      p_ret = NULL;
      goto fim;
   }

   strncpy(proc, str_proc, TD);  /* Normaliza string a ser localizada */
   proc[TD-1] = '\0';
   /*  prep_palavra(proc);  ANANIAS @@@ vai diferenciar acentos */

   achou[0] = '\0';

   pl = (UTEXT *)ap_vetmatriz[num_table].tb_inicio;

   for ( i = 0; i < ap_vetmatriz[num_table].tb_qtd; i++ ) {
      p_str = pl + 1;
      pl += *pl;
      piece((TEXT *)p_str, aux, ap_delitables, piece_find, TD);
      trim(aux);
      ltrim(aux);

      if ( val_inter ) { /* pesquisa aceita por intervalo */
         DOUBLE procura, min, max;
         TEXT strval[TE+1];

         procura = str_to_db( proc, sep_dec);

         piece(aux, strval, '-', 1, TE); strval[TE] = '\0';
         min = str_to_db( strval, sep_dec);

         piece(aux, strval, '-', 2, TE); strval[TE] = '\0';
         max = str_to_db( strval, sep_dec);

         if ( procura >= min && procura <= max ) {
            piece((TEXT *)p_str, achou, ap_delitables, piece_ret, TD);
            trim(achou);
            ltrim(achou);
            p_ret = (UTEXT *)achou;
            break;
         }
      }
      else {
         if ( strncmp(proc, aux, TD) == 0 ) {
            piece((TEXT *)p_str, achou, ap_delitables, piece_ret, TD);
            trim(achou);
            ltrim(achou);
            p_ret = (UTEXT *)achou;
            break;
         }

      }
   }

fim :

   return((TEXT *)p_ret);
}



F__LOC   TEXT *ind_tables(tab_numerica, piece_ret, str_proc, val_inter)
COUNT tab_numerica, piece_ret;
TEXT *str_proc;
COUNT val_inter;
{  COUNT ret;
   TEXT *p, *p_ret;
   POINTER pos;
   TEXT aux[250], proc[TD+1], chave[TD+1];
   static TEXT achou[TD+1];


   p_ret = NULL;
   achou[0] = '\0';

   strncpy(proc, str_proc, TD);
   proc[TD] = '\0';


   sprintf(chave, "%02.2d%s", tab_numerica, proc);

   if ( val_inter ) {
	   ret = GTEKEY(tab_inum, chave, aux, strlen(chave) + 1);
      if ( ret > 0 && strncmp(chave, aux, 2) == 0 ) 
         strcpy(chave, aux);
      else
         ret = 0;
   }
   else
	   ret = EQLKEY(tab_inum, chave, strlen(chave) + 1);

   if ( ret > 0 ) {
      p = strchr(ct_indkey, '\0');
      pos = atol(p+1);

	   if ( RDVREC(tab_dnum, pos, aux, sizeof(aux)) == NO_ERROR ) {
         if ( piece(aux, achou, ap_delitables, piece_ret, TD) > 0 ) {
            achou[TD+1] = '\0';
            p_ret = achou;
            trim(achou);
            while (*p_ret && (*p_ret == ' ' && *p_ret == '\t'))
               p_ret++;
         }
      }
   }

fim :

   return((TEXT *)p_ret);
}


F__LOC   VOID anl_arg(str, tabela, col_psq, col_ret, str_proc, val_inter, tab_numerica)
TEXT *str;
COUNT *tabela;
COUNT *col_psq, *col_ret;
TEXT *str_proc;
COUNT *val_inter;
COUNT *tab_numerica;
{  TEXT *p, *pfinal, c_final=0, direto;
   TEXT aux[TD];
   TEXT str_aux[81];
   COUNT num, i, ind, sele;

   sele = WCB->cb_fordem - 1;
   direto = 0;
   *val_inter = 0;

   pfinal = strchr(str, ']');

   if ( pfinal != NULL ) {  /* mascara fim dos parametros */
      c_final = *pfinal;
      *pfinal = '\0';
   }

   p = strchr(str, '#');

   if ( p != NULL ) {
      p++;
      if ( (*tab_numerica = atoi(p)) == 0 ) {
         if ( (*tabela = tab_number(str, NULL)) < 0 ) {
            direto = 1;
         }
      }
   }
   else
      *tab_numerica = 0;



   while (*str++ != '[')
      ;

   for ( i = 1; i <= 4; i++ ) {

      /* faz p apontar para a string do parametro i */

      piece(str, aux, ',', i, TD);


      /* descobre o que procurar */
      if ( aux[0] == '"' ) {
         if ( (p = strchr(&aux[1], '"')) != NULL )
            *p = '\0';
         p = &aux[1];
      }
      else if ( aux[0] == '@' ) {
         p = NULL;
         ind = ver_campo(sele, &aux[1], &num);
         if ( ind >= 0 ) {
            if ( WCB != NULL  &&  c_bases[sele]->cb_numrec != 0L ) {
               p =  pos_vetcmp(sele)[ind]->cp_newdata;
            }
         }
      }
      else {  /* usa a propria string */
         p = aux;
      }

      switch(i) {
         case 1 : /* valor a ser procurado */
            if ( p == NULL ) str_aux[0] = '\0';
            else {
               strncpy(str_aux, p, 80);
               str_aux[80] = '\0';
            }
            break;

         case 2 : /* piece onde deve ser procurada */
            if ( *tab_numerica > 0 ) {
               /* independentemente do soliitado deve procurar na coluna 1 */
               *col_psq = 1;
            }
            else {
               if ( p == NULL ) *col_psq = 1;
               else             *col_psq = atoi(p);
            }

            break;

         case 3 : /* piece que deve ser retornado */
            if ( p == NULL ) *col_ret = 1;
            else             *col_ret = atoi(p);
            if ( *tab_numerica > 0 ) {
               if ( *col_ret > 1 )
                  *col_ret = *col_ret -1;
            }
            break;

         case 4 : /* se pesquisa por intervalo */
            if ( p == NULL || toupper(*p) != 'I') *val_inter = 0;
            else                                  *val_inter = 1;
            break;
      }
   }

   if ( direto ) {  /* retorna o piece que solicitou do proprio str*/
      *col_psq = 0;
      piece(str_aux, str_proc, ap_delitables, *col_ret, TD);
      str_proc[TD-1] = '\0';
   }
   else {
      strncpy(str_proc, str_aux, TD);
      str_proc[TD-1] = '\0';
      /* prep_palavra(str_proc);   @@@ ANANIAS nao vai diferenciar acentos */
   }

fim :

   if ( pfinal != NULL )
      *pfinal = c_final;
}

COUNT av_expr(str, pc)
TEXT *str;
Campo *pc;
{  COUNT i, num, tam_max, tam, tam_in, ind, sele, tam_deseja=0;
   TEXT *p, *p1, aux[81];

   tam_max = strlen(pc->cp_mascara);
   tam_in  = 0;
   sele = WCB->cb_fordem - 1;

   for ( i = 1; i <= 20; i++ ) {

      /* faz p apontar para a string do parametro i */

      piece(str, aux, '+', i, TD);

      if ( aux[0] == '"' ) {
         if ( (p = strchr(&aux[1], '"')) != NULL )
            *p = '\0';
         p = &aux[1];
      }
      else {
         p = aux;

         /* permite pegar parte de um campo com ,xx */
         if ( (p1 = strchr(aux, ',')) != NULL ) {
            *p1++ = '\0';
            tam_deseja = atoi(p1);
         }

         ind = ver_campo(sele, aux, &num);
         if ( ind >= 0 ) {
            if ( WCB != NULL  &&  c_bases[sele]->cb_numrec != 0L ) {
               p =  pos_vetcmp(sele)[ind]->cp_newdata;
            }
         }

      }

      tam = strlen(p);

      tam = MIN(tam, (tam_max - tam_in));

      if ( tam_deseja > 0 )
         tam = MIN(tam, tam_deseja);


      strncpy(&pc->cp_newdata[tam_in], p, tam);
      tam_in += tam;

      if (tam_in >= tam_max )
         break;
   }

   pc->cp_newdata[tam_in] = '\0';

   return(0);

}






