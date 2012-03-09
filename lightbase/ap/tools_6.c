#include  "lb2all.h"

#include <string.h>
#include  <time.h>
#include  <ctype.h>

#include  "lb2stru.h"
#include  "lb2files.h"
#include  "lb2bases.h"

#include "tools.h"

#ifdef MSDOS
#include <io.h>
#endif


#ifdef MSDOS
   TEXT *pega_info(TEXT *, COUNT);
   COUNT monta_usuario(TEXT *);
   COUNT monta_grupo(TEXT *);
   COUNT monta_base(TEXT *);
   COUNT monta_acesso(TEXT *);
   VOID  seta_prot(TEXT *, COUNT *);
   COUNT cr_acessos(COUNT, COUNT, LONG);
   COUNT open_acessos(COUNT, COUNT, LONG);
   COUNT add_usu( VOID );
   COUNT add_grp( VOID );
   COUNT add_base( VOID );
   COUNT add_acesso( TEXT * );
   COUNT piece( TEXT *, TEXT *, COUNT, COUNT, COUNT);
	int crypt_key ( TEXT *, TEXT * );
#else
   TEXT *pega_info( );
   COUNT monta_usuario( );
   COUNT monta_grupo( );
   COUNT monta_base( );
   COUNT monta_acesso( );
   VOID  seta_prot( );
   COUNT cr_acessos( );
   COUNT open_acessos( );
   COUNT add_usu( );
   COUNT add_grp( );
   COUNT add_base( );
   COUNT add_acesso( );
   COUNT piece( );
	int crypt_key ( );
#endif


static Usuario usuario;
static Grupo grupo;
static Base base;
static Acesso acesso;
static LONG serie_gl;


static COUNT ace_dnum;
static COUNT ace_inum;

extern LONG chaves[10];
extern COUNT ind_chaves;

TEXT ser_full[20];

COUNT refaz_acessos()
{  COUNT ret=0;
   LONG serie;
   TEXT aux[200], *info;
   FILE *fp_in;
   TEXT senha[50], serie_aux [ 20 ], data_val [ 7 ], num_ttys [ 20 ];

   ace_dnum = FILE_DATNO;
   ace_inum = FILE_KEYNO;

   if ( (fp_in = fopen("acessos.txt", "r")) == NULL ) {
      fprintf(stderr, "Erro na abertura de acessos.txt\n");
      ret = -1;
      goto fim;
   }

   fprintf(stderr, "Numero de serie   ? ");
   gets(aux);
   ser_full[0] = '\0';

   if ( strlen(aux) > 6 ) {
      serie_gl = serie = 100000L + atol(aux+6);
      strcpy(ser_full, aux);
   }
   else {
	   serie_gl = serie = atol(aux);
   }

   /* numeros de serie < que 200 sao consideradas bases publicas */
   {
	   fprintf(stderr, "Chave de ativacao ? ");

	   gets(senha);

	   if ( strlen ( aux ) != 11 )
	   {
   		fprintf(stderr, "Chave de ativacao nao confere. Operacao cancelada\n");
	   	ret = -2;
	   	goto fim;
	   }

      strcpy    ( serie_aux, aux );
      crypt_key (serie_aux, serie_aux + 11 );

  		if ( stricmp(serie_aux, senha) == 0 )
			goto continua;

		fprintf ( stderr, "Data de Validade AAMMDD ? " );
		gets ( data_val );

		if ( strlen ( data_val ) == 0  )
		{
			fprintf ( stderr, "Numero de terminais ? ", num_ttys );
			gets ( num_ttys );
		}

		if ( data_val [ 0 ] != 0 )
		{
			sprintf ( serie_aux, "%6s%s", data_val, aux+6);
		}
		else
		{
			if ( atoi ( num_ttys ) != 999 )
			{
		   	TEXT aux_aqui [ 10 ];
			   sprintf ( aux_aqui , "%03d", atoi ( num_ttys ));
			   strcpy  ( serie_aux, aux );
         	memcpy  ( serie_aux+2, aux_aqui, 3);
			}
			else
				strcpy ( serie_aux, aux);

		}

		strlwr(serie_aux);

		crypt_key(serie_aux, serie_aux + 11 );

      if ( stricmp(serie_aux, senha) != 0 )
      {
   		fprintf(stderr, "Chave de ativacao nao confere. Operacao cancelada\n");
      	ret = -2;
         goto fim;
      }

   }

continua:

   chaves[ind_chaves++] = serie;

   if ( cr_acessos( ace_dnum,  ace_inum, serie) != 0 ) {
      fprintf(stderr, "Erro na criacao do cadastro de acessos.\n");
      ret = -3;
      goto fim;
   }

   if ( open_acessos( ace_dnum,  ace_inum, serie) != 0 ) {
      fprintf(stderr, "Erro na abertura do cadastro de acessos.\n");
      ret = -3;
      goto fim;
   }

   while (fgets(aux, sizeof(aux), fp_in) != NULL ) {
      strip_cr(aux);

      info = strchr(aux,'[');

      if ( aux[0] != 'T' || info == NULL ) {
         fprintf(stderr, "Linha invalida : %s\n", aux);
         continue;
      }


      switch(aux[1]) {
         case '1' :  /* usuario */
                     if ( monta_usuario(info) == 0 ) {
                        add_usu();
                     }
                     break;
         case '2' :  /* grupo */
                     if ( monta_grupo(info) == 0 ) {
                        add_grp();
                     }
                     break;
         case '3' :  /* base */
                     if ( monta_base(info) == 0 ) {
                        add_base();
                     }
                     break;
         case '4' :  /* acesso */
                     if ( monta_acesso(info) == 0 ) {
                        add_acesso(info);
                     }
                     break;
      }
   }

fim:

   CLSFIL(ace_dnum, COMPLETE);
   CLSFIL(ace_inum, COMPLETE);

   if ( fp_in != NULL )
      fclose(fp_in);

   return(ret);
}


COUNT monta_usuario(info)
TEXT *info;
{
   memset(&usuario, '\0', sizeof(usuario));

   strcpy(usuario.us_tpreg,    US_ID);
   strcpy(usuario.us_tipo_reg, US_ID);

   strcpy(  usuario.us_ident_usu,   pega_info(info, 1));
   strcpy(  usuario.us_full_name,   pega_info(info, 2));
   strcpy(  usuario.us_grupo,       pega_info(info, 3));
   strcpy(  usuario.us_senha,       pega_info(info, 4));
   strcpy(  usuario.us_loc_trab,    pega_info(info, 5));
   strcpy(  usuario.us_fone,        pega_info(info, 6));
   strcpy(  usuario.us_idioma,      pega_info(info, 7));
   strcpy(  usuario.us_menu,        pega_info(info, 8));
   strcpy(  usuario.us_status,      pega_info(info, 9));

   usuario.us_tmsenha = atoi(pega_info(info, 10));

   usuario.us_uso_futuro[0] = (usuario.us_status[0] - 'A') + 10;

   if ( usuario.us_status[0] < 'A' || usuario.us_status[0] > 'D' ) {
      fprintf(stderr, "Usuario invalido : %s\n", info);
      return(-1);
   }
   else
      return( 0 );
}


COUNT monta_grupo(info)
TEXT *info;
{
   memset(&grupo, '\0', sizeof(grupo));

   strcpy(grupo.gr_tpreg,    GR_ID);

   strcpy( grupo.gr_ident_grp,     pega_info(info, 1));
   strcpy( grupo.gr_full_name,     pega_info(info, 2));
   strcpy( grupo.gr_loc_trab,      pega_info(info, 3));
   strcpy( grupo.gr_fone,          pega_info(info, 4));


   if ( strlen(grupo.gr_ident_grp) == 0 ) {
      fprintf(stderr, "Grupo invalido : %s\n", info);
      return(-1);
   }
   else
      return( 0 );
}

COUNT monta_base(info)
TEXT *info;
{  COUNT i;
   LONG serie_old;

   memset(&base, '\0', sizeof(base));

   base.bs_marca[0] = 'B';
   base.bs_marca[1] = 'S';

   strcpy(  base.bs_nome,    pega_info(info, 1));
   strcpy(  base.bs_path,    pega_info(info, 2));
   strcpy(  base.bs_desc,    pega_info(info, 3));
   strcpy(  base.bs_dono,    pega_info(info, 4));
   strcpy(  base.bs_grupo,   pega_info(info, 5));

   base.bs_idbase = atoi(pega_info(info, 6));

   serie_old = atol(pega_info(info, 7));

   for (i= 0; i < ind_chaves; i++ ) {
      if ( chaves[i] == serie_old )
         break;
   }

   if ( serie_old > 200 && i == ind_chaves ) { /* ainda nao conhece */
         TEXT nome[20], senha[50];
volta:
         fprintf(stderr, "Chave de ativacao para Numero de serie %ld ? ", serie_old);
         gets(senha);
         sprintf(nome, "%06ld", serie_old);
         if ( permissao( nome, senha ) == 0 ) {
            fprintf(stderr, "Chave de ativacao nao confere. Operacao cancelada\n");
            fprintf(stderr, "Deseja retentar (S/N) ? ");
            gets(senha);
            if ( senha[0] == 'S' || senha[0] == 's' )
               goto volta;

            fprintf(stderr, "Setando numero de serie para numero corrente\n");
            base.bs_nserie = serie_gl;
         }
         else {
            chaves[ind_chaves++] = serie_old;
            base.bs_nserie = serie_old;
         }
   }
   else {
      base.bs_nserie = serie_old;
   }

   base.bs_xor    = (UCOUNT)atoi(pega_info(info, 8));

   if ( strlen(base.bs_nome) == 0 ) {
      fprintf(stderr, "Base invalida : %s\n", info);
      return(-1);
   }
   else
      return( 0 );
}

COUNT monta_acesso(info)
TEXT *info;
{  COUNT vazio = 0, i, numericos = 0;
   TEXT *p;

   memset(&acesso, '\0', sizeof(acesso));
   strcpy(acesso.da_tpreg,    DA_ID);

   if ( strlen(pega_info(info, 2)) == 0 ) {
      fprintf(stderr, "Acesso nao especifica base : %s\n", info);
      return(-1);
   }

   strcpy(  acesso.da_ugnome,    pega_info(info, 1));
   strcpy(  acesso.da_base,      pega_info(info, 2));

   calado = 1; /* nao imprime mensagens intermediarias */
   if ( find_rec(ace_dnum, ace_inum, GR_ID, acesso.da_ugnome, '=') == NULL) {
      if ( find_rec(ace_dnum, ace_inum, US_ID, acesso.da_ugnome, '=') == NULL) {
         fprintf(stderr, "Acesso nao e usuario nem grupo : %s.\n", acesso.da_ugnome);
         calado = 0;
         return(-1);
      }
   }


   if ( find_rec( ace_dnum, ace_inum, D0_ID, acesso.da_base, '=') == NULL ) {
         fprintf(stderr, "Base nao existe : %s.\n", acesso.da_base);
         calado = 0;
         return(-1);
   }

   calado = 0;

   for ( i = 2; vazio <= 2; i++) {
      p = pega_info(info, i);

      if ( *p == '\0' ) {
         vazio++;
         continue;
      }

      if ( atoi(p) > 0 ) {
         if ( numericos++ == 0 ) { /* e vistas */
            seta_prot(p, acesso.da_vistas);
         } else { /* e relatorio */
            seta_prot(p, acesso.da_rel);
         }
      }
      else {
         if      ( stricmp(p, "in") == 0 ) acesso.da_inclui   = 'X';
         else if ( stricmp(p, "al") == 0 ) acesso.da_altera   = 'X';
         else if ( stricmp(p, "ex") == 0 ) acesso.da_exclui   = 'X';
         else if ( stricmp(p, "pe") == 0 ) acesso.da_pesquisa = 'X';
         else if ( stricmp(p, "em") == 0 ) acesso.da_emite    = 'X';
         else if ( stricmp(p, "ep") == 0 ) acesso.da_exporta  = 'X';
         else if ( stricmp(p, "im") == 0 ) acesso.da_importa  = 'X';
         else if ( stricmp(p, "pa") == 0 ) acesso.da_parcial  = 'X';
         else if ( stricmp(p, "to") == 0 ) acesso.da_total    = 'X';
         else if ( stricmp(p, "ba") == 0 ) acesso.da_backup   = 'X';
         else if ( stricmp(p, "re") == 0 ) acesso.da_restore  = 'X';
         else if ( stricmp(p, "ta") == 0 ) acesso.da_tabela   = 'X';
      }

   }

   return( 0 );
}

VOID seta_prot(str, prt)
TEXT *str;
COUNT *prt;
{  COUNT i;
   TEXT aux[30];

   for (i=0;i < MAX_VISTAS; i++) {
      if ( piece(str, aux, ',', i+1, 3) <= 0 ) {
         break;
      }

      prt[i] = atoi(aux);
      if ( prt[i] < 1 || prt[i] > MAX_VISTAS) {
         prt[i] = 0;
         return;
      }
   }

}


TEXT *pega_info(str, num)
TEXT *str;
COUNT num;
{  COUNT n, fim;
   TEXT *p_str, *p_info;
   static TEXT info[200];

   n = fim = 0;
   p_str = str;

   while ( fim == 0 && *p_str != '\0') {
      if ( *p_str == '"'  && *(p_str - 1) != '\\') {
         n++;
         if ( n == (num * 2 - 1)) {
            p_str++;
            p_info = info;
            while ( *p_str != '\0') {
               if ( *p_str == '"'  && *(p_str - 1) != '\\') {
                  fim = 1;
                  *p_info = '\0';
                  break;
               }
               else {
                  *p_info++ = *p_str++;
               }
            }
         }
      }
      p_str++;
   }

   if ( fim == 0 )
      info[0]= '\0';

   return(info);
}

COUNT cr_acessos(datno, indno, serie)
COUNT datno, indno;
LONG serie;
{
   COUNT ret = OK, modo;
   UCOUNT num_magico;
	TEXT cadastro[PATH_MAX];
	TEXT indice[PATH_MAX];

   /* menssagens carregadas por mt_cadusu */

   num_magico = 0;

   modo = PERMANENT | EXCLUSIVE | VLENGTH;

   strcpy(cadastro, "acessos.dat");
   strcpy(indice, "acessos.idx");

   if ( access(cadastro, 0) == 0 || access(cadastro, 0) == 0 ) {
      TEXT r[20];
      fprintf(stderr, "Arquivo de acessos ja existe. Recria (S/N) ?");
      gets(r);

      if ( toupper(*r) == 'S') {
         if ( access(cadastro, 0) == 0 && DELFILE(cadastro) != NO_ERROR ) {
            fprintf(stderr, "Erro na exclusao de %s\n", cadastro);
            ret = -1;
            goto fim;
         }
         if ( access(indice, 0) == 0 && DELFILE(indice) != NO_ERROR ) {
            fprintf(stderr, "Erro na exclusao de %s\n", indice);
            ret = -1;
            goto fim;
         }
      }
      else {
         ret = 0;
         goto fim;
      }
   }


   ct_sernum   = 0;

   if ( ser_full[0] != '\0' ) {
		UTEXT *p;
		p = (UTEXT *)&ct_sernum;
		*p++ = ser_full[2];
		*p++ = ser_full[3];
		*p++ = ser_full[4];
		*p   = ser_full[5];

   }



   ret = CREDAT(datno, cadastro, 0, SLOT, modo, serie, num_magico);

	if ( ret != NO_ERROR ) {
      fprintf(stderr, "Erro na criacao de %s\n", cadastro);
		ret = -1;
		goto fim;
	}

	ret = CREIDX(indno, indice, LB_KEYMAX, 0, 0, SLOT, modo, serie, num_magico);

	if ( ret != NO_ERROR ) {
      fprintf(stderr, "Erro na criacao de %s\n", cadastro);
		ret = -1;
		goto fim;
	}

   CLSFIL(datno, COMPLETE);
   CLSFIL(indno, COMPLETE);

   ret = 0;

fim :

   return(ret);
}



/*#  ---------------------------------------------------------------------
**#   Funcao...: open_acessos
**#   Objetivo.: abrir cadastro de acessos
**#   Entrada..: perm_edit -> 0 - abre so pra leitura
**#                        -> 1 - abre permitindo alteracao
**#   Retorna..:
**#
**/
COUNT open_acessos(datno, indno, serie)
COUNT datno, indno;
LONG serie;
{
   COUNT ret, modo;
	TEXT cadastro[PATH_MAX];
	TEXT indice[PATH_MAX];
   UCOUNT magico = 0;


   ret = 0;

   modo = VIRTUAL | EXCLUSIVE;

   strcpy(cadastro, "acessos.dat");
   strcpy(indice, "acessos.idx");


	if ( (ret = OPNFIL(datno, cadastro, modo, serie, magico)) == 0 ) {
   	if ( (ret = OPNFIL(indno, indice, modo, serie, magico)) != 0 )
         CLSFIL(datno, COMPLETE); /* fecha cadastro ja aberto */
   }

   if ( ret != NO_ERROR ) {
      fprintf(stderr, "Erro na abertura dos arquivos de acessos\n");
      ret = -1;
      goto fim;
   }

fim :

   return(ret);
}




/*#  --------------------------------------------------------------------- */
COUNT add_usu()
{  COUNT ret= 0;


   calado = 1; /* nao imprime mensagens intermediarias */
   if ( load_rec(ace_dnum, ace_inum, GR_ID, usuario.us_ident_usu, &grupo, sizeof(Grupo), '=') == NULL) {
	   if ( ! add_rec(ace_dnum, ace_inum, US_ID, usuario.us_ident_usu,
                  &usuario, sizeof(Usuario))) {
         fprintf(stderr, "Erro incluindo usuario : %s.\n", usuario.us_ident_usu);
         ret = -1;
      }
   }
   else {
      fprintf(stderr, "Usuario invalido : %s. Ja existe como Grupo\n", usuario.us_ident_usu);
      ret = -1;
   }

   calado = 0;

   return(ret);
}



/*#  --------------------------------------------------------------------- */
F__LOC   COUNT add_grp()
{  COUNT ret;

   calado = 1; /* nao imprime mensagens intermediarias */
   if ( load_rec(ace_dnum, ace_inum, US_ID, grupo.gr_ident_grp, &usuario, sizeof(Usuario), '=') == NULL) {
	   if ( ! add_rec(ace_dnum, ace_inum, GR_ID, grupo.gr_ident_grp,
                  &grupo, sizeof(Grupo))) {
         fprintf(stderr, "Erro incluindo grupo : %s.\n", grupo.gr_ident_grp);
         ret = -1;
      }
   }
   else {
      fprintf(stderr, "grupo invalido : %s. Ja existe como usuario\n", grupo.gr_ident_grp);
      ret = -1;
   }

   calado = 0;

   return(ret);
}

/*#  --------------------------------------------------------------------- */
F__LOC   COUNT add_base()
{  COUNT ret;
   Base bx;

   calado = 1; /* nao imprime mensagens intermediarias */
   if ( load_rec(ace_dnum, ace_inum, D0_ID, base.bs_nome, &bx, sizeof(Base), '=') == NULL) {
	   if ( ! add_rec(ace_dnum, ace_inum, D0_ID, base.bs_nome,
                  &base, sizeof(Base))) {
         fprintf(stderr, "Erro incluindo base : %s.\n", base.bs_nome);
         ret = -1;
      }
   }
   else {
      fprintf(stderr, "Base ja existe : %s.\n", base.bs_nome);
      ret = -1;
   }

   calado = 0;

   return(ret);
}

/*#  --------------------------------------------------------------------- */
COUNT add_acesso(info)
TEXT *info;
{  COUNT ret = 0;
   LONG pos;
   TEXT chave[100];
   TEXT    nome_base[TB + 1];

   strcpy(nome_base, acesso.da_base);

   calado = 1; /* nao imprime mensagens intermediarias */


	sprintf(chave, "%s!%s", nome_base, acesso.da_ugnome);

   if ( (pos = add_rec(ace_dnum, ace_inum, D1_ID, chave, &acesso, sizeof(Acesso))) == (POINTER)0 ) {
      fprintf(stderr, "Erro na inclusao de acesso: %s.\n", info);
		ret = -1 ;
		goto fim;
	}

   /* inclui chave no indice */
   sprintf(chave, "%s!%s", acesso.da_ugnome, nome_base);
	if ( add_ind(ace_dnum, ace_inum, D2_ID, chave, pos) != 0 ) {
      fprintf(stderr, "Erro na inclusao de indice em acesso: %s.\n", info);
		ret = -1 ;
		goto fim;
	}

   calado = 0;

fim :

   return(ret);
}

#ifdef DDD
F__LOC    COUNT inc_prot(identific)
TEXT *identific;
{  COUNT ret, stat_loc;

   stat_loc = 0;

	if ( val_indice(identific) != OK) {
      fprintf(stderr, "Indice invalido : %s\n", identific);
		ret = -1;
      goto fim;
   }

	/* verifica se identificacao e valida */

   if ( load_rec(ace_dnum, ace_inum, US_ID, identific, &usuario, sizeof(Usuario), '=') != NULL) {
      stat_loc = usuario.us_status[0];
   }
   else {
      if ( load_rec(ace_dnum, ace_inum, GR_ID, identific, &grupo, sizeof(Grupo), '=') != NULL ) {
         stat_loc = 'C'; /*** grupo.gr_status[0]; ****/
      }
   }

	if ( stat_loc == 0 ) {
      fprintf(stderr, "Identificacao inexistente : %s\n", identific);
      ret = -1;
      goto fim;
   }

	if ( stat_loc == 'A' ) {
      fprintf(stderr, "Identificacao e' de administrador : %s\n", identific);
      ret = -1;
      goto fim;
   }

   ret = 0;

fim :

   return (ret);
}


/*******************************************************************/
F__LOC    COUNT prt_save(vet_exc, ind_exc )
Acesso **vet_exc;
COUNT *ind_exc;
{  POINTER 	pos;
	COUNT 	nc, ret;
	Acesso  *pc;
   TEXT    nome_base[TB + 1];
	TEXT 	  chave[LB_KEYMAX];
   WIN *jm = NULL;

   strcpy(nome_base, WBS->bs_nome);

   if ( if_prt_mod( *ind_exc ) == 0 ) { /* nenhuma alteracao */
      ret = 0;
      goto fim;
   }

   /*
   **    Processa protecoes excluidas
   */

   jm = abre_mensagem( M_DB3_WAIT );

   for ( nc = 0 ; nc < *ind_exc ; nc++) {

		pc = vet_exc[nc]; /* pc aponta para protecao excluida */

      /* primeiro exclui D2 = (IDENT + BASE ), so indice */
	   sprintf(chave, "%s!%s", pc->da_ugnome, nome_base);
      pos = exc_rec(ace_dnum, ace_inum, D2_ID, chave, SO_INDEX);
	   if (pos == 0) {
         mens_erro(H_DB3EXC, E_DB3_EXC);
		   ret = -1 ;
         goto fim;
      }

      /* exclui D1 do indice e exclui registro associado */
	   sprintf(chave, "%s!%s", nome_base, pc->da_ugnome);
      pos = exc_rec(ace_dnum, ace_inum, D1_ID,  chave, !SO_INDEX);
	   if (pos == 0) {
         mens_erro(H_DB3EXC, E_DB3_EXC);
		   ret = -1 ;
         goto fim;
      }
   }

   /* restaura condicao de nenhuma protecao na lista de excluidas */
   *ind_exc = 0;


   /*
   **    ATUALIZACAO DA DESCRICAO DAS PROTECOES
   */

	/* atualizacao de protecoes efetivas ( nao deletadas ) */


   for ( nc = 0; nc < num_prot && vtprot[nc] != NULL ; nc++) {

		pc = vtprot[nc]; /* pc aponta para protecao */

		switch ( pc->da_mod_flag ) {

         case 2 : /* modificadas caracteristicas da protecao */

                  /* primeiro exclui D2 = (IDENT + BASE ), so indice */
	               sprintf(chave, "%s!%s", pc->da_ugnome, nome_base);
                  pos = exc_rec(ace_dnum, ace_inum, D2_ID,  chave, SO_INDEX);
	               if (pos == 0) {
                     mens_erro(H_DB3EXC, E_DB3_EXC);
		               ret = -1 ;
                     goto fim;
                  }

                  /* exclui D1 do indice e exclui registro associado */
	               sprintf(chave, "%s!%s", nome_base, pc->da_ugnome);
                  pos = exc_rec(ace_dnum, ace_inum, D1_ID,  chave, !SO_INDEX);
	               if (pos == 0) {
                     mens_erro(H_DB3EXC, E_DB3_EXC);
		               ret = -1 ;
                     goto fim;
                  }

                  /* nao tem break, tem que passar no proximo case */

         case 1 : /* incluido     */


                 strcpy(pc->da_tpreg, DA_ID);

                 /* inclui registro e indice D1 */
					  sprintf(chave, "%s!%s", nome_base, pc->da_ugnome);
                 strcpy(pc->da_tpreg, DA_ID);
					  if ( (pos = add_rec(ace_dnum, ace_inum, D1_ID, chave, pc, sizeof(Acesso))) == (POINTER)0 ) {
                    mens_erro(H_DB3INC, E_DB3_INC);
						  ret = -1 ;
						  goto fim;
					  }

                 /* inclui chave no indice */
                 sprintf(chave, "%s!%s", pc->da_ugnome, nome_base);
					  if ( add_ind(ace_dnum, ace_inum, D2_ID, chave, pos) != 0 ) {
                    mens_erro(H_DB3INC, E_DB3_INC);
						  ret = -1 ;
						  goto fim;
					  }
					  break;


         case 0 : /* Nao alterado */
                  break;

         case 3 : numdebug(784);
                  fim_lb("100");
                  break;

         default : numdebug(785); /* Flag de modificacao nao prevista em prt_save */
                   ret = -1;
                   goto fim;
      }
   }

   ret = 0;

fim :

   fecha_mensagem(jm);

   return(ret);
}




F__LOC   VOID seta_da(p)
Acesso *p;
{	COUNT i;
   strcpy (p->da_ugnome   , pw->ident  );

	p->da_dfcampo  = pw->campo[0] ;
	p->da_dfvista  = pw->vista[0] ;
	p->da_dfacesso = pw->acesso[0];
	p->da_dfrel    = pw->relat[0] ;
	p->da_dfstop   = pw->stop[0]  ;

	p->da_inclui   = pw->inclui[0]  ;
	p->da_altera   = pw->altera[0]  ;
	p->da_exclui   = pw->exclui[0]  ;
	p->da_pesquisa = pw->pesq[0]    ;
	p->da_emite    = pw->emite[0]   ;
	p->da_exporta  = pw->exporta[0] ;
	p->da_importa  = pw->importa[0] ;
	p->da_parcial  = pw->indexa[0]  ;
	p->da_total    = pw->total[0];
	p->da_backup   = pw->backup[0]  ;
	p->da_restore  = pw->restore[0] ;

   for (i = 0; i < MAX_VISTAS; i++) {
       p->da_vistas[i] = pw->vistas[i];
       p->da_rel[i] = pw->relats[i];
   }

}

F__LOC   VOID zera_pw()
{  COUNT i;

	memset((TEXT *)pw, '\0', sizeof(pw));

   for (i = 0; i < MAX_VISTAS; i++) {
       pw->vistas[i] = 0;
       pw->relats[i] = 0;
   }
}
#endif



#ifdef LIXO
COUNT   piece( s1, s2, c, n, tam )
TEXT    *s1, *s2;
COUNT    c, n, tam;
{
   UTEXT   *p1,*p2;
   COUNT   oc;

   oc = n - 1;   p1 = (UTEXT *) s1;   p2 = (UTEXT *) s2;

   while( oc > 0  &&  *p1 != '\0' ) {   if ( *p1++  ==  c )  oc--;  }

   if ( oc == 0 )
   {
      while( tam-- > 0 && *p1 != c && *p1 != '\0' ) {  *p2++ = *p1++;  }
   }

   *p2 = '\0';

   return( strlen( s2 ) );
}
#endif

