#include  "lb2all.h"

#include  <string.h>
#include  HD_DIR
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2perso.h"
#include  "lb2pesq.h"
#include  "lb2prot1.h"
#include  "lb2trans.h"



#define   PSQ_FMASC      "PSQXXXXXX"
#define   CRIA 'C'
#define   MATA 'M'
#define   FLAG_PADRAO   1


#define   H_CFINDPSQ       "CF_INDPSQ"
#define   H_PSQLST         "E_PSQ_LST"
#define   H_PSQNOT         "M_PSQ_NOT"

#define   H_CFVERDO     "CF_VERDO"     /* qualquer tecla para ver documentos selecionados */


#define   N_BLOCO(r)    ((((r) - 1) / PSQ_SLOT) + 1)
#define   OFF_BLOCO(r)  (((r) - ((N_BLOCO(r) - 1) * ((LONG)PSQ_SLOT))) - 1)

/*---------------------------------------------------------------------*/
/*  Mapa de localizacao em disco                                       */
/*---------------------------------------------------------------------*/
extern struct psq_mapa {
   UTEXT  mp_lista;   /* Numero da lista ao qual o bloco pertence */
   UTEXT  mp_bloco;   /* numero do bloco que esta neste buffer */
};

/*---------------------------------------------------------------------*/
/*  Controle de buffers                                                */
/*---------------------------------------------------------------------*/
extern struct {
   SDINDEX *wb_vetor;     /* endereco do buffer */
   COUNT wb_modif;     /* indica que buffer foi modificado */
   COUNT wb_idade;     /* idade do buffer */
} vet_wkbuf[3];


extern struct psq_mapa *vet_mapa;

extern COUNT ord_slot; /* flag de ordenacao de lista */
extern TEXT fname_psq[PATH_MAX+1];
extern FILE *fp_psq;
extern jmp_buf psq_jmperro;
extern SDINDEX sdi_mor;

static LONG reg_ant=0L;

#ifdef MSDOS
   COUNT aloc_work( VOID );
   COUNT aloc_mapa( VOID );
   COUNT aloc_lista( VOID );
   SI    psqcmp_testa(SDINDEX *, SDINDEX *);
   COUNT str_meso(TEXT *, TEXT *, COUNT);
   COUNT new_lista(VOID);
   COUNT pega_buf(COUNT);
   VOID  assoc_buf(COUNT, COUNT);
   COUNT loc_mapa(COUNT, COUNT);
   SDINDEX *load_lista(COUNT, COUNT);
   COUNT ex_loadlista(COUNT, COUNT, TEXT *);
   COUNT ex_savelista(COUNT, TEXT *);
   VOID  merge_bloco(COUNT);
   VOID  save_bloco(COUNT);
   VOID  psq_fileger(COUNT);
   VOID  libera_lista(COUNT);
   COUNT v_menos_1(Tab *);
   VOID  atu_tabela(Tab *, COUNT);
   COUNT show_result(COUNT, TEXT *, LONG, LONG, LONG);
   TEXT  *fonema(TEXT*);
   LONG ger_lstfile(FILE *, COUNT);
   FILE *opfile_flags(TEXT *);
   VOID reset_args( TEXT *);
#else
   COUNT aloc_work( );
   COUNT aloc_mapa( );
   COUNT aloc_lista( );
   SI    psqcmp_testa( );
   COUNT str_meso( );
   COUNT new_lista( );
   COUNT pega_buf( );
   VOID  assoc_buf( );
   COUNT loc_mapa( );
   SDINDEX *load_lista( );
   COUNT ex_loadlista( );
   COUNT ex_savelista( );
   VOID  merge_bloco( );
   VOID  save_bloco( );
   VOID  psq_fileger( );
   VOID  libera_lista( );
   COUNT v_menos_1( );
   VOID  atu_tabela( );
   COUNT show_result( );
   TEXT  *fonema( );
   LONG ger_lstfile( );
   FILE *opfile_flags( );
   VOID reset_args( );
#endif





F__GLB   SDINDEX *prim_sdi(lista)
COUNT lista;
{  SDINDEX *p_sdi ;
   LONG reg;

   if ( vet_lista[lista].flag < 8 ) {
      if (lista > 0 ) {
         p_sdi = load_lista(lista, 1);
      }
      else
         p_sdi = NULL;

   }
   else {

      if ( vet_lista[lista].flag == 9 ) {
         reg = prox_reg( sele_pesq, 0);
         if ( (reg_ant = reg) < 0 )
            reg_ant = 0;
      }
#ifdef CP_FLAG
      else {
         reg = ger_lstfile(vet_lista[lista].file_cp, '0');
      }
#endif
      vet_lista[lista].sdi_lista.registro = reg;
      if ( reg > 0 ) {
         vet_lista[lista].sdi_lista.campo    = 127;
         vet_lista[lista].sdi_lista.paragrafo= 127;
         vet_lista[lista].sdi_lista.frase    = 127;
         vet_lista[lista].sdi_lista.sequencia= 127;
         p_sdi = &vet_lista[lista].sdi_lista;
      }
      else {
         p_sdi = NULL;
      }
   }

   if ( p_sdi != NULL )
      vet_lista[lista].it_item  = 1;
   else
      vet_lista[lista].it_item  = 0;

   return( p_sdi );
}

F__GLB   SDINDEX *last_sdi(lista)
COUNT lista;
{  COUNT u_bl_lista, nb, off_blk;
   LONG reg;
   SDINDEX *p;

   if ( vet_lista[lista].flag < 8 ) {

      u_bl_lista = (((LONG)PSQ_SLOT) - 1 + vet_lista[lista].tot_vet) / ((LONG)PSQ_SLOT) ;

      if ( load_lista(lista, u_bl_lista) == NULL ) {
         p = NULL;
         goto fim;
      }

      nb      = vet_lista[lista].n_buf;

      off_blk = vet_lista[lista].tot_vet - ((u_bl_lista - 1 ) * ((LONG)PSQ_SLOT));

      p = vet_wkbuf[nb].wb_vetor + (off_blk-1);

      vet_lista[lista].it_item  = off_blk;
   }
   else {

      if ( vet_lista[lista].flag == 9 ) {
         reg = ante_reg( sele_pesq, vet_lista[lista].tot_vet + 1);
         if ( (reg_ant = reg) < 0 )
            reg_ant = 0;
      }
#ifdef CP_FLAG
      else {
         reg = ger_lstfile(vet_lista[lista].file_cp, '9');
      }
#endif
      vet_lista[lista].sdi_lista.registro = reg;
      if ( reg > 0 ) {
         vet_lista[lista].sdi_lista.campo    = 127;
         vet_lista[lista].sdi_lista.paragrafo= 127;
         vet_lista[lista].sdi_lista.frase    = 127;
         vet_lista[lista].sdi_lista.sequencia= 127;
         p = &vet_lista[lista].sdi_lista;
      }
      else {
         p = NULL;
      }

      vet_lista[lista].it_item  = vet_lista[lista].tot_doc;
   }

fim:

   return(p);
}


F__GLB   SDINDEX *prox_sdi(lista)
COUNT lista;
{  COUNT off_blk, nb;
   SDINDEX *p;
   LONG reg;

   if ( vet_lista[lista].flag < 8 ) {

      off_blk = vet_lista[lista].it_item   + 1;
      nb      = vet_lista[lista].n_buf;

      if ( off_blk > PSQ_SLOT ) {
         if ( load_lista(lista, vet_lista[lista].it_bloco + 1) == NULL ) {
            p = NULL;
            goto fim;
         }
         off_blk = 1;
      }

      p = vet_wkbuf[nb].wb_vetor + (off_blk-1);

      vet_lista[lista].it_item = off_blk; /* equivale a increment. circular */

      if ( p->registro <= 0L ) {
         p = NULL;   /* acabou registros */
      }
   }
   else {
      if ( vet_lista[lista].flag == 9 ) {
         reg = prox_reg( sele_pesq, reg_ant);
         if ( (reg_ant = reg) < 0 )
            reg_ant = 0;
      }
#ifdef CP_FLAG
      else {
         reg = ger_lstfile(vet_lista[lista].file_cp, 'P');
      }
#endif
      vet_lista[lista].sdi_lista.registro = reg;
      if ( reg > 0 ) {
         vet_lista[lista].sdi_lista.campo    = 127;
         vet_lista[lista].sdi_lista.paragrafo= 127;
         vet_lista[lista].sdi_lista.frase    = 127;
         vet_lista[lista].sdi_lista.sequencia= 127;
         p = &vet_lista[lista].sdi_lista;
      }
      else {
         p = NULL;
      }
      vet_lista[lista].it_item++;
   }

fim :

   return(p);
}


F__GLB   SDINDEX *back_sdi(lista)
COUNT lista;
{  COUNT off_blk, nb;
   SDINDEX *p;
   LONG reg;

   if ( vet_lista[lista].flag < 8 ) {

      off_blk = vet_lista[lista].it_item  - 1;
      nb      = vet_lista[lista].n_buf;

      if ( off_blk <= 0 ) {
         if ( load_lista(lista, vet_lista[lista].it_bloco - 1) == NULL ) {
            p = NULL;
            goto fim;
         }
         off_blk = PSQ_SLOT;
      }

      p = vet_wkbuf[nb].wb_vetor + (off_blk-1);

      vet_lista[lista].it_item = off_blk; /* equivale a increment. circular */

      if ( p->registro <= 0L ) {
         p = NULL;   /* acabou registros */
      }
   }
   else {
      if ( vet_lista[lista].flag == 9 ) {
         reg = ante_reg( sele_pesq, c_bases[sele_pesq]->cb_numrec);
         if ( (reg_ant = reg) < 0 )
            reg_ant = 0;
      }
#ifdef CP_FLAG
      else {
         reg = ger_lstfile(vet_lista[lista].file_cp, 'A');
      }
#endif
      vet_lista[lista].sdi_lista.registro = reg;
      if ( reg > 0 ) {
         vet_lista[lista].sdi_lista.campo    = 127;
         vet_lista[lista].sdi_lista.paragrafo= 127;
         vet_lista[lista].sdi_lista.frase    = 127;
         vet_lista[lista].sdi_lista.sequencia= 127;
         p = &vet_lista[lista].sdi_lista;
      }
      else {
         p = NULL;
      }

      vet_lista[lista].it_item++;
   }

fim :

   return(p);
}




/*#  ---------------------------------------------------------------------
**#   Funcao...: add_sdi - add sdindex
**#   Objetivo.: Inclui elemento SDINDEX no final de uma lista.
**#                  Incrementa : tot_vet
**#   Entrada..: lista - Numero da lista. Lista = 1 --> vet_lista[1]
**#              p_sdi - Endereco de estrutura SDINDEX
**#   Retorna..: VOID
**#   Utiliza..:
**#
**#
**/
F__GLB   VOID add_sdi(lista, p_sdi)
COUNT lista;
SDINDEX *p_sdi;
{  COUNT off_blk, nb;
   SDINDEX *p;

   /* se lista toda nao faz nada */

   if (vet_lista[lista].flag < 8 ) { /* lista toda/lista flags */
      nb      = vet_lista[lista].n_buf;
      off_blk = vet_lista[lista].it_item + 1;

      if ( p_sdi->registro != -1L ) {
         if ( off_blk > PSQ_SLOT ) {
            if ( ord_slot )
               merge_bloco(lista);
            save_bloco(lista); /* aumenta a lista de blocos */
            memset(vet_wkbuf[nb].wb_vetor, '\0', TAM_EVET * PSQ_SLOT);
            off_blk = 1;
            vet_lista[lista].it_bloco++;
         }

         p = vet_wkbuf[nb].wb_vetor + (off_blk-1);

         vet_lista[lista].it_item = off_blk; /* equivale a increment. circular */

         memcpy( p , (TEXT *) &p_sdi->registro, TAM_EVET);

         vet_lista[lista].tot_vet++;
         vet_lista[lista].tot_campo++;
         vet_wkbuf[nb].wb_modif = 1;  /* marca que buffer foi modificado */
      }
      else {
         if ( off_blk > 1 && off_blk <= PSQ_SLOT ) {
            p = vet_wkbuf[nb].wb_vetor + (off_blk-1);
            memcpy( p , (TEXT *) &p_sdi->registro, TAM_EVET);
         }

         if ( ord_slot )
            merge_bloco(lista);
      }
   }
}



F__LOC	 COUNT new_lista()
{  COUNT lista, i, num;

   lista = -1;

   for (i = 1; i < NUM_LISTAS; i++) {
      if ( vet_lista[i].flag == 0 ) {

         lista = i;

         libera_lista(lista);

         num = pega_buf(0);

         assoc_buf(lista, num);

         vet_lista[i].flag = FLAG_PADRAO; /**  deve receber como parametro */

         break;
      }
   }

   return(lista);
}



F__LOC	 COUNT pega_buf(flag)
COUNT flag;
{  static COUNT selec = 0;
   COUNT ret;


   if ( flag == 0 ) {
      selec++;
      ret = selec % 3;
   }
   else {
      ret = ( selec + flag ) % 3;
   }


   return (ret);
}

F__LOC	 VOID assoc_buf(lista, num)
COUNT lista, num;
{  COUNT i, old_lista;


   old_lista = 0;

   /* verifica se existe alguma lista utilizando o buffer */
   for (i = 1; i < NUM_LISTAS; i++) {
      if ( vet_lista[i].n_buf == num ) {
         old_lista = i;
         break;
      }
   }

   if ( old_lista > 0 ) { /* existe lista que esta utilizando buffer */

      if ( vet_wkbuf[num].wb_modif != 0) {
         save_bloco(old_lista);
      }

      /* faz old_lista ficar so no arquivo */
      vet_lista[old_lista].n_buf    = -1;
      vet_lista[old_lista].it_bloco = 0;
      vet_lista[old_lista].it_item  = 0;
   }


   if ( lista > 0 ) {  /* pode ser chamada tambem p/ alocar buffer de work*/
      vet_lista[lista].n_buf = num;
      vet_lista[lista].it_bloco = 1;
      vet_lista[lista].it_item  = 0;
   }

   vet_wkbuf[num].wb_modif = 0;
}



F__LOC	 VOID  save_bloco(lista)
COUNT lista;
{  COUNT ret, nb, i, entrada;

   if ( fp_psq == NULL ) { /* ainda nao abriu arquivo */
      psq_fileger(CRIA);  /* erro tratado por lngjmp */
   }

   nb      = vet_lista[lista].n_buf;

   if ( nb < 0 ){
      numdebug(680);
      ret = -1;
      goto fim;
   }

   if ( vet_wkbuf[nb].wb_modif == 0 ) {  /* marca de buffer nao modificado */
      ret = 0;
      goto fim;
   }

   /* acha entrada disponivel no mapa */
   entrada = -1;

   for (i = 0; i < TAM_MAPA; i++) {
      if ( vet_mapa[i].mp_lista == 0 ) {
         entrada = i;
         break;
      }
   }

   if ( entrada == -1 ) {
      ret = -2;
      goto fim;
   }


   if ( ex_savelista(entrada, (TEXT *)vet_wkbuf[nb].wb_vetor) < 0 ) {
      ret = -3;
      goto fim;
   }

   vet_wkbuf[nb].wb_modif = 0;

   ++vet_lista[lista].u_bl_disco;

   vet_mapa[entrada].mp_lista = lista;
   vet_mapa[entrada].mp_bloco = vet_lista[lista].u_bl_disco;


   ret = 0;

fim :

   if ( ret < 0 ) /* erro de acesso a lista */
      longjmp(psq_jmperro, ret);
}


F__LOC	 COUNT ex_savelista(entrada, buf)
COUNT entrada;
TEXT *buf;
{  COUNT ret;
   LONG pos;

   pos = entrada;
   pos *= ((LONG)PSQ_SLOT);
   pos *= TAM_EVET;


   if ( fseek(fp_psq, pos, 0) != 0 ) {
      ret = -3;
      goto fim;
   }

   if ( fwrite(buf, TAM_EVET, PSQ_SLOT, fp_psq)
          != PSQ_SLOT ) {
      ret = -3;
      goto fim;
   }

   ret = 0;

fim :

   return(ret);
}




F__LOC	 VOID merge_bloco(lista)
COUNT lista;
{  SDINDEX *p_new, *p_old, *p_out;
   COUNT ret, qtd_new, qtd_old, qtd_out;
   LONG tot_regs;
   COUNT nb, old_buf, out_buf;
   COUNT u_bl_disco, bloco_in, bloco_out, bloco_dsk;
   LONG XXX = 0;


   tot_regs = vet_lista[lista].tot_vet;
   qtd_old  = qtd_out = 0;
   bloco_in = bloco_out = 0;


   /* nao tentar ler/gravar bloco superior a ultimo_bloco */
   u_bl_disco = vet_lista[lista].u_bl_disco;
   qtd_new    = tot_regs - ( u_bl_disco * ((LONG)PSQ_SLOT));

   nb         = vet_lista[lista].n_buf;
   p_new      = vet_wkbuf[nb].wb_vetor;

   qsort( (TEXT *)p_new, qtd_new, TAM_EVET, (SI (* )())psqcmp_testa );

   if ( tot_regs <= (LONG)PSQ_SLOT ) {
      ret = 0;
      goto fim;
   };


   old_buf = pega_buf(1);
   assoc_buf(0, old_buf); /* somente dessasocia da lista anterior */

   out_buf = pega_buf(2);
   assoc_buf(0, out_buf); /* somente dessasocia da lista anterior */

   p_out = vet_wkbuf[out_buf].wb_vetor;

   while ( tot_regs-- ) {

      if ( qtd_old == 0 ) {

         if ( ++bloco_in > u_bl_disco ) {
            p_old =  &sdi_mor;
            qtd_old = 1; /* sempre vai ser maior */
         }
         else {
            p_old = vet_wkbuf[old_buf].wb_vetor;
            qtd_old = PSQ_SLOT;
            if ( ex_loadlista(lista, bloco_in, (TEXT *)p_old) < 0 ) {
               ret = -1;
               goto fim;
            }
         }
      }

      if ( qtd_new == 0 ) {
         p_new =  &sdi_mor;
      }

      if ( qtd_out == PSQ_SLOT ) {

         bloco_out++;

         if ( bloco_out <= u_bl_disco ) {

            p_out = vet_wkbuf[out_buf].wb_vetor;

            if ( (bloco_dsk = loc_mapa(lista, bloco_out)) < 0 ) {
               ret = -1;
               goto fim;
            }

            if ( ex_savelista(bloco_dsk, (TEXT *)p_out) < 0 ) {
               ret = -1;
               goto fim;
            }

            qtd_out = 0;
         }
      }

      /* coloca o menor no destino */

      if ( psqcmp_testa(p_new, p_old) < 0 ) {
         if ( XXX > p_new->registro)
            debug("XXX = %l   new = %l",  XXX, p_new->registro);
         XXX = p_new->registro;

         memcpy( (TEXT *)p_out, (TEXT *)p_new, TAM_EVET);
         p_out++;
         qtd_out++;
         p_new++;
         qtd_new--;
      }
      else {
         if ( XXX > p_old->registro)
            debug("XXX = %l   old = %l",  XXX, p_old->registro);
         XXX = p_old->registro;

         memcpy( (TEXT *)p_out, (TEXT *)p_old, TAM_EVET);
         p_out++;
         qtd_out++;
         p_old++;
         qtd_old--;
      }
   }




   if ( qtd_out > 0 ) {
      memset(vet_wkbuf[nb].wb_vetor, '\0', PSQ_SLOT * TAM_EVET);
      memcpy(vet_wkbuf[nb].wb_vetor, vet_wkbuf[out_buf].wb_vetor,
         qtd_out * TAM_EVET);
   }

   ret = 0;

fim :

   if ( ret < 0 ) /* erro de acesso a lista */
      longjmp(psq_jmperro, ret);
}


F__LOC	 SDINDEX *load_lista(lista, bloco)
COUNT lista, bloco;
{  COUNT nb, u_bl_lista;
   SDINDEX *p = NULL;

   /* se lista toda nao faz nada */

   if (vet_lista[lista].flag == 9 ) { /* lista toda */
      p = NULL;
      goto sai;
   }

   if ( vet_lista[lista].tot_vet == 0L ) {
      p = NULL;
      goto sai;
   }

   u_bl_lista = (((LONG)PSQ_SLOT) - 1 + vet_lista[lista].tot_vet) / ((LONG)PSQ_SLOT) ;

   if ( bloco > u_bl_lista  || bloco <= 0 ) {
      p = NULL;
      goto sai;
   }

   nb      = vet_lista[lista].n_buf;

   /* bloco ja disponivel em buffer. Nao precisa ler */
   if ( nb >= 0 && vet_lista[lista].it_bloco == bloco ) {
      p = vet_wkbuf[nb].wb_vetor;
      goto fim;
   }

   /* lista fora dos buffer. Aloca buffer para a lista */
   if ( nb < 0 ) {
      nb = pega_buf(0);
      assoc_buf(lista, nb);
   }

   if ( vet_wkbuf[nb].wb_modif != 0) {
      save_bloco(lista);
   }

   if ( ex_loadlista(lista, bloco, (TEXT *)vet_wkbuf[nb].wb_vetor) < 0 ) {
      p = NULL;
      goto fim;
   }
   else {
      p = vet_wkbuf[nb].wb_vetor;
   }

fim :

   vet_lista[lista].it_bloco = bloco;
   vet_lista[lista].it_item  = 0;

   if ( p == NULL ) {
      longjmp(psq_jmperro, -10);
   }

sai :

   return(p);
}


F__LOC	 COUNT loc_mapa(lista, bloco)
COUNT lista, bloco;
{  COUNT i, entrada;

   entrada = -1;

   for (i = 0, entrada = -1; i < TAM_MAPA; i++) {
      if ( vet_mapa[i].mp_lista == lista && vet_mapa[i].mp_bloco == bloco) {
         entrada = i;
         break;
      }
   }

   if ( entrada == -1 )
      numdebug(682);

   return(entrada);
}



F__LOC	 COUNT ex_loadlista(lista, bloco, buf)
COUNT lista, bloco;
TEXT *buf;
{  COUNT ret, entrada;
   LONG pos;

   entrada = -1;
   ret = -1;


   if ( (entrada = loc_mapa(lista, bloco)) == -1 ) {
      goto fim;
   }

   pos = entrada;
   pos *= ((LONG)PSQ_SLOT);
   pos *= TAM_EVET;



   if ( fseek(fp_psq, pos, 0) == 0 ) {
      if ( fread(buf, TAM_EVET, PSQ_SLOT, fp_psq)
            == PSQ_SLOT ) {
         ret = entrada;
      }
   }

fim :

   return(ret);
}

F__LOC	 VOID libera_lista(lista)
COUNT lista;
{  COUNT i;

   if ( lista < 0 || lista >= NUM_LISTAS ) {
      numdebug(687);
   }
   else {

      if ( vet_lista[lista].flag == 8 && vet_lista[lista].file_cp != NULL )
         fclose(vet_lista[lista].file_cp);

      vet_lista[lista].flag      = 0;
      vet_lista[lista].n_buf     = -1;
      vet_lista[lista].it_bloco  = 0;
      vet_lista[lista].it_item   = 0;
      vet_lista[lista].u_bl_disco= 0;
      vet_lista[lista].tot_doc   = 0L;
      vet_lista[lista].tot_ocor  = 0L;
      vet_lista[lista].tot_campo = 0L;
      vet_lista[lista].tot_vet   = 0L;
      vet_lista[lista].file_cp   = 0L;

      for (i = 0; i < TAM_MAPA; i++) {
         if ( vet_mapa[i].mp_lista == lista ) {
            vet_mapa[i].mp_lista = 0;
            vet_mapa[i].mp_bloco = 0;
         }
      }
   }
}



F__LOC	 VOID  psq_fileger(operacao)
COUNT operacao;
{  COUNT ret;

   switch ( operacao ) {

      case CRIA :

               if ( getenv("LBPESQ") == NULL )
                  strcpy(fname_psq, ap_home_lb);
               else
                  strcpy(fname_psq, getenv("LBPESQ"));

               if (fname_psq[0] != '\0' )
                  strcat(fname_psq, STR_BARRA);

               strcat(fname_psq, PSQ_FMASC);

               if ( (char *)mktemp(fname_psq) == NULL ) {
                  ret = -4;
                  goto fim;
               }

#ifdef MSDOS
               fp_psq = fopen(fname_psq, "w+b");
#else
               fp_psq = fopen(fname_psq, "w+");
#endif

               if ( fp_psq == NULL ) {
                  ret = -5;
                  goto fim;
               }

               else ret = 0;

               break;

      case MATA :

               fclose(fp_psq);
               unlink(fname_psq); /* remove arquivo criado */
               fname_psq[0] = '\0';
               fp_psq = NULL;
               ret = 0;

               break;

   }


fim :

   if ( ret < 0 ) /* erro de acesso a lista */
      longjmp(psq_jmperro, ret);
}




F__LOC	 COUNT v_menos_1(t)
Tab *t;
{
   t = t;
   fwsetcursor( t->win );
   return(-1);
}


F__LOC COUNT show_result(op, arg, total, ocorre, docs)
COUNT op;
TEXT *arg;
LONG total, ocorre, docs;
{  COUNT ret, tam, t, t1;
   COUNT ci, c_info;
   TEXT aux[20], *tit;
   WIN *jt;

   ret = 0;
   tam = 0;

   /* maior titulo de informacao */
   t = strlen(C_RF_02 ); tam = MAX(tam, t);
   t = strlen(C_RF_03 ); tam = MAX(tam, t);
   t = strlen(C_RF_04 ); tam = MAX(tam, t);
   t = strlen(C_RF_05 ); tam = MAX(tam, t);

   /* coluna onde sera impresso o valor */
   c_info = t + 3;

   /* com certeza e o maior numero acrescido de margens e separador */
   sprintf(aux, "%ld", ocorre);
   t  = strlen(aux);
   t1 = strlen(arg);

   t = MAX(t, t1);
   tam = tam + t + 2 + 1 + 2;

   /* verifica se titulo cabe no tamanho da janela */
   tit = ( op == K_F2 ) ? C_RF_01N : C_RF_01R;
   t = strlen(tit) + 2;

   tam = MAX(tam, t);

   ci = (size_x - tam) / 2;


   if ( (jt = abre_win( 7, ci, 14, ci + tam, 0, 0, JS)) == NULL ) {
      ret = K_ESC;
      goto fim;
   }


	fw_title(jt, tit, jt->w_atrib);
	fwbox(jt);

   mvwprintw(jt ,  1,  1, C_RF_02);
   mvwprintw(jt ,  3,  1, C_RF_03);
   mvwprintw(jt ,  4,  1, C_RF_04);
   mvwprintw(jt ,  6,  1, C_RF_05);


   mvwprintw(jt ,  1,  c_info, arg);
   mvwprintw(jt ,  3,  c_info, aux);

   sprintf(aux, "%ld", docs);
   mvwprintw(jt ,  4,  c_info, aux);

   sprintf(aux, "%ld", total);
   mvwprintw(jt ,  6,  c_info, aux);

   fwrefresh(jt);

   ret = men_conf(M_PSQ_SEE, "*", H_CFVERDO, NULL);

   if ( ret != K_ESC ) {
      w_ungc(ret);
      ret = 0;
   }

   fwkill(jt);

fim:

   return(ret);

}


