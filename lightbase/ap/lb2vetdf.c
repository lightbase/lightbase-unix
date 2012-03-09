#include  "lb2all.h"
#include  "lb2stru.h"


#include  <string.h>
#include  HD_IO


#include  "lb2vars.h"
#include  "lb2maces.h"
#include  "lb2vet.h"

#ifdef MSDOS
   VOID v_shfrgt(UTEXT *base, UCOUNT tam_buf, UCOUNT ref, UCOUNT qtd);
   VOID v_shflft(UTEXT *base, UCOUNT tam_buf, UCOUNT ref, UCOUNT qtd);
#else
   VOID v_shfrgt( );
   VOID v_shflft( );
#endif

/*#  ---------------------------------------------------------------------
**#   Funcao...: save_vet
**#   Objetivo.: Salvar em disco um bloco de elementos
**#   Entrada..: Parametros Abaixo
**#   Retorna..: Posicao do aquivo onde foi gravada ou NULL se Erro
**#
**#      onde : fd_arq   ->  file_descriptor do arquivo
**#      onde : file_pos ->  Posicao do disco onde devera ser gravada
**/

F__GLB   POINTER save_vet(vti, file_pos)
Vet_itens *vti;
POINTER file_pos;
{  COUNT ret;
   UCOUNT tam_usar = 0;
   Hdr_bloco hdr;
   TEXT *buf_le;
   TEXT *base;

   ret = 0;
   buf_le = buf_iotxt;


   if ( (vti->vi_status & VET_EDIT) == 0 ) {
      debug("Nao pode gravar sem ter sido chamado para edicao");
      ret = -1;
      goto fim;
   }

   if ( (vti->vi_status & (~VET_EDIT)) == 0 ) {
      debug("Nada foi alterado. Gravacao NAO efetuada");
      ret = -1;
      goto fim;
   }


   hdr.tipo  = vti->vi_tipo;
   hdr.ident = vti->vi_ident;
   hdr.tamanho = vti->vi_mtam;
   hdr.num_elementos = vti->vi_nelem;
   hdr.marca_fim[0] = 0x06;
   hdr.marca_fim[1] = 0x04;

   base            = vti->vi_buffer - TAMANHO_HDR;
   vti->vi_ftam    = hdr.tamanho + TAMANHO_HDR;
   vti->vi_pos     = file_pos;

   memcpy(base, &hdr, TAMANHO_HDR);
   tam_usar = vti->vi_ftam;

   if ( ap_extravet > 0 ) {
      tam_usar += ap_extravet;
      if ( tam_usar > ap_sizeio ) {
         if ( (buf_le = malloc(tam_usar+10)) == NULL ) { /* 10 e folga */
            ret = -3; /* Sem memoria */
            goto fim;
         }
      }

      memmove(buf_le + vti->vi_ftam, buf_iotxt, ap_extravet);
      memcpy(buf_le, base, vti->vi_ftam);
      base = buf_le;
   }


	if ( WRTVREC(lb4_dnum, vti->vi_pos, base, tam_usar) != 0 ) {
      ret = -1; /* erro na gravacao do header */
      goto fim;
   }

   vti->vi_status  = ( vti->vi_status & VET_EDIT ) ? VET_EDIT : 0;

fim :

   if ( ret < 0 ) {
      file_pos = 0;
   }

   if ( buf_le != buf_iotxt && buf_le != NULL )
      free(buf_le);

   return(file_pos);
}



/*#  ---------------------------------------------------------------------
**#   Funcao...: add_item
**#   Objetivo.: Incluir um item no vetor
**#   Entrada..: Parametros Abaixo
**#   Retorna..: Ponteiro para area onde item foi colocado ou NULL se Erro
**#
**#      onde : vti        -> Endereco da estrutura de controle
**#      onde : pos        -> posicao que devera ocupar (Indice em vi_vtit)
**#      onde : item       -> Ponteiro para dados do item (Informacao)
**#      onde : tam_item   -> Tamanho do item
**#      onde : shift_flag -> Indicacao se devera ser efetuado shift nos
**#                           ponteiros de vi_vtit. Explicacao : Funcao de
**#                           manipular tabelas ja executa shift no vetor
**#                           de ponteiros portanto para nao haver duplo
**#                           shift, Incluimos esta flag.
**/
F__GLB   VOID *add_item(vti, pos, item, tam_item, shift_flag)
Vet_itens *vti;
COUNT pos;
VOID *item;
UCOUNT tam_item;
COUNT shift_flag;
{  VOID *p_ret;
   LONG tam_long;
   UTEXT *p;
   COUNT offset, i;
   UCOUNT tam;

   p_ret = NULL;

   if ( (vti->vi_status & VET_EDIT) == 0 ) {
      debug("Nao pode Incluir sem ter sido chamado para edicao");
      goto fim;
   }


   if ( vti == NULL ||  pos < 0  )
      goto fim;

   /* se pos maior que ultimo inclui no final */
   pos = MIN(pos, vti->vi_nelem);

   /* Verifica se pode incluir. Maximo de elementos atingido */
   if ( vti->vi_nelem >= vti->vi_nmax )
      goto fim;

   /* Verifica se pode incluir. Maximo de memoria disponivel atingido */
   if ( vti->vi_mtam + TAM_ITEM(tam_item) + TAM_TAM > vti->vi_mmax )
      goto fim;

   /* se nao for a ultima posicao, abre espaco em buffer */
   if ( pos == vti->vi_nelem ) {
      p = (UTEXT *)vti->vi_buffer + vti->vi_mtam;
   }
   else {
      if ( ! shift_flag ) {
         /* quando e chamada por tabela o indice vem apontando para buffer de edicao */
         if ( pos == 0 ) {
            p = (UTEXT *)vti->vi_buffer + TAM_TAM;
         }
         else {
            p = (UTEXT *)vti->vi_vtit[pos-1];
            tam = (COUNT)GET_LONG((p-TAM_TAM));
            p += (TAM_ITEM(tam) + TAM_TAM );
         }
      }
      else {
         p = (UTEXT *)vti->vi_vtit[pos];
      }
      p -= TAM_TAM;   /* faz apontar para tamanho do item corrente */
      offset = p - (UTEXT *)vti->vi_buffer;
      v_shfrgt((UTEXT*)vti->vi_buffer, vti->vi_mtam, offset,
           TAM_ITEM(tam_item) + TAM_TAM);
   }

   /* coloca item no buffer */
   tam_long = TAM_ITEM(tam_item);
   PUT_LONG(p, tam_long);
   p += TAM_TAM;

   if ( item != NULL )
      memcpy(p, item, TAM_ITEM(tam_item));
   else
      memset(p, 0, TAM_ITEM(tam_item));



   /* coloca item no vetor de ponteiros */
   if ( pos < vti->vi_nelem ) {
      if ( shift_flag )
         for ( i = vti->vi_nelem; i > pos; i-- )
            vti->vi_vtit[i] = vti->vi_vtit[i-1];

      for ( i = vti->vi_nelem; i > pos; i-- ) {
         COUNT x;

         x = TAM_ITEM(tam_item) + TAM_TAM;
         vti->vi_vtit[i] = vti->vi_vtit[i] + x;
      }
   }

   vti->vi_vtit[pos] = (TEXT *)p;

   if ( shift_flag ) /* para nao melar buffer de edicao de tabela */
      vti->vi_vtit[vti->vi_nelem+1] = NULL;

   vti->vi_mtam += (TAM_ITEM(tam_item) + TAM_TAM );
   vti->vi_nelem++;
   vti->vi_curr  = pos;
   vti->vi_clen  = TAM_ITEM(tam_item);

   vti->vi_status  |= VET_INC;  /* marca inclusao no vetor */

   p_ret = p;

fim :

   return(p_ret);
}


/*#  ---------------------------------------------------------------------
**#   Funcao...: del_item
**#   Objetivo.: Deleta item corrente do vetor
**#   Entrada..: Parametros Abaixo
**#   Retorna..: 0 se exclui OK
**#              -1 se  NAO  excluiu
**#
**#      onde : vti        -> Endereco da estrutura de controle
**#      onde : shift_flag -> Indicacao se devera ser efetuado shift nos
**#                           ponteiros de vi_vtit. Explicacao : Funcao de
**#                           manipular tabelas ja executa shift no vetor
**#                           de ponteiros portanto para nao haver duplo
**#                           shift, Incluimos esta flag.
**/
F__GLB   COUNT del_item(vti, shift_flag)
Vet_itens *vti;
COUNT shift_flag;
{  COUNT ret, pos, tam_item;
   UTEXT *p;
   COUNT offset, i;

   ret = -1;

   if ( (vti->vi_status & VET_EDIT) == 0 ) {
      debug("Nao pode Excluir sem ter sido chamado para edicao");
      goto fim;
   }


   if ( vti == NULL ||  vti->vi_nelem == 0  )
      goto fim;

   pos      = vti->vi_curr;
   tam_item = vti->vi_clen;

   /* se NAO for a ultima posicao, Fecha espaco em buffer */
   if ( pos != (vti->vi_nelem -1 )) {
      p = (UTEXT *)vti->vi_vtit[pos+1];
      p -= TAM_TAM;   /* faz apontar para inicio do proximo item */
      offset = p - (UTEXT *)vti->vi_buffer;
      v_shflft((UTEXT*)vti->vi_buffer, vti->vi_mtam, offset,
           TAM_ITEM(tam_item) + TAM_TAM);
   }


   /* retira item no vetor de ponteiros */
   if ( pos != (vti->vi_nelem -1 )) {
      if ( shift_flag )
         for ( i = pos; i < vti->vi_nelem; i++ )
            vti->vi_vtit[i] = vti->vi_vtit[i+1];

      for ( i = pos; i < vti->vi_nelem; i++ ) {
         COUNT x;

         x = TAM_ITEM(tam_item) + TAM_TAM;

         vti->vi_vtit[i] = vti->vi_vtit[i] - x;
      }
   }

   vti->vi_nelem--;
   if ( shift_flag ) /* para nao melar buffer de edicao de tabela */
      vti->vi_vtit[vti->vi_nelem] = NULL;

   vti->vi_mtam -= (TAM_ITEM(tam_item) + TAM_TAM );
   vti->vi_status  |= VET_DEL;  /* marca exclusao no vetor */


   vti->vi_curr  = MIN(pos, (vti->vi_nelem - 1));

   p = (UTEXT *)vti->vi_vtit[vti->vi_curr];

   if (p != NULL )
      vti->vi_clen = (COUNT)GET_LONG((p-TAM_TAM));
   else
      vti->vi_clen = 0;

   ret = 0;

fim :

   return(ret);
}

/*#  ---------------------------------------------------------------------
**#   Funcao...: v_shfrgt
**#   Objetivo.: empura bytes para direita ( abre espaco )
**#
**/
F__LOC   VOID v_shfrgt(base, tam_buf, ref, qtd)
UTEXT *base;
UCOUNT tam_buf, ref, qtd;
{
	FAST UTEXT *dp, *sp;
   COUNT move;

   move = tam_buf - ref;

   sp = &base[tam_buf-1];
   dp = &base[tam_buf+qtd-1];

   while (move--)
		*dp-- = *sp--;
}



/*#  ---------------------------------------------------------------------
**#   Funcao...: v_shflft
**#   Objetivo.: empura bytes para esquerda ( fecha espaco )
**#
**/
F__LOC   VOID v_shflft(base, tam_buf, ref, qtd)
UTEXT *base;
UCOUNT tam_buf, ref, qtd;
{
	FAST UTEXT *sp;

   sp = &base[ref];

	memcpy(&base[ref-qtd], sp, tam_buf - ref);
}

