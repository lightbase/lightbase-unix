#include  "lb2all.h"

#include  <string.h>
#include  HD_MEMO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"

#include  "lb2prot1.h"


#define   GRP_EXPO 32



#define   LF130E      "1 3 0 E"
#define   STR_6LD     "%6ld"
#define   STR_PONTO   "."
#define   STR_HDLIN   "%8.8ld %-14.14s %s"
#define   SIZE_INBUF   2048
#define   DELIMITA ','
#define   ASPAS    '"'

#define   EXT_EXPORT ".lbx"


#define   H_EXWRT    "E_EX_WRT"  /* erro na gravacao de arquivo de exportacao */
#define   H_EXARQ    "E_EX_ARQ"  /* sem nome de arquivo para exportar */
#define   H_EXRCMP  "C_EX_CMP"   /* solicitacao de nomes de campos */
#define   H_EXRDIR  "C_EX_DIR"   /* solicitacao de diretorio */
#define   H_EXRARQ  "C_EX_ARQ"   /* solicitacao de nome de arquivo */
#define   H_EXRPAD  "EX_PADRAO"  /* solicitacao de padrao de exportacao */
#define   H_EX1DIR  "E_EX_1DIR"  /* nome informado para diretorio ja existe como arquivo */
#define   H_CFRWEX  "CF_RWEXP"
#define   H_EX001   "E_EX_001"  /* erro na carga de campo texto. pode ser memoria */
#define   H_EX002   "E_EX_002"  /* Erro na abertura do arquivo de exportacao de texto  */
#define   H_EX003   "E_EX_003"  /* Erro na gravacao do arquivo de exportacao de texto  */
#define   H_EX004   "E_EX_004"  /* muitos caracteres na linha de campos use '*' */
#define   H_CPHD1   "E_CP_HD1"  /* campo especificado nao existe */
#define   H_CPHD2   "E_CP_HD2"  /* Nenhum campo a exportar */

#define   H_CFEXPCAN "CF_CANEX"  /* canfirma cancelamento por esc */




#define   E_EX_WRT      loc_str(GRP_EXPO, 1 )
#define   E_EX_ARQ      loc_str(GRP_EXPO, 2 )
#define   E_EX_001      loc_str(GRP_EXPO, 3 )
#define   E_EX_002      loc_str(GRP_EXPO, 4 )
#define   E_EX_003      loc_str(GRP_EXPO, 5 )
#define   E_EX_004      loc_str(GRP_EXPO, 6 )
#define   E_EX_1DIR     loc_str(GRP_EXPO, 7 )

#define   C_EX_FOOT     loc_str(GRP_EXPO, 8 )
#define   C_EX_CAB      loc_str(GRP_EXPO, 9 )
#define   C_EX_CMP      loc_str(GRP_EXPO, 10)
#define   C_EX_ARQ      loc_str(GRP_EXPO, 11)
#define   C_EX_TVAL     loc_str(GRP_EXPO, 12)
#define   C_EX_FINAL    loc_str(GRP_EXPO, 13)
#define   C_EX_RCMP     loc_str(GRP_EXPO, 14)
#define   C_EX_RARQ     loc_str(GRP_EXPO, 15)
#define   C_EX_DIR      loc_str(GRP_EXPO, 16)
#define   C_EX_RDIR     loc_str(GRP_EXPO, 17)

#define   C_EX_FMT      loc_str(GRP_EXPO, 18)
#define   C_EX_RPAD     loc_str(GRP_EXPO, 19)
#define   C_EX_OPCPAD   loc_str(GRP_EXPO, 20)

#define   TW_EX_D00     loc_str(GRP_EXPO, 21)
#define   TW_EX_D99     loc_str(GRP_EXPO, 22)
#define   TW_EX_D01     loc_str(GRP_EXPO, 23)
#define   TW_EX_D02     loc_str(GRP_EXPO, 24)

#define   TW_EX_D20     loc_str(GRP_EXPO, 25)
#define   TW_EX_D29     loc_str(GRP_EXPO, 26)
#define   TW_EX_D21     loc_str(GRP_EXPO, 27)
#define   TW_EX_D22     loc_str(GRP_EXPO, 28)
#define   C_EX_ESC      loc_str(GRP_EXPO, 29)


#define   E_CP_HD1      loc_str(GRP_EXPO, 30)
#define   E_CP_HD2      loc_str(GRP_EXPO, 31)

#define   TW_EX_D30     loc_str(GRP_EXPO, 32)
#define   TW_EX_D39     loc_str(GRP_EXPO, 33)
#define   TW_EX_D31     loc_str(GRP_EXPO, 34)
#define   TW_EX_D32     loc_str(GRP_EXPO, 35)



#ifdef MSDOS
   COUNT par_export(WIN *, TEXT *, TEXT *, TEXT *);
   COUNT exec_export(COUNT, TEXT *);
   COUNT exp_db_arq(Campo *);
   COUNT exp_ln_arq(Campo *, TEXT *);
   COUNT vl_arq(d_entry *);
   COUNT vl_expath(d_entry *);
   COUNT vl_campos(d_entry *);
   COUNT anal_campos(COUNT, TEXT *);
#else
   COUNT par_export( );
   COUNT exec_export( );
   COUNT exp_db_arq( );
   COUNT exp_ln_arq( );
   COUNT vl_arq( );
   COUNT vl_expath( );
   COUNT vl_campos( );
   COUNT anal_campos( );
#endif


FILE  *fpo_exp;

static COUNT *vet_ic;
static COUNT sele_loc, padrao_export = 'L';
static LONG numarq_loc;
static TEXT *nomearq_loc;
static TEXT *dir_ex_loc;

extern SI   errno;
/* extern LONG ap_nregexp;      */ /* setado no reprocessamento lb2ferri.c */

F__GLB   COUNT exporta(sele, pfun)
COUNT sele;
#ifdef MSDOS
   LONG (*pfun)( COUNT, LONG );
#else
   LONG (*pfun)( );
#endif
{  COUNT ret, fim, tam, i, carac;
   LONG nreg, reg_ok;
   TEXT *buf;
	TEXT nome[PATH_MAX+1];
	TEXT basename[TE+1];
   Campo **vetcmp;
   TEXT *ajuda_ant = win_ajuda;
   WIN  *j0, *jm, *ji;

   win_ajuda = "EXPORT";
   ap_ignorauso = 1;

   up_str(GRP_EXPO);

   j0 = jm  = ji = NULL;

   fpo_exp     = NULL;
   reg_ok      = 0L;
   buf         = NULL;
   vet_ic      = NULL;
   numarq_loc  = 0;


   buf            = malloc(SIZE_INBUF);
   nomearq_loc    = malloc(PATH_MAX+1);
   dir_ex_loc     = malloc(PATH_MAX+1);
   vet_ic         = calloc(MAX_CAMPOS + 1, sizeof(COUNT));

   if ( buf == NULL || nomearq_loc == NULL || dir_ex_loc == NULL || vet_ic == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto sai;
   }

	if (WCB == NULL || WCB->cb_fordem == 0 ) {
      ret = -1;
      goto sai;
   }

   /* bases comerciais nao podem ser exportadas */
   if ( WBS->bs_finger != 0 && pode_exportar(WBS->bs_idbase) != 'S' )
      goto sai;

   if ( pfun == NULL )
      pfun = prox_reg;

   j0 = abre_janela(J_UTIL);      /* abre janela para salvar conteudo */
   ji = abre_insercol(C_EX_FOOT);
   jm = abre_mensagem(str_nada);


   fw_title(j0, C_EX_CAB, j0->w_atrib);
	fwbox(j0);


   mvwprintw(j0 ,  3,  2, C_EX_CMP );
   mvwprintw(j0 ,  5,  2, C_EX_DIR );
   mvwprintw(j0 ,  7,  2, C_EX_ARQ );
   mvwprintw(j0 ,  7, 40, C_EX_FMT );
   fwrefresh(j0);


   for ( i = 0; i < MAX_CAMPOS; i++)
      vet_ic[i] = -1;


   if ( par_export(j0, buf, basename, dir_ex_loc) != 0 ) {
      ret = -1;
      goto fim_gt;
   }


   fwframe(9, 30, 11, 50, SIMPLES, j0);
   mvwprintw(j0 , 9,  30 + ((20 - strlen(C_EX_TVAL)) / 2), C_EX_TVAL);

   fwrefresh(j0);



   if ( info_base(basename, dir_ex_loc, str_nada, nome)  < 0 ) {
      mens_erro(H_NOREG, E_NOREG, nome);
      ret = -1;
      goto fim_gt;
   }

   if ( ( fpo_exp = fopen(nome, "w")) == NULL ) {
      mens_erro(H_OPESO, E_OPE_SO, errno, nome);
      ret = -1;
      goto fim_gt;
   }

   ret = 0;

   if ( padrao_export == 'D' ) {
      /*  imprime nome dos campos no arquivo  */
      if ( strcmp(buf, "*") == 0 ) {
         vetcmp = pos_vetcmp(sele);
         for (i=0; vet_ic[i] != -1; i++) {
            carac = (vet_ic[i+1] == -1) ? '\n' : ',';
            if (fputs(vetcmp[i]->cp_nome, fpo_exp) == EOF ||
               fputc(carac, fpo_exp) == EOF ){
               ret = -1;
               break;
            }
         }
      }
      else {
         if ( fputs(buf, fpo_exp) == EOF || fputc('\n', fpo_exp) == EOF )
            ret = -1;
      }

      if ( ret < 0 ){
         mens_erro(H_EXWRT, E_EX_WRT);
         goto fim_gt;
      }
   }
   else {
      if ( fputs("!C25", fpo_exp) == EOF || fputc('\n', fpo_exp) == EOF ){
         mens_erro(H_EXWRT, E_EX_WRT);
         ret = -1;
         goto fim_gt;
      }
   }

   nreg = 0L;
   fim  = 0;

   while ( ! fim  ) {

      if ( w_kbhit(j0) && fwnqgc(j0) == K_ESC ) {
         if ( decida(TW_EX_D20, TW_EX_D29, H_CFEXPCAN,
                     2, 33, "%s\n%s", TW_EX_D21, TW_EX_D22 ) == 1 ) {
            break;
         }
      }


      if ( (nreg = (*pfun)( sele, nreg )) <= 0 ) {

         if ( nreg < 0 ) {
            mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
            ret = -1;
            goto fim_gt;
         }
         fim = 1;
         break;
      }

      if ( (ret = read_record(sele, nreg, 0)) != 0 ) {

         /* pode pegar registro NOVO/LIBERADO em read_record */
         if ( ret == 2 || ret == 4 ) {
            ret = 0;
            continue;
         }
         else {
            ret = -1;
            fim = 1;
            goto fim_gt;
         }
      }

      if ( exec_export(sele, buf) < 0 ) {
         ret = -1;
         fim = 1;
         continue;
      }

      if ( padrao_export == 'D' ) {

         /* retira virgula final se a mesma existir */
         tam = strlen(buf);
         if ( tam > 0  &&  buf[tam - 1] == DELIMITA )
            buf[tam - 1] = '\0';


         if ( fputs(buf, fpo_exp) == EOF || fputc('\n', fpo_exp) == EOF ){
            mens_erro(H_EXWRT, E_EX_WRT);
            ret = -1;
            goto fim_gt;
         }
      }

      fwat(37, 10, j0);
      fwprintf(j0, STR_6LD, ++reg_ok);
      fwrefresh(j0);

   }

   ret = 0;

fim_gt :

   if ( fpo_exp != NULL ) fclose(fpo_exp);

   fpo_exp = NULL;

   if ( ret == 0 )
      men_conf(C_EX_FINAL, "*", win_ajuda, NULL);



	fecha_mensagem(jm);
   fecha_insercol(ji);
	if ( j0 != NULL ) fwkill(j0);


sai :

   if ( vet_ic != NULL ) free(vet_ic);
   vet_ic = NULL;
   if ( dir_ex_loc != NULL ) free(dir_ex_loc);
   if ( nomearq_loc != NULL )   free(nomearq_loc);
   if ( buf != NULL ) free(buf);

   padrao_export = 'L';

   down_str(GRP_EXPO);
   ap_ignorauso = 0;
   win_ajuda = ajuda_ant;
   return(ret);
}






F__LOC   COUNT par_export(jt, campos, nome, path)
WIN *jt;
TEXT *campos, *nome, *path;
{  COUNT ret, le_inic;
   DTela nr[5];
   TEXT *mascara = NULL;
   TEXT padrao[30], aux[30];

   if ( (mascara = malloc(256)) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }
   else {
      /* monta mascara X255 */
      memset(mascara, 'X', 255);
      mascara[255] = '\0';
   }


   seta_dtela(&nr[0], campos, mascara, C_EX_RCMP,   3, 21, 45, E_ALFA,  H_EXRCMP, NULL,        NULL, vl_campos );
   seta_dtela(&nr[1], path,     XPATH, C_EX_RDIR,   5, 21,  0, E_ALFA,  H_EXRDIR, NULL,        NULL, vl_expath );
   seta_dtela(&nr[2], nome,      xARQ, C_EX_RARQ,   7, 21,  0, E_ALFA,  H_EXRARQ, NULL,        NULL, vl_arq    );
   seta_dtela(&nr[3], padrao,     x14, C_EX_RPAD,   7, 50,  0, E_LISTA, H_EXRPAD, C_EX_OPCPAD, NULL, NULL);
   nr[4].destino = NULL;


   campos[0] = '\0';
   nome[0]   = '\0';
   strcpy(path, STR_PONTO);

   sel_campo(sele_loc, NULL, 1 ); /* flag 1 so resetar ultimo campo */

   le_inic = 1;

le :

   ret = ed_read(jt, &nr[0], le_inic, LIMPA_LE, LF130E);

	if ( ret == K_F0 )
      goto le;

	if ( ret < 0 || ret == K_ESC ) {  /* erro ou ESC */
		ret = -1;
      goto fim;
   }

   else {
      if (campos[0] == '\0' ) {
         mens_erro(H_CPHD2, E_CP_HD2);
         goto le;
      }
      if (nome[0] == '\0' ) {
         mens_erro(H_EXARQ, E_EX_ARQ);
         goto le;
      }
   }

   piece(C_EX_OPCPAD, aux, '@', 1, TE);

   if ( strcmp( padrao,  aux) == 0 )
      padrao_export = 'L';
   else {
      if ( decida(TW_EX_D30, TW_EX_D39, NULL,
                  2, 46, "%s\n%s", TW_EX_D31, TW_EX_D32 ) == 0 ) {
         padrao_export = 'D';
      }
      else {
         le_inic = 4;
         goto le;
      }
   }


   ret = 0;

fim :

   if ( mascara != NULL ) free(mascara);

   return(ret);
}




F__LOC   COUNT vl_campos(entry)
d_entry *entry;
{  COUNT ret, tam;
   COUNT tecla;
   TEXT *nome_ret;

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP ||
		  tecla == K_HOME || tecla == K_PGUP) {
		ret = ACEITA;
		goto fim;
	};

   if ( tecla == K_F0 ) {
      if ( (nome_ret = sel_campo(sele_loc, NULL, 0) ) != NULL ) {
         /* push nome do campo no teclado */
         w_ungc(',');
         w_ungs(nome_ret);
         ret = REJEITA;
         goto fim;
      }

   }

   tam = strlen(entry->str);

   if ( strcmp(entry->str, "*") == 0 ) {
      if ( tam > 0 ) {
         if ( entry->str[tam - 1] == DELIMITA )
            entry->str[tam - 1] = '\0';
      }
   }


   if ( anal_campos(sele_loc, entry->str) < 0 )
      ret = REJEITA;
   else
      ret = ACEITA;

fim :

   return(ret);
}


F__LOC   COUNT vl_expath(entry)
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
/*
      ANANIAS ........ propoe corrente ???
      mens_erro("E_EX_DIR", E_EX_DIR, entry->str);
      ret = REJEITA;
      goto fim;
*/
   }


   /**
   ***   Verifica se existe diretorio informado
   **/

   ret = info_base(str_nada, entry->str, str_nada, aux);

   if ( ret != -1 ) {
      mens_erro(H_EX1DIR, E_EX_1DIR);
      ret = REJEITA;
      goto fim;
   }

   strcpy(entry->str, aux);

  	ret = ACT_REIMP;

fim :

   return(ret);
}



F__LOC   COUNT vl_arq(entry)
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
      mens_erro(H_EXARQ, E_EX_ARQ);
      ret = REJEITA;
      goto fim;
   }


   ret = info_base(entry->str, dir_ex_loc, str_nada, aux);

   if ( ret < 0 ) {
      mens_erro(H_NOREG, E_NOREG, entry->str);
      ret = REJEITA;
      goto fim;
   }

   if ( ret != 0  ) {
      if ( decida(TW_EX_D00, TW_EX_D99, H_CFRWEX,
                  2, 38, "%s\n%s", TW_EX_D01, TW_EX_D02 ) != 1 ) {
         ret = REJEITA;
         goto fim;
      }
   }

  	ret = ACT_REIMP;

fim :

   return(ret);
}


F__LOC   COUNT exec_export(sele, buf)
COUNT sele;
TEXT *buf;
{  COUNT ret=0, i, qtd_utilizada, tam, ind;
   Campo **vetcmp, *pc;
   TEXT *p_buf, *p_aux;

   qtd_utilizada = 0;

   p_buf = buf;

   vetcmp = pos_vetcmp(sele);

   for ( i = 0; i < MAX_CAMPOS; i++) {

      if ( vet_ic == NULL )
         if ( vetcmp[i] == NULL )
            break;
         else
            ind = i;
      else
         ind = vet_ic[i];

      if ( ind != -1 ) {

         pc  = vetcmp[ind];

         if ( pc->cp_tipo == CP_TEXTO ) {
            if ( padrao_export != 'D' ) {
               if ( exp_ln_arq(pc, buf) != 0 ) {
                  ret = -1;
                  goto fim;
               }
               continue;
            }
            else {
               p_aux = nomearq_loc;
               if ( exp_db_arq(pc) != 0 ) {
                  ret = -1;
                  goto fim;
               }
            }
         }
         else {
            p_aux = pc->cp_newdata;
            trim(p_aux);
         }

         if ( padrao_export != 'D' ) {
            if ( *p_aux != '\0' ) {
            	if ( ap_nregexp == 0L )
               	sprintf(buf, STR_HDLIN, WCB->cb_numrec, pc->cp_nome,  p_aux);
               else
               	sprintf(buf, STR_HDLIN, ap_nregexp, pc->cp_nome,  p_aux);
               if ( fputs(buf, fpo_exp) == EOF || fputc('\n', fpo_exp) == EOF ) {
                  mens_erro(H_EXWRT, E_EX_WRT);
                  ret = -1;
                  goto fim;
               }
            }
         }
         else {
            /* coloca dado do campo no buffer */
            tam = strlen(p_aux);
            qtd_utilizada += ( tam + 1 );

            if ( pc->cp_tipo != CP_TEXTO ) {  /* ASPAS */
               qtd_utilizada += 2;
               if ( tam > 0 ) qtd_utilizada++; /* caracter @ */
            }

            if ( qtd_utilizada >=  SIZE_INBUF ) {
               mens_erro(H_EX004, E_EX_004);
               ret = -1;
               goto fim;
            }

            if ( pc->cp_tipo != CP_TEXTO )
               *p_buf++ = ASPAS;
            else if ( tam > 0 ) /* existe nome de arquivo */
               *p_buf++ = '@';

            strcpy(p_buf, p_aux);

            p_buf   += tam;

            if ( pc->cp_tipo != CP_TEXTO ) *p_buf++ = ASPAS;

            *p_buf++ = DELIMITA;
         }
      }
   }

   if ( qtd_utilizada > 0 )
      *(p_buf - 1 ) = '\0'; /* retira ultima virgula colocada */

fim :

   return(ret);
}


F__LOC   COUNT anal_campos(sele, buf)
COUNT sele;
TEXT *buf;
{  COUNT ret, i, j, max=0, numcps;
   TEXT aux[TD+1];

   numcps = qtd_vetcmp(sele);

   if ( strcmp(buf, "*") == 0 )
      max = numcps;


   for ( i = 0; i < numcps; i++) {

      vet_ic[i] = -1;

      if ( max > 0 ) {
         if ( i < max )
            vet_ic[i] = i;
      }
      else {
         piece(buf, aux, ',', (i+1), TE);

         if ( aux[0] == '\0' ) continue;

         vet_ic[i] = ver_campo(sele, aux, &j);

         if ( vet_ic[i] == -1 ) { /* campo nao existe */
            mens_erro(H_CPHD1, E_CP_HD1, aux);
            ret = -1;
            goto fim;
         }
      }
   }

   ret = 1;

   for ( i = 0; i < numcps; i++)
      if ( vet_ic[i] != -1 || max > 0 ) {ret = 0; break; }

   if ( ret != 0 ) { /* nenhum campo a exportar */
      mens_erro(H_CPHD2, E_CP_HD2);
      ret = -1;
      goto fim;
   }

fim :

   return(ret);
}





F__LOC   COUNT exp_db_arq(pc)
Campo *pc;
{  COUNT ret;
   LONG tam;
   POINTER pos;
	TEXT nome[PATH_MAX+1];
   FILE *fp = NULL;


   tam = 0L;
   ret = 0;

   nomearq_loc[0] = '\0';

   if ( pc->cp_adoc == NULL ) {

      memcpy((TEXT *)&pos, (TEXT *)pc->cp_newdata , sizeof(pos));

      if ( pos != (POINTER) 0 ) {
         pc->cp_adoc = txt_load(AD_NUM(sele_loc), pos, &tam, pc->cp_mhor, WCB->cb_numrec);
         if ( pc->cp_adoc == NULL ) {
            debug(M_PRDTXT, c_bases[BASE_EDIT]->cb_numrec, pc->cp_nome);
            ret = 0; /* Esta dando debug para nao comprometer todo o resto */
            /* ret = -1; */
            goto fim;
#ifdef ASSIM_E_PIOR
            ret = -1;
            mens_erro(H_EX001, E_EX_001);
            goto fim;
#endif
         }
      }
   }


   if ( tam != 0 ) { /* tem informacao para gravar no arquivo */

      sprintf(nomearq_loc, "%8.8lu%s", ++numarq_loc, EXT_EXPORT);

      if ( info_base(nomearq_loc, dir_ex_loc, str_nada, nome)  < 0 ) {
         mens_erro(H_NOREG, E_NOREG, nome);
         ret = -1;
         goto fim;
      }


      if ( (fp = fopen(nome, "w")) == NULL ) {
         mens_erro(H_EX002, E_EX_002, nome);
         ret = -2;
         goto fim;
      }


      pc->cp_adoc->tipo    = ET_HANDLE;
      pc->cp_adoc->handle  = fileno(fp);
      pc->cp_adoc->buf_len = (LONG)65000L;

      ret = save_file(pc->cp_adoc); /* igual a zero indica que nao salvou */

      if ( ret == 0 ) {
         mens_erro(H_EX003, E_EX_003);
         ret = -2;
         goto fim;
      }

      else ret = 0;
   }

fim :

   if ( fp != NULL ) {
      if ( fclose(fp) < 0 ) {
         mens_erro(H_EX003, E_EX_003);
         ret = -2;
      }
   }


   return(ret);
}


F__LOC   COUNT exp_ln_arq(pc, buf)
Campo *pc;
TEXT *buf;
{  COUNT ret, num_lin;
   ARG *ant_adoc;
   LONG tam;
   POINTER pos;
   TEXT linha[250];

   tam = 0L;
   num_lin = 0;
   ret = 0;

   ant_adoc = pc->cp_adoc;

   if ( pc->cp_adoc == NULL ) {

      memcpy((TEXT *)&pos, (TEXT *)pc->cp_newdata , sizeof(pos));

      if ( pos != (POINTER) 0 ) {
         pc->cp_adoc = txt_load(AD_NUM(sele_loc), pos, &tam, pc->cp_mhor, WCB->cb_numrec);
         if ( pc->cp_adoc == NULL )  {
            debug(M_PRDTXT, c_bases[BASE_EDIT]->cb_numrec, pc->cp_nome);
            ret = 0; /* Esta dando debug para nao comprometer todo o resto */
            /* ret = -1; */
            goto fim;
#ifdef ASSIM_E_PIOR
            ret = -1;
            mens_erro(H_EX001, E_EX_001);
            goto fim;
#endif
         }
      }
   }


   if ( tam != 0 ) { /* tem informacao para gravar no arquivo */
      while ( lin_texto( pc->cp_adoc, num_lin++, linha, 0) > 0 ) {
         if ( ap_nregexp == 0L )
         	sprintf(buf, STR_HDLIN, WCB->cb_numrec, pc->cp_nome, linha);
         else
         	sprintf(buf, STR_HDLIN, ap_nregexp, pc->cp_nome, linha);
         if ( fputs(buf, fpo_exp) == EOF || fputc('\n', fpo_exp) == EOF ) {
            mens_erro(H_EXWRT, E_EX_WRT);
            ret = -1;
            goto fim;
         }
      }
   }

fim :

   if ( ant_adoc == NULL && pc->cp_adoc != NULL) {
      free_buffer(pc->cp_adoc );
      pc->cp_adoc = NULL;
   }

   return(ret);
}
