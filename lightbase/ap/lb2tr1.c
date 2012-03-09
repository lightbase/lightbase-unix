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

#include  HD_IO

#ifdef    VAXVMS
#include  <types.h>
#include  <stat.h>
#else
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

#define   TR_DEFINE

#include  "lb2files.h"
#include  "lb2stru.h"
#include  "lb2trans.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2prot1.h"


#define  D_RTLOCK   "OPC_LOCK"  /* opcoes de atuacao no caso de nao conseguir lock */


#ifdef MSDOS
   unsigned char ShareCheck(void);
   COUNT lock_controle( COUNT );
   COUNT unlock_controle( VOID );
#else
   COUNT lock_controle( );
   COUNT unlock_controle( );
#endif




#ifdef MSDOS
   COUNT new_status(COUNT);
   COUNT cria_lck(TEXT *);
#else
   COUNT new_status( );
   COUNT cria_lck( );
#endif


TEXT ap_dirlock[50];

/*********************************************************************/
/*     INIC_TRANS                                                    */
/*********************************************************************/
F__GLB   VOID INIC_TRANS()
{  time_t secsnow;

   if ((ap_trcabec = calloc(TR_TAM, 1)) == NULL ) {
      fim_lb(SEM_MEMO);
      exit(13);
   }
#ifdef MSDOS
   if ( ap_max_users > 1 && ((par_cfg.flags_especiais & 0X16) == 0) ) {
      if ( ShareCheck() == 0 ) {
         fim_lb(E_NOSHARE);
      }
   }
#endif
   ap_trusu   = (Tr_usu *)(((TEXT *)ap_trcabec) + sizeof(Tr_cabec));
   ap_trfile  = -1;

   time(&secsnow);
   ap_trpid = (LONG)secsnow;
}




/*********************************************************************/
/*     LOCK_RECURSO          -1 - Erros genericos                    */
/*                           -2 - Excesso de usuarios                */
/*                           -3 - Temporariamente inviavel           */
/*********************************************************************/
F__GLB   COUNT LOCK_RECURSO(base, path, operacao)
TEXT *base, *path;
COUNT operacao;
{  COUNT ret, fecha=0, lock_ok=0;
   static TEXT last_base[TE+1];
   TEXT nx[PATH_MAX+1];

   if ( ap_max_users <= 1 ) { /* funcao nao faz nada se um unico usuario */
      ap_trcabec->tr_status   = 'T';
      ret = 0;
      goto fim;
   }

   if ( ap_trfile > 0 && strcmp(base, last_base) != 0 ) {
      close(ap_trfile);
      lock_ok = 0;
      ap_trfile = -1;
   }

   /* por omissao de atribuicao a ret, a funcao falha */
   ret = -1;

   if ( ap_dirlock[0] == '\0' )
      info_base(base, path, EXT_LOCK, nx);
   else
      info_base(base, ap_dirlock, EXT_LOCK, nx);


   if ( access(nx, 0) != 0 )  /* NAO existe arquivo de lock */
      if ( cria_lck(nx) != 0 )
         goto fim;


   if ( ap_trfile < 0 ) {
#ifdef MSDOS
   ap_trfile = open(nx, (O_BINARY | O_RDWR | O_DENYNONE) , (S_IREAD | S_IWRITE));
#else
   ap_trfile = open(nx, O_RDWR);
#endif
   }

   if ( ap_trfile < 0 )
      goto fim;

/* para aseguarar que arquivo de lock e para o mesmo numero de usuarios permitidos */
{  FILE *fp;
   LONG tam_file;
#ifdef MSDOS
   fp = fopen(nx, "rb");
#else
   fp = fopen(nx, "r");
#endif
   fseek(fp, 0L, 2);
   tam_file = ftell(fp);
   if ( tam_file != TR_TAM ) {
      debug("Tamanho do arquivo de gerencia de lock incompativel (%d) : %ld\nRemova o arquivo de lock (.lck)", TR_TAM, tam_file);
      ret = -1;
      goto sai;
   }
   fclose(fp);
}



   lseek(ap_trfile, 0L, 0);




   if ( lock_controle(OPCIONAL) != 0 ) {
      ret = -1;
      goto fim;
   }

   lock_ok = 1;

   if ( read(ap_trfile, (TEXT *)ap_trcabec, TR_TAM) < 0 )
      goto fim;

   ap_trident = ap_trcabec->tr_ident;


   switch (operacao) {
      case LCK_STATUS :
            ret = 0;
            break;

      case LCK_EXCLUSIVO :
            if ( ap_trcabec->tr_status == 0 ||
                  (ap_trcabec->tr_status == 'T' &&
                     ap_trcabec->tr_pid == ap_trpid) ) {
               ret = new_status(LCK_EXCLUSIVO);
            }
            else
               ret = -3;

            break;

      case LCK_COMPARTILHADO :

            if ( ap_trcabec->tr_status == 'C' || ap_trcabec->tr_status == 0 )
               ret = new_status(LCK_COMPARTILHADO);
            else
               ret = -3;

            break;

      case LCK_LIBERA :

            ret = new_status(LCK_LIBERA);
            fecha = 1;

            break;

      case LCK_TRAVA  :

            if ( ap_trcabec->tr_status == 0 )
               ret = new_status(LCK_TRAVA);
            else
               ret = -3;

            break;

      case LCK_DESTRAVA       :

            ret = new_status(LCK_DESTRAVA);
            fecha = 1;
            break;
   }

sai :

   if ( fecha || ret != 0) {
      close(ap_trfile);
      last_base[0] = '\0';
      lock_ok = 0;
      ap_trfile = -1;
   }
   else {
      strcpy(last_base, base);
   }

fim :

   if ( lock_ok == 1 )
      unlock_controle();

   return(ret);
}


/*********************************************************************/
/*     new_status            -1 - Erros genericos                    */
/*                           -2 - Excesso de usuarios                */
/*********************************************************************/
F__LOC   COUNT new_status(status)
COUNT status;
{  COUNT ret, old_status, i;

   ret = 0;
   old_status = ap_trcabec->tr_status;

   switch(status) {
      case LCK_EXCLUSIVO :  /*  */
                  ap_trind = 0;

                  memset((TEXT*)(ap_trcabec)+4, 0, TR_TAM-4); /* +4 para nao zerar tr_ident */

                  if ( old_status == 'T')
                     ap_trcabec->tr_status   = 'T';
                  else  {
                     ap_trcabec->tr_status   = 'E';
                  }

                  ap_trcabec->tr_logados  = 1;
                  ap_trcabec->tr_pesquisa = 0;
                  ap_trcabec->tr_pid      = ap_trpid;
                  strcpy(ap_trcabec->tr_nome, ap_nome);
                  break;

      case LCK_COMPARTILHADO :
                  ap_trind = -1;

                  for ( i = 0; i < ap_max_users; i++) {
                     if ( ap_trusu[i].trus_pid == 0L ) {
                        ap_trind = i;
                        break;
                     }
                  }

                  if ( ap_trind == -1 ) {
                     ret = -2;
                  }
                  else {
                     ap_trusu[ap_trind].trus_reg  = 0L;
                     ap_trusu[ap_trind].trus_modo = 0;
                     ap_trusu[ap_trind].trus_pid  = ap_trpid;
                     strcpy(ap_trusu[ap_trind].trus_nome, ap_nome);

                     ap_trcabec->tr_status   = 'C';
                     ap_trcabec->tr_logados++;
                  }

                  break;

      case LCK_LIBERA  :

                  for ( i = 0; i < ap_max_users; i++) {
                     if ( ap_trusu[i].trus_pid == ap_trpid ) {
                        ap_trind = i;
                        break;
                     }
                  }

                  if ( ap_trind >= ap_max_users )
                     debug("ap_trind invalido : %d", ap_trind);

                  if ( ap_trind >= 0 ) {
                     ap_trusu[ap_trind].trus_pid  = 0;
                     ap_trusu[ap_trind].trus_nome[0] = '\0';
                  }

                  ap_trind = -1;

                  if ( ap_trcabec->tr_logados > 1 ) {
                     ap_trcabec->tr_logados--;
                  }
                  else {
                     if ( old_status != 'T') {
                        ap_trcabec->tr_status  = 0;  /* disponivel */
                        ap_trcabec->tr_logados = 0;
                        ap_trcabec->tr_pid     = 0;
                        ap_trcabec->tr_nome[0] = '\0';
                     }
                  }



                  break;

      case LCK_TRAVA  :
                  ap_trind = 0;
                  memset((TEXT*)(ap_trcabec)+4, 0, TR_TAM-4); /* +4 para nao zerar tr_ident */
                  ap_trcabec->tr_status   = 'T';
                  ap_trcabec->tr_logados  = 1;
                  ap_trcabec->tr_pesquisa = 0;
                  ap_trcabec->tr_pid      = ap_trpid;
                  strcpy(ap_trcabec->tr_nome, ap_nome);
                  break;

      case LCK_DESTRAVA  :
                  if ( old_status == 'T') {
                     ap_trind = -1;
                     memset((TEXT*)(ap_trcabec)+4, 0, TR_TAM-4); /* +4 para nao zerar tr_ident */
                  }
                  else
                     debug("Destravando base nao travada");
                  break;
   }

   if ( ret == 0 ) {
      lseek(ap_trfile, 0L, 0);

      if ( write(ap_trfile, ap_trcabec, TR_TAM) == TR_TAM )
         ret = 0;
      else
         ret = -1;
   }

   return(ret);
}



/*********************************************************************/
/*     cria_lck                                                      */
/*********************************************************************/
F__LOC   COUNT cria_lck(nx)
TEXT *nx;
{  COUNT ret;
   COUNT fd;
   time_t secsnow;

   ret = -1;

#ifdef MSDOS
	fd = open(nx, (O_CREAT | O_TRUNC | O_BINARY | O_RDWR),
                     (S_IREAD | S_IWRITE));
#else
   fd = creat(nx, 0666);
#endif


   if ( fd >= 0)  {

      memset(ap_trcabec, 0, TR_TAM);

      lseek(fd, 0L, 0);

      time(&secsnow);
      ap_trcabec->tr_ident = (LONG)secsnow;

      if ( write(fd, ap_trcabec, TR_TAM) == TR_TAM )
         ret = 0;

      close(fd);
      ap_trfile = -1;
   }

   return(ret);
}
#ifdef MSDOS
/*******************************************************************************
* ShareCheck   SHARE.EXE Installation Check                                    *
*******************************************************************************/
#include "dos.h"

unsigned char ShareCheck(void)
{ int x;
union REGS regin, regout;
regin.x.ax = 0x1000;
int86 (0x2F, &regin, &regout);
x = (int)regout.h.al;
return ( (x == 0xFF) ? 1 : 0);
}

#endif
#endif

