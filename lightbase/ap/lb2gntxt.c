#include  "lb2all.h"

#include  <string.h>
#include  HD_DIR
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2maces.h"
#include  "lb2prot1.h"

struct hdtxt {
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




/**************************************************************************/
/*           I N I C I A L I Z A C A O                                    */
/**************************************************************************/
/* */
/* aloca memoria para buffer de IO para de campos textos */
/* inicializa ponteiro do bloco de controle de partes do texto */

F__GLB   VOID txt_start()
{
   p_hdtxt = (struct hdtxt *) buf_iotxt;
}

/**************************************************************************/
/*           L O A D    T E X T O                                         */
/**************************************************************************/
/* Era ini_texto */
F__GLB   ARG *txt_load(filno, pos, tam, largura, reg)
COUNT    filno;
POINTER  pos;
LONG    *tam;
COUNT    largura;
LONG     reg;
{  ARG   *adoc;
   TEXT *p_aux;

   adoc = NULL;
   if ( pos == 0 ) {
      debug("Leitura de texto em posicao invalida");
      return(adoc);
   }

   *tam = GTVLEN(filno, pos);
   *tam = GTVLEN(filno, pos);  /* ANANIAS assim tira um bug */

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
   }
   else {


      if ( par_cfg.flags_especiais & 0X01 && reg > 0 ) {
         /**** ATENCAO esta parte do codigo e para ler arquivos gerados na versao 1.5 */
         /**** so devera ser usada para exportar dados da base anterior ***/
         /* stop words e lida direto no formato novo pois ja foi convertida **/

         *tam = *tam + SZHDR_TEXTO - 6;
         p_aux = (TEXT *) (buf_iotxt + 6);
         adoc = load_buffer( largura, ET_MEM, p_aux, *tam);
      }
      else {
         p_aux = (TEXT *) (buf_iotxt + SZHDR_TEXTO);

         /* se nao couber todo o texto no buffer, usa arquivo temporario */
         if ( p_hdtxt->txt_proximo != (POINTER) 0 )
            adoc = ld_fromfile(filno, tam, largura);
         else {
            if ( filno != lb4_dnum )
               /*** WWWW deve descriptografar tam de p_aux */
               de_cripta((UTEXT *)p_aux, (UCOUNT)(*tam));

            adoc = load_buffer( largura, ET_MEM, p_aux, *tam);
         }
      }

   }

   return(adoc);
}


/**************************************************************************/
/*           S A L V A     T E X T O                                      */
/**************************************************************************/
/* */

F__GLB   POINTER txt_save( filno, adoc, num_reg, num_cp)
COUNT filno;
ARG *adoc;
LONG num_reg;
COUNT num_cp;
{  POINTER pos;
   LONG tam_texto;

   pos = 0L;

   tam_texto = tam_buffer(adoc, ET_HANDLE);

   if ( tam_texto + SZHDR_TEXTO > ap_sizeio ) {
      /* utiliza arquivo temporario para salvar */
      pos = save_withfile(filno, adoc, num_reg, num_cp);
   }
   else {
      p_hdtxt->txt_marca   = MARCA_TEXTO;
      p_hdtxt->txt_record  = num_reg;
      p_hdtxt->txt_campo   = num_cp;
      p_hdtxt->txt_parte   = 1;
      p_hdtxt->txt_proximo = (POINTER) 0L;

      if ( adoc != NULL ) {
         adoc->tipo    = ET_MEM;
         adoc->buffer  = buf_iotxt + SZHDR_TEXTO;
         adoc->buf_len = tam_texto;

         /* salva em memoria */
         if ( save_file(adoc) == 0 ) /* igual a zero indica que nao salvou */
            pos = (POINTER) 0L;
         else {

            if ( (pos = NEWVREC(filno, (VRLEN)(tam_texto + SZHDR_TEXTO))) != (POINTER) 0L) {
               if ( filno != lb4_dnum )
                  /*** WWWW deve criptografar tam_texto de buf_iotxt + SZHDR_TEXTO */
                  en_cripta((UTEXT *)(buf_iotxt + SZHDR_TEXTO), (UCOUNT)tam_texto);

               WRTVREC(filno, pos, buf_iotxt, (VRLEN)(tam_texto + SZHDR_TEXTO));
               /* retira o lock dado em newvrec */
               LOKREC(filno, FREE, pos );
            }

            if ( pos == 0 || uerr_cod != NO_ERROR ) {
               mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
               pos = (POINTER) 0L;
               goto fim;
            }
         }
      }
   }

fim :

   return(pos);
}


/**************************************************************************/
/*           L I B E R A     T E X T O                                    */
/**************************************************************************/
/* */


F__GLB   COUNT txt_free( filno, pos)
COUNT filno;
POINTER pos;
{  COUNT ret;

   ret = 0;

   while ( pos != (POINTER) 0L ) {

	   if ( RDVREC(filno, pos, buf_iotxt, ap_sizeio) != NO_ERROR ||
         p_hdtxt->txt_marca != MARCA_TEXTO ) {
         if ( filno == lb4_dnum )
            mens_erro(H_LB4BAD, E_LB4BAD, uerr_cod);
         else {
            /**** WWW  mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod); */
            /* Nao vou imprimir erro pois pode atrasar o processamento de outros usuarios */
         }
         ret = -1;
         break;
      }
      else {
         if ( RETVREC(filno, pos) != NO_ERROR ) {
            ret = -1;
            break;
         }
      }

      pos = p_hdtxt->txt_proximo;
   }

   return(ret);
}

