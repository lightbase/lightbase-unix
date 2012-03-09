#include  "lb2all.h"

#include  <time.h>
#include  <string.h>
#include  HD_MEMO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "is_pers.h"

#define   GRP_CADUSU    13

#define   US_ID         "US"
#define   MAX_TENTA     5
#define   MAXSERIE      11

#define   H_ACNOT   "ACESSNOT"    /* Cadastro de acessos inexistente */
#define   H_ACBAD   "ACESSBAD"    /* Cadastro de acessos danificado  */
#define   H_NUMSE   "NUMSERIE"    /* Numero de serie */
#define   H_CHATV   "CHAVEATV"    /* Chave de ativacao */

#define   H_DELAC   "DELACESS"    /* Elimina cadastro de acessos */
#define   H_CFDAC   "CFDELACE"    /* Confirma exclusao do cadastro de acessos */
#define   H_ERDAC   "ERDELACE"    /* Erro na exclusao do cadastro de acessos */
#define   H_CRDAC   "ERCRIACE"    /* Erro na criacao do cadastro de acessos */


#define   TW_US00  loc_str(GRP_CADUSU,  1)
#define   TW_US99  loc_str(GRP_CADUSU,  2)
#define   TW_US01  loc_str(GRP_CADUSU,  3)
#define   TW_US02  loc_str(GRP_CADUSU,  4)
#define   TW_US03  loc_str(GRP_CADUSU,  5)
#define   TW_US04  loc_str(GRP_CADUSU,  6)
#define   TW_US05  loc_str(GRP_CADUSU,  7)
#define   TW_US06  loc_str(GRP_CADUSU,  8)

#define   R_NUMSE  loc_str(GRP_CADUSU,  9)
#define   R_CHATV  loc_str(GRP_CADUSU, 10)
#define   T_NUMSE  loc_str(GRP_CADUSU, 11)
#define   T_CHATV  loc_str(GRP_CADUSU, 12)

#define   TW_US20  loc_str(GRP_CADUSU, 13)
#define   TW_US21  loc_str(GRP_CADUSU, 14)
#define   TW_US22  loc_str(GRP_CADUSU, 15)
#define   TW_US23  loc_str(GRP_CADUSU, 16)
#define   TW_US24  loc_str(GRP_CADUSU, 17)
#define   TW_US29  loc_str(GRP_CADUSU, 18)

#define   E_NOSERIE  loc_str(GRP_CADUSU, 504)
#define   E_BADSERIE  loc_str(GRP_CADUSU, 505)


#ifdef MSDOS
   COUNT mt_cadusu( COUNT);
   COUNT cr_acessos( COUNT, COUNT);
   COUNT mt_acessos( COUNT, COUNT);
   COUNT permissao( TEXT *, TEXT * );
   VOID  encrypt( UTEXT * semente, UTEXT * buffer );
	int crypt_key ( TEXT *, TEXT * );
#else
   COUNT mt_cadusu( );
   COUNT cr_acessos( );
   COUNT mt_acessos( );
   COUNT permissao( );
   VOID  encrypt( );
	int crypt_key ( );
#endif



/*#  ---------------------------------------------------------------------
**#   Funcao...: open_acessos
**#   Objetivo.: abrir cadastro de acessos
**#   Entrada..: perm_edit -> 0 - abre so pra leitura
**#                        -> 1 - abre permitindo alteracao
**#   Retorna..:
**#
**/
F__GLB   COUNT open_acessos( perm_edit, serie, magico, rebuild)
COUNT perm_edit;   /* flag de edicao. Abre para alterar */
LONG serie;
UCOUNT magico;
COUNT rebuild;
{
   COUNT ret, ret1, ret2, modo, up_grp;
	TEXT cadastro[PATH_MAX], *msg_erro;
	TEXT indice[PATH_MAX];

   perm_edit = perm_edit;

   msg_erro = cadastro; /* compartilhar area */

   ret = 0;
   up_grp = 0;

   if ( rebuild && (ret = mt_cadusu(rebuild)) != 0 ) {
      goto fim;
   }

   if ( ap_max_users == 0 )
      modo = VIRTUAL | EXCLUSIVE;
   else
      modo = VIRTUAL | SHARED;


   ret1 = info_base(USU_FILE, ap_home_lb, USU_CAD, cadastro);
   ret2 = info_base(USU_FILE, ap_home_lb, USU_EXT, indice);

   if ( ret1 > 0 && ret2 > 0 ) { /* Existem arquivos para abrir */
      ret = OPNFIL(ace_dnum, cadastro, modo, serie, magico);

	   if ( ret == 0 ) {
	      LONG retl;

         retl = PEGA_SERIE(ace_dnum);

         if ( retl != 1 && retl != serie ) {
            mens_erro("", E_NOSERIE);
            CLSFIL(ace_dnum, COMPLETE); /* fecha cadastro ja aberto */
            ret = -1;
         }
   	   else if ( (ret = OPNFIL(ace_inum, indice, modo, serie, magico)) != 0 ) {
            CLSFIL(ace_dnum, COMPLETE); /* fecha cadastro ja aberto */
            ret = -1;
         }
      }
      else
         ret = -1;
   }
   else {
      ret = -1;
   }

   if ( ret != 0 ) /* alguma mensagem vai ser impressa */
      if ( stat_str(GRP_CADUSU) == 0 )
         up_grp = up_str(GRP_CADUSU);

   switch(ret) {
      case 0  :   /* Arquivos abertos OK */
                  break;

      case -1 :   /* Arquivo de acessos nao localizado */
                  decida( TW_US00, str_nada, H_ACNOT, 7, 42,
                          "%s\n%s\n%s\n%s\n\n%s\n%s", TW_US01, str_nada,
                           TW_US03, TW_US04, str_nada, TW_US05 );
                  break;

      default :   /* Arquivo nao foi fechado normalmente  */

                  sprintf(msg_erro, "%s %d", TW_US99, ret);

                  decida( TW_US00, msg_erro, H_ACBAD, 7, 42,
                          "%s\n%s\n%s\n%s\n\n%s\n%s", TW_US02, str_nada,
                           TW_US03, TW_US04, str_nada, TW_US06 );
                  break;

   }

fim :

   if ( up_grp )
      down_str(GRP_CADUSU);

   return(ret);
}




F__GLB   COUNT t_numse(nome, senha, num)
TEXT *nome, *senha;
LONG num;
{  COUNT ret = !OK;
   ULONG serie;
	TEXT cripserie[100];
   if ( strlen(nome) == MAXSERIE ) {  /* numeros de serie da infocon */
   	SI dd, mm, aa;
      if ( pe_data(&aa,&mm,&dd) != NULL ) {
      	sprintf(cripserie, "%2.2d%2.2d%2.2d%s", aa, mm, dd, nome+6); }
      else
         if ( pe_tty () != 999  ) {
         	TEXT aux[10];
         	sprintf(aux, "%03d", pe_tty());
	      	strcpy(cripserie, nome);
         	memcpy(cripserie+2, aux, 3);
         }
         else
	      	strcpy(cripserie, nome);

      strlwr(cripserie);
      crypt_key(cripserie, cripserie + MAXSERIE);
      if ( stricmp(cripserie, senha) != 0 ||
         ( num != 0 && (atol(nome+6) + 100000L != num))) {
         ret = !OK;
      }
      else {
         ret = OK;
      }
   }
   else { /* numeros de serie da light */
      serie = atol(nome);
      if ( serie > 0 ) {
         if ( num != 0 && serie != num ) {
            ret = !OK;
            goto fim;
         }
         if ( permissao( nome, senha ) ) {
            ret = OK;
         }
      }
	}

fim:

   return(ret);
}



F__LOC   COUNT mt_cadusu(flag)
COUNT flag;
{
   COUNT ret = -2;         /* para sai do loop de leitura por limite */
   COUNT conta_tenta = 0;  /* numero de tentativas para acertar senha */
	TEXT usuario[20], senha[20];
	WIN *j1 = NULL;
	DTela vi[3];
	LONG num_tmp;

   up_str(GRP_CADUSU);

   /* num_serie / ativacao  */
   seta_dtela(&vi[0], usuario, X14, R_NUMSE, 0, 12, 0, E_ALFA, H_NUMSE, NULL, NULL, NULL);
   seta_dtela(&vi[1], senha,   X14, R_CHATV, 1, 12, 0, E_ALFA, H_CHATV, NULL, NULL, NULL);
   seta_dtela(&vi[2], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);


   j1 = abre_janela(NUMSE_WIN);

	fwbox(j1);

	mvwprintw(j1, 0, 0, T_NUMSE);  /* Serie */
	mvwprintw(j1, 1, 0, T_CHATV);  /* ativacao */


   while ( conta_tenta++ < MAX_TENTA ) {

      INSERT_OFF();

      usuario[0] = senha[0] = '\0';

      ret = ed_read(j1, &vi[0], 1, LIMPA_LE, "1 E");

	   if ( ret < 0 || ret == K_ESC ) {  /* erro ou ESC */
         break;
      }

   	if ( strlen(usuario) == MAXSERIE )   /* numeros de serie da infocon */
	      num_tmp = atol(usuario + 6) + 100000L;
	   else
	      num_tmp = atol(usuario);

		if ( num_tmp != ap_nserie ) {
	      mens_erro("", E_BADSERIE);
	      ret = !OK;
	   }

      else if ( t_numse(usuario, senha, ap_nserie) == OK ||
           ap_nserie < 100 ) {
         ret = OK;
         break;
      }

      else ret = !OK;

   }

	fwkill(j1);

   if ( ret == OK ) {  /* identificacao de numero de serie OK */
      if ( flag == 'I' || flag == 'i' )
         ret = cr_acessos(ace_dnum, ace_inum);
      else if ( flag == 'M' || flag == 'm' )
         ret = mt_acessos(ace_dnum, ace_inum);
   }

   down_str(GRP_CADUSU);
   return(ret);
}




F__LOC   COUNT cr_acessos(datno, indno)
COUNT datno, indno;
{
   COUNT ret = OK, ret1, ret2;
   LONG num_magico;
	TEXT cadastro[PATH_MAX];
	TEXT indice[PATH_MAX];

   /* menssagens carregadas por mt_cadusu */

   num_magico = 0;

   ret1 = info_base(USU_FILE, ap_home_lb, USU_CAD, cadastro);
   ret2 = info_base(USU_FILE, ap_home_lb, USU_EXT, indice);

   if ( ret1 == 0 && ret2 == 0 )
      goto cria;

   if ( decida( TW_US20, TW_US29, H_CFDAC, 4, 42, "%s\n%s\n%s\n%s",
                TW_US21, TW_US22, TW_US23, TW_US24 ) != 1 ) {
      ret = -1;
      goto fim;
   }


   /*  Elimina arquivo de acessos  */
   switch ( ret1 ) {
      case  0  :  break;
      default  :  if ( DELFILE(cadastro) != NO_ERROR ) {
                     mens_erro(H_ERDAC, E_DLF_SO, cadastro);
                     ret = -1;
                     goto fim;
                  }
   }

   /*  Elimina arquivo de indice de acessos  */
   switch ( ret2 ) {
      case  0  :  break;
      default  :  if ( DELFILE(indice) != NO_ERROR ) {
                     mens_erro(H_ERDAC, E_DLF_SO, indice);
                     ret = -1;
                     goto fim;
                  }
   }

cria:
	if ( strcmp("serie", pe_serie()) == 0 ) { /* sem perrsonalizar */
	   ct_sernum = 0;
	}
	else {
		UTEXT serie[20], *p;
		strcpy((TEXT *)serie, pe_serie());
		p = (UTEXT *)&ct_sernum;
		*p++ = serie[2];
		*p++ = serie[3];
		*p++ = serie[4];
		*p   = serie[5];
	}

   ret = CREDAT(datno, cadastro, 0, SLOT, (PERMANENT | EXCLUSIVE | VLENGTH),
                ap_nserie, num_magico);

	if ( ret != NO_ERROR ) {
      mens_erro(H_CRDAC, E_CRF_MA, ret, cadastro);
		ret = -1;
		goto fim;
	}

	ret = CREIDX(indno, indice, LB_KEYMAX, 0, 0,
                SLOT, (PERMANENT | EXCLUSIVE | VLENGTH),
                ap_nserie, num_magico);

   FLUSHFIL(datno);
   FLUSHFIL(indno);

	if ( ret != NO_ERROR ) {
      mens_erro(H_CRDAC, E_CRF_MA, ret, indice);
      CLSFIL(datno, COMPLETE);
		ret = -1;
		goto fim;
	}

#ifndef LITE
   if ( atu_usuario(INCLUSAO) != 0 ) {  /* forca inclusao de um usuario */
      DELFILE(cadastro);
      DELFILE(indice);
		ret = -1;
      goto fim;
   }
#endif

   CLSFIL(datno, COMPLETE);
   CLSFIL(indno, COMPLETE);

fim :

   return(ret);
}


F__LOC   COUNT mt_acessos(datno, indno)
COUNT datno, indno;
{
   COUNT ret = OK;

   datno = datno;
   indno = indno;

   return(ret);
}



/*************************************************************************/
/*************************************************************************/
/***********      ELABORADA PELO WAGNER                    ***************/
/*************************************************************************/
/*************************************************************************/

F__LOC   COUNT permissao( semente, senha )
TEXT *semente, *senha;
{
   TEXT buffer[32];

   trim(senha);
   trim(semente);

   encrypt( (UTEXT *)semente, (UTEXT *)buffer );

   return( (COUNT)!stricmp( senha, buffer ) );
}




F__LOC   VOID encrypt (sem_ori, senha )
UTEXT *sem_ori;
UTEXT *senha;
{
   COUNT  ptr, sigma, tamanho;
   UTEXT semente[32];
   UTEXT valor;

#ifdef MSDOS
   COUNT primeira_fase( UTEXT * );
   VOID  mistura( UTEXT *, UTEXT * );
   COUNT dobra_semente( UTEXT *, UTEXT * );
   COUNT somatoria( UTEXT * );
#else
   COUNT primeira_fase( );
   VOID  mistura( );
   COUNT dobra_semente( );
   COUNT somatoria( );
#endif

   strncpy((TEXT *)semente, (TEXT *)sem_ori, 30);
   semente[30]='\0';

   if (*semente == '\0') {                       /* handle a null string    */
     *senha = '\0';
     return;
   }
   sigma = primeira_fase( semente );
   srand( (semente[0] + sigma) * sigma );
   valor = lb_random( (semente[0] + sigma) * 'a' );
   senha[0] = (valor % ('z' - 'a' + 1)) + 'a';
   ptr = 0;
   while ((semente[ptr] != '\0') && (ptr < 8)) {
     valor = ((semente[ptr] + senha[ptr]) << 1) ^ 0x55;
     senha[++ptr] = (valor % ('z' - 'a' + 1)) + 'a';
   }
   while (ptr < 8) {
      if (ptr == 1)
         senha[ptr] = ((senha[0] + 12) % ('z' - 'a' + 1)) + 'a';
      else
         senha[ptr] = (((senha[ptr-1] + senha[ptr-2]) + 12) % ('z' - 'a' + 1)) + 'a';
      ptr++;
   }
   tamanho = (COUNT)strlen( (TEXT *)semente );
   senha[2] = ((senha[2] + semente[tamanho - 1]) % ('z' - 'a' + 1)) + 'a';
   senha[3] = ((senha[3] + semente[tamanho - 2]) % ('z' - 'a' + 1)) + 'a';
   senha[4] = ((senha[4] + semente[tamanho - 4]) % ('z' - 'a' + 1)) + 'a';
   senha[5] = ((senha[5] + semente[tamanho - 3]) % ('z' - 'a' + 1)) + 'a';
   senha[ptr] = '\0';
#if 0
   for (ptr = 0; ptr < 8; ptr++)               /* insure no :'s in cipher */
      if (senha[ptr] == ':') senha[ptr] = 'X';
#endif
}















