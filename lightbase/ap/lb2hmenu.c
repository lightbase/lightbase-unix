#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2prot1.h"

#ifdef    VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif

#include  "lb2locs.h"
#include  "lb2entd.h"

#define   MOR_FILE   10000
#define   MAX_NIV    15
#define   MAX_AUX    250

#ifdef ESPANHA
#   define   TOK_MENU   "MENU "
#   define   TOK_SEL    "SELBASE "
#   define   TOK_PESQ   "BUSQUEDA "
#   define   TOK_INC    "INCLUYE "
#   define   TOK_EXC    "EXCLUYE "
#   define   TOK_CLEAN  "CLEAN "
#   define   TOK_ALT    "ALTERA "
#   define   TOK_ALTG   "GL_ALTERA "
#   define   TOK_FIM    "FIN "
#else
#   define   TOK_MENU   "MENU "
#   define   TOK_SEL    "SELBASE "
#   define   TOK_PESQ   "PESQUISA "
#   define   TOK_INC    "INCLUI "
#   define   TOK_EXC    "EXCLUI "
#   define   TOK_CLEAN  "CLEAN "
#   define   TOK_ALT    "ALTERA "
#   define   TOK_ALTG   "GL_ALTERA "
#   define   TOK_FIM    "FIM "
#endif

#define   H_SEMBASE  "SEMBASE"
#define   H_SEMPERMI  "SEMPERMI"
#define   H_SEMPERME  "SEMPERME"

#define   E_SEMBASE  loc_str(STR_GLB, 584)
#define   E_ARQMENU  loc_str(STR_GLB, 585)
#define   E_SEMPERMI loc_str(STR_GLB, 586)
#define   E_SEMPERME loc_str(STR_GLB, 587)

#define   C_MENU     1
#define   C_SEL      2
#define   C_PESQ     3
#define   C_INC      4
#define   C_EXC      5
#define   C_CLEAN    6
#define   C_ALT      7
#define   C_ALTG     8
#define   C_FIM      9


static TEXT aux_h[PATH_MAX + 10];
static TEXT aux_2[MAX_AUX+2];
static COUNT niv_hmenu;
static TEXT **hm_comandos[MAX_NIV+2];
static TEXT *p_parm;   /* Ponteiro para parametro de linha de comando */

#ifdef MSDOS
   COUNT hmenu(TEXT *);
   COUNT exec_hmenu(Selecao *);
   VOID cor_menu(SS, SS*, SS*, SS*, SS*);
   COUNT hm_selbase(TEXT *);
   VOID hm_increg(TEXT *);
   VOID hm_excreg(TEXT *);
   VOID hm_clean(TEXT *);
   VOID hm_mod(TEXT *);
   VOID hm_modg(TEXT *);
   VOID hm_pesquisa(TEXT *);
   COUNT aval_parm(TEXT *);
   VOID fname(TEXT *, TEXT *);
   LONG descobre_nreg( TEXT *);
   COUNT auto_getfield( VOID );
   COUNT    exec_iae( COUNT, COUNT, LONG, WIN*);
#else
   COUNT hmenu( );
   COUNT exec_hmenu( );
   VOID cor_menu( );
   COUNT hm_selbase( );
   VOID hm_increg( );
   VOID hm_excreg( );
   VOID hm_clean( );
   VOID hm_mod( );
   VOID hm_modg( );
   VOID hm_pesquisa( );
   COUNT aval_parm( );
   VOID fname( );
   LONG descobre_nreg( );
   COUNT auto_getfield( );
   COUNT    exec_iae( );
#endif

#ifndef RUNTIME
   extern TEXT auto_last[];
   extern COUNT ap_flag_auto;
#endif


F__GLB   COUNT hmenu(arquivo)
TEXT *arquivo;
{  COUNT ret=0, conta, i, x;
   LONG tam;
   TEXT *area, *p;
   TEXT *rod, *com, *str_menu, *status;
   TEXT **vet;
   FILE *fp;
	Selecao m0;

   fp   = NULL;
   area = NULL;
   p    = NULL;
   vet  = NULL;
   str_menu = NULL;
   conta = 0;

   up_str(GRP_ENTD);

   niv_hmenu++;
   hm_comandos[niv_hmenu] = NULL;

   if ( niv_hmenu >= MAX_NIV ) {
      ret = -9;
      goto fim;
   }


   fname(arquivo, aux_h);



   strip_nl(aux_h);

   fp = fopen(aux_h, "r");

   if ( fp == NULL ) {
      if ( niv_hmenu == 1 )
         mens_erro("NOTMENU", E_ARQMENU, aux_h);
      ret = -1;
      goto fim;
   }

   /* verifica tamanho do arquivo */
   fseek(fp, 0L, 2);
   tam = ftell(fp);
   fseek(fp, 0L, 0);

   if ( tam > MOR_FILE ) {
      ret = -2;
      goto fim;
   }

   if ( (area = calloc(1, (COUNT)tam+100)) == NULL ) {
      ret = -3;
      goto fim;
   }


   /* conta numero de linhas do menu */
   for( conta = 0; fgets(area, 250, fp) != NULL; ) {
      if ( p[0] == '#' ) continue;
         conta++;
   }

   /* Volta para inicio do arquivo */
   fseek(fp, 0L, 0);

   vet  = (TEXT **) calloc((conta + 3) * 3, sizeof(TEXT *));

   hm_comandos[niv_hmenu] = (TEXT **) calloc((conta+3), sizeof(TEXT *));


   if ( vet == NULL ) {
      ret = -3;
      goto fim;
   }

   /* Monta menu */
   p = area;
   x = strlen(TOK_MENU);
   for( conta = 0; fgets(p, 250, fp) != NULL; ) {

      if ( p[0] == '#' ) continue;

      strip_nl(p);
      ltrim(p);
      tam = strlen(p) + 1;

      if ( strncmp(TOK_MENU, p, x) == 0 ) {
         aval_parm(p); /* so seta p_parm */
         str_menu = p_parm;
      }
      else {
         hm_comandos[niv_hmenu][conta] = NULL;
         status = "0"; /* indica de opcao NAO vai estar ativa */
         if ( (rod = strchr(p, '|')) != NULL ) {
            *rod++ = '\0';  /* finaliza opcao e indica inicio do radape */
            ltrim(rod);
            if ( (com = strchr(rod, '|')) != NULL ) {
               *com++ = '\0'; /* finaliza rodape e indica inicio do comando */
               ltrim(com);
               hm_comandos[niv_hmenu][conta] = com;
               status = "1"; /* Existe comando opcao vai estar ativa */
               for ( i = 0; piece(com, aux_2, ';', i + 1, MAX_AUX) > 0; i++) {
                  if ( aval_parm(aux_2) == C_MENU ) {
                     fname(p_parm, aux_h);
                     if ( access(aux_h, 0) != 0 ) {
                        status = "0";
                        break;
                     }
                  }
               }
            }
         }
         else {
            rod = str_nada;
         }

	      vet[OP(conta)] = p;
	      vet[AT(conta)] = status;
	      vet[AJ(conta)] = rod;
         conta++;
      }

      p += (COUNT)tam;
   }

   for ( i = conta - 1; i > 0 ; i-- ) {
	   if ( vet[OP(i)][0] != '\0' )
         break;
      else
         vet[OP(i)] = NULL;
   }

   fclose(fp);

   if ( str_menu != NULL ) {
      SI li, ci, larg, alt, cols, cor, borda;
      SS l_atr, t_atr, b_atr, d_atr;
      TEXT *top, *bot;

      sscanf(str_menu, "%d %d %d %d %d %d %d",
                       &li, &ci, &larg, &alt, &cols, &cor, &borda);

      li   = COTA(2, li, size_y - 4);
      ci   = COTA(1, ci, size_x - 2);
      larg = COTA(5, larg, size_x - 4);
      alt  = COTA(2, alt, size_y - 6);
      cols = COTA(1, cols, 20);
      cor  = COTA(0, cor, 6);
      borda= COTA(1, borda, 2);


      if ( (top = strchr(str_menu, '|')) != NULL ) {
         *top++ = '\0';  /* finaliza menu e indica inicio do cabecalho */
         ltrim(top);
         if ( (bot = strchr(top, '|')) != NULL ) {
            *bot++ = '\0';  /* finaliza cabecalho e indica inicio do rodape */
         }
         else
            bot = str_nada;
         ltrim(bot);
      }
      else
         top = bot = str_nada;

      cor_menu(cor, &l_atr, &t_atr, &b_atr, &d_atr);

      def_menu( &m0, ci, li, larg, alt, cols, l_atr, t_atr,
            borda, b_atr, d_atr,
            (TEXT *(*)[]) vet, top, bot);

      m0.executa = exec_hmenu;

   	ret = menu( &m0 );          /* Abertura do menu */
   }

fim :

   if ( hm_comandos[niv_hmenu] != NULL )
      free(hm_comandos[niv_hmenu]);

   if ( vet != NULL )  free(vet);
   if ( area != NULL ) free(area);

   niv_hmenu--;

   down_str(GRP_ENTD);

   return(ret);
}



F__LOC   COUNT exec_hmenu(s)
Selecao *s;
{  COUNT ret, i;
   TEXT *com;

   /* se a ultima tecla pressionada foi ESC, retorna -1 para fechar */
   /* menu principal */

   if ( s->tecla == K_ESC ) {
      ret = -1;
      goto fim;
   }

   ret = s->escolhido;

   com = hm_comandos[niv_hmenu][s->escolhido];

   if ( com != NULL ) {
      for ( i = 0; piece(com, aux_2, ';', i + 1, MAX_AUX) > 0; i++) {
         switch ( aval_parm(aux_2) ) {
            case C_MENU :  /*  */
                           fname(p_parm, aux_h);
                           hmenu(aux_h);
                           break;

            case C_SEL  :  /*  */
                           if ( hm_selbase(p_parm) < 0 ) {
                              i = 999;
                              break;
                           }
                           break;

            case C_PESQ :  /*  */
                           hm_pesquisa(p_parm);
                           break;

#ifndef RUNTIME
            case C_INC  :  /*  */
                           hm_increg(p_parm);
                           break;

            case C_EXC  :  /*  */
                           hm_excreg(p_parm);
                           break;

            case C_CLEAN :  /*  */
                           hm_clean(p_parm);
                           break;

            case C_ALT  :  /*  */
                           hm_mod(p_parm);
                           break;
            case C_ALTG  :  /*  */
                           hm_modg(p_parm);
                           break;
#endif

            case C_FIM  :  /*  */
                           ret = -1;
                           break;

            default     :  /* Comando nao implementado */
                           break;
         }
      }
   }


fim :

   return(ret);
}


F__LOC VOID cor_menu(set, l_atr, t_atr, b_atr, d_atr)
SS set;
SS *l_atr,   /* atributo para listar os elementos                */
   *t_atr,   /* atributo da tarja de selecao.                    */
   *b_atr,   /* atributo da borda.                               */
   *d_atr;   /* atributo destaque das opcoes                     */
{
   switch ( set ) {

      default :
      case 0  :   *l_atr = MP_CAMPO;
                  *t_atr = MP_BARRA;
                  *b_atr = MP_JANELA;
                  *d_atr = MP_DESTAQUE;
                  break;

      case 1  :   *l_atr = JE_CAMPO;
                  *t_atr = MP_BARRA;
                  *b_atr = JS_JANELA;
                  *d_atr = JE_DESTAQUE;
                  break;


      case 2  :   *l_atr = JP_CAMPO;
                  *t_atr = JP_BARRA;
                  *b_atr = JP_JANELA;
                  *d_atr = JP_DESTAQUE;
                  break;

      case 3   :  *l_atr = MS_JANELA;
                  *t_atr = MS_BARRA;
                  *b_atr = MS_JANELA;
                  *d_atr = MS_DESTAQUE;
                  break;


      case 4  :   *l_atr = JS_CAMPO;
                  *t_atr = JS_JANELA;
                  *b_atr = JS_BARRA;
                  *d_atr = JS_DESTAQUE;
                  break;


      case 5  :   *l_atr = JE_CAMPO;
                  *t_atr = JE_JANELA;
                  *b_atr = JE_BARRA;
                  *d_atr = JE_DESTAQUE;
                  break;

   }
}
F__LOC COUNT hm_selbase(str)
TEXT *str;
{  COUNT ret = 0;
   TEXT *d;
   TEXT base[30];


   d = base;

   while ( *str != ' ' && *str )
      *d++ = *str++;

   *d = '\0';

   if ( v_bases[BASE_EDIT] == NULL ||  stricmp(v_bases[BASE_EDIT]->bs_nome, base) != 0 )
      ret = sel_base(BASE_EDIT, base, str_nada);

fim :

   return(ret);
}


F__LOC VOID hm_pesquisa(str)
TEXT *str;
{
   if ( v_bases[BASE_EDIT] != NULL ) {

      if ( strcmp(str, "") == 0 ) {
         pesquisa(BASE_EDIT);
      }
      else {
         pre_pesquisa(BASE_EDIT, str);
      }
   }
   else {
      mens_erro(H_SEMBASE, E_SEMBASE);
   }

}

#ifndef RUNTIME
F__LOC VOID hm_increg(str)
TEXT *str;
{
   str = str;

   if ( v_bases[BASE_EDIT] == NULL ) {
      mens_erro(H_SEMBASE, E_SEMBASE);
   }
   else {
   	if ( ap_acesso_ativo.da_inclui   == 'X' || ap_status == 'A')
   		ent_dados(BASE_EDIT, INCLUSAO);
   	else
         mens_erro(H_SEMPERMI, E_SEMPERMI);

   }
}

F__LOC VOID hm_clean(str)
TEXT *str;
{
   str = str;

   auto_last[0] = '\0';

}

F__LOC VOID hm_mod(str)
TEXT *str;
{
   str = str;

   if ( v_bases[BASE_EDIT] == NULL ) {
      mens_erro(H_SEMBASE, E_SEMBASE);
   }
   else {
   	if ( ap_acesso_ativo.da_altera == 'X' || ap_status == 'A') {
         ent_dados(BASE_EDIT, ALTERACAO);
      }
   	else
         mens_erro(H_SEMPERME, E_SEMPERME);
   }
}

F__LOC VOID hm_modg(str)
TEXT *str;
{
   str = str;

   if ( v_bases[BASE_EDIT] == NULL ) {
      mens_erro(H_SEMBASE, E_SEMBASE);
   }
   else {
   	if ( ap_acesso_ativo.da_altera == 'X' || ap_status == 'A') {
         if ( ap_flag_auto && *auto_last != '\0' ) {
            LONG nreg;
            nreg = descobre_nreg( auto_last );
            if ( nreg > 0 ) {
               WIN *win_ent;
               if ( ( win_ent = abre_janela(J_REGDISP)) != NULL ) {
                  exec_iae(BASE_EDIT, ALTERACAO, nreg, win_ent);
                  dn_textos(BASE_EDIT, 0);
                  fwkill( win_ent );
               }
            }
         }
      }
   	else
         mens_erro(H_SEMPERME, E_SEMPERME);
   }
}


F__LOC VOID hm_excreg(str)
TEXT *str;
{
   str = str;

   if ( v_bases[BASE_EDIT] == NULL ) {
      mens_erro(H_SEMBASE, E_SEMBASE);
   }
   else {
   	if ( ap_acesso_ativo.da_exclui  == 'X' || ap_status == 'A' )
   		ent_dados(BASE_EDIT, EXCLUSAO);
   	else
         mens_erro(H_SEMPERME, E_SEMPERME);
   }
}


#endif

F__LOC COUNT aval_parm(str)
TEXT *str;
{  COUNT ret;

   p_parm = str_nada;

   while ( *str == ' ' && *str  )
      str++;

   if ( strncmp(TOK_MENU, str, strlen(TOK_MENU) -1 ) == 0 ) {
      ret = C_MENU;
   }
   else if ( strncmp(TOK_SEL, str, strlen(TOK_SEL) -1 ) == 0 ) {
      ret = C_SEL;
   }
   else if ( strncmp(TOK_PESQ, str, strlen(TOK_PESQ) -1 ) == 0 ) {
      ret = C_PESQ;
   }
   else if ( strncmp(TOK_INC, str, strlen(TOK_INC) -1 ) == 0 ) {
      ret = C_INC;
   }
   else if ( strncmp(TOK_EXC, str, strlen(TOK_EXC) -1 ) == 0 ) {
      ret = C_EXC;
   }
   else if ( strncmp(TOK_CLEAN, str, strlen(TOK_CLEAN) -1 ) == 0 ) {
      ret = C_CLEAN;
   }
   else if ( strncmp(TOK_ALT, str, strlen(TOK_ALT) -1 ) == 0 ) {
      ret = C_ALT;
   }
   else if ( strncmp(TOK_ALTG, str, strlen(TOK_ALTG) -1 ) == 0 ) {
      ret = C_ALTG;
   }
   else if ( strncmp(TOK_FIM, str, strlen(TOK_FIM) -1 ) == 0 ) {
      ret = C_FIM;
   }
   else {
      ret = 999;
   }


   if ( ret != 999 ) {

      if ( (p_parm = strchr(str, ' ')) != NULL) {
         while ( *p_parm == ' ' && *p_parm )
            p_parm++;
      }

      else
         p_parm = str_nada;
   }

   return(ret);
}



F__LOC VOID fname(nome_arq, area)
TEXT *nome_arq, *area;
{  TEXT *p, *path;
   TEXT arquivo[PATH_MAX + 10];

   strcpy(arquivo, nome_arq);

   for ( p = arquivo; *p == ' ' && *p ; p++)
      ;

   path = ap_dirbase;

   switch(*p) {
      case '.' :
      case '\\' :
      case '/'  : path = str_nada;
                  break;
   }

#ifdef MSDOS
   if ( *(p+1) == ':' )
      path = str_nada;
#endif

   info_base(p, path, str_nada, area);
}



#ifndef RUNTIME
F__LOC LONG descobre_nreg(str)
TEXT *str;
{  COUNT i;
   LONG reg;
   Campo **vet_cmp, *pc;

   reg = 0L;

   vet_cmp  = pos_vetcmp(BASE_EDIT);
   i = auto_getfield();

   if ( vet_cmp[i]->cp_chave_unica == 'S' ) {
      pc = vet_cmp[i];
      touch_unica(pc, &reg, str, '<');
   }

   return( reg );
}
#endif
