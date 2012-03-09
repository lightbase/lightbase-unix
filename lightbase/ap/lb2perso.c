#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2locs.h"
#include  "lb2perso.h"
#include  "lb2prot1.h"

#ifdef    VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif

#define   GRP_PERSO  14

#define   NUM_CONFIG 6

#define   DBG_TBPNULL "ap_tabpsq NULL"


#define   H_NOCFG   "NOUSUCFG"   /* Nao existe usuario ativo para personalizar */
#define   H_DIRPN   "NODIRPSN"   /* nao existe diretorio de personalizacao */
#define   H_PSNUS   "PERSOUSU"   /* Erro atualiz. arq personaliz. usuario  */
#define   H_AUTO    "CFG_AUTO"   /* Help de menu */
#define   H_SAVE    "CFG_SAVE"   /* Help de menu */
#define   H_SINO    "CFG_SINO"   /* Help de menu */
#define   H_FONE    "CFG_FONE"   /* Help de menu */
#define   H_GRAV    "CFG_GRAV"   /* Help de menu */

#define   C_INDEX   loc_str(GRP_PERSO,  1)
#define   C_SAVE    loc_str(GRP_PERSO,  2)
#define   C_SINO    loc_str(GRP_PERSO,  3)
#define   C_FONE    loc_str(GRP_PERSO,  4)
#define   C_GRAVA   loc_str(GRP_PERSO,  5)
#define   R_GRAVA   loc_str(GRP_PERSO,  6)

#define   E_NOCFG   loc_str(GRP_PERSO,  501)
#define   E_DIRPSN  loc_str(GRP_PERSO,  502)

#ifdef MSDOS
   COUNT muda_flag(Selecao *);
#else
   COUNT muda_flag( );
#endif



F__GLB   COUNT persona(full_usuario, base)
TEXT *full_usuario, *base;
{  COUNT ret;
   SI   numero, l_in, c_in, l_fi, c_fi, l_vi, c_vi, tipo; /* NAO use short */
   LONG pos_seek;
   FILE *fp = NULL;
   TEXT dir_persona[PATH_MAX];
   UTEXT *p;
   TEXT aux[240], usuario[10], base_oito[10];

   strncpy(usuario, full_usuario, 8);
   usuario[8] = '\0';

   if ( base != NULL ) sprintf(base_oito, "%8.8s", base);


   p = (UTEXT *)getenv("LBPERSON");

   if ( p != NULL ) {
      strcpy(dir_persona, (TEXT *)p);
   }
   else if ( info_base(DIR_PERSONA, ap_home_lb, str_nada, dir_persona) != -1 ) {
      /* nao existe diretorio */
      ret = -1;
      goto fim;
   }

   ret = info_base(usuario, dir_persona, str_nada, aux);

   if ( (ret <= 0 ) || ( fp = fopen(aux, "r")) == NULL ) {
      ret = -1;
      goto fim;
   }



   while ( ! feof(fp) )  {


      pos_seek = ftell(fp);

      p = (UTEXT *)fgets( aux, sizeof(aux) - 1, fp );

      if ( p == NULL )
         continue;

      for ( p = (UTEXT *)aux; *p != '\0'; p++ )
         if ( *p < 32 ) *p = ' ';

      if ( aux[0] == '#' ) { /* posicoes de telas */
         /* #999 99 99 99 99 99 99 99 */

         if ( base != NULL ) continue;

         sscanf(&aux[1], "%d %d %d %d %d %d %d %d",
            &numero,  &l_in, &c_in,  &l_fi, &c_fi,  &l_vi, &c_vi,  &tipo);

         if ( tipo < 1 || tipo > 8 || l_in < 1 || numero >= JANELA_MOR) {
            ret = -1;
            goto fim;
         }

         ap_winloc[numero][L_IN] = (UTEXT) l_in;
         ap_winloc[numero][C_IN] = (UTEXT) c_in;
         ap_winloc[numero][L_FI] = (UTEXT) l_fi;
         ap_winloc[numero][C_FI] = (UTEXT) c_fi;
         ap_winloc[numero][L_VI] = (UTEXT) l_vi;
         ap_winloc[numero][C_VI] = (UTEXT) c_vi;
         ap_winloc[numero][TIPO] = (UTEXT) tipo;

      }
      else if ( aux[0] == '@' ) { /* pesquisa de bases */
         /* @BASE     99 pesquisa */

         if ( base == NULL ) continue;

         if ( strncmp(&aux[1], base_oito, 8) != 0 ) continue;

         /* @12345678 NN xxxxxxxxxxxxxxxxxxxxxxxxxxxxx... */

         sscanf(&aux[10], "%d", &numero);

         if ( numero < 1 || numero > MAX_PESQUISAS ) continue;

         numero--;

         if ( ap_tabpsq[numero] == NULL ) {
            debug(DBG_TBPNULL);
            continue;
         }

         sprintf(ap_tabpsq[numero], "%-230.230s", &aux[10]); /* TAM_LINPSQ */
         ap_tabctl[numero] = 0;
         ap_taboff[numero] = pos_seek;
      }
      else if ( aux[0] == '-' ) { /* Autosave e indexacao */
         if ( strncmp(aux, "-AS", 3) == 0 ) {
            if ( aux[3] == '-' ) ap_autosave = 0;
            else                 ap_autosave = 1;
            continue;
         }
         else if ( strncmp(aux, "-IN", 3) == 0 ) {
            if ( aux[3] == '-' ) ap_pos_index = POS_INDEX;
            else                 ap_pos_index = NOW_INDEX;
            continue;
         }
      }
   }

   ret = 0;

fim :

   if ( fp != NULL ) fclose ( fp );

   return(ret);
}

F__GLB   COUNT tabpsq_inic( so_libera )
COUNT so_libera; /* se 1 so libera areas alocadas */
{  COUNT ret, i;

   for ( i = 0; i < MAX_PESQUISAS; i++) {
      ap_tabctl[i] = 0;
      ap_taboff[i] = 0L;

      if ( ap_tabpsq[i] != NULL ) {
         free(ap_tabpsq[i]);
         ap_tabpsq[i] = NULL;
      }

      if ( so_libera ) continue;

      if ( (ap_tabpsq[i] = calloc(1, TAM_LINPSQ)) == NULL) {
         ret = -1;
         goto fim;
      }

      ap_tabpsq[i][0] = '\0';

      sprintf(ap_tabpsq[i], TAB_FMT, i + 1, 0L, C_PSQ_EM, "  /  /  ", str_nada);
   }

   ap_tabpsq[MAX_PESQUISAS] = NULL;

   if ( ap_tabpsq[MAX_PESQUISAS + 1] != NULL) {
      free(ap_tabpsq[MAX_PESQUISAS + 1]);
      ap_tabpsq[MAX_PESQUISAS + 1] = NULL;
   }

   if ( !so_libera ) {

      /* monta mascara da tabela */

      if ( ap_tabpsq[MAX_PESQUISAS + 1] != NULL) {
         free(ap_tabpsq[MAX_PESQUISAS + 1]);
         ap_tabpsq[MAX_PESQUISAS + 1] = NULL;
      }

      if ( (ap_tabpsq[MAX_PESQUISAS + 1] = calloc(1, TAM_LINPSQ)) == NULL) {
         ret = -1;
         goto fim;
      }

      memset(&ap_tabpsq[MAX_PESQUISAS + 1][0], 'P', TAM_PSQPROT);
      memset(&ap_tabpsq[MAX_PESQUISAS + 1][TAM_PSQPROT], 'X',
                     TAM_LINPSQ - TAM_PSQPROT - 1);
      ap_tabpsq[MAX_PESQUISAS + 1][TAM_LINPSQ - 1] = '\0';
   }

   ret = 0;

fim :

   return(ret);
}



/*----------------------------------------------------move_janela----------*/
F__GLB   COUNT move_janela(win)
WIN *win;
{  COUNT ret;
   COUNT numero;

   if ( (numero = win->ident) == 0 ) {
      ret = 0;
      goto fim;
   }

   if ( ap_winloc[numero][L_IN] != (UTEXT) win->w_y ) {
      ap_winloc[numero][L_IN] = (UTEXT) win->w_y;
      ap_winloc[numero][TIPO] |= 0X80;
   }

   if ( ap_winloc[numero][C_IN] != (UTEXT) win->w_x ) {
      ap_winloc[numero][C_IN] = (UTEXT) win->w_x;
      ap_winloc[numero][TIPO] |= 0X80;
   }

   if ( ap_winloc[numero][L_VI] != (UTEXT) win->w_aheight ) {
      ap_winloc[numero][L_VI] = (UTEXT) win->w_aheight;
      ap_winloc[numero][TIPO] |= 0X80;
   }

   if ( ap_winloc[numero][C_VI] != (UTEXT) win->w_alen ) {
      ap_winloc[numero][C_VI] = (UTEXT) win->w_alen;
      ap_winloc[numero][TIPO] |= 0X80;
   }

   ret = 0;

fim :

   return(ret);

}



F__GLB   COUNT config_lb(win)
WIN *win;
{  COUNT ret, tam_str, tam_op, i, flag;
   COUNT x_o, y_o;
   TEXT *status, *rodape, *titulo;
	Selecao m0;
	WIN *jf;
	TEXT *pode, *p_nome, *p_opcao,  *m_config[NUM_CONFIG * 3 + 1];

   up_str(GRP_PERSO);

   for ( i = 0 ; i < (NUM_CONFIG * 3 + 1); i++ )
      m_config[i] = NULL;

   if ( ap_nome[0] == '\0') {
      mens_erro(H_NOCFG, E_NOCFG);
      ret = -1;
      goto fim;
   }


   tam_op  = MAX( (COUNT)strlen(OP_SIM) , (COUNT)strlen(OP_NAO) );

   tam_str = 0;
   tam_str = MAX (tam_str, (COUNT)strlen(C_INDEX) );
   tam_str = MAX (tam_str, (COUNT)strlen(C_SAVE ) );
   tam_str = MAX (tam_str, (COUNT)strlen(C_SINO ) );
   tam_str = MAX (tam_str, (COUNT)strlen(C_FONE ) );
   tam_str = MAX (tam_str, (COUNT)strlen(C_GRAVA) );


   tam_str = tam_str + tam_op + 3;

   for ( i = 0 ; i < NUM_CONFIG ; i++ ){

   	if ( (m_config[OP(i)] = (TEXT *)calloc( 1, tam_str + 1))  == NULL ) {
         mens_erro(H_NOMEMO, SEM_MEMO);
         ret = -1;
         goto fim;
      }

      memset((TEXT *) m_config[OP(i)], ' ', tam_str);

      p_nome    = m_config[OP(i)];
      p_opcao   = p_nome + tam_str - tam_op ;
      pode = OP_NAO;
      rodape = M_INVERTE;
      status = "1";

      switch ( i + 1 ) {
         case 1 : /* Autoindex */
                  if ( ap_pos_index != POS_INDEX ) pode = OP_SIM;
                  titulo = C_INDEX;
                  break;

         case 2 : /* Autosave */
                  if ( ap_autosave  != 0 ) pode = OP_SIM;
                  titulo = C_SAVE;
                  break;

         case 3 : /* sinonimos */
#ifdef SINONIMOS
                  if ( ap_sinonimos != 0 ) pode = OP_SIM;
                  titulo = C_SINO;
                  break;
#else
                  pode = OP_NAO;
                  titulo = C_SINO;
                  rodape = str_nada;
                  status = "0";
                  break;
#endif
         case 4 : /* Uso futuro */

                  pode = OP_NAO;
                  titulo = C_FONE;
                  rodape = str_nada;
                  status = "0";
                  break;

         case 5 : /* traco */
                  titulo = "--";
                  rodape = str_nada;
                  status = "2";
                  break;

         case 6 : /* Grava */
                  titulo = C_GRAVA;
                  rodape = R_GRAVA;
                  status = "1";
                  break;

      }

      strncpy(p_nome , titulo, strlen(titulo));

      if ( i < NUM_CONFIG - 2) { /* nao coloca nos dois ultimos */
         memset(p_opcao, ' ', tam_op);
         strncpy(p_opcao, pode, strlen(pode));
      }

      m_config[AT(i)] = status;
      m_config[AJ(i)] = rodape;
   };


   x_o = (size_x - tam_str) / 2;
   y_o = (size_y - NUM_CONFIG) / 2;


   jf = abre_funcoes(funcao("1 E"));

   /* coluna  linha  larg_aparente   alt_aparente   num_colunas */
	def_menu( &m0, x_o, y_o, tam_str + 2, NUM_CONFIG, 1,
            win->w_boxatr, MS_BARRA, 2, MS_JANELA, MS_DESTAQUE,
				&m_config, str_nada, str_nada);

   m0.comando = muda_flag;

	ret = menu( &m0 );

   if ( ret != -1 ) {

      for (i = 0; i < NUM_CONFIG; i++) {
         p_nome    = m_config[OP(i)];
         p_opcao   = p_nome + tam_str - tam_op ;

         if ( strncmp(p_opcao, OP_SIM, strlen(OP_SIM) ) == 0 ) flag = 1;
         else flag = 0;

         switch ( i + 1 ) {
            case 1 : /* Autoindex */
                     if ( flag == 1 ) ap_pos_index = NOW_INDEX;
                     else             ap_pos_index = POS_INDEX;
                     break;

            case 2 : /* Autosave */
                     ap_autosave = flag;
                     break;

            case 3 : /* sinonimos */
                     ap_sinonimos = flag;
                     break;

            case 4 : /* fonetica */
                     ap_fonetica = flag;
                     break;
         }
      }

      if ( ret + 1 == NUM_CONFIG ) {  /* Grava sempre sera a ultima opcao */
         config_save(ap_nome, NULL, 1);
      }
   }

   fecha_funcoes(jf);

fim :

   for ( i = 0 ; i < NUM_CONFIG; i++ )
   	if ( m_config[OP(i)] != NULL ) free((TEXT *)m_config[OP(i)]);

   ret = 1; /* sempre fecha menu que o chamou */

   down_str(GRP_PERSO);

	return(ret);
}


F__GLB   COUNT config_save(full_usuario, base, flag)
TEXT *full_usuario, *base;
COUNT flag;  /* indica se devera salvar posicoes de janelas */
{  COUNT ret, i;
   COUNT l_in, c_in, l_fi, c_fi, l_vi, c_vi, tipo;
   LONG pos;
   FILE *fp = NULL;
   TEXT dir_persona[PATH_MAX], *p;
   TEXT aux[300], usuario[10];
   LONG pos_seek = 0L;

   strncpy(usuario, full_usuario, 8);
   usuario[8] = '\0';

   p = getenv("LBPERSON");

   if ( p != NULL ) {
      strcpy(dir_persona, p);
   }
   else if ( info_base(DIR_PERSONA, ap_home_lb, str_nada, dir_persona) != -1 ) {
      mens_erro(H_DIRPN, E_DIRPSN); /* nao existe diretorio */
      ret = -1;
      goto fim;
   }

   ret = info_base(usuario, dir_persona, str_nada, aux);

   if ( ret == 0 ) {
      if ( ( fp = fopen(aux, "w")) != NULL ) {
         fclose(fp);
         fp = NULL;
      }
      else ret = -1;
   }

   if ( (ret < 0 ) || ( fp = fopen(aux, "r+")) == NULL ) {
      mens_erro(H_PSNUS, E_NOACES, aux);
      ret = -1;
      goto fim;
   }

   if ( flag ) {

      for ( i = 1; i < JANELA_MOR; i++ ) {

         if ( FOI_MODIFICADO(i) ) {

            l_in = ap_winloc[i][L_IN];
            c_in = ap_winloc[i][C_IN];
            l_fi = ap_winloc[i][L_FI];
            c_fi = ap_winloc[i][C_FI];
            l_vi = ap_winloc[i][L_VI];
            c_vi = ap_winloc[i][C_VI];
            tipo = ap_winloc[i][TIPO] & 0x7F; /* desliga flag em tipo */

            fprintf(fp, "#%d %d %d %d %d %d %d %d\n",
               i, l_in, c_in, l_fi, c_fi, l_vi, c_vi, tipo);
         }
      }

      fprintf(fp, "-AS%c\n", ap_autosave  == 0 ? '-' : ' ');
      fprintf(fp, "-IN%c\n", ap_pos_index == POS_INDEX ? '-' : ' ');
   }


   if ( base != NULL ) {
      for ( i = 0; i < MAX_PESQUISAS; i++ ) {

         if ( ap_tabctl[i] == 0 ) continue;

         if ( (ap_tabctl[i] & TAB_MODI) != 0 ) {
            if ( ap_taboff[i] != 0L ) {
               fseek(fp, ap_taboff[i], 0); /* apaga marca antiga */
               fputc('-', fp);
            }
            fseek(fp, 0L, 2); /* fim do arquivo */
            pos_seek = ftell(fp);

            if ( ap_tabpsq[i] == NULL)
               debug(DBG_TBPNULL);

            sprintf(aux, "@%8.8s %s", base, ap_tabpsq[i]);
            trim(aux);
            ap_tabctl[i] = 0;
            ap_taboff[i] = pos_seek;
            fseek(fp, pos_seek, 0);
            fprintf(fp, "%s\n", aux);
         }

         else {
            pos = ap_taboff[i] + 10; /* @12345678 NN xxxxxxxxxxxxxxxxxxxxxxxxxxxxx... */
            fseek(fp, pos, 0); /* mesma posicao anterior */
            strcpy(aux, ap_tabpsq[i]);
            aux[TAM_PSQPROT] = '\0';
            fprintf(fp, aux);
            ap_tabctl[i] = 0;
         }
      }
   }

fim:

   if ( fp != NULL ) {
      fclose ( fp );
      if ( ferror(fp) ) {
         ret = -1;
      }
   }
   else ret = 0;

   return(ret);
}



F__LOC   COUNT muda_flag(s)
Selecao *s;
{  COUNT ret, tam_op;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *p_nome, *p_opcao;

   switch ( s->escolhido + 1 ) {
      case 1 : /* Autoindex */
               win_ajuda = H_AUTO;
               break;

      case 2 : /* Autosave */
               win_ajuda = H_SAVE;
               break;

      case 3 : /* sinonimos */
               win_ajuda = H_SINO;
               break;

      case 4 : /* fonetica */
               win_ajuda = H_FONE;
               break;

      case 6 : /* Grava */
               win_ajuda = H_GRAV;
               break;
   }

   /* se a ultima tecla pressionada foi ESC, retorna -1 para fechar */

   s->tecla = fwnqgc(s->win); /* pega tecla sem refresh */

   if ( s->tecla == K_ESC || s->tecla == K_ENTER ) {
      ret = -3; /* sai */
      goto fim;
   }

   if ( s->tecla != ' ' ) {
      ret = -2;  /* assume tecla */
      goto fim;
   }

   if ( s->escolhido < NUM_CONFIG - 2) { /* nao coloca nos dois ultimos */
      tam_op    = MAX( strlen(OP_SIM) , strlen(OP_NAO) );
      p_nome   = (*s->opcoes)[OP(s->escolhido)];
      p_opcao  = p_nome + strlen(p_nome) - tam_op ;

      if ( strncmp(p_opcao, OP_SIM, strlen(OP_SIM) ) == 0 ) {
         memset(p_opcao, ' ', tam_op);
         strncpy(p_opcao, OP_NAO, strlen(OP_NAO));
      }
      else {
         memset(p_opcao, ' ', tam_op);
         strncpy(p_opcao, OP_SIM, strlen(OP_SIM));
      };
   }


   ret = -2; /* assume tecla */

fim :

   win_ajuda = ajuda_ant;

   return(ret);
}
