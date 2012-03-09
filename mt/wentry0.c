#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <ctype.h>
#include   "window.h"
#include   "entrada.h"
#include   "mensagem.h"

static SS  nada( );

static SS nada( e ) /* Nao pode ser NEAR */
d_entry     *e;
{
   e = e;  return( 0 );
}

SC   *tira_separador( dir )

SC   *dir;
{
   SS   I;

   if ( dir != NULL  &&
       (I = strlen( dir )) > 0  &&
       dir[ I - 1 ] == DIRSEP    )   dir[ I - 1 ] = '\0';

   return( dir );
}

SC   *ins_brancos( str, quant, carac )

SC   *str;
SS   quant;
SS   carac;
{
   SC   *S, *T;

   for ( S = str + strlen( str ), T = S + quant;   S >= str;   *T-- = *S-- )
      ;

   for (    ;   T >= str;   *T-- = carac )
      ;

   return ( str );
}

/***
Primitiva que justifica linha.
Retorna o tamanho final da linha que normalmemte e "tam".
Leva em conta o tamanho de "orig" em "len".
Pede o caracter de enchimento em "chr".
E devolve o deslocamento em "off" da posicao "pos".
***/
SS   p_just_line( dest, orig, tam, len, chr, pos, off )

SC   *dest, *orig;
SS   tam, len, chr, pos, *off;
{
   SC   *P, *C;
   SS   I, J, frestas, div, res;

   /* "C" e "off" sao usadas para ver deslocamento do CURSOR em ETMAIN.C */
   *off = 0;
   C = dest + pos;

   *dest = '\0';

   if ( len == 0 || tam == 0 || orig == NULL || *orig == '\0' )   return 0;

   strncpy( dest, orig, len );

   dest[ len ] = '\0';
   dest[ tam ] = '\0';

   /*** Garantimos que esta sem brancos no final ***/
   trim( dest );

   I = strlen( dest );
   if ( I >= tam )    return( I );

   /*** Posicionemos no primeiro NAO branco ***/
   for ( P = dest;   *P != '\0' && *P == ' ';   P++ )
      ;

   for( frestas = 0;   *P != '\0';   frestas++ )
   {
      for (    ;   *P != '\0' && *P != ' ';   P++ )
         ;

      for (    ;   *P != '\0' && *P == ' ';   P++ )
         ;

      if ( *P == '\0' )    break;
   }

   if ( ! frestas )   return( I );

   /*** I NAO pode ser maior que "tam" ***/
   J = tam - I;

   if ( J )
   {
      div = J / frestas;
      res = J % frestas;

      for ( P = dest + I;   frestas && P > dest;   frestas-- )
      {
         for (    ;   P > dest && *P != ' ';   P-- )
            ;

         if ( res ) {  J = 1;  res--;  }
         else          J = 0;

         /*** Calcula offset relativo a posicao fornecida ***/
         /*** Usado para ver deslocamento do CURSOR em ETMAIN.C ***/
         if ( C > P )   *off += div + J;

         ins_brancos( P + 1, div + J, chr );

         for (    ;   P > dest && *P == ' ';   P-- )
            ;
      }
   }

   return ( tam );
}


SC   *just_line( dest, orig, tam )

SC   *dest, *orig;
SS   tam;
{
   SS   off;

   p_just_line( dest, orig, tam, tam, (SS) ' ', 0, &off );

   return ( dest );
}

SC   *eliminabr( str )

SC   *str;
{
   /* ---> Tira todos os brancos e TABs do inicio, fim e meio de "str" */
   SC *p, *s;

   s = p = str;

   while( *p )
   {
      while( *p == ' ' ||  *p == TAB_CHAR )   ++p;

      while( *p  &&  *p != ' '  &&  *p != TAB_CHAR )   *s++ = *p++;
   }

   *s = 0;

   return ( str );
}

SC   *tirabr( str )

SC   *str;
{
   /* ---> Tira todos os brancos e TABs do inicio e do fim de str, e os
         brancos excessivos internos de str.
   */

   SC *S, *P;

   S = P = str;

   while( *P == ' ' || *P == TAB_CHAR )   ++P;

   while( *P )
   {
      while( *P != ' ' &&  *P != TAB_CHAR )
      {
         *S++ = *P++;

         if ( ! *P )   break;
      }

      if ( *P == ' ' || *P == TAB_CHAR )
      {
         *S++ = *P++;

         while( *P == ' ' || *P == TAB_CHAR )   ++P;

         if ( ! *P )   --S;
      }
   }

   *S = 0;

   return ( str );
}


SC *strip_nl( buf )

SC *buf;
{
   SS  tam;

   tam = strlen( buf );

   while ( --tam >= 0  &&  (buf[ tam ] == '\n'  ||  buf[ tam ] == '\r') )
   {
      buf[ tam ] = '\0';
   }

   return ( buf );
}

SC *trim( S )

SC *S;
{
   SC   *p = S;

   while ( *p != '\0' )  p++;   p--;

   while ( p >= S  &&  *p == ' ' )   p--;   p++;

   *p = '\0';

   return ( S );
}


SC *ltrim( S )

SC *S;
{
   SC  *p = S;

   while ( *p == ' ' )   p++;

   if ( p != S )  strcpy( S, p );

   return ( S );
}


VOID seta_insert( x )
SS x;
{
   insercao = x;
}


US e_print( c )

US  c;
{
   SS      I;
   static  UC  montou = 0;
   static  UC  ug_print[ 96 ];
   /*** A Tabela BRASCII nao possui caracteres entre 128 e 159 ***/

   if ( ! montou )
   {
      montou++;

      for ( I = 0;   I < 96;   I++ )   ug_print[ I ] = 0;

      ug_print[ K_EXCLA_INV - 160 ] = 1;
      ug_print[ K_PARA      - 160 ] = 1;
      ug_print[ K_TREMA     - 160 ] = 1;
      ug_print[ K_ORD_a     - 160 ] = 1;
      ug_print[ K_MAISMENOS - 160 ] = 1;
      ug_print[ K_ORD_o     - 160 ] = 1;
      ug_print[ K_INTER_INV - 160 ] = 1;
      ug_print[ K_GRA_A     - 160 ] = 1;
      ug_print[ K_AGU_A     - 160 ] = 1;
      ug_print[ K_CIR_A     - 160 ] = 1;
      ug_print[ K_TIL_A     - 160 ] = 1;
      ug_print[ K_TRE_A     - 160 ] = 1;
      ug_print[ K_BOL_A     - 160 ] = 1;
      ug_print[ K_CED_C     - 160 ] = 1;
      ug_print[ K_GRA_E     - 160 ] = 1;
      ug_print[ K_AGU_E     - 160 ] = 1;
      ug_print[ K_CIR_E     - 160 ] = 1;
      ug_print[ K_TRE_E     - 160 ] = 1;
      ug_print[ K_GRA_I     - 160 ] = 1;
      ug_print[ K_AGU_I     - 160 ] = 1;
      ug_print[ K_CIR_I     - 160 ] = 1;
      ug_print[ K_TRE_I     - 160 ] = 1;
      ug_print[ K_TIL_N     - 160 ] = 1;
      ug_print[ K_GRA_O     - 160 ] = 1;
      ug_print[ K_AGU_O     - 160 ] = 1;
      ug_print[ K_CIR_O     - 160 ] = 1;
      ug_print[ K_TIL_O     - 160 ] = 1;
      ug_print[ K_TRE_O     - 160 ] = 1;
      ug_print[ K_COR_O     - 160 ] = 1;
      ug_print[ K_GRA_U     - 160 ] = 1;
      ug_print[ K_AGU_U     - 160 ] = 1;
      ug_print[ K_CIR_U     - 160 ] = 1;
      ug_print[ K_TRE_U     - 160 ] = 1;
      ug_print[ K_BETA      - 160 ] = 1;
      ug_print[ K_GRA_a     - 160 ] = 1;
      ug_print[ K_AGU_a     - 160 ] = 1;
      ug_print[ K_CIR_a     - 160 ] = 1;
      ug_print[ K_TIL_a     - 160 ] = 1;
      ug_print[ K_TRE_a     - 160 ] = 1;
      ug_print[ K_BOL_a     - 160 ] = 1;
      ug_print[ K_CED_c     - 160 ] = 1;
      ug_print[ K_GRA_e     - 160 ] = 1;
      ug_print[ K_AGU_e     - 160 ] = 1;
      ug_print[ K_CIR_e     - 160 ] = 1;
      ug_print[ K_TRE_e     - 160 ] = 1;
      ug_print[ K_GRA_i     - 160 ] = 1;
      ug_print[ K_AGU_i     - 160 ] = 1;
      ug_print[ K_CIR_i     - 160 ] = 1;
      ug_print[ K_TRE_i     - 160 ] = 1;
      ug_print[ K_TIL_n     - 160 ] = 1;
      ug_print[ K_GRA_o     - 160 ] = 1;
      ug_print[ K_AGU_o     - 160 ] = 1;
      ug_print[ K_CIR_o     - 160 ] = 1;
      ug_print[ K_TIL_o     - 160 ] = 1;
      ug_print[ K_TRE_o     - 160 ] = 1;
      ug_print[ K_COR_o     - 160 ] = 1;
      ug_print[ K_GRA_u     - 160 ] = 1;
      ug_print[ K_AGU_u     - 160 ] = 1;
      ug_print[ K_CIR_u     - 160 ] = 1;
      ug_print[ K_TRE_u     - 160 ] = 1;
      ug_print[ K_COM_MAC   - 160 ] = 1;
      ug_print[ K_ETPARA    - 160 ] = 1;
   }

   if ( (UC) c >= ' '  &&  (UC) c <= 127 )   return( c );

   /*** Comeco da tabela BRASCII = 160 ***/
   if ( (UC) c >= 160  &&  /*** (UC) c <= 255  && sempre TRUE ***/
                           ug_print[ (UC) c - 160 ] )   return( c );

   return( 0 );
}


SS   piece( s1, s2, c, n, tam )

SC    *s1, *s2;
SS    c, n, tam;
{
   UC   *p1,*p2;
   SS   oc;

   oc = n - 1;   p1 = (UC *) s1;   p2 = (UC *) s2;

   while( oc > 0  &&  *p1 != '\0' ) {   if ( *p1++  ==  c )  oc--;  }

   if ( oc == 0 )
   {
      while( tam-- > 0 && *p1 != c && *p1 != '\0' ) {  *p2++ = *p1++;  }
   }

   *p2 = '\0';

   return( strlen( s2 ) );
}


/*#  ---------------------------------------------------------------------
**#   Funcao...: VOID pr_escape
**#   Objetivo.: Prepara string para ser escapes da funcao entrada
**#   Entrada..: teclas - string contendo as teclas 1234567890 ENUD
**#              linha  - endereco da string a ser montada
**#   Retorna..: nada
**#
**/

VOID pr_escape( teclas, linha )

SC   *teclas;
UC   *linha;
{
   int i;

   UC *p;

   p = linha;

   for (i=0; i <= strlen(teclas); i++) {

      switch (teclas[i]) {
         case ' ' :                  break;         /*   BRANCO    */
         case '1' : *p++ = K_F1;     break;         /*   F1    */
         case '2' : *p++ = K_F2;     break;         /*   F2    */
         case '3' : *p++ = K_F3;     break;         /*   F3    */
         case '4' : *p++ = K_F4;     break;         /*   F4    */
         case '5' : *p++ = K_F5;     break;         /*   F5    */
         case '6' : *p++ = K_F6;     break;         /*   F6    */
         case '7' : *p++ = K_F7;     break;         /*   F7    */
         case '8' : *p++ = K_F8;     break;         /*   F8    */
         case '9' : *p++ = K_F9;     break;         /*   F9    */
         case '0' : *p++ = K_F0;     break;         /*   F10   */
         case 'B' : *p++ = K_BTAB;   break;         /*   BTAB  */
         case 'D' : *p++ = K_PGDN;   break;         /*   PgDn  */
         case 'E' : *p++ = K_ESC;    break;         /*   ESC   */
         case 'G' : *p++ = K_CPGDN;  break;         /*  ^PgDn  */
         case 'N' : *p++ = K_ENTER;  break;         /*   ENTER */
         case 'P' : *p++ = K_CPGUP;  break;         /*  ^PgUp  */
         case 'T' : *p++ = K_TAB;    break;         /*   TAB   */
         case 'U' : *p++ = K_PGUP;   break;         /*   PgUp  */
      }
   }

   *p = '\0';
}


SS  entrada( e, cursor )

d_entry     *e;
SS          cursor;
{
   if ( e->ok  !=  ENT_OK )   return( -1 );

   switch ( e->tipo & 0xFF )
   {
   default      :
   case E_ALFA  :    return ( ent_alfa ( e, cursor ) );

   case E_VALOR :    return ( ent_valor( e ) );

   case E_LISTA :    return ( ent_lista( e, cursor ) );
   }
}


VOID d_entrada( e, w, str, visivel, masc, mens, col, lin, tipo )

d_entry        *e;
struct Window  *w;
SC           *str;
SS            visivel;
SC           *masc;
SC           *mens;
SS            col;
SS            lin;
US            tipo;
{
   e->ok       = 0;
   e->win      = w;
   e->mensagem = mens;
   e->col      = col;
   e->lin      = lin;
   e->tipo     = tipo;
   e->mascara  = masc;
   e->e_atr    = w->w_optatr;
   e->n_atr    = w->campoatr;
   e->escape = ( UC * ) "";
   e->lista    = ( SC * ) e->escape;
   e->men      = NULL;
   e->prepara  = nada;
   e->critica  = nada;
   e->crit_char= nada;
   e->tecla    = K_RT;
   e->str      = str;
   e->visivel  = visivel;
   e->refresh  = 1;
   e->masc_alfa= loc_str( STR_OBRIG, MASC_ALFA );
   e->masc_num = loc_str( STR_OBRIG, MASC_NUM  );

   e->ok       = ENT_OK;
}



/*#  ---------------------------------------------------------------------
**#   Funcao...: SS ed_read()
**#   Objetivo.: Possibilitar a entrada de dados de tela cheia
**#   Entrada..: j - Janela onde se dara a entrada
**#              ed_vet - vetor de definicoes de campos
**#              inic   - numero do campo onde devera ser iniciada a leitura
**#              flag   - Tipo de comportamento da funcao. Pode ser:
**#                       0 - Limpa/le
**#                       1 - mostra/le
**#                       2 - so limpa
**#                       3 - so mostra
**#              tec_fun - Teclas de funcoes que deverao ser aceitas
**#                        1 a 0 - F1 a F10
**#                        E     - ESC (sempre deve ser informado)
**#                        N     - ENTER
**#                        U     - PG_UP
**#                        D     - PG_DN
**#
**#   Retorna..: Ultima tecla pressionada.  Pode ser :
**#                 - 0 --> nada a ser lido ( ult < 0 )
**#                 - K_ESC
**#                 - K_UP
**#                 - K_PGUP
**#                 - K_DN
**#                 - K_PGDN
**#                 - K_ENTER
**/

SS ed_read( j, ed_vet, inic, flag, tec_fun )

WIN   *j;
DTela ed_vet[];
SS    inic;
SS    flag;
SC    *tec_fun;
{
   SS      fim_loop = 0, ult, ret_ent, ult_tecla = 0, i, lin;
   US      tipo;
   SC      linha[ MAX_EDIT + 1 ], *pl;
   SC      *ajuda_ant = win_ajuda;
   UC      str_escape[20];
   WIN     *jm = NULL, *jf = NULL;
   DTela   *t;
   d_entry work;

   ult = 0;

   while ( ed_vet[ult].destino != NULL) {

      if (ed_vet[ult].visivel == 0)
         ed_vet[ult].visivel = strlen(ed_vet[ult].mascara);

      ult++;
   }

   if (--ult < 0 ) { ret_ent = 0; goto fim; };

   inic--;                      /* o primeiro campo e o campo zero */
   if (inic < 0  ) inic = 0;
   if (inic > ult) inic = ult;

   /*
   **    Display dos campos
   */

   for ( i = 0; i <= ult; i++ ) {

      t = &ed_vet[i];

      if ( flag == SO_LIMPA ) { pl = linha; linha[0] = '\0'; }
      else                      pl = t->destino;

      if ( (lin = t->linha) < 0 ) lin = j->w_cy;

      d_entrada( &work, j, pl, t->visivel, t->mascara, NULL,
                  t->coluna, lin, (t->param | E_MOSTRA) );

      if ( t->opcoes != NULL ) work.lista   = t->opcoes;
      if ( t->prep != NULL )   work.prepara = t->prep;

      ret_ent = entrada( &work, 0);

   }

   if ( flag == SO_LIMPA || flag == SO_MOSTRA ) { /* nao envolvem leitura */
      ret_ent = 0;
      goto fim;
   }

   pr_escape(tec_fun, str_escape);

   jm = abre_mensagem("");
   jf = abre_funcoes(funcao(tec_fun));

   while ( ! fim_loop ){

      t = &ed_vet[inic];

      w_lastread = inic + 1;   /* armazana o ultimo campo lido */

      if ( t->visivel < strlen(t->mascara ) ) tipo = t->param | E_CONFIRMA;
      else                                    tipo = t->param;

      if ( (lin = t->linha) < 0 ) lin = j->w_cy;

      d_entrada( &work, j, t->destino, t->visivel, t->mascara, t->rodape,
                 t->coluna, lin, tipo );

      work.men     = jm;
      work.escape  = str_escape;
      win_ajuda    = t->help;

      if ( t->opcoes != NULL ) work.lista   = t->opcoes;

      if (t->prep != NULL ) work.prepara = t->prep;
      if (t->crit != NULL ) work.critica = t->crit;

      ult_tecla = ret_ent; /* salva ultima tecla digitada para retorno */
                           /* prepara = 0 */

      ret_ent = entrada( &work, 0);

      /*
      **    Verifica se Campo deve ser saltado,
      **    assuma o sentido da ultima tecla
      */
      if ( ret_ent == 0 ) {
         if ( ult_tecla  == K_UP )         ret_ent = K_UP;
         else if ( ult_tecla == K_ENTER )  ret_ent = K_ENTER;
         else                              ret_ent = K_DN;
      }

      /*
      **    Verifica se leitura saiu por uma tecla de ESCAPE
      */

      else if ( strchr((SC *)str_escape, ret_ent) != NULL ) { /* ANANIAS @ sera que strrchr vai achar > 127 */
         fim_loop = 1;
         break;
      }

      switch ( ret_ent ) {

         case K_ESC  :  fim_loop = 1;
                        break;

         case K_BTAB :
         case K_UP   :  if (inic == 0 )   fim_loop = 1;
                        else              inic--;
                        break;

         case K_PGUP :  fim_loop = 1;
                        break;

         case K_TAB  :
         case K_PGDN :
         case K_DN   :
         case K_ENTER:
         default     :  if (inic == ult ) fim_loop = 1;
                        else              inic++;
                        break;
      }
   }

fim :

   win_ajuda = ajuda_ant; /* restaura ajuda anterior */

   fecha_funcoes(jf);
   fecha_mensagem(jm);

   return(ret_ent);
}


VOID   seta_dtela( DT, destino, mascara, rodape, linha, coluna, visivel,
                   param, help, opcoes, prep, crit )

DTela  *DT;
SC     *destino;
SC     *mascara;
SC     *rodape;
SS     linha;
SS     coluna;
SS     visivel;
US     param;
SC     *help;
SC     *opcoes;
SS     (*prep)();
SS     (*crit)();
{
   DT->destino = destino;
   DT->mascara = mascara;
   DT->rodape  = rodape;
   DT->linha   = linha;
   DT->coluna  = coluna;
   DT->visivel = visivel;
   DT->param   = param;
   DT->help    = help;
   DT->opcoes  = opcoes;
   DT->prep    = prep;
   DT->crit    = crit;
}


#ifdef  Exemplo_de_usu_ED_READ

DTela dx[ ] =
{
/* Descr  */ { cw.desc, x40, "Informe a descricao do campo",
               0, 15, 37, E_ALFA, "CP_DESC", NULL, NULL, NULL },

/* rodape */ { cw.rodape, x50, "Informe a mensagem de rodape para o campo",
               1, 15, 37, E_ALFA, "CP_RODA", NULL, NULL, NULL },

/* tipo   */ { cw.tipo, x30, "Utilize <ESPACO> para selecionar",
               2, 15, 0, E_LISTA, "CP_TIPO", OP_CAMPOS, pr_tipo, vl_tipo },

/* tam    */ { cw.tamanho, "999", "Informe o tamanho do campo",
               3, 15,  0, E_VALOR, "CP_TAM", NULL, pr_tam, vl_tam },

/* mascara*/ { cw.mascara, xTM, "Informe a mascara de edicao",
               4, 15, 37, E_ALFA, "CP_MASC", NULL, pr_mascara, vl_mascara },

/* critica*/ { cw.critica, x30, "Utilize <ESPACO>",
               5, 15, 0, E_LISTA, "CP_CRIT", OP_CRITICAS, pr_crit, vl_crit },

/* Index  */ { cw.index, "xxx", M_INVERTE,
               8, 48,  0, E_LISTA, "CP_INDE", OP_SIM_NAO, NULL, NULL },

/* Unica  */ { cw.unica, "xxx", M_INVERTE,
               9, 48,  0, E_LISTA, "CP_UNIC", OP_SIM_NAO, NULL, NULL },

/* branco */ { cw.branco,"xxx", M_INVERTE,
               10, 48,  0, E_LISTA, "CP_BRAN", OP_SIM_NAO, NULL, NULL },

/* freq   */ { cw.freq,  "xxx", M_INVERTE,
               11, 48,  0, E_LISTA, "CP_FREQ", OP_SIM_NAO, NULL, vl_freq },

/* fim    */ { NULL, NULL }
};

ed1_read( j2, &dx[0], 0, 3, "" );

#endif  /* Exemplo do ED_READ() */




