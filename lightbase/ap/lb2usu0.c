#include  "lb2all.h"

#include  <time.h>
#include  <string.h>

#ifdef UNIX
#   include  <pwd.h>
#endif


#include  HD_MEMO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2locs.h"
#include  "lb2prot1.h"



#define   GRP_IDENT    12

#define   US_ID         "US"
#define   MAX_TENTA     5

#define   H_IDUSU   "IDENTUSU"    /* era ID_NOME*/
#define   H_SEUSU   "SENHAUSU"    /* era ID_SENHA*/
#define   H_USDES   "USUDESAB"    /* era E_USDESA*/
#define   H_NOUSU   "SEMIDENT"    /* Usuario nao cadastrado ou invalido */


#define   R_IDUSU   loc_str(GRP_IDENT,  1)
#define   R_SEUSU   loc_str(GRP_IDENT,  2)
#define   T_IDUSU   loc_str(GRP_IDENT,  3)
#define   T_SEUSU   loc_str(GRP_IDENT,  4)
#define   E_USDES   loc_str(GRP_IDENT,  5)

#define   E_NOUSU   loc_str(GRP_IDENT,  6)



#ifdef MSDOS
   COUNT primeira_fase( UTEXT * );
   VOID  mistura( UTEXT *, UTEXT * );
   COUNT dobra_semente( UTEXT *, UTEXT * );
   COUNT el_anterior( int, COUNT );
   COUNT el_posterior( int, COUNT );
   COUNT somatoria( UTEXT * );
   COUNT conf_senha( TEXT *, TEXT *, Usuario *);
   COUNT senha_ok( TEXT *, TEXT *, COUNT);
#else
   COUNT primeira_fase( );
   VOID  mistura( );
   COUNT dobra_semente( );
   COUNT el_anterior( );
   COUNT el_posterior( );
   COUNT somatoria( );
   COUNT conf_senha( );
   COUNT pega_uid( );
   COUNT senha_ok( );
#endif

#ifndef LITE
/**
**#  ---------------------------------------------------------------------
**#   Funcao...: COUNT pega_usu()
**#   Objetivo.: Solicitar e validar Identificacao/senha do usuario
**#   Entrada..: Nenhuma
**#   Retorna..:  OK -  Usuario se identificou OK
**#              !OK -  Usuario Nao  identificado
**#
**#   Obs......: modifica variaveis globais :
**#               ap_nome
**#               ap_local
**#               ap_grupo
**#               ap_idioma
**#               ap_status
**#
**/
F__GLB   COUNT pega_usu()
{
   COUNT ret = -2;         /* para sai do loop de leitura por limite */
   COUNT conta_tenta = 0;  /* numero de tentativas para acertar senha */
	TEXT usuario[20], senha[20];
   Usuario dados_usu;
	DTela vc[3];
	WIN *j1 = NULL;

   up_str(GRP_IDENT);

#ifdef UNIX
   if ( par_cfg.tipo_login == 2 ) {
      if ( ( ret = pega_uid(&dados_usu)) != 0 )
         mens_erro(H_NOUSU, E_NOUSU);
      goto fim;
   }
#endif

   /* usuario / senha */
   seta_dtela(&vc[0], usuario, X14, R_IDUSU, 0, 11, 0, E_ALFA, H_IDUSU, NULL, NULL, NULL);
   seta_dtela(&vc[1], senha,   S14, R_SEUSU, 1, 11, 0, E_ALFA, H_SEUSU, NULL, NULL, NULL);
   seta_dtela(&vc[2], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

   j1 = abre_janela(SENHA_WIN);

	fwbox(j1);

	mvwprintw(j1, 0, 0, T_IDUSU);  /* Nome */
	mvwprintw(j1, 1, 0, T_SEUSU);  /* chave */

   memset((TEXT *)&dados_usu, '\0', sizeof(Usuario));


   while ( conta_tenta++ < MAX_TENTA ) {

      INSERT_OFF();

      usuario[0] = senha[0] = '\0';

      if ( par_cfg.senha != NULL ) {
         w_ungc(K_ENTER);
         w_ungs(par_cfg.senha);
         conta_tenta=99;
      }

      if ( par_cfg.usuario != NULL ) {
         w_ungc(K_ENTER);
         w_ungs(par_cfg.usuario);
      }

		ret = ed_read(j1, &vc[0], 1, LIMPA_LE, "1 E");

      if ( par_cfg.usuario != NULL ) {
         if ( stricmp(par_cfg.usuario, usuario) != 0) {
		      ret = !OK;
            goto fim;
         }
      }

	   if ( ret < 0 || ret == K_ESC ) {  /* erro ou ESC */
			ret = !OK;
         goto fim;
      }

      /* conf_senha seta ap_usu_ativo */
		ret = conf_senha(usuario, senha, &dados_usu);

		if (  ret == OK )  break;

   }

fim :

   if ( ret == OK ) {
      persona(dados_usu.us_ident_usu, NULL);

      /* so para garantia de um intruso nao burlar status do usuario */
      if (dados_usu.us_uso_futuro[0] != 0 ) {
         if ( dados_usu.us_uso_futuro[0] != (dados_usu.us_status[0] - 'A') + 10 )
            dados_usu.us_status[0] = 'C';
      }
   }
   else {
      memset((TEXT *)&dados_usu, '\0', sizeof(Usuario));
   }

   strcpy(ap_nome,   dados_usu.us_ident_usu);
   strcpy(ap_local,  dados_usu.us_loc_trab);
   strcpy(ap_grupo,  dados_usu.us_grupo);
   strcpy(ap_idioma, dados_usu.us_idioma);
   ap_status = dados_usu.us_status[0];

   if ( dados_usu.us_menu[0] != '\0' )
      strcpy(par_cfg.filemenu, dados_usu.us_menu);

	if ( j1 != NULL )
	   fwkill(j1);

   down_str(GRP_IDENT);
   return(ret);
}

/**
**#  ---------------------------------------------------------------------
**#   Funcao...: COUNT conf_senha()
**#   Objetivo.: Valida usuario/senha para entrar no LB
**#   Entrada..: id_usuario - Identificacao do usuario (NOME)
**#              senha      - Senha informada
**#              endereco de estrutura usuario
**#   Retorna..: OK  - Usuario existe e senha esta correta
**#              !OK - Usuario/Senha invalidos
**#   Utiliza..: GLOBAL ap_usu_ativo para armazenar dados do usuario
**#              ativo.
**/
F__LOC   COUNT conf_senha(id_usuario, senha, PU)
TEXT *id_usuario, *senha;
Usuario *PU;
{  COUNT ret;

   ret = !OK;

   strupr(senha);

	if ( val_indice(id_usuario) != OK ||
		  load_rec(ace_dnum, ace_inum, US_ID, id_usuario, PU, sizeof(Usuario), '=') == NULL ||
        senha_ok(senha, PU->us_senha, PU->us_tmsenha) != 0 ) {
      win_bell(BEL_ATTN);
   }
   else {
      if ( PU->us_status[0] == 'D' )
         mens_erro(H_USDES, E_USDES);
      else
         ret = OK;
   }

   return(ret);
}


#ifdef UNIX
/**
**#  ---------------------------------------------------------------------
**#   Funcao...: COUNT pega_uid()
**#   Objetivo.: Pega usuario corrente para entrar no LB
**#   Entrada..: endereco de estrutura usuario
**#   Retorna..: OK  - Usuario existe e senha esta correta
**#              !OK - Usuario/Senha invalidos
**#   Utiliza..: GLOBAL ap_usu_ativo para armazenar dados do usuario
**#              ativo.
**/
F__LOC   COUNT pega_uid( PU )
Usuario *PU;
{  COUNT ret;
	TEXT id_usuario[50];
	struct passwd *p_st;
   int uid;


   ret = !OK;

   uid = geteuid();

   p_st = getpwuid(uid);

   if ( p_st == NULL )
      goto fim;



   strncpy(id_usuario, p_st->pw_name, 40);

   strupr(id_usuario);

	if ( val_indice(id_usuario) != OK ||
		  load_rec(ace_dnum, ace_inum, US_ID, id_usuario, PU, sizeof(Usuario), '=') == NULL ) {
      win_bell(BEL_ATTN);
   }
   else {
      if ( PU->us_status[0] == 'D' )
         mens_erro(H_USDES, E_USDES);
      else
         ret = OK;
   }

fim :

   return(ret);
}
#endif


F__LOC   COUNT senha_ok(chute, original, tam)
TEXT *chute, *original;
COUNT tam;
{  COUNT ret;
   TEXT  calculada[TE+1];

   if ( tam == '\0' ) {
      ret = (COUNT)stricmp(chute, original);
   }
   else {
      if ( original[0] == '\0' && chute[0] == '\0' && tam == ' ') {
         ret = 0;  /* senha valida */
      }
      else {
         lb_encrypt((UTEXT *) chute, (UTEXT *) calculada, (UTEXT) original[0]);
         ret = (COUNT)stricmp(original, calculada);
      }
   }

   return(ret);
}

/* fim do ifdef LITE */
#endif



/*************************************************************************/
/*************************************************************************/
/***********      ELABORADA PELO WAGNER                    ***************/
/*************************************************************************/
/*************************************************************************/


/**********************************************/
/**                                          **/
/**   Encripta senha internas                **/
/**                                          **/
/**********************************************/
F__GLB   VOID lb_encrypt (plain, coded, seed)
UTEXT *plain, *coded, seed;
{
   COUNT  ptr, value;
   struct tm *timeptr;
   time_t secsnow;

   if (plain[0] == '\0') {                       /* handle a null string    */
     coded[0] = '\0';
     return;
   }
   if (seed == '\0') {                           /* if seed not set, create */
      time(&secsnow);
      timeptr = localtime(&secsnow);
      seed = (TEXT)(((timeptr->tm_hour % 24) +      /* system time             */
             (timeptr->tm_min % 60) +
             (timeptr->tm_sec % 60)) % 90) + 35;
   }
   coded[0] = seed;                              /* start with the seed     */
   ptr = 0;                                      /* for first 8 chars of    */
   while ((plain[ptr] != '\0') && (ptr < 8)) {   /* the text, create cipher */
     value = ((plain[ptr] + coded[ptr]) << 1) ^ 0x55;
     coded[++ptr] = (TEXT)((value % 90) + 35);
   }                                             /* continue creating until */
   while (ptr < 13) {                            /* cipher is 13 chars long */
     if (ptr == 1) coded[ptr] = ((coded[0] + 12) % 90) + 35;
     else coded[ptr] = (((coded[ptr-1] + coded[ptr-2]) + 12) % 90) + 35;
     ptr++;
   }
   coded[ptr] = '\0';                            /* terminate the cipher    */
}




F__GLB   COUNT primeira_fase( semente )
UTEXT *semente;
{
   UTEXT buffer[32];
   COUNT somatoria;

   somatoria = dobra_semente( semente, buffer );
   mistura( semente, buffer );
   return somatoria;
}

F__GLB   VOID mistura( semente, buffer )
UTEXT *semente;
UTEXT *buffer;
{
   while (*semente) {
      *semente += *buffer - '0';
      if (*semente > '9')
         *semente -= 10;
      ++semente;
      ++buffer;
   }
}

F__GLB   COUNT dobra_semente( semente, buffer )
UTEXT *semente;
UTEXT *buffer;
{
   COUNT subscrito, soma;
   COUNT dadireita, daesquerda, ultimo;

   ultimo = (COUNT)strlen( (TEXT *)semente ) - 1;
   soma = somatoria( semente );
   daesquerda = (ultimo) % soma;
   dadireita = el_posterior( daesquerda, ultimo );
   for (subscrito = 0; subscrito <= ultimo; subscrito++ )
      if (subscrito & 0x01) {        /* casas impares */
         buffer[subscrito] = semente[dadireita];
         dadireita = el_posterior( dadireita, ultimo );
      }
      else {                         /* casas pares */
         buffer[subscrito] = semente[daesquerda];
         daesquerda = el_anterior( daesquerda, ultimo );
      }
   buffer[subscrito] = '\0';
   return soma;
}

F__GLB   COUNT el_anterior( elem, lim )
COUNT elem, lim;
{
   return (elem == 0) ? lim : --elem;
}

F__GLB   COUNT el_posterior( elem, lim )
COUNT elem, lim;
{
   return (elem == lim) ? (COUNT)0 : (COUNT)++elem;
}

F__GLB   COUNT somatoria ( semente )
UTEXT *semente;
{
   COUNT soma = 0;

   while (*semente)
      soma += *semente++;
   return soma;
}

