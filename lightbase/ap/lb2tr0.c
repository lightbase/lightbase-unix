#include  "lb2all.h"

#ifndef MONO_USUARIO   /* Vai ate o final do arquivo */

#ifdef    VAXVMS
#include  <file.h>
#else
#include  <fcntl.h>
#endif

#include  <string.h>
#include  <time.h>

#ifdef UNIX
#include  <unistd.h>
#endif

#ifdef MSDOS
#include  <dos.h>
#endif

#include  HD_IO

#ifdef    VAXVMS
#include  <types.h>
#include  <stat.h>
#else
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2stru.h"
#include  "lb2trans.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2prot1.h"

#define  H_REGUTI    "REGUTIL"  /* registro em utilizacao. Nao pode editar */
#define  H_RTLOCK   "OPC_LOCK"  /* opcoes de atuacao no caso de nao conseguir lock */
#define  H_WBASEEXC "WAITBASE"  /* Esperando acesso a base */

#define   BASE_EM_ATUALIZACAO  (ap_trcabec->tr_pesquisa == 255)
#define   BASE_DISPONIVEL      (ap_trcabec->tr_pesquisa == 0)

COUNT tr_funcao;
COUNT ps_incrementou;
COUNT ag_incrementou;

#ifdef MSDOS
   COUNT ver_registro(LONG, COUNT);
   COUNT le_controle( VOID );
   COUNT grava_controle( VOID );
   COUNT lock_controle( COUNT );
   COUNT unlock_controle( VOID );
   VOID  FLUSFILES( VOID );
   VOID  RECALFILES( VOID );
   COUNT fazer_oque(VOID);
   VOID  indica_uso(COUNT, TEXT *, TEXT *);
#else
   COUNT ver_registro( );
   COUNT le_controle( );
   COUNT grava_controle( );
   COUNT lock_controle( );
   COUNT unlock_controle( );
   VOID  FLUSFILES( );
   VOID  RECALFILES( );
   COUNT fazer_oque( );
   VOID  indica_uso( );
#endif

/*   -1 -> Operacao negada */
/*   */
/*********************************************************************/
/*     BEG_TRANS                                                     */
/*********************************************************************/
F__GLB   COUNT BEG_TRANS(funcao, nreg, lock)
COUNT funcao;
LONG nreg;
COUNT lock;

{  COUNT ret, resp, tentativa = 0, n = 0;
   time_t secsnow;

   ag_incrementou = 0;
   ps_incrementou = 0;
   ret = 0;

   if ( ap_trcabec->tr_status == 'E' || ap_trcabec->tr_status == 'T' )
      goto fim;

   if ( ap_trcabec->tr_ident != ap_trident ) {
      fim_lb(E_CNTLOCK);
      exit(97);
   }

   switch(funcao) {
      case TR_CARREGAR :   /*  */
               while ( 1 ) {
                  le_controle();

                  if ( ap_trcabec->tr_aguarde > 0 ) {
                     TEXT aux[20];
                     sprintf(aux, "(%d)" , ap_trcabec->tr_aguarde);
                     indica_uso(++tentativa, "CARREGAR", aux);
                     continue;
                  }

                  if ( ! BASE_EM_ATUALIZACAO ) {
                     if ( lock_controle(OPCIONAL) == 0 ) {
                        le_controle();
                        if ( BASE_EM_ATUALIZACAO ) {
                           unlock_controle();
                           continue;
                        }
                        else {
                           ap_trcabec->tr_pesquisa++;
                           grava_controle();
                           ps_incrementou = 1;
                           RECALFILES();
                           unlock_controle();
                           break;
                        }
                     }
                     else {
                        ret = -1;
                        goto fim;
                     }
                  }
               }

               break;



      case TR_READRECORD :   /*  */
            while ( 1 ) {
               le_controle();

               if ( ap_trcabec->tr_aguarde > 0 ) {
                  TEXT aux[20];
                  sprintf(aux, "(%d)" , ap_trcabec->tr_aguarde);
                  indica_uso(++tentativa, "READ(c/ag)", aux);
                  continue;
               }

               if ( ! BASE_EM_ATUALIZACAO ) {
                  if ( lock_controle(OPCIONAL) == 0 ) {
                     le_controle();
                     if ( BASE_EM_ATUALIZACAO ) {
                        unlock_controle();
                        continue;
                     }
                     else {
                        if ( ver_registro(nreg, lock) != 0 ) {
                           TEXT linha[50], linha2[50];
                           sprintf(linha, W_NOREGLOCK, nreg);
                           sprintf(linha2, W_USULOCKING, usu_usando);

                           unlock_controle();
                           resp = decida( M_ATENCAO, TW_RDA9, H_REGUTI,
                                          2, 35, "%s\n%s", linha, linha2);
                           switch(resp) {
                              default :
                              case  1 : /* desiste */
                                       ret = -98;
                                       goto fim;
                              case  0 : /* Retenta */
                                       break;
                              case  2 : /* aborta */
                                       fim_lb(M_USCANCELA);
                                       exit(100);
                           }
                           continue;
                        }

                        if ( ap_trind < 0 || ap_trind >= ap_max_users )
                           debug("ap_trind invalido : %d", ap_trind);

                        ap_trusu[ap_trind].trus_reg  = nreg;
                        ap_trusu[ap_trind].trus_modo = (lock) ? 'E' : 'C';

                        grava_controle();
                        RECALFILES();
                        unlock_controle();
                        break;
                     }
                  }
                  else {
                     ret = -1;
                     goto fim;
                  }
               }
               else {
                  if ( ++n % 10 == 0 ) {
                     indica_uso(++tentativa, "READ(atu)", "");
                  }
               }
            }

            break;

      case TR_FREERECORD : /*  */
                           lock_controle(OBRIGATORIO);
                           le_controle();

                           if ( ap_trind < 0 || ap_trind >= ap_max_users )
                              debug("ap_trind invalido : %d", ap_trind);

                           ap_trusu[ap_trind].trus_reg  = 0L;
                           ap_trusu[ap_trind].trus_modo = 0L;
                           grava_controle();
                           unlock_controle();

                           break;

      case TR_SALVATXT  :   /*  */
      case TR_PALINDEX  :   /*  */
      case TR_PALDINDEX :   /*  */
      case TR_FLUSHIND  :   /*  */
      case TR_DELRECORD :   /*  */
      case TR_NEWRECORD :   /*  */
      case TR_WRRECORD  :   /*  */

               if ( tr_funcao != 0 )  /* base ja esta sob controle da funcao especificada */
                  break;

               while ( 1 ) {

                  le_controle();

                  if ( ! BASE_DISPONIVEL && ap_trcabec->tr_pid == ap_trpid )
                     break;

                  if ( BASE_DISPONIVEL ) {

                     if ( lock_controle(OPCIONAL) == 0 ) {
                        le_controle();

                        if ( ! BASE_DISPONIVEL ) {
                           unlock_controle();
                           continue;
                        }
                        else {
                           if ( ag_incrementou ) {
                              ap_trcabec->tr_aguarde--;
                              ag_incrementou = 0;
                           }

                           time(&secsnow);
                           ap_trcabec->tr_pesquisa = 255;
                           ap_trcabec->tr_pid      = ap_trpid;
                           ap_trcabec->tr_time     = secsnow;
                           tr_funcao   = funcao;

                           grava_controle();
                           RECALFILES();
                           unlock_controle();
                           break;
                        }
                     }
                     else {
                        ret = -1;
                        goto fim;
                     }
                  }
                  else {
                     if ( ag_incrementou == 0 ) {
                        lock_controle(OBRIGATORIO);
                        le_controle();
                        ap_trcabec->tr_aguarde++;
                        ag_incrementou = 1;
                        grava_controle();
                        unlock_controle();
                     }
                     if ( ++n % 20 == 0 ) {
                        TEXT aux[20], aux2[30];
                        sprintf(aux, "MULT (%c)" , funcao);
                        sprintf(aux2, "(ag=%d)" , ap_trcabec->tr_aguarde);
                        indica_uso(++tentativa, aux, aux2);
                     }
                  }
               }

               break;
   }

fim :

   if ( ag_incrementou ) {
      lock_controle(OBRIGATORIO);
      le_controle();
      ap_trcabec->tr_aguarde--;
      ag_incrementou = 0;
      grava_controle();
      unlock_controle();
   }

   return(ret);
}


/*********************************************************************/
/*     END_TRANS                                                     */
/*********************************************************************/
F__GLB   COUNT END_TRANS(funcao)
COUNT funcao;
{  COUNT ret;

   ret = 0;

   if ( ap_trcabec->tr_status == 'E' || ap_trcabec->tr_status == 'T' )
      goto fim;

   if ( ap_trcabec->tr_ident != ap_trident ) {
      fim_lb(E_CNTLOCK);
      exit(97);
   }

   switch(funcao) {
      case TR_CARREGAR :   /*  */
                           lock_controle(OBRIGATORIO);
                           le_controle();
                           ap_trcabec->tr_pesquisa--;
                           grava_controle();
                           ps_incrementou = 0;
                           FLUSFILES();
                           unlock_controle();

                           break;

      case TR_SALVATXT  :   /*  */
      case TR_PALINDEX  :   /*  */
      case TR_PALDINDEX :   /*  */
      case TR_FLUSHIND  :   /*  */
      case TR_DELRECORD :   /*  */
      case TR_NEWRECORD :  /*  */
      case TR_WRRECORD  :  /*  */

                           if ( funcao != tr_funcao )
                              break;

                           lock_controle(OBRIGATORIO);
                           le_controle();


                           ap_trcabec->tr_pesquisa = 0;
                           ap_trcabec->tr_pid      = 0L;
                           ap_trcabec->tr_time     = 0L;
                           tr_funcao   = 0;

                           if ( ap_trind < 0 || ap_trind >= ap_max_users )
                              debug("ap_trind invalido : %d", ap_trind);

                           if ( funcao == TR_WRRECORD )
                              ap_trusu[ap_trind].trus_modo = 'C';

                           grava_controle();
                           FLUSFILES();
                           unlock_controle();

                           break;


   }

fim :

   return(ret);
}

/*********************************************************************/
/*     ABORT_TRANS                                                     */
/*********************************************************************/
F__GLB   VOID ABORT_TRANS()
{
   if ( ap_trfile != -1 && ag_incrementou || ps_incrementou )  {
      lock_controle(OBRIGATORIO);
      le_controle();
      if ( ps_incrementou ) {
         ap_trcabec->tr_pesquisa--;
         ps_incrementou = 0;
      }
      if ( ag_incrementou ) {
         ap_trcabec->tr_aguarde--;
         ag_incrementou = 0;
      }
      grava_controle();
      unlock_controle();
   }
}


F__LOC   COUNT ver_registro(nreg, lock)
LONG nreg;
COUNT lock;
{  COUNT ret, i;

   ret = 0;

   if ( ap_ignorauso ) /* setado em relatorio e exportacao */
      goto fim;

   for ( i = 0 ; i < ap_max_users; i++ ) {

      if ( ap_trusu[i].trus_pid != 0L ) {
         if ( i != ap_trind && ap_trusu[i].trus_reg == nreg ) {
            if ( lock ) {
               ret = -1;
               break;
            }
            else
               if ( ap_trusu[i].trus_modo == 'E' ) {
                  ret = -1;
                  break;
               }
         }
      }
   }

   if ( ret < 0 ) {
      if ( i < 0 || i >= ap_max_users )
         debug("ap_trind(i) invalido : %d", i);

   	strcpy(usu_usando, ap_trusu[i].trus_nome);
   }

fim:

   return(ret);
}


F__LOC   COUNT lock_controle(obrigatorio)
COUNT obrigatorio;
{  COUNT ret=0, resp, lock_ok, tenta = 0;

   if ( ap_ignorauso == 9 ) /* setado pela funcao encerra */
      goto fim;

   lseek(ap_trfile, 0L, 0);

   lock_ok = -1;

   while ( lock_ok != 0 ) {

      lseek(ap_trfile, 0L, 0);
#ifdef MSDOS
      lock_ok = lock(ap_trfile, 0L, TR_TAM);
#else
#ifndef VAXVMS
      lock_ok = lockf(ap_trfile, F_TLOCK, 0L);
#endif
#endif
      if ( lock_ok != 0 ) {
         tenta++;
         if ( tenta % 5 == 0 )
            sleep(1);
      	if ( tenta % 30  == 0 )  {
            if ( obrigatorio == OBRIGATORIO )
               resp = 0;
            else
               resp = decida( M_ATENCAO, TW_RDA9, H_RTLOCK, 2, 30,
                                "%s", W_ATUALIZA);
            switch(resp) {
               default :
               case  1 : /* desiste */
                         ret = -1;
                         goto fim;
               case  0 : /* Retenta */
                         break;
               case  2 : /* aborta */
                         fim_lb(M_USCANCELA);
                         exit(100);
            }
      	}
      }
   }

fim :

   return(ret);
}

F__LOC   COUNT le_controle()
{  COUNT ret=0, tenta = 0;

   if ( ap_ignorauso == 9 ) /* setado pela funcao encerra */
      goto fim;

   lseek(ap_trfile, 0L, 0);
   while ( read(ap_trfile, (TEXT *)ap_trcabec, TR_TAM) != TR_TAM ) {
      tenta++;
      if ( tenta % 5 == 0 )
         sleep(2);
      if ( tenta % 30 == 0 )
         if ( fazer_oque( ) == 2 ) {
            fim_lb(M_USCANCELA);
            break;
         }
      lseek(ap_trfile, 0L, 0);
   }

fim :

   return(ret);
}

F__LOC   COUNT grava_controle()
{  COUNT ret=0, tenta = 0, gravou;

   if ( ap_ignorauso == 9 ) /* setado pela funcao encerra */
      goto fim;

   while ( 1 ) {
      lseek(ap_trfile, 0L, 0);
      gravou = write(ap_trfile, (TEXT *)ap_trcabec, TR_TAM);

      if (gravou == TR_TAM )
         break;
      if ( tenta++ % 50 == 0 )
         if ( fazer_oque( ) == 2 ) {
            fim_lb(M_USCANCELA);
         }
   }

fim:

   return(ret);
}

F__LOC   COUNT unlock_controle()
{  COUNT ret;

   if ( ap_ignorauso == 9 ) /* setado pela funcao encerra */
      goto fim;

   lseek(ap_trfile, 0L, 0);

#ifdef MSDOS
      ret = unlock(ap_trfile, 0L, TR_TAM);
#else
#ifndef VAXVMS
      ret = lockf(ap_trfile, F_ULOCK, 0L);
#endif
#endif

   if ( ret != 0 ) {
      fim_lb("Situacao Nao prevista. Remova arquivo de LOCK.");
   }

fim:

   return(ret);
}

F__LOC COUNT fazer_oque(  )
{	COUNT ret = 0, resp;

    /* Desiste de tentar */
    if ( w_kbhit(curr_win) && fwnqgc(curr_win) == K_ESC ) {
       resp = decida( M_ATENCAO, TW_RDA9, H_WBASEEXC,
                      2, 62, "%s", W_FAZEROQUE);
       switch(resp) {
          default : /* retenta */
                   ret = -1;
                   break;
          case  1 : /* aborta */
                   ret = 2;
                   break;
       }
    }

    return(ret);
}

F__LOC   VOID FLUSFILES()
{
      FLUSHFIL(AC_NUM(BASE_EDIT));
      FLUSHFIL(AD_NUM(BASE_EDIT));
      FLUSHFIL(AI_NUM(BASE_EDIT));
}


F__LOC   VOID RECALFILES()
{
      RECALFIL(AC_NUM(BASE_EDIT));
      RECALFIL(AD_NUM(BASE_EDIT));
      RECALFIL(AI_NUM(BASE_EDIT));
}


#endif



VOID indica_uso(n, loc, str)
COUNT n;
TEXT *loc, *str;

{  WIN *jm;
   TEXT mens[60];
   extern COUNT ap_xxdebug;

   if ( ap_xxdebug & 0x1 )
      sprintf(mens, "%s %d - %s %s", M_AGUARDE, n, loc, str);
   else
      sprintf(mens, "%s %d", M_AGUARDE, n);


   jm = abre_mensagem(mens);
   sleep(1);
   fecha_mensagem(jm);

   if ( w_kbhit(curr_win) && fwnqgc(curr_win) == K_F3 ) {
      /* so para parar por aborta */
   }

   return;

}
