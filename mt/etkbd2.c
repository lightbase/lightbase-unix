
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  "window.h"
#include  "ethead.h"
#include  <mensagem.h>

#ifdef    VAXVMS
#include  <types.h>
#include  <stat.h>
#else
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

#define   MAX_GET_RD    200
#define   MAX_KEYLIST   200

#ifdef MEDIT

#ifdef PROTO_ANSI
static VOID Add_HK( SC *, SI );
static SI   comp_HK( SI, SC *, SC *, SS, TECLAS *, SS, COMANDOS *, SS );
static SI   get_k_or_ctrl( SC *, SC ** );
#else
static VOID Add_HK( );
static SI   comp_HK( );
static SI   get_k_or_ctrl( );
#endif

#endif

SS comp_diret( original, limpo, linha, teclas, nteclas )

SC     *original,
       *limpo;
SS      linha;
TECLAS *teclas;
SS      nteclas;
{
   /***************** Compila diretivas ********************/

   SS           TS, I;
   SC          *PS;
   SC           diret[ MAX_CHAR * 3 + 10 ]; /* Concatenadas 3 loc_str() */
   SC          *ptchar;
   struct stat  status;
   SI           stat();
   SI           tblen;
   TECLAS      *Tcl;
   TOKEN        Tok;

   sprintf( diret, "%s%s%s", loc_str( STR_COMP, COMP_189 ),
                             loc_str( STR_COMP, COMP_190 ),
                             loc_str( STR_COMP, COMP_191 ) );

   ptchar = &limpo[1];
   I = pega_token( ptchar, "=;", &Tok );    /* diretiva */
   eliminabr( Tok.cadeia );
   if( !( *Tok.cadeia ) )
   {
      erro_par( original, loc_str( STR_COMP, COMP_514 ), linha );
      return 1;
   }

   strupr( Tok.cadeia );
   ptchar = strstr( diret, Tok.cadeia );
   if( !ptchar )
   {
      erro_par( original, loc_str( STR_COMP, COMP_515 ), linha );
      return 1;
   }

   while( *(--ptchar) != '|' );
   switch( atoi( ++ptchar ) )
   {
   case 0:					/*  HELPDIR  */
      if( I && (Tok.sep == '=') )
      {
         pega_token ( NULL, ";", &Tok );   /* pegar o diretorio */
         eliminabr( Tok.cadeia );
      }

      if( !I || !(*Tok.cadeia) )
      {
         erro_par( original, loc_str( STR_COMP, COMP_516 ), linha );
         return 1;
      }

      if( stat( Tok.cadeia, &status ) == -1 )
      {
         erro_par( original, loc_str( STR_COMP, COMP_517 ), linha );
         return 1;
      }

      if( (status.st_mode & S_IFDIR) == S_IFDIR )
      {
         dir_ajuda = (SC *) malloc( strlen(Tok.cadeia)+2 );
         strcpy( dir_ajuda, Tok.cadeia );
      }
      else
      {
         erro_par( original, loc_str( STR_COMP, COMP_517 ), linha );
         return 1;
      }
      break;

   case 1:					/* INIFIMBLOCO */
      blk_inifim = 1;   /* Faz com que o inicio de bloco se comporte como */
      break;            /* Final de bloco compatibilizar com Norton Editor */

   case 2:					/*  TAB  */
      if( I && (Tok.sep == '=') )
      {
         pega_token ( NULL, ";", &Tok );   /* pegar tablen */
         eliminabr(Tok.cadeia);
      }

      if( !I || !(*Tok.cadeia) || !(tblen = atoi( Tok.cadeia )))
      {
         erro_par( original, loc_str( STR_COMP, COMP_518 ), linha );
         return 1;
      }

      tab_width = tblen;
      break;

   case 3:					/*  HELPKEY */
      if( I && (Tok.sep == '=') )
      {
         pega_token ( NULL, ";", &Tok );   /* pegar nome de tecla */
         eliminabr( Tok.cadeia );
      }

      if( !I || !(*Tok.cadeia) )
      {
         erro_par( original, loc_str( STR_COMP, COMP_519 ), linha );
         return 1;
      }

      strupr(Tok.cadeia);
      if( !(Tcl = bsearch((SC *) Tok.cadeia, (SC *) teclas,
                                  nteclas, sizeof(TECLAS), compara2 )))
      {
         erro_par( original, loc_str( STR_COMP, COMP_520 ), linha );
         return 1;
      }

      tec_ajuda = Tcl->numero;
      break;

   case 4:					/* UTILKEY */
      if( I && (Tok.sep == '=') )
      {
         pega_token ( NULL, ";", &Tok );   /* pegar nome de tecla */
         eliminabr(Tok.cadeia);
      }

      if( !I || !( *Tok.cadeia ) )
      {
         erro_par( original, loc_str( STR_COMP, COMP_521 ), linha );
         return 1;
      }

      strupr( Tok.cadeia );                            /* Garante maiusculas */
      if( !(Tcl = bsearch((SC *) Tok.cadeia, (SC *) teclas,
                          nteclas, sizeof(TECLAS), compara2 )))
      {
         erro_par( original, loc_str( STR_COMP, COMP_520 ), linha );
         return 1;
      }

      tec_util = Tcl->numero;
      break;

   case 5:					/* MOSTRATAB */
      tab_ug_sp = 1;    /* Os Tabs serao mostrados como Falso Branco */
      break;            /* Com efeito apenas visual no video */

   case 6:					/* BRANCOFIM */
      no_br_fim = 0;    /* Nao Retira os Brancos e Tabs do final da linha */
      break;            /* Com efeito apenas ao salvar os arquivos */

   case 7:					/* TABBRANCO */
      tab_tab = 0;      /* Tabs serao expandidos por brancos respeitando */
      break;            /* os complementos do tamanho, video e arquivo */

   case 8:					/* BRANCOTAB */
      br_tab = 1;       /* Ao salvar, os primeiros brancos serao trocados */
      break;            /* por TABs e complementados por brancos */

   case 9:					/* MULTIKEY */
                        /* Associa uma linha de funcao a primeira tecla */
                        /* de um comando multiplo */
      if( I && (Tok.sep == '=') )
      {
         pega_token ( NULL, ":", &Tok );   /* pegar nome de tecla */
         eliminabr(Tok.cadeia);
      }

      if( !I || !Tok.achou || !( *Tok.cadeia ) )
      {
         erro_par( original, loc_str( STR_COMP, COMP_522 ), linha );
         return 1;
      }

      strupr( Tok.cadeia );                            /* Garante maiusculas */
      if( !(Tcl = bsearch((SC *) Tok.cadeia, (SC *) teclas,
                          nteclas, sizeof(TECLAS), compara2 )))
      {
         erro_par( original, loc_str( STR_COMP, COMP_520 ), linha );
         return 1;
      }

      pega_token ( NULL, ";", &Tok );   /* pegar a linha de funcao */
      tirabr( Tok.cadeia );

      TS = strlen( Tok.cadeia );
      if ( !TS )
      {
         erro_par( original, loc_str( STR_COMP, COMP_520 ), linha );
         return 1;
      }

      if ( (PS = (SC *)malloc( TS + 1 )) != NULL )
      {
         for ( I = 0; I < MAX_MULTKEY && key_func[ I ].funcao != NULL; I++ )
            ;

         if ( I >= MAX_MULTKEY )
         {
            debug( loc_str( STR_COMP, COMP_523 ) );
            return( 1 );
         }

         strcpy( PS, Tok.cadeia );

         key_func[ I ].tecla  = Tcl->numero;
         key_func[ I ].funcao = PS;

         if ( Tcl->numero == NULA )   def_key_func = PS; /* Funcao Default */
      }

      break;

   case 10:         /* confirmacao de abandono/termino */
      if( I && (Tok.sep == '=') )
      {
         pega_token ( NULL, ";", &Tok );   /* pegar o numero da confirmacao */
         eliminabr( Tok.cadeia );
      }

      if( !I || !(*Tok.cadeia) )
      {
         erro_par( original, loc_str( STR_COMP, COMP_524 ), linha );
         return 1;
      }

      I = atoi( Tok.cadeia );
      if( I<0 || I>3 )
      {
         erro_par( original, loc_str( STR_COMP, COMP_524 ), linha );
         return 1;
      }
      flag_medit &=(4 | I);
      break;

   case 11:         /* NAO GERA BAK */
      flag_medit |= 4;
      break;

   case 12:					/* MOSTRAEXT */
      if( I && (Tok.sep == '=') )
      {
         pega_token ( NULL, ";", &Tok );
         eliminabr(Tok.cadeia);
      }

      mostra_oito = atoi( Tok.cadeia );
      
      if ( mostra_oito < 0 )    mostra_oito = 0;
      if ( mostra_oito > 3 )    mostra_oito = 3;
      
      if( !I )
      {
         erro_par( original, loc_str( STR_COMP, COMP_528 ), linha );
         return 1;
      }
      break;

   case 13:					/* CRPROXLINHA */
      cr_new_line = 0;
      break;
   }

   return 0;
}

SS carga_macs( mac_arq, teclas, nteclas, comandos )

SC  *mac_arq;
TECLAS *teclas;
SS nteclas;
SS comandos;
{
   SS linha, I, contamacros;
   SC *ptchar;
   SC arqmacro[ MAX_GET_RD + 5 ];
   SC buffer[ MAX_GET_RD + 5 ], copia_buf[ MAX_GET_RD + 5 ];
   FILE *ptfile;
   TECLAS   *Tcl1,
            *Tcl2;
   TOKEN     Tok;

   contamacros = 0;       /* Apontador para o vetor de macros */

   I = comandos;

   sprintf( arqmacro, "%.64s", mac_arq );

   if ((ptfile = fopen( arqmacro, MODO_RO )) == NULL )   goto fim;

   /*** Fique apenas com o diretorio ***/
   if ( (ptchar = strrchr( mac_arq, DIRSEP )) != NULL )   *ptchar = 0;

   for ( linha = 0;  fgets( buffer, MAX_GET_RD, ptfile ); ++linha )
   {
      strcpy(copia_buf, buffer);

      buffer[strlen(buffer)-1] = 0;   /* tiro o line-feed */

      eliminabr(buffer);

      if(!(*buffer))   /* se linha em branco */
         continue;

      if(*buffer == '#')    /* se linha de comentario ... */
         continue;

      pega_token( buffer, ":", &Tok );
      if( !Tok.achou )
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_511 ), linha );
         break;
      }

      if( strchr( Tok.cadeia, DIRSEP ) )    /* nome de arq. nao pode ter path */
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_512 ), linha );
         break;
      }

      /* Alocar memoria para colocar o nome do arquivo no vetor */

      mac_files[ contamacros ] = (SC *) malloc( strlen( Tok.cadeia ) +
                                                strlen( mac_arq ) + 3 );
      if ( mac_files[ contamacros ] == NULL )
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_513 ), linha );
         break;
      }

      sprintf( mac_files[contamacros], "%s%c%s", mac_arq, DIRSEP, Tok.cadeia );

      pega_token( NULL, ",", &Tok ); /* primeira tecla */
      eliminabr( Tok.cadeia );
      if( !Tok.achou ||!(*Tok.cadeia) ||
          !(Tcl1 = bsearch((SC *) strupr( Tok.cadeia ), (SC *) teclas,
                                  nteclas, sizeof(TECLAS), compara2 )))
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_508 ), linha );
         break;
      }

      vet_com[ I ].prim_tecla = Tcl1->numero;

      pega_token( NULL, ";", &Tok);
     eliminabr( Tok.cadeia );
      if( !( *Tok.cadeia ) )
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_509 ), linha );
         break;
      }

      if( *Tok.cadeia != '\'' )
      {
         strupr( Tok.cadeia );        /* Garante maiusculas */
         if( !(Tcl2 = bsearch((SC *) Tok.cadeia, (SC *) teclas,
                                     nteclas, sizeof(TECLAS), compara2 )))
         {
            erro_par( copia_buf, loc_str( STR_COMP,  COMP_509 ), linha );
	         break;
         }
         vet_com[ I ].seg_tecla = Tcl2->numero;
      }
      else
      {
         if( strlen(Tok.cadeia) != 3 )
         {
            erro_par( copia_buf, loc_str( STR_COMP,  COMP_510 ), linha );
   	      break;
         }
         vet_com[ I ].seg_tecla = ( SS ) Tok.cadeia[ 1 ];
      }

      vet_com[ I ].num_comando = ++contamacros;
      vet_com[ I ].escape      = 2;

      /*** E' bom que seja aqui, pois existe comando "continue" ***/
      if ( ++I > MAX_COM )
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_507 ), linha );
         break;
      }
   }

   /* inicializar mais um com zeros */

   vet_com[ I ].prim_tecla  = 0;
   vet_com[ I ].seg_tecla   = 0;
   vet_com[ I ].num_comando = 0;
   vet_com[ I ].escape      = 0;

/*****************************************
{
int z;

for(z = 0; vet_com[ z ].num_comando; ++z)
   fprintf(stderr,"%3d: %03d / %03d / %03d / %03d\n", z,
                  vet_com[ z ].num_comando,
                  vet_com[ z ].prim_tecla,
                  vet_com[ z ].seg_tecla,
                  vet_com[ z ].escape);
}
******************************************/
   fclose( ptfile );

fim:
   return I;
}

SS carga_coms( kbd_file, mac_arq, comandos, ncoms, teclas, nteclas )

SC  *kbd_file;
SC  *mac_arq;
COMANDOS *comandos;
SS ncoms;
TECLAS *teclas;
SS nteclas;
{
   SS linha;
   SI I;
   SC buffer[ MAX_GET_RD + 5 ], copia_buf[ MAX_GET_RD + 5 ];
   FILE *ptfile;
   COMANDOS *Cmd;
   TECLAS   *Tcl1,
            *Tcl2;
   TOKEN     Tok;
#ifdef   MEDIT
   SI        n_hots = 0;
#endif

   if ( kbd_file == NULL  ||
       (ptfile = fopen( kbd_file, MODO_RO )) == NULL )   goto  erro1;

   /* OK, arquivo de configuracao encontrado. Nao interessa onde */

   for ( I = 0;   I < COM_TOTAL;   I++ )
   {
      vet_com[ I ].prim_tecla  = 0;
      vet_com[ I ].seg_tecla   = 0;
      vet_com[ I ].num_comando = 0;
      vet_com[ I ].escape      = 0;
   }

   /* Comandos Nao configuraveis Maximo de 3, alterar em ethead.h */

   I = 0;

   vet_com[ I ].num_comando = RETORNO;
   vet_com[ I ].prim_tecla  = K_ENTER;
   I++;

   vet_com[ I ].num_comando = PROXTAB;
   vet_com[ I ].prim_tecla  = K_TAB;
   I++;

   vet_com[ I ].num_comando = TABANT;
   vet_com[ I ].prim_tecla  = K_BTAB;
   I++;

   for ( linha = 1;   fgets( buffer, MAX_GET_RD, ptfile );   linha++ )
   {
      buffer[strlen(buffer)-1] = 0;   /* tiro o line-feed */
      strcpy(copia_buf, buffer);

      tirabr(buffer);

      if(!(*buffer))   /* se linha em branco */
         continue;

      if(*buffer == '#')    /* se linha de comentario ... */
         continue;

      if(*buffer == '.' )   /* se linha de diretiva */
      {
         if( comp_diret( copia_buf, buffer, linha, teclas, nteclas ) )
            goto erro2;
         continue;
      }

      pega_token( buffer, ":", &Tok);   /* Identificar o comando */
      strupr( Tok.cadeia );                               /* Garante maiusculas */
      eliminabr( Tok.cadeia );

      if( !Tok.achou || !( *Tok.cadeia ) ||
          !(Cmd = bsearch((SC *) Tok.cadeia, (SC *) comandos,
                                 ncoms, sizeof(COMANDOS), compara1 )))
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_501 ), linha);
         goto  erro2;
      }

      vet_com[ I ].num_comando = Cmd->numero;
      vet_com[ I ].escape      = Cmd->escape;

#ifdef   MEDIT
      /* se comando for TECLAQUENTE, e' tratado em funcao propria */

      if ( vet_com[ I ].num_comando == TECLAQUENTE )
      {
		strcpy( buffer, copia_buf );
		if ( !comp_HK( I, copia_buf, buffer, linha, teclas, nteclas, comandos, ncoms ) )
			goto erro2;
		vet_com[ I ].num_comando = n_hots+512;
		++n_hots;
		goto next_com;
      }
#endif

      /* carregar o nome da primeira tecla */
      pega_token( NULL, ",", &Tok );
      eliminabr( Tok.cadeia );
      strupr( Tok.cadeia );           /* Garante maiusculas */

      if( !Tok.achou || !(*Tok.cadeia) ||
          !(Tcl1 = bsearch((SC *) Tok.cadeia, (SC *) teclas,
                                  nteclas, sizeof(TECLAS), compara2 )))
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_502 ), linha);
         goto  erro2;
      }

      vet_com[ I ].prim_tecla = Tcl1->numero;

      /* carregar o nome da segunda tecla */
      pega_token( NULL, ";", &Tok );
      eliminabr( Tok.cadeia );

      if( !(*Tok.cadeia) )
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_503 ), linha);
         goto  erro2;
      }

      if( *Tok.cadeia != '\'' )
      {
         strupr( Tok.cadeia );                            /* Garante maiusculas */
         if( !(Tcl2 = bsearch((SC *) Tok.cadeia, (SC *) teclas,
                                     nteclas, sizeof(TECLAS), compara2 )))
         {
            erro_par( copia_buf, loc_str( STR_COMP,  COMP_504 ), linha);
            goto  erro2;
         }
         vet_com[ I ].seg_tecla = Tcl2->numero;
      }
      else
      {
         if( !(strlen( Tok.cadeia ) == 3) )
         {
            erro_par( copia_buf, loc_str( STR_COMP,  COMP_505 ), linha);
            goto  erro2;
         }
         vet_com[ I ].seg_tecla = ( SS ) Tok.cadeia[ 1 ];
      }
      next_com:
      /*** E' bom que seja aqui, pois existe comando "continue" ***/
      if ( ++I > MAX_COM )
      {
         erro_par( copia_buf, loc_str( STR_COMP,  COMP_507 ), linha );
         goto  erro2;
      }
   }

   vet_com[ I ].num_comando = 0;
   vet_com[ I ].prim_tecla  = 0;
   vet_com[ I ].seg_tecla   = 0;
   vet_com[ I ].escape      = 0;

   fclose(ptfile);
   goto ordena;

erro2:
   fclose(ptfile);

erro1:

   I = setdefaults( );

ordena:

   if ( mac_arq != NULL )   I = carga_macs( mac_arq, teclas, nteclas, I );

   /* ordenar o vetor de comandos para facilitar a procura depois */

   qsort((SC *) vet_com, I, sizeof( COMSEQ ), comparaseq );

#ifdef NAODEF
			{
				SI J;
				FILE *ff;
				ff = fopen( "coms", "w" );

				fprintf( ff, "Tc1, Tc2, Com\n" );

				for(   J=0; J<I  ; ++J  )
				{
					fprintf(ff,"%03d, %03d, %03d\n",
						vet_com[J].prim_tecla,
						vet_com[J].seg_tecla,
						vet_com[J].num_comando );
				}
				fclose( ff );
			}
#endif

   return 0;
}

VOID erro_par( buf, mens, linha)

SC *buf,
   *mens;
SS linha;
{
   SC   msg[ 150 ];
   SC   lin[ 120 ];

   /* E necessario copiar, pois vamos chamar outra loc_str() > 500 ***/
   strcpy( msg, mens );
   strcpy( lin, loc_str( STR_COMP,  COMP_506 ));

   debug( "%s[%d] - \"%s\"\n%s", lin, linha, msg, buf );
}

SI comparaseq( pt1, pt2 )

COMSEQ *pt1,
       *pt2;
{
   US   I, J;

   I = (pt1->prim_tecla << 8) + pt1->seg_tecla;
   J = (pt2->prim_tecla << 8) + pt2->seg_tecla;

   if ( I < J )
      return -1;

   if( I > J )
      return 1;

   return 0;
}

SI compara1( ptr1, ptr2 )

SC *ptr1;
COMANDOS *ptr2;
{
   return strcmp(ptr1, ptr2->ident);
}

SI compara2( ptr1, ptr2 )
SC     *ptr1;
TECLAS *ptr2;
{
   return strcmp(ptr1, ptr2->ident);
}

SI pega_token( cadeia, separador, result )

SC *cadeia, *separador;
TOKEN *result;
{
   /*
      Pega tudo de cadeia até encontrar separador. Quando encontrar
      coloca 'zero' na posicao e retorna 1. Se nao encontrar retorna 0.
   */

   static SC *pt_inicio, *pt_start;

   if( cadeia )   pt_inicio = cadeia;
   else           pt_inicio = pt_start;

   result->cadeia = pt_inicio;
   result->achou  = 0;

   for( ; *pt_inicio; ++pt_inicio )
   {
      if( strchr( separador, *pt_inicio ) )
      {
         result->achou = 1;
         result->sep   = *strchr( separador, *pt_inicio );
         *pt_inicio = 0;
         pt_start   = ++pt_inicio;
         goto fim;
      }
   }
   pt_start = pt_inicio;

fim:
   return result->achou;
}

#ifdef   MEDIT

static SI comp_HK( Item, copia, buffer, linha, teclas, nteclas,
                   comandos, ncoms )
SI      Item;
SC     *copia;
SC     *buffer;
SS      linha;
TECLAS *teclas;
SS      nteclas;
COMANDOS *comandos;
SS ncoms;
{

      /* Compila Hot-Key */

      COMANDOS *Cmd;
      SC        token[120];
      SC        key_list[ MAX_KEYLIST ];
      SC       *pt;
      TECLAS   *Tcl;
      SI        cont_key;
      SI        I,J;

      /* carregar o nome da primeira tecla */

      pt = strchr( buffer, ':' )+1;

      I = get_k_or_ctrl( token, &pt );
      if ( I == 0 || I == '2' )
      {
         erro_par( copia, loc_str( STR_COMP,  COMP_501 ), linha );
         return 0;
      }

      if ( I == 1 )  /* Somente uma tecla */
      {
	vet_com[ Item ].prim_tecla = token[0];
      }
      else
      {
         if( !(Tcl = bsearch((SC *) token, (SC *) teclas,
                                     nteclas, sizeof(TECLAS), compara2 )))
         {
            erro_par( copia, loc_str( STR_COMP,  COMP_502 ), linha);
            return 0;
         }
         vet_com[ Item ].prim_tecla = Tcl->numero;
      }

      I = get_k_or_ctrl( token, &pt );
      if ( I == 0 || I == '2' )
      {
         erro_par( copia, loc_str( STR_COMP,  COMP_503 ), linha );
         return 0;
      }

      if ( strlen( token ) == 1 )  /* Somente uma tecla */
      {
	vet_com[ Item ].seg_tecla = token[0];
      }
      else
      {
         if( !(Tcl = bsearch((SC *) token, (SC *) teclas,
                                     nteclas, sizeof(TECLAS), compara2 )))
         {
            erro_par( copia, loc_str( STR_COMP,  COMP_504 ), linha);
            return 0;
         }
         vet_com[ Item ].seg_tecla = Tcl->numero;
      }

      /* Pegar as teclas para unget */

      cont_key = 0;
      while( I = get_k_or_ctrl( token, &pt ) )
      {
	switch( I )
        {
        case    1:      /* Tecla unica */
		key_list[ cont_key++ ] = token[0];
        	break;

        case    2:      /* Seq. de teclas entre aspas */
        	strcpy( &key_list[ cont_key ], token );
        	cont_key += strlen( token );
        	break;

        default:        /* Tecla ou comando direto. */
        	/* Ver primeiro as teclas de comandos nao configuraveis */

        	if ( !strcmp( token, loc_str( STR_COMP, COMP_192 ) ) ) /* Enter */
        	{
			key_list[ cont_key++ ] = K_ENTER;
			break;
        	}

        	if ( !strcmp( token, loc_str( STR_COMP, COMP_193 ) ) ) /* TAB */
        	{
			key_list[ cont_key++ ] = K_TAB;
			break;
        	}

        	if ( !strcmp( token, loc_str( STR_COMP, COMP_194 ) ) ) /* BackTAB */
        	{
			key_list[ cont_key++ ] = K_BTAB;
			break;
        	}

		if( (Cmd = bsearch((SC *) token, (SC *) comandos,
                                 ncoms, sizeof(COMANDOS), compara1 )) != NULL )
		{
			for(   J=0;   vet_com[ J ].num_comando;  ++J )
                        {
                        	if ( vet_com[ J ].num_comando == Cmd->numero )
                        	{
					key_list[ cont_key++ ] = vet_com[ J ].prim_tecla;
					if( vet_com[ J ].seg_tecla )
						key_list[ cont_key++ ] = vet_com[ J ].seg_tecla;
					break;
                        	}
                        }
                        if ( vet_com[ J ].num_comando ) break;
                        /* Comando nao definido */
			erro_par( copia, loc_str( STR_COMP, COMP_527 ), linha);
			return 0;
		}

		if( (Tcl = bsearch((SC *) token, (SC *) teclas,
			nteclas, sizeof(TECLAS), compara2 )))
		{
			key_list[ cont_key++ ] = Tcl->numero;
			break;
		}
		erro_par( copia, loc_str( STR_COMP, COMP_526 ), linha);
		return 0;
        }
      }

      if ( cont_key == 0 )
      {
            erro_par( copia, loc_str( STR_COMP,  COMP_525 ), linha);
            return 0;
      }

      Add_HK( key_list, cont_key );

      return 1;

}

static VOID Add_HK( key_list, cont_key )
SC *key_list;
SI  cont_key;
{
	static SI n_lists = 0, max_lists = 0;

	if ( n_lists == max_lists )
	{
		max_lists += 8;
		if( max_lists == 8 ) Hot_Keys = ( SC ** ) malloc (           max_lists*sizeof( SC * ) );
		else                 Hot_Keys = ( SC ** ) realloc( Hot_Keys, max_lists*sizeof( SC * ) );
	}

	Hot_Keys[ n_lists ] = (SC *) malloc( (cont_key+1)*sizeof( SC ) );

	memcpy( Hot_Keys[ n_lists ], key_list, cont_key*sizeof( SC ) );
        Hot_Keys[ n_lists ][ cont_key ] = 0;
	++n_lists;

	return;
}

static SI get_k_or_ctrl( token, start )
SC *token;
SC **start;
{

      /* Busca, a partir de start, em buffer, uma tecla ou um control ... */


      SC *pt;
      SI I;
      SI ret = 0;

      pt = *start;

      while( *pt == ' ' || *pt == ',' ) ++pt;
      if( *pt == 0 || *pt == ';' ) goto bad_end;

      if ( *pt == '\'' )    /* Para teclas tipo '?' */
      {
        ++pt;
        if( *pt ) *token = *(pt++);
        else      goto bad_end;

	if( (*pt) != '\'' ) goto bad_end;
	token[1] = 0;
	ret = 1;
	goto good_end;
      }

      I=0;
      if ( *pt == '"' )     /* Para conjuntos tipo "...." */
      {
	++pt;
	while( *pt != '"' )
	{
		if ( *pt == '\\' ) ++pt;
		if ( *pt ) token[I++] = *pt;
		else       goto bad_end;
		++pt;
	}
	token[I] = 0;
	ret = 2;
	goto good_end;
      }

      while( *pt != ' ' && *pt != ',' )
      {
	if( *pt ) token[I++] = *pt;
	else      break;
	++pt;
      }
      token[I] = 0;
      strupr( token );           /* Garante maiusculas */
      ret = 3;

good_end:
      *start = ++pt;
      return ret;

bad_end:
      *token = 0;
      return 0;
}

#endif


