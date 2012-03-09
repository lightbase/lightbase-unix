#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2prot1.h"

#include  "lb2db.h"

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif

extern   Cw    *cw;
extern   Campo  *pc_glbcrit;

#define   GRP_DFCMP    20

#define   H_TPCIN   "TIPOCPIN"    /* Tipo de campo invalido */
#define   H_NMTAB   "NOMETAB"    /* Nome de tabela invalido */
#define   H_TAMCP   "TAMCAMPO"   /* Tamanho de campos : min1 // txt 132 nor 80   num/for 15 4 decimais */
#define   H_CRTIN   "CRITINV"    /* Tipo de critica incompativel com tipo de campo */
#define   H_ESPMT   "ESPMATR"    /* explicao de matrizes */
#define   H_NOUNI   "NOPDUNIC"   /* Nao pode ser chave unica */
#define   H_TAMUN   "TAMCHUNI"    /*  Tamanho de chave unica */

#define   H_VLIT1   "DATAINV"     /* Data invalida */
#define   H_VLIT2   "HORAINV"     /* Hora invalida */
#define   H_VLIT3   "MAXMMIN"     /* Maximo nao pode ser menor que minimo */
#define   H_VLIT4   "FORAINT"     /* Valor fora do intervalo */

#define   H_SEMMASC "SEMMASC"     /* Campo sem mascara */
#define   H_SOALFA  "usealfa"     /* Use somente campo alfanumerico */

#define  E_TPCIN      loc_str(GRP_DFCMP, 1 )
#define  E_VLTM1      loc_str(GRP_DFCMP, 2 )
#define  E_VLTM2      loc_str(GRP_DFCMP, 3 )
#define  E_VLTM3      loc_str(GRP_DFCMP, 4 )
#define  E_VLTM4      loc_str(GRP_DFCMP, 5 )
#define  E_VLTM5      loc_str(GRP_DFCMP, 6 )
#define  E_LMDAT      loc_str(GRP_DFCMP, 7 )
#define  E_LMHOR      loc_str(GRP_DFCMP, 8 )
#define  E_CRTIN      loc_str(GRP_DFCMP, 9 )
#define  E_NAOBR      loc_str(GRP_DFCMP, 10)
#define  E_USOCOL     loc_str(GRP_DFCMP, 11)
#define  E_NOUNI      loc_str(GRP_DFCMP, 12)
#define  E_TAMUN      loc_str(GRP_DFCMP, 13)
#define  E_VLIT1      loc_str(GRP_DFCMP, 14)
#define  E_VLIT2      loc_str(GRP_DFCMP, 15)
#define  E_VLIT3      loc_str(GRP_DFCMP, 16)
#define  E_VLIT4      loc_str(GRP_DFCMP, 17)


#define  M_TAMTXT     loc_str(GRP_DFCMP, 18)
#define  M_FORMULA    loc_str(GRP_DFCMP, 19)
#define  M_INTEIRO    loc_str(GRP_DFCMP, 20)

#define  E_SOALFA     loc_str(GRP_DFCMP, 64)

#define  E_SEMMASC      loc_str(GRP_DFCMP, 501)

#ifdef MSDOS
   COUNT pega_param(TEXT);
   COUNT op_critica(TEXT);
   COUNT vl_minimo(d_entry *);
   COUNT vl_maximo(d_entry *);
   COUNT vl_ntab(d_entry *);
#else
   COUNT pega_param( );
   COUNT op_critica( );
   COUNT vl_minimo( );
   COUNT vl_maximo( );
   COUNT vl_ntab( );
#endif


#define   DEFINE     'D'
#define   CARREGA    'C'
#define   SELECAO    'S'
#define   FINALIZA   'F'

F__GLB   COUNT cr_campo()
{  COUNT ret, tam;
   d_entry entry;

   ret = 0;
   entry.tecla = 0;

   /* critica do tipo de campo */
   switch( cw->tipo_x ) {
      case 'T' :
      case 'x' :
      case 'N' :
      case 'F' :
      case 'D' :
      case 'H' :
      case 'd' :
      case 'h' :  break;

      default  :  mens_erro(H_TPCIN, E_TPCIN);
                  ret = 3;
                  goto fim;
   }



   tam = atoi(cw->tamanho);

   if (tam <= 0 ) {
      mens_erro(H_TAMCP, E_VLTM1);
      ret = 4;
      goto fim;
   }

   switch( cw->tipo_x ) {

		case 'T' :  if ( tam > 132 ) {
                     mens_erro(H_TAMCP, E_VLTM2);
							ret = 4;
							goto fim;
						}
						break;
		case 'x' :  if ( tam > TM ) {
                     mens_erro(H_TAMCP, E_VLTM3);
							ret = 4;
							goto fim;
						}
						break;

		case 'F' :
		case 'N' :	if ( tam > TAM_CPNUM ) {
                     mens_erro(H_TAMCP, E_VLTM4);
							ret = 4;
							goto fim;
						}
						break;

      case 'D' :
		case 'd' :	if ( tam != 8 ) {
                     mens_erro(H_TAMCP, E_LMDAT);
							ret = 4;
							goto fim;
						}
						break;

      case 'H' :
		case 'h' :	if ( tam != 5 ) {
                     mens_erro(H_TAMCP, E_LMHOR);
							ret = 4;
							goto fim;
						}
						break;
	}

   /* segura F2 no tamanho do campo */
   if ( cw->tipo_x != 'T' && tam > 0 && cw->mascara[0] == '\0' ) {
      mens_erro(H_SEMMASC, E_SEMMASC);
      ret = 6;
      goto fim;
   }


   /* valida tipo de critica em funcao do tipo de campo */

   if (cw->crit_x == ' ' || cw->tipo_x == 'T' || cw->tipo_x == 'F')
      cw->critica[0] = 'N';

   if ( cw->tipo_x == 'D' || cw->tipo_x == 'd' || cw->tipo_x == 'H' || cw->tipo_x == 'h' ) {
      if ( cw->crit_x != 'N' && cw->crit_x != 'I' && cw->crit_x != 'T') {
         mens_erro(H_CRTIN, E_CRTIN);
         ret = 6;
         goto fim;
      }
   }

   if ( cw->crit_x == 'I' ) {
      entry.str = cw->param.inter.minimo;
      if ( vl_minimo(& entry ) != 0 ) {
         ret = 6;
         goto fim;
      }
      entry.str = cw->param.inter.maximo;
      if ( vl_maximo(& entry ) != 0 ) {
         ret = 6;
         goto fim;
      }
      if (cw->param.inter.minimo[0] == '\0' && cw->param.inter.maximo[0] == '\0' )
         cw->crit_x = 'N';
   }

   if ( cw->crit_x == 'T' ) {
      entry.str = cw->param.tab_auto.tabela;
      if ( vl_ntab( & entry ) != 0 ) {
         ret = 6;
         goto fim;
      }
   }

fim :

   return(ret);
}



F__LOC   COUNT vl_rodape(entry)
d_entry *entry;
{  COUNT ret, c, abre, fecha;
   COUNT tecla;
   TEXT *p;

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP ||
		  tecla == K_HOME || tecla == K_PGUP) {
		ret = ACEITA;
		goto fim;
	};

   if ( entry->str[0] == '@' && entry->str[1] == '#' ) {
      abre = fecha = 0;
      for ( p=entry->str; *p != '\0' ; p++) {
         c = *p;
         if ( c == ' ' ) {
            mens_erro(H_ESPMT, E_NAOBR);
            ret = REJEITA;
            goto fim;
         }
         if ( c == '[' )  abre = 1;
         if ( c == ']' )  fecha = 1;
      };

      if ( abre == 0 || fecha == 0 ) {
         mens_erro(H_ESPMT, E_USOCOL);
         ret = REJEITA;
         goto fim;
      }
   }

	ret = ACEITA;

fim :

   return(ret);
}



F__GLB   COUNT pr_tipo(entry)
d_entry *entry;
{  COUNT tipo;

   switch ( cw->tipo_x ) {
      case 'x' : tipo = 1; break;
      case 'T' : tipo = 2; break;
      case 'N' : tipo = 3; break;
      case 'F' : tipo = 4; break;
      case 'D' : tipo = 5; break;
      case 'H' : tipo = 6; break;
      case 'd' : tipo = 7; break;
      case 'h' : tipo = 8; break;
      default  : tipo = 1; break;
   }

   piece(OP_CAMPOS, entry->str, '@', tipo, TD);

   return( 0 );
}



F__GLB   COUNT vl_tipo(entry)
d_entry *entry;
{  COUNT ret, i;
   COUNT tecla;
   TEXT aux[80];

	tecla = entry->tecla;

	if ( tecla == K_ESC ) {
		ret = ACEITA;
		goto fim;
	};

   if ( entry->mudou != 0 ) {
      memset((TEXT *)cw->param.tab_auto.tabela, '\0', sizeof(cw->param.tab_auto.tabela));
      memset((TEXT *)cw->param.tab_auto.autonum, '\0', sizeof(cw->param.tab_auto.autonum));
	   memset((TEXT *)cw->mascara,      '\0', sizeof(cw->mascara));
   }

   ret = REJEITA;

   for ( i = 1 ; i < 20; i++ ) {
      if ( piece(OP_CAMPOS, aux, '@', i, TD) == 0 ) break;

      if ( strcmp( aux, entry->str) == 0 ) {
         ret = ACEITA;
         switch( i ) {
            case  1 : cw->tipo_x = 'x';  break;
            case  2 : cw->tipo_x = 'T';  break;
            case  3 : cw->tipo_x = 'N';  break;
            case  4 : cw->tipo_x = 'F';  break;
            case  5 : cw->tipo_x = 'D';  break;
            case  6 : cw->tipo_x = 'H';  break;
            case  7 : cw->tipo_x = 'd';  break;
            case  8 : cw->tipo_x = 'h';  break;
         }
         break;
      }
   }

   if ( cw->tipo_x == 'T' || cw->tipo_x == 'F') {
      if ( strcmp( cw->unica,  OP_SIM) == 0 ) { /* nao pode chave unica */
         ret = REJEITA;
         mens_erro(H_NOUNI, E_NOUNI);
      }
   }


fim:

   return(ret);

}

F__GLB   COUNT pr_tam(entry)
d_entry *entry;
{  COUNT ret = 0, tam = 0;

   if ( cw->tipo_x == 'T' ){
		if ( atoi(cw->tamanho) <= 0 || cw->mascara[0] != '\0') {
         tam = 73;
   		sprintf(entry->str, "%d", tam);
      }
      entry->mensagem = M_TAMTXT;
      ret = 0;
      goto fim;
   }

   if      ( cw->tipo_x == 'D' || cw->tipo_x == 'd' ) tam = 8;
   else if ( cw->tipo_x == 'H' || cw->tipo_x == 'h' ) tam = 5;


   if ( cw->tipo_x == 'N' || cw->tipo_x == 'F' )
      entry->mensagem = M_INTEIRO;

   if ( tam != 0 ) {
      /* re_imprime o campo e nao para para ler */
		sprintf(entry->str, "%d", tam);
      ret = 1;
   }

fim :

   return(ret);
}



F__GLB   COUNT vl_tam(entry)
d_entry *entry;
{  COUNT ret = 0, tam;
   COUNT tecla;

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP ||
		  tecla == K_HOME || tecla == K_PGUP || tecla == K_F2) {
		ret = ACEITA;
		goto fim;
	};

   tam = atoi(entry->str);

   if (tam <= 0 ) {
      mens_erro(H_TAMCP, E_VLTM1);
      ret = REJEITA;
      goto fim;
   }

   switch( cw->tipo_x ) {

		case 'T' :  if ( tam > 132 ) {
                     mens_erro(H_TAMCP, E_VLTM2);
							ret = REJEITA;
							goto fim;
						}
						break;
		case 'x' :  if ( tam > TM ) {
                     mens_erro(H_TAMCP, E_VLTM3);
							ret = REJEITA;
							goto fim;
						}
						break;

		case 'F' :
		case 'N' :	if ( tam > TAM_CPNUM ) {
                     mens_erro(H_TAMCP, E_VLTM4);
							ret = REJEITA;
							goto fim;
						}
						break;


		default  : 	numdebug(203);
						ret = REJEITA;
                  goto fim;
	}


   if ( tam > LB_MAXUNICA  && strcmp( cw->unica,  OP_SIM) == 0 ) {
      ret = REJEITA;
      mens_erro(H_TAMUN, E_TAMUN);
      goto fim;
   }

	ret = ACEITA;

fim :

   return(ret);
}


F__GLB   COUNT pr_dec(entry)
d_entry *entry;
{  COUNT ret = 0;

   entry = entry;

   if ( cw->tipo_x != 'N' && cw->tipo_x != 'F') {
      ret = 1;
      goto fim;
   }

fim :

   return(ret);
}




F__GLB   COUNT vl_dec(entry)
d_entry *entry;
{  COUNT ret = 0, tam;
   COUNT tecla;

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP ||
		  tecla == K_HOME || tecla == K_PGUP || tecla == K_F2) {
		ret = ACEITA;
		goto fim;
	};

   tam = atoi(entry->str);


   if (tam < 0 || tam > TAM_DECIMAIS ) {
      mens_erro(H_TAMCP, E_VLTM5);
      ret = REJEITA;
      goto fim;
   }

   if ( tam + atoi(cw->tamanho) > TAM_CPNUM ) {
      mens_erro(H_TAMCP, E_VLTM4);
      ret = REJEITA;
      goto fim;
   }

fim :

   return(ret);
}


F__GLB   COUNT pr_mascara(entry)
d_entry *entry;
{  COUNT ret = 0, tam, tam_ant, dec;
	TEXT linha[TM+1], *p;


	/* limpa parte visivel do campo mascara */
	memset((TEXT *)linha, ' ', entry->visivel);	linha[entry->visivel] = '\0';
   fwat(entry->col, entry->lin, entry->win);
	fwprintf(entry->win, "%5A%s", linha);


   /* prepara mascara padrao */
   tam_ant = strlen(cw->mascara);
	tam     = atoi(cw->tamanho);
	dec     = atoi(cw->decimais);

   if ( cw->tipo_x == 'N' || cw->tipo_x == 'F' ) {
      p = linha;
      while ( tam ) {
         *p++ = '9';
         if ( ((tam - 1) % 3 == 0) && tam > 2 )
            *p++ = sep_int;
         tam--;
      }
      if ( dec > 0 ) {
         *p++ = sep_dec;
         while ( dec-- ) {
            *p++ = '9';
         }
      }
      *p = '\0';
   }


	switch ( cw->tipo_x ) {
      case 'T' :
                  linha[0] = '\0';
                  entry->str[0] = '\0';
                  ret = 1;  /* imprime e retorna 0. NAO edita */
                  break;

      case 'N' :  strcpy(entry->str, linha);
                  ret = 1;  /* 1 - imprime e retorna 0. NAO edita */
                  break;

      case 'F' :  entry->mensagem = M_FORMULA;
                  strcpy(cw->param.masc_formula, linha);
                  ret = 0;
                  break;

		case 'x' :
                  memset((TEXT *)linha, 'x', TM);
                  linha[tam] = '\0';

                  /* monta mascara do campo */
                  if ( tam_ant > tam )    cw->mascara[tam] = '\0';
	               else                    strcat(cw->mascara, &linha[tam_ant]);

                  if ( (entry->tipo & E_MOSTRA) == 0  ) {
	                  memset((TEXT *)entry->mascara, 'x', (tam = strlen(cw->mascara)));
	                  entry->mascara[tam] = '\0';
                  }

                  ret = 0;
                  break;

      case 'D' :
      case 'd' :  tam_ant = tam;
                  strcpy(entry->str, "99/99/99");
                  ret = 1;
						break;

      case 'H' :
		case 'h' :  tam_ant = tam;
                  strcpy(entry->str, "99:99");
                  ret = 1;
						break;

		default  : 	ret = 0;
                  break;
   }

   if ( ret == 1 ) {
   	memset((TEXT *)entry->mascara, 'x', TM);  /* seta default da mascara da estrutura read */
   }

	return(ret);

}


F__GLB   COUNT vl_mascara(entry)
d_entry *entry;
{  COUNT ret = 0;
   COUNT tecla;

	tecla = entry->tecla;
   ret = 1;

	if ( tecla == K_ESC || tecla == K_UP ||
		  tecla == K_HOME || tecla == K_PGUP ||
        (tecla == K_F2 && entry->mudou == 0) ) {
		ret = 0;
		goto fim;
	};

   if ( cw->tipo_x == 'F' ) {
      WIN *jm;
      DOUBLE val;

      jm = abre_mensagem(str_nada);
      if ( calc_avalia(entry->str, &val, jm, calc_token, 'A') >= 0 ) {
         ret = ACEITA;
      }
      else {
         ret = REJEITA;
         fwqgc(jm);
      }

      fecha_mensagem(jm);
   }

fim :

	memset((TEXT *)entry->mascara, 'x', TM);  /* seta default da mascara da estrutura read */

	return(ret);
}





F__GLB   COUNT pr_critica(entry)
d_entry *entry;
{  COUNT ret = 0, tipo;

	if ( cw->tipo_x == 'T' || cw->tipo_x == 'F') {
      cw->crit_x = 'N';
      piece(OP_CRITICAS, entry->str, '@', 1, TD);
      ret = ACT_REIMP;         /* re_imprime o campo */
   }

   else {

      switch ( cw->crit_x ) {
         case 'N' : tipo = 1;  break;
         case 'I' : tipo = 2;  break;
         case 'T' : tipo = 3;  break;
         case 'M' : tipo = 4;  break;
         case 'C' : tipo = 5;  break;
         case 'G' : tipo = 6;  break;
#ifdef AUTO_INCREMENTO
         case 'A' : tipo = 7;  break;
#endif
         default  : tipo = 1;  break;
      }

      piece(OP_CRITICAS, entry->str, '@', tipo, TD);

      ret = 0;

   }

   return(ret);
}

F__GLB   COUNT vl_critica(entry)
d_entry *entry;
{  COUNT ret, i;
	COUNT tecla;
	TEXT  aux[80];

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP ) {
		ret = ACEITA;
		goto fim;
	};



   ret = REJEITA;

   for ( i = 1 ; i < 20; i++ ) {
      if ( piece(OP_CRITICAS, aux, '@', i, TD) == 0 ) break;
      if ( strcmp( aux, entry->str) == 0 ) {
         ret = ACEITA;
         switch( i ) {
            case  1 : cw->crit_x = 'N';  break;
            case  2 : cw->crit_x = 'I';  ret = pega_param('I'); break;
            case  3 : cw->crit_x = 'T';  ret = pega_param('T'); break;
            case  4 : cw->crit_x = 'M';  break;
            case  5 : cw->crit_x = 'C';  break;
            case  6 : cw->crit_x = 'G';  break;
#ifdef AUTO_INCREMENTO
            case  7 : cw->crit_x = 'A';  break;
#endif
         }
         break;
      }
   }

   if ( cw->tipo_x != 'x' && cw->crit_x == 'A' ) {
      mens_erro(H_SOALFA, E_SOALFA);
      ret = REJEITA;
   }

fim:

   return(ret);
}



F__GLB   COUNT vl_ntab(entry)
d_entry *entry;
{  COUNT ret, coluna;
   COUNT tecla, n;
   SC     str[ 200 ];
   SC     nome_tab[PATH_MAX];
   SC     titulo_tab[TE+1];
   VOID   *opcoes;


   str[ 0 ] = '\0';

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP ||
		  tecla == K_HOME || tecla == K_PGUP || tecla == K_F2) {
		ret = 0;
		goto fim;
	};

   strcpy(nome_tab, entry->str);
   sprintf(titulo_tab," %s ", entry->str);


   n = atoi(nome_tab);
   if ( n > 0 && n < 99 && strlen(nome_tab) < 3 ) {
      /* tabelas numericas nao sao carregadas. Ficam indexadas */
      ret = 0;
      goto fim;
   }


	if ( val_indice( nome_tab ) != OK) {
      mens_erro(H_NMTAB, E_NOMEIN);
		ret = -1;
   }

   else {   /* ANANIAS @ validar nome da tabela arquivos especiais */

   	if ( tecla != 0 ) {

         coluna = 74 - MAX(strlen(titulo_tab), strlen(cw->mascara));

         file_tabela( nome_tab, str, coluna, 9,
                     size_x, 10, cw->mascara,
                     MP_CAMPO, MP_BARRA, 1, MP_JANELA, MP_DESTAQUE,
                     titulo_tab, str_nada, INSERE | ALTERA | DELETA, DEFINE , &opcoes);

         file_tabela( NULL,NULL,0,0,0,0,NULL,0,0,0,0,0,NULL,NULL,0,
                      FINALIZA, &opcoes);

      }

      ret = 0;
   }

fim:

   return(ret);
}


F__GLB   COUNT vl_minimo(entry)
d_entry *entry;
{  COUNT ret;
   COUNT tecla;


	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP ||
		  tecla == K_HOME || tecla == K_PGUP || tecla == K_F2) {
		ret = ACEITA;
		goto fim;
	};


   ret = ACEITA;

   if (cw->tipo_x == 'D' || cw->tipo_x == 'd' ) {
      if ( valdata(cw->param.inter.minimo) < 0 ) {
         mens_erro(H_VLIT1, E_VLIT1);
         ret = REJEITA;
      }
   }
   else if (cw->tipo_x == 'H' || cw->tipo_x == 'h' ) {
      if ( valhora(cw->param.inter.minimo) < 0 ) {
         mens_erro(H_VLIT2, E_VLIT2);
         ret = REJEITA;
      }
   }

fim :

   return(ret);
}



F__GLB   COUNT vl_maximo(entry)
d_entry *entry;
{  COUNT ret;
   COUNT tecla;
   TEXT *v1, *v2;


	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP ||
		  tecla == K_HOME || tecla == K_PGUP || tecla == K_F2) {
		ret = 0;
		goto fim;
	};


   ret = ACEITA;

   if (cw->tipo_x == 'D' || cw->tipo_x == 'd' ) {
      if ( valdata(cw->param.inter.maximo) < 0 ) {
         mens_erro(H_VLIT1, E_VLIT1);
         ret = REJEITA;
         goto fim;
      }
   }
   else if (cw->tipo_x == 'H' || cw->tipo_x == 'h' ) {
      if ( valhora(cw->param.inter.maximo) < 0 ) {
         mens_erro(H_VLIT2, E_VLIT2);
         ret = REJEITA;
         goto fim;
      }
   }


   v1 = cw->param.inter.minimo;
   v2 = cw->param.inter.maximo;

	if ( intervalo(v1, v2, v2, ( (cw->tipo_x == 'D' || cw->tipo_x == 'd') ? 'D' : 'V') ) < 0 ) {
      mens_erro(H_VLIT3, E_VLIT3);
      ret = -1;
   }
   else
      ret = 0;


fim:

   return(ret);
}


F__GLB   COUNT cr_unica(entry)
d_entry *entry;
{  COUNT ret;

	ret = ACEITA;

	if ( entry->tecla == K_ESC ) {
		goto fim;
	};

   if ( strcmp( cw->unica,  OP_SIM) != 0 ) {
      goto fim;
   }

   if ( atoi(cw->tamanho) > LB_MAXUNICA ) {
      ret = REJEITA;
      mens_erro(H_TAMUN, E_TAMUN);
      goto fim;
   }

   if ( cw->tipo_x == 'T' || cw->tipo_x == 'F') {
      ret = REJEITA;
      mens_erro(H_NOUNI, E_NOUNI);
      goto fim;
   }

fim :

   return(ret);
}

