#ifdef MSDOS
#ifndef DISCADOR
   extern unsigned _stklen = 8000;
#endif
#endif


#ifdef PROT_INFOCON
#   include	<is_pers.h>
#endif

#include  "lb2all.h"


#include  <ctype.h>
#include  <string.h>
#include  <ctype.h>

#ifdef    VAXVMS
#include  <types.h>
#include  <stat.h>
#else
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

#include  HD_DIR
#include  HD_IO

#ifdef UNIX /* colocado para DECALFA */
#include <unistd.h>
#endif

#define MODO_DEFINICAO SIM

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2head.h"
#include  "lb2locs.h"    /* declarar variavel */
#include  "lb2maces.h"
#include  "lb2perso.h"
#include  "lb2prot1.h"

#ifdef VISUAL
#	ifdef    VAXVMS
#		include  "vlb.h"
#	else
#		include  "../mt/vlb.h"
#	endif
#endif




#define   LB_VERSAO "2.4"
#define   LB_EDICAO "2.0"


#define   H_NOIDI   "NOIDIOMA"    /* arquivo de idiomas inexiste */
#define   H_INIMA   "INICMTDA"    /* inicializacao do metodo de acesso */
#define   H_ARQMS   "ARQMENS"     /* arquivo de mensagens corrompido */

#define   C_VERSAO     loc_str(STR_GLB, 531)
#define   C_RELEASE    loc_str(STR_GLB, 532)
#define   C_SERIE      loc_str(STR_GLB, 533)
#define   C_USUARIOS   loc_str(STR_GLB, 534)


#define   E_MASTAR   loc_str(STR_GLB, 540)
/*************************************541*/
#define   E_MT1STAR  loc_str(STR_GLB, 542)
#define   E_MT2STAR  loc_str(STR_GLB, 543)
#define   E_NO_ID    loc_str(STR_GLB, 544)
#define   E_CH_ID    loc_str(STR_GLB, 545)

#ifdef MSDOS
   COUNT encerra( VOID );
   VOID seta_delimitadores(VOID);
   VOID inic_p0(COUNT, TEXT **, TEXT **);
   VOID inic_p1( VOID );
   VOID inic_p2( VOID );
   VOID inic_p4( COUNT *, COUNT *, LONG *);
   VOID inic_p3( TEXT * );
   VOID rever_args(COUNT, TEXT **);
   COUNT ld_textos(TEXT *idioma, TEXT *fname);
   COUNT str_fname(TEXT *, COUNT);
   COUNT load_soundex(VOID);
#  ifdef RUNTIME
   COUNT    deleta_base( VOID );
#  endif
#else
   COUNT encerra( );
   VOID seta_delimitadores( );
   VOID inic_p0( );
   VOID inic_p1( );
   VOID inic_p2( );
   VOID inic_p4( );
   VOID inic_p3( );
   VOID rever_args( );
   COUNT ld_textos( );
   COUNT str_fname( );
   COUNT load_soundex( );
#  ifdef RUNTIME
   COUNT    deleta_base( );
#  endif
#endif


   TEXT *ap_mg = "\nCod.Er. #%d %s\n";


#ifdef ESPANHA
#ifdef LITE
   TEXT *ap_cr_cliente = "S.S.I. -  Demonstracion";
#else
   TEXT *ap_cr_cliente = "S.S.I.";
#endif
   TEXT *ap_m6 = "Light Base - Copyright 1991 - Light Software";
  TEXT *ap_m6a = "             Copyright 1996 - Light-Infocon S/A";
   TEXT *ap_m7 = "LB [-Parametro]";
   TEXT *ap_m8 = "   -AI Inicializacion del fichero de accesos";
   TEXT *ap_m9 = "   -Pnomearq  Realiza procedimiento padron";
   TEXT *ap_m10= "   -Hmenu     Realiza menu especificado";
#  ifdef RUNTIME
   TEXT *ap_m11= "   -Au Manutencao do cadastro de usuarios (Run-time)";
   TEXT *ap_m12= "   -Ab Inclusao da Bases no cadastro de acessos (Run-time)";
   TEXT *ap_m13= "   -Ae Exclui Base do cadastro de acessos (Run-time)";
#  else
   TEXT *ap_m11= "";
   TEXT *ap_m12= "";
   TEXT *ap_m13= "";
#  endif
   TEXT *ap_mp = "\nProtecion : %d\n";
   TEXT *ap_md = "\nDemonstracion fuera del plazo previsto\n";
   TEXT *ap_msg = "\nFichero de mensagens desatualizado.\n";

   TEXT ap_ = "";
   TEXT ap_ = "";
   TEXT ap_ = "";
   TEXT ap_ = "";

#else
#ifdef LITE
   TEXT *ap_cr_cliente = "Light-Infocon S/A";
#else
   TEXT *ap_cr_cliente = "Light-Infocon S/A - Demonstracao ";
#endif
   TEXT *ap_m6 = "Light Base - Copyright 1991 - Light Software  ";
  TEXT *ap_m6a = "             Copyright 1996 - Light-Infocon S/A";
   TEXT *ap_m7 = "LB [-Parametro]                              ";
   TEXT *ap_m8 = "   -Ai Inicializacao de cadastro de acessos  ";
   TEXT *ap_m9 = "   -Pnomearq  Executa procedimento padrao    ";
   TEXT *ap_m10= "   -Hmenu     Executa menu especificado      ";
#  ifdef RUNTIME
   TEXT *ap_m11= "   -Au Manutencao do cadastro de usuarios (Run-time)";
   TEXT *ap_m12= "   -Ab Inclusao da Bases no cadastro de acessos (Run-time)";
   TEXT *ap_m13= "   -Ae Exclui Base do cadastro de acessos (Run-time)";
#  else
   TEXT *ap_m11= "";
   TEXT *ap_m12= "";
   TEXT *ap_m13= "";
#  endif
   TEXT *ap_mp = "\nProtecao : %d      \n";
   TEXT *ap_md = "\nDemonstracao fora do prazo previsto\n";
   TEXT *ap_msg = "\nArquivo de mensagens desatualizado.\n";

   TEXT *ap_emg1 = "Nao foi possivel setar variavel de ambiente.";
   TEXT *ap_emg2 = "Altere a diretiva shell em config.sys incluindo a opcao /e";
   TEXT *ap_emg3 = "  ex : shell = c:\\command.com /p /e:1024";
   TEXT *ap_emg4 = "Nao foi possivel abrir arquivo de mensagens.";
   TEXT *ap_emg5 = "Verifique a existencia do arquivo :";
   TEXT *ap_emg6 = "Nao foi possivel localizar arquivo : ";
   TEXT *ap_emg7 = "Verifique o diretorio de instalacao do Light Base";
   TEXT *ap_emg8 = "Use a variavel ambiental LBHOME para especificar o diretorio onde o";
   TEXT *ap_emg9 = "Light Base esta instalado. (ex.: set LBHOME=c:\\LIGHT18)";
   TEXT *ap_emg10= "Arquivo de configuracao Invalido";
   TEXT *ap_emg11= "  Verifigue arquivo ";
   TEXT *ap_emg12= "Sem memoria para inicializar sistema de janelas";
   TEXT *ap_emg13= "Arquivo de descricao de terminais invalido";
   TEXT *ap_emg14= "Erro na inicializacao da memoria virtual";
   TEXT *ap_emg15= "Arquivo de mensagens invalido";
#endif

#ifdef MSDOS
   TEXT *ap_dir  = "c:\\light             ";
#else
   TEXT *ap_dir  = "/usr/lib/light        ";
#endif


#define   MAX_IDIOMAS     4

TEXT     F3_1Stat[30];
TEXT     F3_2Stat[30];
TEXT     Versao_usr[TE + 1];
TEXT     curr_idioma[TE + 1];
TEXT     ap_term[TE+1];
TEXT     ap_cliente[80];   /* Nome do cliente */
COUNT    ap_rb_acesso;
#ifdef RUNTIME
COUNT    ap_mexe_cad;
#endif
COUNT    ap_placavideo;
TEXT     lbsep_int;
TEXT     lbsep_dec;
TEXT     *ap_hmenu;
COUNT dias_restantes;
TEXT  data_limite[12];

COUNT sai_stat = 0;
COUNT ap_ctbuffers = 0;

extern COUNT ct_maxuser, ct_ignoraupd, ct_updwarning, ct_semseguranca;
#ifndef RUNTIME
   extern TEXT auto_last[];
#endif


F__GLB   main(argc, argv)
int argc;
char *argv[];
{  TEXT *nome_proc;
#ifdef PROT_INFOCON
   COUNT cod;
#endif
#ifndef MSDOS
	TEXT *tty_io;

	tty_io = malloc(1024);
	setvbuf(stdout, tty_io, _IOFBF,1024);
#endif

   /* So para testar signed char */
   { char c = -1;
     if ( c != -1) { printf("default compiler is unsigned. Recompile\n"); exit(1); }
   }

   umask(0);

#ifdef PROT_INFOCON
	if( pe_teclou_x( argc, argv ) ) {
		pe_x();
		exit( 0 );
	}
/***	Testando e apresentando a personalizacao
***/
   ap_aprovado = C_DEMO;
	if ( ( cod = pe_map() ) != PE_OK) {
	   switch(cod) {
	    case 2 :
		/*
		fprintf( stderr, "Problema na personalizacao (Cod. 2)\n");
		exit( 1 );
		*/
		break;

	    case 4 :
		fprintf( stderr, "Data limite de utilizacao atingida\n");
		exit( 1 );
	    default :
		fprintf( stderr, "Instalacao. Codigo de erro %d\n", cod);
		exit( 1 );
	   }
	}
	else {
           ap_aprovado = C_OK;
	}
#endif



   seta_delimitadores();  /* para funcao de indexacao */

   inic_p0(argc, argv, &nome_proc);  /* Interpretacao de parametros */

   inic_p1();  /* diretorio home */

   inic_p2();  /* configuracao do LB */


   if ( getenv("LBTERM") == NULL ) {
      TEXT aux[80], *p;
      sprintf(aux, "LBTERM=%s", ap_term);
      trim(aux);
      p = malloc(strlen(aux)+1);
      strcpy(p, aux);
      if ( putenv(p) != 0 ) {
         /* nao foi possivel setar variavel de ambiente */
         fprintf(stderr, "%s\n", ap_emg1);
         fprintf(stderr, "%s\n", ap_emg2);
         fprintf(stderr, "%s\n", ap_emg3);
         exit(1);
      }
   }

   /* monta nome de arquivo de mensagens conforme idioma */
   /* seta tmp_fname que sera' utilizado em inic_p3 */
   if ( str_fname(ap_tabidioma[0], 1) < 0 ) {
      exit(5); /* Erro na carga do arquivo de mensagens */
   }

   inic_p3(nome_proc);  /***    Inicializacao do sistema de janelas ***/

#ifdef DISCADOR
   cli_init();  /* inicializa servidor de discagem para Bradesco */
#endif


   up_str(STR_GLB);

   inic_p4(&ap_max_users, &dias_restantes, &ap_nserie);  /*** verificacao de validade da copia */

   /* Atualiza display de Edicao/maxusers */
   strcpy(F3_1Stat, C_VERSAO);
   strcat(F3_1Stat, LB_VERSAO);
   strcat(F3_1Stat, C_RELEASE);
   strcat(F3_1Stat, LB_EDICAO);

   {  TEXT aux[10];
   	strcpy(F3_2Stat, C_SERIE);
	   sprintf(aux, "%6.6ld", ap_nserie);
	   strcat(F3_2Stat, aux);
	   strcat(F3_2Stat, C_USUARIOS);
	   sprintf(aux, "%d", ap_max_users);
	   strcat(F3_2Stat, aux);
   }


   versao = F3_1Stat;
   edicao = F3_2Stat;
   util_flag = U_CALENDARIO | U_CALCULA | U_STATUS | U_GRAVA | U_EXECUTA | U_REESCREVE;

   up_str(19);  /* GRP_TABS */

   piece(OP_SIM_NAO, OP_SIM, '@', 1, 9); trim(OP_SIM); ltrim(OP_SIM);
   piece(OP_SIM_NAO, OP_NAO, '@', 2, 9); trim(OP_NAO); ltrim(OP_NAO);
   strcpy(op_sn, OP_SIM);
   strcat(op_sn, "@");
   strcat(op_sn, OP_NAO);

   ap_vetprinter[ap_maxprinter++] = M_OUT_FILE;

	/*
   **    Inicializacao buffers de tratamento de texto
   */

   if ( (buf_iotxt = malloc(ap_sizeio)) == NULL ) {
		sai_stat = 12;
      goto fim_main;
   }

   txt_start();



	/*
   **    Inicializacao do Metodo de acesso
   */
   ct_maxuser  = ap_max_users;

   if ( ap_ctbuffers <= 0 )
      ap_ctbuffers = MA_NBUFERS;

	if ( INTREE(ap_ctbuffers, NUM_FILES, 8) ) {
		sai_stat = 7;
      mens_erro(H_INIMA, E_MASTAR);
      goto fim_main;
	}
   else {
      ace_dnum = 0;
      ace_inum = 1;
      tab_dnum = 2;
      tab_inum = 3;
      lb4_dnum = 4;
   }

	/*
   **    Inicializacao do gerenciador de recursos
   */

   INIC_TRANS( );

	fwframe(1, 0, size_y - 3, size_x - 1, SIMPLES, curr_win);

   mvwprintw(curr_win,  0,  0, ap_cliente); /* imprime nome do cliente */

   imp_logo(1); /* imprime logotipo */


   /* so para nao pedir soundex.lb em runtime (1.7***) */
   if ( VERSAO_CORRENTE > 600 && (sai_stat = load_soundex()) != 0 ) {
      goto fim_main;
   }

   if ( (sai_stat = open_acessos(0, ap_nserie, 0, ap_rb_acesso)) != 0 )
      goto fim_main;

   ap_rb_acesso = 0;

#ifdef AINDANAO
   /* copia demo nao pode ter assinatura para gerar bases p/ RUNTIME */
   if ( strcmp(ap_aprovado, C_OK) != 0 ) {
      assinatura[0] = '\0';
   }
#endif

   /* Abre tabelas geniras do Light Base */
   { TEXT n_dat[80], n_idx[80];

        info_base( "tables", ap_home_lb, ".dat", n_dat);
        info_base( "tables", ap_home_lb, ".idx", n_idx);

        tabelas_aberta = 0;
        if ( access(n_idx, 0) == 0 && access(n_dat, 0) == 0 ) {
        	if ( OPNFIL(tab_dnum, n_dat, (PERMANENT | SHARED ), 0L, 0) == NO_ERROR ) {
           	if ( OPNFIL(tab_inum, n_idx, (PERMANENT | SHARED ), 0L, 0) == NO_ERROR ) {
                 tabelas_aberta = 1;
              }
           }
        }
   }


   rever_args(argc, argv);  /* Opcoes especicificas na sessao */

   do {

      /* desliga todas as opcoes de utilitarios exceto status */
      util_flag = U_CALENDARIO | U_CALCULA | U_STATUS | U_GRAVA | U_EXECUTA | U_REESCREVE;


      memset((TEXT *)&ap_acesso_ativo, '\0', sizeof(Acesso));
      ap_nome[0] = '\0';
      ap_local[0] = '\0';
#ifndef RUNTIME
      auto_last[0] = '\0';
#endif
      par_cfg.filemenu[0] = '\0';

      if ( strcmp(ap_aprovado, C_OK) != 0 && ! DEMO_VALIDA) {
         sai_stat = 1000;
         goto fim_main;
      }
#ifdef LITE
      strcpy(ap_nome,   "LIGHT");
      strcpy(ap_grupo,  "");
      strcpy(ap_idioma, ap_tabidioma[0]);
      ap_status = 'A';
      par_cfg.tipo_login = 1; /* nao entra em loop */
#else
		if ( pega_usu() != 0 )
         break;
#endif
      util_flag = -1;

      switch(ap_status ) {
         case 'A' :  edit_help = 1;                            break;
         case 'B' :  edit_help = 0;  util_flag &= (~ U_DOS);   break;
         case 'C' :  edit_help = 0;  util_flag &= (~ U_DOS);   break;
         case 'D' :  continue;
      }

      switch ( str_fname(ap_idioma, 0)) {
         case 1 : /* idioma ja ativo */
                  break;

         case 0 : /* idioma disponivel mas nao ativo */
                  if ( ld_textos(ap_idioma, tmp_fname) < 0 ) {
		               sai_stat = 9 ;
                     mens_erro(H_ARQMS, E_CH_ID);
                     goto fim_main;
                  }
                  else {
                     /* mudou idioma. Recarrega grupo generico */
                     up_str(STR_GLB);
                     up_str(19);  /* GRP_TABS */

                     piece(OP_SIM_NAO, OP_SIM, '@', 1, 9);
                                 trim(OP_SIM); ltrim(OP_SIM);
                     piece(OP_SIM_NAO, OP_NAO, '@', 2, 9);
                                 trim(OP_NAO); ltrim(OP_NAO);
                     strcpy(op_sn, OP_SIM);
                     strcat(op_sn, "@");
                     strcat(op_sn, OP_NAO);

                  }
                  break;

         default: /* situacao de erro. Arquivo inexiste */
                  mens_erro(H_NOIDI, E_NO_ID);
                  break;
      }


      if ( strcmp(loc_str(10, 7), LB_VERSAO) != 0 ||
           strcmp(loc_str(10, 8), LB_EDICAO) != 0 ) {
		   sai_stat = 10 ;
         mens_erro(NULL, ap_msg);
         goto fim_main;
      }



#ifdef RUNTIME
   switch ( ap_mexe_cad ) {
      case  'U' : if ( ap_status == 'A' )
#ifndef LITE
                     cad_usuarios();
#endif
                  break;
      case  'B' : if ( ap_status == 'A' )
                     def_base();
                  break;
      case  'E' : if ( ap_status == 'A' ) {
                     deleta_base( );
                  }
                  break;
   }
#endif


      /* se existir opcao de linha de comando e usuario nao tiver menu */
      if ( ap_hmenu != NULL && par_cfg.filemenu[0] == '\0' ) {
         strncpy(par_cfg.filemenu, ap_hmenu, PATH_MAX);
      }


      if ( par_cfg.filemenu[0] != '\0' ) {
         hmenu(par_cfg.filemenu);
      }
      else {
         sel_base(0, str_nada, M_SELBASE);
		   principal();
      }

      close_base( );

      imp_base(str_nada);

      imp_logo(1);

   } while ( par_cfg.tipo_login == 0 );


fim_main :

	/* ANANIAS @ loop p/ liberar cntl_files */

   down_str(19);  /* GRP_TABS */
   down_str(STR_GLB);

   encerra();

   win_quit();

   return(sai_stat);
}


/*#  ---------------------------------------------------------------------
**#   Funcao...:  seta_delimitadores
**#   Objetivo.:  setar delimitadores variaveis
**/
F__LOC   VOID seta_delimitadores()
{
   prgf_delimita[0] = (UTEXT) PARA_CHR;
   tudo_delimita[1] = (UTEXT) PARA_CHR;

   plv_delimita[1] = ET_NORMAL;
   plv_delimita[2] = ET_SUBLIN;
   plv_delimita[3] = ET_ENFAT;
   plv_delimita[4] = ET_ITAL;

   plv_delimita[5] = ' ';
   plv_delimita[6] = ' ';


   tudo_delimita[2] = ET_NORMAL;
   tudo_delimita[3] = ET_SUBLIN;
   tudo_delimita[4] = ET_ENFAT;
   tudo_delimita[5] = ET_ITAL;

   tudo_delimita[6] = ' ';
   tudo_delimita[7] = ' ';

}






/*#  ---------------------------------------------------------------------
**#   Funcao...:  inic_p0()
**#   Objetivo.:  Interpretacao dos parametros.
**#               Retorna nome de macro se existir
**/
F__LOC   VOID inic_p0(argc, argv, nome_proc)
COUNT argc;
TEXT *argv[];
TEXT **nome_proc;
{  COUNT i, erro, op;

   erro = 0;

   ap_rb_acesso    = 0;
   *nome_proc = NULL;
   ap_hmenu = NULL;
   ct_ignoraupd = 0;

	for( i = 1;   i < argc;   i++ ) {

      if ( argv[i][0] != '-' ) { /* erro */
         erro = 1;
      }

      else if ( toupper(argv[i][1]) ==  'A' ) {

         if ( toupper(argv[i][2]) == 'I' ) {
            ap_rb_acesso = 'I';
         }
         else if ( toupper(argv[i][2]) == 'M' ) {
            ap_rb_acesso = 'M';
         }
#ifdef RUNTIME
         else if ( toupper(argv[i][2]) == 'U' ) {
            ap_mexe_cad = 'U';
         }
         else if ( toupper(argv[i][2]) == 'B' ) {
            ap_mexe_cad = 'B';
         }
         else if ( toupper(argv[i][2]) == 'E' ) {
            ap_mexe_cad = 'E';
         }
#endif
         else {
            erro = 1;
         }
      }

#ifdef VISUAL
      else if ( toupper(argv[i][1]) ==  'V' ) {
         if      ( toupper(argv[i][2]) == 'C' ) { ap_placavideo = CGA; }
         else if ( toupper(argv[i][2]) == 'E' ) { ap_placavideo = EGA; }
         else if ( toupper(argv[i][2]) == 'V' ) { ap_placavideo = VGA; }
         else if ( toupper(argv[i][2]) == 'S' ) { ap_placavideo = SVGA; }
         else ap_placavideo = AUTO;
      }
#endif

      else if ( toupper(argv[i][1]) ==  'P' ) {
         /* seta nome de arquivo para macro  */
         *nome_proc = &argv[i][2];
      }

      else if ( toupper(argv[i][1]) ==  'H' ) {
         /* seta nome de arquivo para macro  */
         ap_hmenu = &argv[i][2];
      }
      else if ( toupper(argv[i][1]) ==  'W' ) {
         /* Nao interrompe utilizacao em caso de erro no OPNFIL do ct */
         ct_ignoraupd = 1;
      }
      else if ( toupper(argv[i][1]) ==  'S' ) {
         /* assume seguranca 0. Nao grava em disco operacoes intermediarias */
         if ( argv[i][2] == '0' )
            ct_semseguranca = 1;
      }
      else if ( toupper(argv[i][1]) ==  'F' ) {
         ap_ctbuffers = atoi(&argv[i][2]);
      }
      else if ( toupper(argv[i][1]) ==  'X' ) {
         /* 0x01 - txt_load reconhece header de 6 bytes para exportar */
         /* 0x02 - Nao indexa nao NAO_INDEXADOS */
         /* 0x04 - processamento especial para campos flags */
         /* 0x08 - Forca tratamento de bases nao criptografadas */
         /* 0x16 - Nao faz consistencia de share ativo */

         par_cfg.flags_especiais = atoi(&argv[i][2]);

      }
      else {

         op = toupper(argv[i][1]);

         switch(op) {
            case 'U' :  /* usuario a utilizar */
            case 'K' :  /* senha do usuario a utilizar */
            case 'B' :  /* restringe a Base a ser utilizada */
            case 'L' :  /* Forma de login */
                        break;
            default  :  erro = 1;
                        break;
         }
      }

      if ( erro ) {
         printf("\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
            ap_m6, ap_m6a, ap_m7, ap_m8, ap_m9, ap_m10, ap_m11, ap_m12, ap_m13);
         exit(10); /* chamada invalida */
      }
   }
}


/*#  ---------------------------------------------------------------------
**#   Funcao...:  rever_args()
**#   Objetivo.:  Interpretacao dos parametros de sobreposicao de defaults.
**/
F__LOC   VOID rever_args(argc, argv)
COUNT argc;
TEXT *argv[];
{  COUNT i;
   TEXT c;

   par_cfg.usuario = NULL;
   par_cfg.senha   = NULL;
   par_cfg.base    = NULL;

   for( i = 1;   i < argc;   i++ ) {

      if ( argv[i][0] != '-' )
         continue;

      switch ( toupper(argv[i][1]) ) {

         case 'U' :  /* usuario a utilizar */
                     par_cfg.usuario = &argv[i][2];
                     break;

         case 'K' :  /* senha do usuario a utilizar */
                     par_cfg.senha = &argv[i][2];
                     break;

         case 'B' :  /* restringe a Base a ser utilizada */
                     par_cfg.base = &argv[i][2];
                     if ( strlen(par_cfg.base) > 8 )
                        par_cfg.base[8] = '\0';
                     break;

         case 'L' :  /* Forma de login */
                     c = argv[i][2];
                     if ( c >= '0'&& c <= '2' )
                        par_cfg.tipo_login = c - '0';
                     break;
      }
   }

   if (par_cfg.usuario == NULL )
      par_cfg.senha = NULL;

   if (par_cfg.senha != NULL )
      par_cfg.tipo_login = 1; /* senao so sai com F3 aBandona */


}



/*#  ---------------------------------------------------------------------
**#   Funcao...:  inic_p1()
**#   Objetivo.:  Inicializa diretorio HOME
**/
F__LOC   VOID inic_p1()
{  TEXT *dir, aux[200], *p;
   COUNT ret = -1;
   TEXT dir_curr[PATH_MAX];
   FILE *fp_ctl = NULL;

   dir = getenv("LBHOME");

   if ( dir == NULL ) {
      sprintf(aux, "LBHOME=%s", ap_dir);
      trim(aux);
      p = malloc(strlen(aux)+1);
      strcpy(p, aux);
      if ( putenv(p) != 0 ) {
         fprintf(stderr, "%s\n", ap_emg1);
         fprintf(stderr, "%s\n", ap_emg2);
         fprintf(stderr, "%s\n", ap_emg3);
         exit(1); /* nao foi possivel setar variavel de ambiente */
      }
   }

   dir = getenv("LBHOME");

   if ( dir != NULL ) {

      if ( strcmp(dir, ".") == 0 ) {
         TEXT *p;
         p = getcwd(dir_curr, PATH_MAX);
         if ( p != NULL )
            dir = dir_curr;
      }

   	strcpy(aux,  dir );
   	strcat(aux,  STR_BARRA );
      strcat(aux, FILE_CONFIG);
      strcat(aux, EXT_CONFIG);

      if ( (fp_ctl = fopen(aux, "r")) == NULL ) {
         ret = -1;
         goto fim;
      }
      else {
         ret = 0;
         fclose(fp_ctl);
      }

   }

fim:

   if ( ret < 0 ) {    /* LBHOME invalido */
      fprintf(stderr, "%s %s\n", ap_emg6, aux);
      fprintf(stderr, "%s\n%s\n%s\n", ap_emg7, ap_emg8, ap_emg9);
      exit(3);
   }
   else {
      strcpy(ap_home_lb,  dir );
      trim(ap_home_lb);
   }
}

/*#  ---------------------------------------------------------------------
**#   Funcao...:  inic_p2()
**#   Objetivo.:  Configuracao Inicial
**/
F__LOC   VOID inic_p2()
{  COUNT i;

   /* inicializa nome do cliente */
   strcpy(ap_cliente, ap_cr_cliente);

   if ( (i = config()) != 0 ) {
      TEXT aux[60], num[10];
      strcpy(aux, FILE_CONFIG);
      strcat(aux, EXT_CONFIG);
      sprintf(num, " [%d]", i);
      strcat(aux, num);

      fprintf(stderr, "%s\n", ap_emg10);
      fprintf(stderr, "%s %s\n", ap_emg11, aux);
      exit(4); /* Erro na carga do arquivo de configuracao */
   }
}

/*#  ---------------------------------------------------------------------
**#   Funcao...:  inic_p4()
**#   Objetivo.:  Verificacao de tipo de protecao da copia
**#   Retorna..:  Numero maximo de usuarios permitidos
**/

F__LOC   VOID inic_p4(maxusers, dias_restantes, num_serie)
COUNT *maxusers;
COUNT *dias_restantes;
LONG  *num_serie;
{  COUNT erro=0;

#ifdef PROT_INFOCON
   int dd,mm,aa;
   TEXT *p;

   dias_restantes = dias_restantes;

   *num_serie = -1L;

   if ( strcmp(ap_aprovado, C_OK) == 0 ) {
      p = pe_serie();

#ifdef RUNTIME
      if ( toupper(p[2]) != 'R' || toupper(p[3]) != 'T' )
         erro = 1;
#else
      if ( toupper(p[2]) == 'R' && toupper(p[3]) == 'T' )
         erro = 1;
#endif

      if ( erro ) {
         debug("Numero de serie invalido para tipo de copia (RunTime)");
   	   encerra();
         win_quit( );
         exit( 6 ); /* Erro de protecao */
      }

      *num_serie = atol(p+6) + 100000L; /* iwschxnnnnn */
      *maxusers = pe_tty();

      if ( *maxusers == 999 )
#ifdef MSDOS
         *maxusers = 1;
#else
         *maxusers = 4;
#endif

      strcpy(ap_cliente, pe_pers());

      if ( pe_data(&aa,&mm,&dd) != NULL )
         ap_aprovado = C_DEMO;
   }
   else {
      *num_serie = 1;
      if ( getenv("LBUSERS") == NULL )
#ifdef MSDOS
	*maxusers = 1;
#else
	*maxusers = 4;
#endif
      else
         *maxusers = atoi(getenv("LBUSERS"));
      ap_aprovado = C_DEMO;
   }
#else
   COUNT ret=0;

   *num_serie = -1L;
   ret = checa_copia( maxusers, dias_restantes, num_serie );

   if ( ret != 0 ) {
   	encerra();
      win_quit( );
      exit( 6 ); /* Erro de protecao */
   }
#endif

   return;
}

/*#  ---------------------------------------------------------------------
**#   Funcao...:  inic_p3()
**#   Objetivo.:  Inicializa sistemas de janelas
**#               Inicializa Macro se existir
**#               Carrega arrquivo de mensagens
**/
F__LOC   VOID inic_p3(nome_proc)
TEXT *nome_proc;
{  COUNT ret;
   TEXT *p;

   env_term = "LBTERM";
   env_dir  = "LBHOME";

#ifdef VISUAL
   if ( getenv("VLB_PATH") == NULL )
      VLBSetPathFonts("\\vlb\\fontes"); /*** Inicializa path dos fontes (*.fnt) ***/
   else
      VLBSetPathFonts(getenv("VLB_PATH")); /*** Inicializa path dos fontes (*.fnt) ***/

   if ( getenv("VLB_IMG") == NULL )
      VLBSetPathImg("\\vlb\\imagens"); /*** Inicializa path das imagens ***/
   else
      VLBSetPathImg(getenv("VLB_IMG")); /*** Inicializa path das imagens ***/


  if( VLBInit(ap_placavideo) == 0 ) {    /*** Inicializa funcoes do ambiente grafico ***/
     printf("Erro de inicalizando ambiente VLB. Encerrando o programa.\n");
     exit(1);
  }
#endif

#ifdef MSDOS
  VM_maxslot  = 3;
  VM_minslot  = 3;

#ifdef MEMO_VIRTUAL_NOVA
  VM_mingroup = 4000;
  VM_coreleft = ....;
#endif
#endif

   ret = win_init(tmp_fname, 0);

   if ( ret != 0 ) {

      switch(ret) {
         case -1 :   /* nao consegue alocar memoria ou */
                     if ( (p = malloc(4000)) == NULL ) {
                        fprintf(stderr, "%s\n", ap_emg12);
                     }
                     else {
                        free(p);
                        /* arquivo de descricao de terminal invalido */
                        fprintf(stderr, "%s : %s%c%s\n", ap_emg13,
                                    getenv("LBHOME"), BARRA, getenv("LBTERM"));
                     }
                     break;
         case -4 :
         case -2 :   /* inicializacao de memoria virtual */
                     fprintf(stderr, "%s\n", ap_emg14);
                     break;
         case -3 :   /* arquivo de mensagens invalido */
                     fprintf(stderr, "%s : [%s]\n", ap_emg15, tmp_fname);
                     break;
      }

		exit(7); /* Inicializacao de win_init */
   }
   else {

      if ( strcmp(loc_str(10, 7), LB_VERSAO) != 0 ||
           strcmp(loc_str(10, 8), LB_EDICAO) != 0 ) {
         mens_erro(NULL, ap_msg);
		   win_quit();
		   exit(9);
      }

      sep_int = lbsep_int;
      sep_dec = lbsep_dec;

      curr_win = curr_win;

      setquit( encerra );
#ifndef RUNTIME
      setconfig( config_lb );
#endif
      setmove( move_janela );
      if ( nome_proc != NULL ) /* seta nome de procedimento padrao */
         mr_inic( nome_proc );

	   curr_win->w_atrib  = JP_JANELA;
	   curr_win->w_optatr = JP_BARRA;
	   curr_win->w_defatr = JP_DESTAQUE;
	   curr_win->w_boxatr = JP_JANELA;
		curr_win->t_atr    = JP_JANELA;
		curr_win->campoatr = JP_CAMPO;
      fwcls(curr_win);

      /* limpa linha de mensagem, auxiliar e linha de funcoes */
	   curr_win->w_atrib  = LM_CAMPO;
      fwat(0, size_y - 2, curr_win);
      fwcle(curr_win);

	   curr_win->w_atrib  = JP_JANELA;



   }
}







F__LOC   COUNT str_fname(full_idioma, mens)
TEXT *full_idioma;
COUNT mens; /* indica se deve imprimir mensagem */
{  COUNT ret;
   static TEXT idioma[10];

   if ( strncmp(idioma, full_idioma, 8) == 0 )
      ret = 1;
   else {
      strncpy(idioma, full_idioma, 8);
      idioma[8] = '\0';
      if ( info_base(idioma, ap_home_lb, EXT_MENS, tmp_fname) <= 0 ) {
         if ( mens ) {
            fprintf(stderr, "%s\n", ap_emg4);
            fprintf(stderr, "%s %s\n", ap_emg5, tmp_fname);
         }
         ret = -1;
      }
      else
         ret = 0;
   }

   return(ret);
}



F__LOC   COUNT ld_textos(idioma, fname)
TEXT *idioma, *fname;
{  COUNT ret;


   if ( start_str(fname) ) {
      strcpy(curr_idioma, idioma);
      ret = 0;
   }
   else {
      curr_idioma[0] = '\0';
      ret = -1;
   }

   return(ret);
}


