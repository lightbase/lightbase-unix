#include  "lb2all.h"

#include  "lb2stru.h"
#include  "lb2vars.h"

#include  "lb2maces.h"

#include  <string.h>
#include  HD_IO


#include  "lb2vet.h"

#ifdef MSDOS
   VOID converte_buf(TEXT *, TEXT *, COUNT);
#else
   VOID converte_buf( );
#endif



/*#  ---------------------------------------------------------------------
**#   Funcao...: load_vet
**#   Objetivo.: Carregar em memoria um bloco de elementos
**#   Entrada..: Parametros Abaixo
**#   Retorna..: Ponteiro para estrutura alocada ou NULL se Erro
**#
**#      onde : tipo -> 'C' - Campos
**#                  -> 'V' - Vistas
**#                  -> 'R' - Relatorio
**#                  -> 'I' - Itens de Vistas
**#                  -> 'E' - Itens de Relatorio
**#                  -> 'A' - Acessos
**#      onde : Ident-> Numero de identificacao do vetor
**#      onde : modo -> 'F' - Fixo. Nada pode ser alterado
**#                  -> 'E' - Itens podem ser modificados (edicao)
**#      onde : fd_arq   ->  file_descriptor do arquivo
**#      onde : file_pos ->  Posicao em disco do bloco com itens. Se NULL
**#                          inicaliza bloco.
**#      onde : tam_max  ->  Tamanho a ser alocado   em caso de edicao
**#      onde : max_elem ->  Maximo de elementos     em caso de edicao
**/

F__GLB   Vet_itens *load_vet(tipo, ident, modo, pos, max_elem, max_tam)
COUNT tipo, ident;
TEXT modo;
POINTER pos;
COUNT max_elem;
UCOUNT max_tam;
{  COUNT ret, tam, i, libera_buf;
   TEXT *buf_le;
   Vet_itens *vti;
   UCOUNT qtd_buf, qtd_ptr, tam_usar;
   Hdr_bloco hdr;
   TEXT *p1, *p2;
   UTEXT *p;

   ret = 0;
   p1 = p2 = NULL;
   vti = (Vet_itens *)NULL;
   ap_extravet = 0;

   buf_le = buf_iotxt;
   libera_buf = 0;   /* nao tem buffer alocado. so o faz se buf_iotxt for pequeno */

   memset(&hdr, '\0', sizeof(hdr));
   memset(buf_le, '\0', ap_sizeio);


   /* primeiro verifica se em posicionamento tem header valido */

   if ( pos != 0L ) {
      tam_usar = GTVLEN(lb4_dnum, pos);
      if ( tam_usar > ap_sizeio ) {
         if ( (buf_le = malloc(tam_usar+10)) == NULL ) { /* 10 e folga */
            ret = -3; /* Sem memoria */
            goto fim;
         }
         libera_buf = 1;
      }


      if ( RDVREC(lb4_dnum, pos, buf_le, tam_usar) != NO_ERROR ) {
         ret = -1; /* nao tem header */
         goto fim;
      }

      memcpy(&hdr, buf_le, sizeof(hdr));

      /* verifica se em posicionamento tem bloco solicitado */
      if ( hdr.marca_fim[0] != 0x6 || hdr.marca_fim[1] != 0x4 ) {
         ret = -1; /* header invalido */
         goto fim;
      }
      if ( hdr.tipo != tipo || ( ident != 0 && hdr.ident != ident) ) {
         ret = -2; /* Bloco solicitado nao confere com posicionamento */
         goto fim;
      }

      ident = hdr.ident;
   }

   /* Calcula quantidade de memoria a ser alocada para o bloco */

   if ( modo == EDIT_OK )  {
      qtd_ptr = max_elem;
      qtd_buf = max_tam;
   }
   else {
      qtd_ptr = max_elem = hdr.num_elementos;
      qtd_buf = max_tam  = hdr.tamanho;
   }


   /* aloca estrutura controle + area ponteiros + area info */
   if ( !(vti = (Vet_itens *) malloc(sizeof(Vet_itens)))) {
      ret = -3; /* Sem memoria */
      goto fim;
   }

   p1 = calloc(sizeof(TEXT *), qtd_ptr+1);
   p2 = malloc(qtd_buf + TAMANHO_HDR + 10 ); /* 10 e folga */

   if ( p1 == NULL || p2 == NULL ) {
      ret = -3; /* Sem memoria */
      goto fim;
   }


   /* Leitura do bloco propriamente dito */
   if ( lb4_dnum > 0  && pos != 0) {
      memcpy(p2 + TAMANHO_HDR, buf_le + TAMANHO_HDR, hdr.tamanho);
      /* verifica se informacao extra no bloco. Atualiza em ap_extravet */
      /* se existir deixar a mesma em buf_iotxt */
      ap_extravet = tam_usar - hdr.tamanho - TAMANHO_HDR;
   }
   else {
      hdr.tamanho = 0;
      hdr.num_elementos = 0;
   }

   vti->vi_tipo    = tipo;
   vti->vi_ident   = ident;
   vti->vi_status  = (modo == EDIT_OK) ? VET_EDIT : 0;

   vti->vi_pos     = pos;
   vti->vi_ftam    = TAMANHO_HDR + hdr.tamanho;

   vti->vi_mmax    = qtd_buf;
   vti->vi_nmax    = qtd_ptr;

   vti->vi_buffer  = p2 + TAMANHO_HDR;
   vti->vi_vtit    = (TEXT **)p1;

   vti->vi_mtam    = 0;
   vti->vi_nelem   = 0;
   vti->vi_curr    = 0;
   vti->vi_clen    = 0;

   /* monta vetor de ponteiros para informacoes */

   for ( i = 0, p = (UTEXT*) vti->vi_buffer; i < hdr.num_elementos; i++ ) {
      tam = (COUNT)GET_LONG(p);
      p += TAM_TAM;   /* faz apontar para o item corrente */

      tam = TAM_ITEM(tam);

      vti->vi_vtit[i] = (TEXT *)p;
      p += tam;

      vti->vi_mtam += (tam + TAM_TAM);
      vti->vi_nelem++;
   }


fim :

   if ( ret < 0 ) {
      if ( p2  != NULL ) free(p2);
      if ( p1  != NULL ) free(p1);
      if ( vti != NULL ) free(vti);
      vti = NULL;
   }
   else {
      if ( ap_extravet > 0 )  {
         p1 = buf_le + TAMANHO_HDR + hdr.tamanho;
         memcpy(buf_iotxt, p1, ap_extravet);
      }
   }

   if ( libera_buf ) free(buf_le);

   return(vti);
}



/*#  ---------------------------------------------------------------------
**#   Funcao...: rlse_vet
**#   Objetivo.: Libera memoria utilizada por uma lista
**#
**/
F__GLB   VOID rlse_vet(vti)
Vet_itens *vti;
{  TEXT *p1;
   if ( vti != NULL ) {
      p1 = vti->vi_buffer;
      if ( p1 != NULL ) free(p1 - TAMANHO_HDR);
      if ( vti->vi_vtit   != NULL ) free(vti->vi_vtit);
      free(vti);
   }
}




/*#  ---------------------------------------------------------------------
**#   Funcao...: frs_item
**#   Objetivo.: Retorna endereco de memoria do primeiro item
**#
**/
F__GLB   VOID *fst_item(vti)
Vet_itens *vti;
{  VOID *p_ret;
   UTEXT *p;

   if ( vti == NULL || vti->vi_nelem < 1 ) {
      p_ret = NULL;
   }
   else {
      vti->vi_curr = 0;

      p_ret = vti->vi_vtit[vti->vi_curr];

      if (p_ret == NULL )
         debug("vi_vtit[*] == NULL");

      p = p_ret;
      vti->vi_clen = (COUNT)GET_LONG((p-TAM_TAM));
   }

   return(p_ret);
}




/*#  ---------------------------------------------------------------------
**#   Funcao...: nxt_item
**#   Objetivo.: Retorna endereco de memoria do proximo item
**#
**/
F__GLB   VOID *nxt_item(vti)
Vet_itens *vti;
{  VOID *p_ret;
   UTEXT *p;

   if ( vti == NULL || (vti->vi_curr + 1) >= vti->vi_nelem  ) {
      p_ret = NULL;
   }
   else {
      vti->vi_curr++;

      p_ret = vti->vi_vtit[vti->vi_curr];

      if (p_ret == NULL )
         debug("vi_vtit[*] == NULL");

      p = p_ret;
      vti->vi_clen = (COUNT)GET_LONG((p-TAM_TAM));
   }

   return(p_ret);
}



/*#  ---------------------------------------------------------------------
**#   Funcao...: pos_item
**#   Objetivo.: Retorna endereco de memoria do item mencionado
**#
**/
F__GLB   VOID *pos_item(vti, pos)
Vet_itens *vti;
COUNT pos;
{  VOID *p_ret;
   UTEXT *p;

   if ( vti == NULL || pos >= vti->vi_nelem || pos < 0  ) {
      p_ret = NULL;
   }
   else {
      vti->vi_curr = pos;

      p_ret = vti->vi_vtit[vti->vi_curr];

      if (p_ret == NULL )
         debug("vi_vtit[*] == NULL");

      p = p_ret;
      vti->vi_clen = (COUNT)GET_LONG((p-TAM_TAM));
   }

   return(p_ret);
}


F__LOC VOID converte_buf(dest, orig, qtd)
TEXT *dest, *orig;
COUNT qtd;
{  COUNT i, tam;
   UTEXT *p;
   LONG tam_long;

   p = (UTEXT *)orig;

   for ( i = 0; i <= qtd; i++ ) {
      tam = (*p++) * 256;
      tam += *p++;


            /* coloca item no buffer */
            tam_long = tam;
            PUT_LONG(dest, tam_long);
            dest += TAM_TAM;

            memcpy(dest, p, tam);

            dest += TAM_ITEM(tam);

      p += tam;
   }
}
