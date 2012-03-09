#include  "lb2all.h"

#include  <string.h>
#include  <ctype.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"

#include  "lb2prot1.h"


#define   GRP_INP 28

#define   LF13E       "1 3 E"


#define   SIZE_INBUF   (1024 * 6)
#define   DELIMITA ','
#define   ASPAS    '"'

#define   FIM_PARAGRAFO      'F'
#define   MERECE_PARAGRAFO   'M'
#define   SEM_BRANCO         'L'
#define   MESMO_PARAGRAFO    'C'



#define   H_IN_HD0    "E_IN_HD0"  /* erro. Falta header de inportacao */
#define   H_IN_HD1    "E_IN_HD1"  /* erro. campo nao existe no header */
#define   H_IN_HD2    "E_IN_HD2"  /* erro. nenhum campo a importar (header) */
#define   H_CFINPCAN  "CF_INPCAN" /* confirmacao de cancelar importacao */
#define   H_FIMINP    "C_IN_FINAL"  /* fim de importacao. qq tecla */
#define   H_INFIL     "C_IN_FIL"    /* nome do arquivo de importacao */
#define   H_INLOG     "C_IN_LOG"    /* nome do arquivo de log */
#define   H_INPRO     "C_IN_PRO"    /* o que fazer com registros invalidos */
#define   H_ININD     "C_IN_IND"    /* se indexa durante importacao */

#define   H_INNAME     "E_IN_NAME"  /* sem nome de arquivo para importar */
#define   H_NOTINP     "NOT_IMPO"   /* Arquivo nao existe */




#define   C_IN_FOOT     loc_str(GRP_INP, 2  )
#define   C_IN_CAB      loc_str(GRP_INP, 3  )
#define   C_IN_TNAME    loc_str(GRP_INP, 4  )
#define   C_IN_TLOG     loc_str(GRP_INP, 5  )
#define   C_IN_TPROC    loc_str(GRP_INP, 6  )
#define   C_IN_TINDEX   loc_str(GRP_INP, 7  )
#define   C_IN_TVAL     loc_str(GRP_INP, 8  )
#define   C_IN_TINVAL   loc_str(GRP_INP, 9  )
#define   C_IN_OPPROC   loc_str(GRP_INP, 10 )
#define   C_IN_RNAME    loc_str(GRP_INP, 11 )
#define   C_IN_RLOG     loc_str(GRP_INP, 12 ) /****/
#define   C_IN_RPROC    loc_str(GRP_INP, 13 )
#define   C_IN_RINDEX   loc_str(GRP_INP, 14 )
#define   C_IN_FINAL    loc_str(GRP_INP, 15 )
#define   C_IN_FLOG     loc_str(GRP_INP, 16 )
#define   C_IN_JLOG     loc_str(GRP_INP, 17 )
#define   C_IN_LINH     loc_str(GRP_INP, 18 )
#define   C_IN_LICA     loc_str(GRP_INP, 19 )
#define   C_IN_WAIT     loc_str(GRP_INP, 20 )
#define   C_IN_ESC      loc_str(GRP_INP, 21 )



#define   E_IN_NAME     loc_str(GRP_INP, 22 )
                           /*  (GRP_INP, 23 ) */
#define   E_IN_READ     loc_str(GRP_INP, 24 )
#define   E_IN_ABT      loc_str(GRP_INP, 25 )
#define   E_IN_HD0      loc_str(GRP_INP, 26 )
#define   E_IN_HD1      loc_str(GRP_INP, 27 )
#define   E_IN_HD2      loc_str(GRP_INP, 28 )
#define   E_IN_001      loc_str(GRP_INP, 29 )
#define   E_IN_002      loc_str(GRP_INP, 30 )
#define   E_IN_003      loc_str(GRP_INP, 31 )  /******/
#define   E_IN_004      loc_str(GRP_INP, 32 )
#define   E_IN_005      loc_str(GRP_INP, 33 )
#define   E_IN_006      loc_str(GRP_INP, 34 )
#define   E_IN_007      loc_str(GRP_INP, 35 )

#define   E_IN_008      loc_str(GRP_INP, 36 )
#define   E_IN_009      loc_str(GRP_INP, 37 )
#define   E_IN_010      loc_str(GRP_INP, 38 )
#define   E_IN_011      loc_str(GRP_INP, 39 )
#define   E_IN_012      loc_str(GRP_INP, 40 )
#define   E_IN_013      loc_str(GRP_INP, 41 )



#define   TW_IN_D20     loc_str(GRP_INP, 42 )
#define   TW_IN_D29     loc_str(GRP_INP, 43 )
#define   TW_IN_D21     loc_str(GRP_INP, 44 )
#define   TW_IN_D22     loc_str(GRP_INP, 45 )

#define   M_KEYDES   loc_str(GRP_INP, 46)

#ifdef MSDOS
   COUNT vl_log(d_entry *);
   COUNT par_inport(WIN *, TEXT *, TEXT *, COUNT *, COUNT *);
   COUNT pr_proc(d_entry *);
   COUNT pr_inde(d_entry *);
   COUNT vl_nome(d_entry *);
   COUNT exec_dbinport(COUNT, TEXT *, WIN *);
   COUNT exec_lninport(COUNT, TEXT *, WIN *);
   COUNT hdr_inport(COUNT, TEXT *);
   COUNT cospe_piece(TEXT *, TEXT *, COUNT, COUNT (*)());
   COUNT seta_campo(TEXT *, COUNT, COUNT);
   COUNT inp_arq(TEXT *, Campo *);
   COUNT monta_reg(COUNT, TEXT *);
   COUNT trata_lntexto(COUNT, TEXT *, COUNT, Campo *, COUNT);
   COUNT ve_fimlinha(TEXT *);
   COUNT ve_mesmo(TEXT *, TEXT *, COUNT);
   VOID enche_autonum( COUNT );
#else
   COUNT vl_log( );
   COUNT par_inport( );
   COUNT pr_proc( );
   COUNT pr_inde( );
   COUNT vl_nome( );
   COUNT exec_dbinport( );
   COUNT exec_lninport( );
   COUNT hdr_inport( );
   COUNT cospe_piece( );
   COUNT seta_campo( );
   COUNT inp_arq( );
   COUNT monta_reg( );
   COUNT trata_lntexto( );
   COUNT ve_fimlinha( );
   COUNT ve_mesmo( );
   VOID enche_autonum(  );
#endif


/*--------------------------------------------  variaveis estaticas ----*/
static LONG  linha_loc;
static COUNT *vet_ic;
static COUNT padrao_inport, col_inport;
static FILE  *fpi_loc;
static LONG  nreg_inport;
static COUNT proc_loc, inde_loc;
static COUNT ex_para_mark, ex_hifens, ex_brfinais;
static COUNT imp_autonum; /* autonumera campos quando em branco */
static Campo **vet_cmp;
static TEXT *path_inport;
static WIN  *jlixo;

extern SI errno;


#ifdef AUTO_INCREMENTO
#ifdef MSDOS
   LONG auto_incre( TEXT *, COUNT );
   COUNT auto_getfield( VOID );
#else
   LONG auto_incre( );
   COUNT auto_getfield( );
#endif
extern COUNT ap_flag_auto;
extern COUNT ap_autonum;
#endif



F__GLB   COUNT importa(sele)
COUNT sele;
{  COUNT ret, i, salva_flush;
   LONG reg_ok, reg_rej;
   TEXT *buf, *p;
	TEXT nome[PATH_MAX+1], log[PATH_MAX+1];
   TEXT *ajuda_ant = win_ajuda;
   COUNT ind_ant   = ap_pos_index;
   WIN  *j0, *jm, *ji;

   win_ajuda = "INPORT";
   salva_flush = ap_flushfile;
   ap_flushfile = 100;

   j0 = jm = ji = NULL;
   jl_glb = NULL;
   fpi_loc = fpo_glb = NULL;
   reg_ok = reg_rej = 0L;
   buf = NULL;
   vet_ic = NULL;
   path_inport = NULL;
   nreg_inport = 0L;
   col_inport = 0;


   up_str(GRP_INP);

   j0 = abre_janela(J_UTIL);      /* abre janela para salvar conteudo */
   ji = abre_insercol(C_IN_FOOT);
   jm = abre_mensagem(str_nada);

   jlixo = abre_janela(J_EXCUSU); /* Nao serve pra nada so para tirar core dumped do total wrap no xenix */
           fwbot(jlixo);

	if (WCB == NULL || WCB->cb_fordem == 0 ) {
      ret = -1;
      goto fim;
   }

   vet_cmp = pos_vetcmp(sele);


   free_record(sele);

   fw_title(j0, C_IN_CAB, j0->w_atrib);
	fwbox(j0);


   mvwprintw(j0 ,  1, 2, C_IN_TNAME );
   mvwprintw(j0 ,  3, 2, C_IN_TLOG  );
   mvwprintw(j0 ,  5, 2, C_IN_TPROC );
   mvwprintw(j0 ,  7, 2, C_IN_TINDEX);
   fwrefresh(j0);


   if ( par_inport(j0, nome, log, &proc_loc, &inde_loc) != 0 ) {
      ret = -1;
      goto sai;
   }

   if ( inde_loc == 0 ) ap_pos_index = NOW_INDEX;
   else                 ap_pos_index = POS_INDEX;

   fwframe(9, 10, 11, 30, SIMPLES, j0);
   mvwprintw(j0 , 9,  10 + ((20 - strlen(C_IN_TVAL)) / 2), C_IN_TVAL);

   fwframe(9, 45, 11, 65, SIMPLES, j0);
   mvwprintw(j0 , 9,  45 + ((20 - strlen(C_IN_TINVAL)) / 2), C_IN_TINVAL);

   fwrefresh(j0);

	jl_glb = abre_janela( J_LOGINPORT);

   fw_title(jl_glb, C_IN_JLOG, jl_glb->w_atrib);
	fwbox(jl_glb);
   jl_glb->w_flags |=  W_WRAP;
   jl_glb->w_flags &= ~W_NOSCROL;

   if ( ( fpo_glb = fopen(log, "w")) == NULL ) {
      mens_erro(H_OPESO, E_OPE_SO, errno, log);
      ret = -1;
      goto fim;
   }
   else e_log_glb = 0;

   ap_adverte = 0;

   if ( ( fpi_loc = fopen(nome, "r")) == NULL ) {
      mens_erro(H_OPESO, E_OPE_SO, errno, nome);
      ret = -1;
      goto fim;
   }

   /* prepara path para arquivos de inportacao (textos) */
   if ( strchr(nome, BARRA) != NULL ) {
      /* utiliza variavel log para armazenar path */
      strcpy(log, nome);
      p = strrchr(log, BARRA);
      p++;
      *p = '\0';
      path_inport = log;
   }

   buf = malloc(SIZE_INBUF);

   if ( buf == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   if ( (vet_ic = calloc(MAX_CAMPOS + 1, sizeof(COUNT))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }
   else {
      for ( i = 0; i < MAX_CAMPOS; i++)
         vet_ic[i] = -1;
   }

   linha_loc = 1L;

   ex_para_mark = 0;
   ex_hifens = 0;
   ex_brfinais = 0;
   imp_autonum = 0;


rele_hdr:

   if ( fgets (buf, SIZE_INBUF - 1 , fpi_loc) == NULL ){
      mens_erro(H_IN_HD0, E_IN_HD0);
      ret = -1;
      goto fim;
   }
   else {

      strip_nl(buf);

      if ( (nreg_inport = atol(buf)) > 0L ) {
         padrao_inport = 'L';  /* padrao linear */
         /* deixa linha em buffer para exec_lninport ja iniciar com linha lida */
      }
      else {
         padrao_inport = 'D'; /* padrao DBASE */
         ret = hdr_inport(sele, buf);
         switch(ret) {
            case 1 : goto rele_hdr;
            case 0 : break;
            default: ret = -1;
                     goto fim;
         }
      }
   }


   while ( ! feof(fpi_loc) ) {

      if ( w_kbhit(j0) && fwnqgc(j0) == K_ESC ) {
         if ( decida(TW_IN_D20, TW_IN_D29, H_CFINPCAN,
                     2, 33, "%s\n%s", TW_IN_D21, TW_IN_D22 ) == 1 ) {
            wr_log(0, C_IN_ESC);
            break;
         }
      }


      if ( padrao_inport == 'D' ) {
         ret = exec_dbinport(sele, buf, jm);
      }
      else {
         /* importante. Buf ja contem linha de inicio do novo registro */
         ret = exec_lninport(sele, buf, jm);
      }


      if ( ret == -1 ) {      /* Erro fatal na inportacao */
            wr_log(1, E_IN_ABT);
            ret = -1;
            goto fim;
      }
      else if ( ret == 0 ) {  /* registro importado com sucesso */
            fwat(17, 10, j0);
            fwprintf(j0, "%6ld", ++reg_ok);
      }
      else if ( ret == -8 ) {  /* erro de leitura. fim de importacao */
            wr_log(1, E_IN_READ);
            wr_log(1, E_IN_ABT);
            mens_erro(H_RDSO, E_RD_SO, errno, nome);
            break;
      }
      else if ( ret == -9 ) {  /* fim de importacao */
            wr_log(1, C_IN_FLOG);
            break;
      }
      else {
            fwat(54, 10, j0);
            fwprintf(j0, "%6ld", ++reg_rej);
      }

      if ( e_log_glb == 2 ) {
         /* erro na gravacao do log. Usuario decidiu cancelar importacao */
         ret = -1;
         goto fim;
      }

      fwrefresh(j0);

   }

   ret = 0;

fim :

   ret = fim_index();

   if ( fpi_loc != NULL && fpo_glb != NULL &&  ap_adverte > 0 ) {
      TEXT mens[100];
      sprintf(mens, M_LOGWAR, ap_adverte);
      wr_log(0, mens);
   }

   if ( fpi_loc != NULL ) fclose(fpi_loc);
   if ( fpo_glb != NULL ) {
      fclose(fpo_glb);
      fpo_glb = NULL;
   }

   if ( ret == 0 )
      men_conf(C_IN_FINAL, "*", H_FIMINP, NULL);

sai :

   if ( vet_ic != NULL ) free(vet_ic);
   if ( buf != NULL ) free(buf);



	if ( jl_glb != NULL ) fwkill(jl_glb);

	if ( jlixo != NULL ) fwkill(jlixo);
	fecha_mensagem(jm);
   fecha_insercol(ji);
	if ( j0 != NULL ) fwkill(j0);

   win_ajuda = ajuda_ant;
   ap_flushfile = salva_flush;
   ap_pos_index = ind_ant;

   down_str(GRP_INP);

   return(ret);
}






F__LOC	 COUNT par_inport(jt, nome, log, proc, index)
WIN *jt;
TEXT *nome, *log;
COUNT *proc, *index;
{  COUNT ret;
   DTela nr[5];
   TEXT aux[30];
   TEXT proc_str[TD+1];
   TEXT index_str[TD+1];

   seta_dtela(&nr[0], nome,    XPATH, C_IN_RNAME,  1, 36, 0, E_ALFA,  H_INFIL, NULL,        NULL, vl_nome );
   seta_dtela(&nr[1], log,     XPATH, C_IN_RLOG,   3, 36, 0, E_ALFA,  H_INLOG, NULL,        NULL, vl_log );
   seta_dtela(&nr[2], proc_str,  x20, C_IN_RPROC,  5, 36, 0, E_LISTA, H_INPRO, C_IN_OPPROC, pr_proc, NULL );
   seta_dtela(&nr[3], index_str, x14, C_IN_RINDEX, 7, 36, 0, E_LISTA, H_ININD, op_sn,       pr_inde, NULL );
   nr[4].destino = NULL;

#ifdef MSDOS
   strcpy(log, C_LOGDOS);
#else
   strcpy(log, C_LOGUNIX);
#endif

   if (log[0] != '\0') strcat(log, STR_BARRA);


   if (v_bases[0] != NULL) {
      strcat(log, v_bases[0]->bs_nome);
      strcat(log, ".");
      strcat(log, C_TMPLOG);
   }
   else {
      strcat(log, C_TMPLOG);
   }

   nome[0] = '\0';
   proc_str[0] = '\0';
   index_str[0] = '\0';

le :

   ret = ed_read(jt, &nr[0], 1, LIMPA_LE, LF13E);

	if ( ret < 0 || ret == K_ESC ) {  /* erro ou ESC */
		ret = -1;
      goto fim;
   }

   else {
      if (nome[0] == '\0' ) {
         mens_erro(H_INNAME, E_IN_NAME);
         goto le;
      }
      if (nome[0] == '\0' ) {
         mens_erro(H_ARQLOG, E_IN_LOG);
         goto le;
      }
   }


   piece(C_IN_OPPROC, aux, '@', 1, TD);
   if ( strcmp( aux, proc_str) == 0 ) *proc = 0; /* rejeita */
   else                               *proc = 1; /* limpa */

   if ( strcmp( OP_SIM, index_str) == 0 ) *index = 0;
   else                                   *index = 1;

   ret = 0;

fim :

   return(ret);
}



F__LOC	 COUNT pr_proc(entry)
d_entry *entry;
{  COUNT ret = 0;

	if ( entry->str[0] == '\0' ) {
      piece(C_IN_OPPROC, entry->str, '@', 2, TD);
      ret = ACT_REIMP;
   }
   else {
      ret = 0;
   }

   return(ret);
}

F__LOC	 COUNT pr_inde(entry)
d_entry *entry;
{  COUNT ret = 0;

	if ( entry->str[0] == '\0' ) {
      piece(OP_SIM_NAO, entry->str, '@', 1, TD);
      ret = ACT_REIMP;
   }
   else {
      ret = 0;
   }

   return(ret);
}



F__LOC	 COUNT vl_nome(entry)
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


   ret = info_base(entry->str, str_nada, str_nada, aux);

   if ( ret < 0 ) {
      mens_erro(H_NOREG, E_NOREG, entry->str);
      ret = REJEITA;
      goto fim;
   }
   if ( ret == 0  ) {
      mens_erro(H_NOTINP, E_NOFILE, entry->str);
      ret = REJEITA;
      goto fim;
   }
   if ( (ret & PODE_LER) == 0 ){
      mens_erro(H_PERMAC, E_PERMAC, entry->str);
      ret = REJEITA;
      goto fim;
   }

  	ret = ACT_REIMP;

fim :

   return(ret);
}



F__LOC	 COUNT exec_dbinport(sele, buf, jm)
COUNT sele;
TEXT *buf;
WIN *jm;
{  COUNT ret;
   TEXT work_buf[80], aux[100];


   ret = 0;


   if ( fgets (buf, SIZE_INBUF - 1 , fpi_loc) == NULL ){
      if ( ! feof(fpi_loc) ) {
         ret = -8;
         goto fim;
      }
      else {
         ret = -9;
         goto fim;
      }
   }

   linha_loc++;

   strip_nl(buf);


   if ( new_record(sele, 0) < 0 ) {  /* ANANIAS talvez tenha que mudar para importar parte de registros */
      ret = -1;
      goto fim;
   }

   mens_1(jm, C_IN_WAIT, c_bases[sele]->cb_numrec);

   ret = cospe_piece(buf, work_buf, 80, seta_campo);

   if ( ret == 0 ) {

      trata_outs(BASE_EDIT, 0);
      trata_formula(BASE_EDIT);
      trata_outs(BASE_EDIT, 0); /* repetido para sanar referencia multipla */

      if ( write_record(sele, ACUMULA_IDX) != 0 ) {
         if ( n_cp_unica >= 0 ) {
            sprintf(aux, M_KEYDES, c_bases[BASE_EDIT]->cb_numrec,
                   pos_vetcmp(BASE_EDIT)[n_cp_unica]->cp_nome,
                   pos_vetcmp(BASE_EDIT)[n_cp_unica]->cp_newdata);
            wr_log(1, aux);
            ret = -2;
         }
         else
            /* retorna -1 para indicar erro fatal na importacao */
            ret = -1;
      }
   }
   else {
      if ( ret == -99 ) {
         sprintf(aux, C_IN_LINH, linha_loc, E_IN_001);
         wr_log(1, aux);
      }

      /* erro ja impresso. Retorna -2 para contar registros rejeitados */
      ret = -2;
   }

fim :

   free_record(sele);

   return(ret);
}



F__LOC	 COUNT exec_lninport(sele, buf, jm)
COUNT sele;
TEXT *buf;
WIN *jm;
{  COUNT ret;
   TEXT aux[100];


   ret = 0;

   if ( new_record(sele, 0) < 0 ) {  /* ANANIAS talvez tenha que mudar para importar parte de registros */
      ret = -1;
      goto fim;
   }

   mens_1(jm, C_IN_WAIT, c_bases[sele]->cb_numrec);

   ret = monta_reg(sele, buf);

   if ( ret == 0 ) {

      /* preenche campos autonumerados, se for o caso */
      if ( imp_autonum )
         enche_autonum(BASE_EDIT);

      trata_outs(BASE_EDIT, 0);
      trata_formula(BASE_EDIT);
      trata_outs(BASE_EDIT, 0); /* repetido para sanar referencia multipla */

      if ( write_record(sele, ACUMULA_IDX) != 0 ) {
         if ( n_cp_unica >= 0 ) {
            sprintf(aux, M_KEYDES, c_bases[BASE_EDIT]->cb_numrec,
                   pos_vetcmp(BASE_EDIT)[n_cp_unica]->cp_nome,
                   pos_vetcmp(BASE_EDIT)[n_cp_unica]->cp_newdata);
            wr_log(1, aux);
            ret = -2;
         }
         else
            /* retorna -1 para indicar erro fatal na importacao */
            ret = -1;
      }
   }
   else {
      if ( ret == -99 ) {
         sprintf(aux, C_IN_LINH, linha_loc, E_IN_001);
         wr_log(1, aux);
      }

      if ( ret == -3 ) {
         if ( strlen(buf) > 20 ) {
            buf[20]= '.';
            buf[21]= '.';
            buf[22]= '.';
            buf[23]= '\0';
         }
         sprintf(aux, E_IN_008, buf);
         wr_log(1, aux);
         /* descarta linha invalida */
         memset(buf, '\0', col_inport+1);
         fgets (buf, SIZE_INBUF - 1 , fpi_loc);

      }

      /* erro ja impresso. Retorna -2 para contar registros rejeitados */
      ret = -2;
   }

fim :

   free_record(sele);

   return(ret);
}



COUNT monta_reg(sele, buf)
COUNT sele;
TEXT *buf;
{  COUNT ret, ind, i, j, tam_reg, tam_padrao;
   TEXT registro[TE+1], campo[TE+1], *p;
   Campo *pc;



   /* importante. Buf ja contem linha de inicio do novo registro */

   /* armarena em registro o numero do registro processado */
   for (i=0; isdigit(buf[i]); i++)
      registro[i] = buf[i];

   registro[i] = '\0';
   tam_reg = i;
   ret = 0;

   linha_loc = atol(registro);

   if ( linha_loc == 0 ) {
      /* sinaliza erro */
      ret = -3;
      goto fim;
   }

   /* enquanto for linha do mesmo registro continua montando registro */
   while ( strncmp(buf, registro, tam_reg) == 0 ) {

      if ( ret == 0 ) {
         /* descarta brancos e tabs entre registro e nome do campo */
         for (p = &buf[tam_reg]; *p == '\t' || *p == ' '; p++)
            ;

         /* descobre nome do campo */
         for ( i = 0; *p != '\t' && *p != ' ' && *p != '\0'; p++)
            if ( i <= TE )
               campo[i++] = *p;
         campo[i] = '\0';

         if ( col_inport == 0 )
            p++; /* pula branco apos o nome do campo */
         else
            p = &buf[col_inport-1];


         if ( isdigit(campo[0]) ) {
            j = atoi(campo);
            if ( j < MAX_CAMPOS ) {
               ind = vet_ic[j];
            }
            else ind = -1;
         }
         else {
            ind = ver_campo(sele, campo, &j);
         }

         if ( ind != -1 ) {

            pc = vet_cmp[ind];

            if ( pc->cp_tipo == 'T' ) {
               tam_padrao = p - buf;
               ret = trata_lntexto(sele, buf, tam_padrao, pc, ind);
            }
            else
               ret = seta_campo(p, strlen(p), ind);
         }
      }

le_outra:

      memset(buf, '\0', col_inport+1);
      if ( fgets (buf, SIZE_INBUF - 1 , fpi_loc) == NULL )
         break;

      strip_nl(buf);

      if ( buf[0] == '\0' )
         goto le_outra;
   }

fim :

   return(ret);
}


F__LOC	 COUNT trata_lntexto(sele, buf, tam_padrao, pc, ind)
COUNT sele;
TEXT *buf;
COUNT tam_padrao;
Campo *pc;
COUNT ind;
{  COUNT ret, skip_campo, como_term_anterior, last_line;
   TEXT aux[100], padrao[51], *p;
   LONG last_seek;
   POINTER pos;
   ARG *adoc;

   adoc = NULL;
   ret = 0;
   skip_campo = 0;
   last_seek = 0;

   if ( tam_padrao > 50 )
      tam_padrao = 50;

   strncpy(padrao, buf, 50);
   padrao[tam_padrao] = '\0';

   /* nao pode repetir texto para mesmo registro */
   memcpy((TEXT *)&pos, (TEXT *)pc->cp_newdata , sizeof(pos));
   if ( pos != (POINTER) 0 ) {
      sprintf(aux, E_IN_009, padrao);
      wr_log(1, aux);
      skip_campo = 1;
   }
   else {
      adoc = load_buffer( pc->cp_mhor, ET_MEM, str_vazia, 0L);


      if ( adoc == NULL ) {
         sprintf(aux, E_IN_010, padrao);
         wr_log(1, aux);
         skip_campo = 1;
      }
      else {
         /* JAIRO isto e um gato para voce tirar. ANANIAS (xenix) */
         adoc->je = jlixo;
      }


   }


   last_line = 0;
   como_term_anterior = 0;

   while ( strncmp(buf, padrao, tam_padrao) == 0 ||
           ve_mesmo(buf, padrao, tam_padrao) == 0 ) {
      if ( ! skip_campo ) {

         p = &buf[tam_padrao];

         if ( como_term_anterior == MERECE_PARAGRAFO ||
              ( como_term_anterior != FIM_PARAGRAFO &&
                como_term_anterior != 0 &&
                *p == ' ' ) ) {
            UTEXT linha_nova[2];
            linha_nova[0] = PARA_CHR;
            linha_nova[1] = '\0';
            if ( add_linbuffer(adoc, (TEXT *)linha_nova) != 1 ) {
               sprintf(aux, E_IN_011, padrao);
               wr_log(1, aux);
               skip_campo = 1;
            }
         }


         /* verifica como termina linha. Importante ser antes de add_linha */
         /* para retirar hifens, marcas de paragrafos etc. se for o caso */

         como_term_anterior = ve_fimlinha(p);

         if ( add_linbuffer(adoc, p) != 1 ) {
            sprintf(aux, E_IN_012, padrao);
            wr_log(1, aux);
            skip_campo = 1;
         }

         if ( ex_hifens && last_line == '-') {
            adoc->clin--;
            line_join(adoc);
         }

         last_line = como_term_anterior;

      }

      memset(buf, '\0', col_inport+1);

      last_seek = ftell(fpi_loc);
      if ( fgets (buf, SIZE_INBUF - 1 , fpi_loc) == NULL )
         break;

      strip_nl(buf);

   }


   if ( adoc != NULL ) {
      if ( ! skip_campo ) {
         pc->cp_adoc = adoc;
         pc->cp_mod_flag = 1;

         /* retira primeira linha do texto pois obrigatoriamente sera em branco */
         if ( pc->cp_adoc->t_lines > 1 ) {
            ret_linha(pc->cp_adoc, 0);
         }

         total_wrap(pc->cp_adoc, 0);

         ret = salva_textos(sele, c_bases[sele]->cb_numrec, ind);

         if ( ret < 0 ) {
            numdebug(1001); /* Erro em salva textos */
            pc->cp_mod_flag = 0;
            skip_campo = 1;
         }

         pc->cp_adoc = NULL;
      }

      free_buffer( adoc );
      adoc = NULL;
   }

   if ( skip_campo  && proc_loc == 0) { /* Rejeita registro */
      sprintf(aux, E_IN_013, padrao);
      wr_log(1, aux);
      ret = -1;
   }
   else {
      ret = 0;
   }

   if ( last_seek != 0L )
      fseek(fpi_loc, last_seek, 0);

   return(ret);
}



F__LOC	 COUNT ve_mesmo(buf, padrao, tamanho)
TEXT *buf;
TEXT *padrao;
COUNT tamanho;
{  COUNT t, c1, c2, ret;
   TEXT *p1, *p2;

   p1 = buf;
   t = strlen(p1);
   t = MIN(t, tamanho);

   p1 += (t-1);

   while (*p1 == ' ')
      p1--;

   p1++;
   c1 = *p1;
   *p1 = '\0';


   p2 = padrao;
   t = strlen(p2);
   t = MIN(t, tamanho);

   p2 += (t-1);

   while (*p2 == ' ')
      p2--;

   p2++;
   c2 = *p2;
   *p2 = '\0';




   ret = strcmp(buf, padrao);

   *p1 = c1;
   *p2 = c2;

   return(ret);
}


F__LOC	 COUNT ve_fimlinha(buf)
TEXT *buf;
{  COUNT ret, tam;
   UTEXT *p;

   tam = strlen(buf);

   ret = MERECE_PARAGRAFO;

   while ( tam ) {

      p = (UTEXT *)&buf[tam-1];

      if ( ex_para_mark && *p == PARA_CHR) {
         *p = '\0';
         tam--;
         continue;
      }

      while ( *p == ' ' && p > (UTEXT *)buf) {
         if ( ex_brfinais )
            *p = '\0';
         p--;
      }

      if ( ex_hifens && *p == '-' && tam > 3 && isalpha(*(p-1)) ) {
         *p = '\0';
         tam--;
         ret = '-';
         break;
      }

      if ( *p == PARA_CHR )                           ret = FIM_PARAGRAFO;
      else if ( *p == ';' || *p == '.' || *p == ':')  ret = MERECE_PARAGRAFO;
      else                                            ret = MESMO_PARAGRAFO;

      break;
   }

   if ( ret == MESMO_PARAGRAFO && *p != ' ' ) {
      p++;
      *p++ = ' ';
      *p++ = '\0';
   }


   return(ret);

}


F__LOC	 COUNT hdr_inport(sele, buf)
COUNT sele;
TEXT *buf;
{  COUNT ret, i, j, num;
   TEXT aux[TD+1];
   TEXT numero[TE+1], campo[TE+1];

   ret = 0;

   if ( buf[0] == '@' || buf[0] == '!'  ) {
      switch(buf[1]) {
         case 'p' :  /* Ignora marca de paragrafo */
         case 'P' :  /* Ignora marca de paragrafo */
                     ex_para_mark = 1;
                     break;

         case '-' :  /* Suprime hifens finais */
                     ex_hifens = 1;
                     break;

         case 'b' :  /* Suprime brancos finais */
         case 'B' :  /* Suprime brancos finais */
                     ex_brfinais = 1;
                     break;

         case 'a' :  /* Autonumera Campos em branco */
         case 'A' :  imp_autonum = 1;
                     break;

         case 'c' :  /* coluna inicial das informacoes */
         case 'C' :  /* coluna inicial das informacoes */
                     col_inport = atoi(&buf[2]);
                     break;

         case 'm' :  /* mapa de equivalencia */
         case 'M' :  /* mapa de equivalencia */
                     piece(&buf[2], numero, ' ', 1, TE);
                     piece(&buf[2], campo, ' ', 2, TE);
                     num = atoi(numero);
                     num = COTA(1, num, MAX_CAMPOS);
                     vet_ic[num] = ver_campo(sele, campo, &j);
                     break;
      }
      ret = 1;
   }
   else if ( (nreg_inport = atol(buf)) > 0L ) {
      padrao_inport = 'L';  /* padrao linear */
   }
   else {
      for ( i = 0; i < MAX_CAMPOS; i++) {

         vet_ic[i] = -1;

         piece(buf, aux, DELIMITA, (i+1), TE);

         ltrim(aux);
         trim(aux);

         if ( aux[0] == '\0' ) continue;

         vet_ic[i] = ver_campo(sele, aux, &j);

         if ( vet_ic[i] == -1 ) { /* campo nao existe */
            mens_erro(H_IN_HD1, E_IN_HD1, aux);
            ret = -1;
            goto fim;
         }
      }

      ret = 1;

      for ( i = 0; i < MAX_CAMPOS; i++)
         if ( vet_ic[i] != -1 ) {ret = 0; break; }

      if ( ret != 0 ) { /* nenhum campo a importar */
         mens_erro(H_IN_HD2, E_IN_HD2);
         ret = -1;
         goto fim;
      }
   }

fim :

   return(ret);

}


F__LOC   COUNT cospe_piece(origem, work_area, tam_work, pfuncao)
TEXT *origem, *work_area;
COUNT tam_work;
#ifdef MSDOS
   COUNT (*pfuncao)(TEXT *, COUNT, COUNT);
#else
   COUNT (*pfuncao)( );
#endif
{  TEXT c, especial;
   COUNT ret, estado, piece, tam;
   TEXT *p, *dest;

   p = origem;
   piece = 0;
   tam = 0;
   ret = 0;

   estado = 1;

   while ( *p != '\0' && ret == 0 ) {

      if ( estado == 2 || estado == 4 ) {
         if ( (c = *p++) == '\\' ) { c = *p++;  especial = 1; }
         else                        especial = 0;
      }

      switch(estado) {
         case 1 : /* marca inicio do campo */
                  piece++;
                  estado = 2;
                  dest = work_area;
                  tam = 0;
                  break;

         case 2 : /* procurando virgula final */

						if ( c == DELIMITA && ! especial )    estado = 10;

						else if ( c == ASPAS && ! especial )  estado = 4;

						else if ( ++tam < tam_work )          *dest++ = c;

                  break;

         case 4 : /* procura aspas final */

						if ( c == ASPAS && ! especial )   estado = 2;

						else if ( ++tam < tam_work )      *dest++ = c;

						break;

         case 10: /* fim de campo */
						*dest = '\0';
						ret = pfuncao(work_area, tam,  piece);
                  estado = 1;
                  break;
      }
   }

   if ( estado == 4 ) { /* aspas pendente */
      ret = -99;
      goto fim;
   }

   if ( ret == 0 && tam != 0 ) {
		*dest = '\0';
		ret = pfuncao(work_area, tam, piece);
	}

fim :

   return(ret);

}


/* retorna -1 se for necessario invalidar registro ou       */
/*          0 se campo pode ser setado, mesmo que em branco */
F__LOC   COUNT seta_campo(buf, tam_buf, piece)
TEXT *buf;
COUNT tam_buf;
COUNT piece;
{  COUNT ret, param;
   Campo *pc;
   TEXT aux[160], *masc;

   if ( tam_buf == 0 ) {
      ret = 0;
      goto fim;
   }

   if ( padrao_inport == 'D' ) {

      pc = vet_cmp[vet_ic[piece-1]];

      if ( vet_ic[piece-1] < 0 || pc->cp_tipo == 'F' ) {
         ret = 0;
         goto fim;
      }
   }
   else {
      /* piece vai conter o proprio indice de vetcampos */
      pc = vet_cmp[piece];
   }

   if ( pc->cp_tipo == 'T' ) { /* so e chamada para padrao DBASE */
      if ( buf[0] != '@' ) {
         sprintf(aux, C_IN_LICA, linha_loc, piece, E_IN_006);
         wr_log(1, aux);
         ret = -1;
      }
      else  {
         ret = inp_arq(buf, pc);

         if ( ret == -2 ) {
            sprintf(aux, C_IN_LICA, linha_loc, piece, E_IN_007);
            wr_log(1, aux);
            ret = -1;
         }
      }
   }
   else {

      trim(buf);

      tam_buf = strlen(buf);

      if ( tam_buf > pc->cp_mhor ) {
         sprintf(aux, C_IN_LICA, linha_loc, piece, E_IN_002);
         wr_log(1, aux);
         buf[pc->cp_mhor] = '\0';
         tam_buf = pc->cp_mhor;
      }

      if ( pc->cp_tipo == 'N' || pc->cp_tipo == 'F' )
         param = E_VALOR;
      else
         param = E_ALFA;

      if ( pc->cp_tipo == 'F' )
         masc = pc->cp_param.cp_msc_form;
      else
         masc = pc->cp_mascara;

      formata_campo(pc->cp_newdata, buf, masc, param);

      pc->cp_newtam = strlen(pc->cp_newdata);

      if ( pc->cp_tipo == 'N' &&
              (str_to_db(buf, sep_dec) != str_to_db(pc->cp_newdata, sep_dec))) {
         sprintf(aux, C_IN_LICA, linha_loc, piece, E_IN_002);
         wr_log(1, aux);
         ret = -1;
      }
      else {
         set_erro(0);
         ret = critica_campo(pc);
         set_erro(1);
      }
   }

   if ( ret < 0  ) {
      if ( proc_loc == 0 ) { /* Rejeita registro */
         sprintf(aux, C_IN_LICA, linha_loc, piece, E_IN_004);
         strcat(aux, " - ");
         strcat(aux, buf);
         wr_log(1, aux);
         ret = -1;
         goto fim;
      }
      else { /* limpa campo */
         pc->cp_newdata[0] = '\0';
         pc->cp_newtam = 0;
         sprintf(aux, C_IN_LICA, linha_loc, piece, E_IN_005);
         strcat(aux, " - ");
         strcat(aux, buf);
         wr_log(1, aux);
         ret = 0;
         goto fim;
      }
   }
   else {
      pc->cp_mod_flag = 1;
      ret = 0;
   }

fim :

   return(ret);
}



F__LOC	 COUNT inp_arq(nome, pc)
TEXT *nome;
Campo *pc;
{  COUNT ret;
   POINTER pos;
   FILE *fp = NULL;
   TEXT full_name[PATH_MAX+1];

   if ( nome[0] == '@' ) nome++;

   if ( path_inport != NULL ) {
      strcpy(full_name, path_inport);
      strcat(full_name, nome);
      nome = full_name;
   }

   if ( (fp = fopen(nome,"r")) == NULL ) {
      ret = -2;
      goto fim;
   }


   if ( pc->cp_adoc != NULL ) {
      ret = -1;
      goto fim;
   }

   memcpy((TEXT *)&pos, (TEXT *)pc->cp_newdata , sizeof(pos));

   if ( pos != (POINTER) 0 ) {
      ret = -1;
      goto fim;
   }

   pc->cp_adoc = load_buffer( pc->cp_mhor, ET_HANDLE, (TEXT *) fileno(fp),
                          (LONG)65000L );

   if ( pc->cp_adoc == NULL ) {
      ret = -1;
      goto fim;
   }
   else
      ret = 0;

fim :

   if ( fp != NULL ) fclose(fp);

   return(ret);
}



F__LOC	 VOID enche_autonum(sele)
COUNT sele;
{
#ifdef AUTO_INCREMENTO

         COUNT auto_fieldnumber;
   		Campo **C_Cmp, *pc;

   		if ( ( auto_fieldnumber = auto_getfield( ) ) != -1 ) {

   			C_Cmp = pos_vetcmp(sele);
            pc = C_Cmp[auto_fieldnumber];
   			/* ver se tem dado antes de incrementar */
   			if ( pc->cp_newdata[0] == 0 ) {
   				auto_incre(pc->cp_newdata, pc->cp_mhor);
   				pc->cp_newtam = strlen(pc->cp_newdata);
   			}
   		}
#endif


}
