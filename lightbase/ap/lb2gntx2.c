#include  "lb2all.h"

#include  <string.h>
#include  HD_DIR
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2maces.h"
#include  "lb2prot1.h"



extern struct hdtxt {
   UTEXT txt_marca;
   LONG  txt_record;
   UTEXT txt_campo;
   UTEXT txt_parte;
   POINTER txt_proximo;
} *p_hdtxt;

#define   SZHDR_TEXTO     sizeof(struct hdtxt)

#ifdef MSDOS
   ARG *ld_fromfile(COUNT, LONG *, COUNT);
   POINTER save_withfile(COUNT, ARG *, LONG, COUNT);
   FILE *file_work(COUNT);
#else
   ARG *ld_fromfile( );
   POINTER save_withfile( );
   FILE *file_work( );
#endif


/* Carrega texto usando arquivo temporario */
/* Ja esta com o primeiro bloco no buffer */

F__LOC   ARG *ld_fromfile(filno, tam, largura)
COUNT    filno;
LONG    *tam;
COUNT    largura;
{  COUNT ret = 0;
   ARG *adoc=NULL;
   FILE *fp;
   LONG tam_tot;
   POINTER pos;


   /* buffer io_txt ja contem o primeiro bloco e tam diz qual e' */
   tam_tot = 0L;
   pos = p_hdtxt->txt_proximo;

   if ( (fp = file_work(0)) == NULL )
      goto fim;

   while ( pos != (POINTER) 0L ) {

      if ( filno != lb4_dnum )
         /*** WWWW deve descriptografar tam  de buf_iotxt + SZHDR_TEXTO */
         de_cripta((UTEXT *)(buf_iotxt + SZHDR_TEXTO), (UCOUNT)(*tam));

      if ( fwrite( (buf_iotxt + SZHDR_TEXTO), (SI)(*tam), 1, fp) != 1 ) {
         ret = -1;
         break;
      }

      tam_tot += *tam;

      if ( (pos = p_hdtxt->txt_proximo) != (POINTER) 0L ) {
         *tam = (LONG)GTVLEN(filno, pos);
         *tam = (LONG)GTVLEN(filno, pos);
         *tam -= SZHDR_TEXTO;

         if ( *tam > ap_sizeio ) {
            debug(WARN_001, *tam);
            fim_lb("");
         }


	      if ( RDVREC(filno, pos, buf_iotxt, ap_sizeio) != NO_ERROR ||
            p_hdtxt->txt_marca != MARCA_TEXTO ) {
            if ( filno == lb4_dnum )
               mens_erro(H_LB4BAD, E_LB4BAD, uerr_cod);
               else {
                  /* mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod); */
               }
            ret = -1;
            break;
         }
      }
   };

   fflush(fp);
   fseek(fp, 0L, 0);



   if ( ret == 0 ) {
      adoc = load_buffer( largura, ET_HANDLE, (TEXT *) fileno(fp), tam_tot);
   }

fim :

   file_work(1);

   return(adoc);
}





F__LOC   POINTER save_withfile( filno, adoc, num_reg, num_cp)
COUNT filno;
ARG *adoc;
LONG num_reg;
COUNT num_cp;
{  FILE *fp;
   LONG tam_tot, max_parte, tam_parte, num_partes;
   POINTER pos;



   pos = (POINTER) 0;
   tam_tot = tam_buffer(adoc, ET_HANDLE);

   if ( (fp = file_work(0)) == NULL )
      goto fim;

   adoc->tipo    = ET_HANDLE;
   adoc->handle  = fileno(fp);
   adoc->buf_len = tam_tot;

   if ( save_file(adoc) == 0 ) { /* igual a zero indica que nao salvou */
      goto fim;
   }

   fflush(fp);



   max_parte = ap_sizeio - SZHDR_TEXTO;
   num_partes = tam_tot / max_parte;

   if ( (tam_parte = tam_tot - (num_partes * max_parte)) > 0 )
      num_partes++;
   else tam_parte = max_parte;


   while ( num_partes-- ) {
      fseek(fp, (num_partes * max_parte), 0);
      if ( fread( (buf_iotxt + SZHDR_TEXTO), (SI)(tam_parte), 1, fp) != 1 ) {
         pos = (POINTER) 0;
         break;
      }

      p_hdtxt->txt_marca   = MARCA_TEXTO;
      p_hdtxt->txt_record  = num_reg;
      p_hdtxt->txt_campo   = num_cp;
      p_hdtxt->txt_parte   = num_partes + 1;
      p_hdtxt->txt_proximo = pos;


      if ( (pos = NEWVREC(filno, (VRLEN)(tam_parte + SZHDR_TEXTO))) != (POINTER) 0L) {

         if ( filno != lb4_dnum )
            /*** WWWW deve criptografar tam_parte de buf_iotxt + SZHDR_TEXTO */
            en_cripta((UTEXT *)(buf_iotxt + SZHDR_TEXTO), (UCOUNT)tam_parte);

         WRTVREC(filno, pos, buf_iotxt, (VRLEN)(tam_parte + SZHDR_TEXTO));
         /* retira o lock dado em newvrec */
         LOKREC(filno, FREE, pos );
      }

      if ( pos == 0 || uerr_cod != NO_ERROR ) {
         mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
         pos = (POINTER) 0;
         break;
      }

      tam_parte = max_parte;
   }


fim :

   file_work(1);

   return(pos);
}


/**************************************************************************/
/*           D U P L I C A     T E X T O                                  */
/**************************************************************************/
/* */

F__GLB   POINTER txt_dup(filno, filout, pos, num_rec)
COUNT filno, filout;
POINTER pos;
LONG num_rec;
{
   COUNT ret;
   UCOUNT tam, tam_prox;
   POINTER pos_ret, pos_wr, pos_aloc;
   extern COUNT versao_velha;

   ret = 0;
   pos_ret = 0L;

   tam = GTVLEN(filno, pos);
   tam = GTVLEN(filno, pos);  /* Tira um Bug persistente */

   if ( pos == (POINTER) 0 || tam == 0 ) {
      pos_ret = (POINTER)0;
      goto fim;
   }

   /***** Leitura do primeiro bloco ******/
#define   VERSAO_LONG   410  /* se mudar mude tambem lb2fer.h */
   if ( versao_velha != 0 && versao_velha < VERSAO_LONG ) {
      COUNT desvio;

      desvio= SZHDR_TEXTO - 6;

#ifdef UNIX
      desvio= SZHDR_TEXTO - 6;
/*      desvio=..; */
/*** tratar variacoes */
#endif

      if ( tam > ap_sizeio ) {
         debug(WARN_001, tam);
         fim_lb("");
      }


	   if ( RDVREC(filno, pos, buf_iotxt+desvio, ap_sizeio) != NO_ERROR ) {
         ret = -1;
         goto fim;
      }
      p_hdtxt->txt_marca = MARCA_TEXTO;

      p_hdtxt->txt_campo = (UTEXT)*(buf_iotxt + desvio + 5);
      p_hdtxt->txt_proximo = (POINTER) 0L;
      p_hdtxt->txt_record  = num_rec;

      tam += desvio;
   }
   else {
      if ( tam > ap_sizeio ) {
         debug(WARN_001, tam);
         fim_lb("");
      }

	   if ( RDVREC(filno, pos, buf_iotxt, ap_sizeio) != NO_ERROR ||
         p_hdtxt->txt_marca != MARCA_TEXTO ) {
         ret = -1;
         goto fim;
      }
   }

   /** Aloca area para segmento */
   if ( (pos_wr = NEWVREC(filout, tam)) == (POINTER) 0L) {
      ret = -1;
      goto fim;
   }

   /* marca bloco inicial para retorno da funcao */
   pos_ret = pos_wr;

   while ( p_hdtxt->txt_proximo != (POINTER) 0L ) {

      pos = p_hdtxt->txt_proximo;
      tam_prox = GTVLEN(filno, pos);
      tam_prox = GTVLEN(filno, pos);

      if ( (pos_aloc = NEWVREC(filout, tam_prox)) == (POINTER) 0L) {
         ret = -1;
         goto fim;
      }

      p_hdtxt->txt_proximo = pos_aloc;
      p_hdtxt->txt_record  = num_rec;

/* para reprocessar versoes velhas */
{  extern COUNT versao_velha;
      if ( versao_velha != 0 && versao_velha < 601)
         en_cripta((UTEXT *)(buf_iotxt + SZHDR_TEXTO), tam - SZHDR_TEXTO);
}

      WRTVREC(filout, pos_wr, buf_iotxt, tam);
      LOKREC(filout, FREE, pos_wr );           /* retira o lock dado em newvrec */


      tam = tam_prox;
      pos_wr = pos_aloc;

      if ( tam > ap_sizeio ) {
         debug(WARN_001, tam);
         fim_lb("");
      }

	   if ( RDVREC(filno, pos, buf_iotxt, ap_sizeio) != NO_ERROR ||
         p_hdtxt->txt_marca != MARCA_TEXTO ) {
         ret = -1;
         goto fim;
      }
   }


   p_hdtxt->txt_proximo = (POINTER) 0L;
   p_hdtxt->txt_record  = num_rec;

/* para reprocessar versoes velhas */
{  extern COUNT versao_velha;
      if ( versao_velha != 0 && versao_velha < 601)
         en_cripta((UTEXT *)(buf_iotxt + SZHDR_TEXTO), tam - SZHDR_TEXTO);
}

   WRTVREC(filout, pos_wr, buf_iotxt, tam);
   LOKREC(filout, FREE, pos_wr );           /* retira o lock dado em newvrec */



fim :

   if ( ret != 0 ) {
      if ( filno == lb4_dnum )
         mens_erro(H_LB4BAD, E_LB4BAD, uerr_cod);
      else
         mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
      pos_ret = 0L;
   }

   return(pos_ret);
}



FILE *file_work(flag)
COUNT flag; /* se 0 cria arquivo se 1 mata */
{  static TEXT fname[15];
   static FILE *fp = NULL;

   if ( flag == 0 ) { /* cria */
      strcpy(fname, "LB$XXXXXX");

      if ( (char *)mktemp(fname) != NULL ) {
#ifdef MSDOS
         fp = fopen(fname, "w+b");
#else
         fp = fopen(fname, "w+");
#endif

         if ( fp != NULL ) {
            setvbuf(fp, NULL, _IONBF, 0);
         }
      }
   }
   else {
      if ( fp != NULL ) fclose(fp);
      unlink(fname);    /* remove arquivo criado */
   }

   return(fp);
}


