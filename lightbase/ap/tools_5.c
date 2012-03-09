#include  "lb2all.h"

#include <string.h>
#include  <time.h>
#include  <ctype.h>

#include  "lb2stru.h"
#include  "lb2files.h"
#include  "lb2bases.h"

#include "tools.h"

#ifdef MSDOS
#include <io.h>
#endif

#ifdef MSDOS
   COUNT ve_acesso(LONG, UCOUNT *);
   VOID out_usuario(VOID);
   VOID out_grupo(VOID);
   VOID out_base(VOID);
   VOID out_acesso(LONG);
   TEXT *nome_base(TEXT *, LONG);
   VOID concat( TEXT *, TEXT *);
	int crypt_key ( TEXT *, TEXT * );
#else
   COUNT ve_acesso( );
   VOID out_usuario( );
   VOID out_grupo( );
   VOID out_base( );
   VOID out_acesso( );
   TEXT *nome_base( );
   VOID concat( );
	int crypt_key ( );
#endif

static struct vrfmt {	/* beginning of var len records */
	UCOUNT	recmrk;	/* record marker		*/
	UCOUNT	trclen;	   /* total record length		*/
	UCOUNT	urclen;	   /* utilized record length	*/
} VHDR;

LONG chaves[10];
COUNT ind_chaves=0;

static FILE *fp, *fp_log;
static FILE *fp_out;
static vai_indice; /* indica se o arquivo de indices pode ser usado */

static Usuario usuario;
static Grupo grupo;
static Base base;
static Acesso acesso;



typedef struct wfils {	/* File Control Structure 		*/
	COUNT	   verson;	/* ATT. 8 BYTES   configuartion options at create	*/
	UCOUNT	extsiz;	/*                extend file (chunk) size		*/
	UCOUNT	reclen;	/*                data record length			*/
	UCOUNT	recsiz;	/*                node record size			*/

	COUNT	   flmode;	/* file mode (virtual, etc)		*/
	COUNT	   clstyp;	/* flag for type of virtual close	*/
	TEXT	   updflg;	/* update (corrupt) flag		*/
	TEXT	   ktype;	/* file type flag			*/

	POINTER	sernum;	/* serial number			*/
	POINTER	delstk;	/* top of delete stack: fixed len data	*/
	POINTER	numrec;	/* last byte offset written		*/
	POINTER	phyrec;	/* last byte offset of file		*/
	LONG     root;	   /* B-Tree root 				*/
	LONG   	nodstk;	/* node delete stack (unused)		*/

   LONG     numserie; /* numero de serie do software */
   UCOUNT   baseid;   /* Identificacao da base */
   COUNT    pega1;    /* xor de numserie e baseid */

	COUNT	   nmem;	   /* number of members			*/
	COUNT	   kmem;	   /* member number			*/
	UCOUNT   finger;	/* Finger do arquivo - Ananias */
	UCOUNT	maxkbl;	/* maximum key bytes leaf-var		*/
	UCOUNT	maxkbn;	/* maximum key bytes non leaf-var 	*/
	COUNT    maxkvl;	/* max key values per leaf node-fixed	*/
	COUNT    maxkvn;	/* max key values per non leaf node-fxd	*/
	COUNT    length;	/* key length 				*/

	UTEXT	   release[2];	/* Uso como COUNT para nao perder compatibilidade */

	/* end of permanent information */

	TEXT	   flname[MAX_NAME];	/* file name		*/
	POINTER	sekpos;	      /* current abs file position		*/
	LONG   	retnod;	      /* current node				*/

	COUNT	   filnum;	/* file number assigned by user		*/
	UCOUNT	usecnt;	/* lru age				*/
	UCOUNT   lokcnt;  /* keep track of pending locks		*/
	COUNT	   retelm;	/* current node element			*/
	TEXT	   chnacs;	/* 'y' 'n' 'v'  ||||||  'w' 'm' 'c'  */
	RNDFILE	fd;	/* file descritor			*/
} CTFILE;




COUNT trata_acessos()
{  COUNT ret=0, tipo, conta=0;
   TEXT aux[80], log[50];
  	TEXT ser_str[20], data_val [ 20 ], num_ttys [ 20 ], serie_aux [ 20 ];
   UCOUNT tam;
   LONG proximo, serie;
	CTFILE stru;
#ifndef PROT_INFOCON
   LONG pos=0;
#endif

   vai_indice = 0;

   strcpy(aux, "acessos");
   strcat(aux, ".dat");

   if ( access(aux, 0) != 0 ) {
      fprintf(stderr, "Arquivo de acessos nao existe.\n");
      ret = 1;
      goto fim;
   }


#ifdef MSDOS
   fp = fopen( aux, "rb");
#else
   fp = fopen( aux, "r");
#endif

   strcpy(log, "acessos.log");

   if ( (fp_log = fopen(log, "w")) == NULL )
      fp_log = stderr;

   /* abre e fecha so para evitar erro no futuro */
   if ( (fp_out = fopen("acessos.@@@", "w")) == NULL ) {
      fprintf(stderr, "Erro na criacao do arquivo acessos.@@@\n");
      ret = -1;
      goto fim;
   }
   else {
      fclose(fp_out);
      param.lista_pos = 1;
      if ( lista_ind("acessos.idx", "acessos.@@@") == 0 )
         vai_indice = 1;
   }


   if ( (fp_out = fopen("acessos.txt", "w")) == NULL )
      fp_out = stderr;


#ifndef PROT_INFOCON


#ifdef MSDOS
   pos = 38;
#endif

#ifdef SCOUNIX
   pos = 40;
#endif

#ifdef U6000
   pos = 40;
#endif

#ifdef ULTRIX
   pos = 40;
#endif

#ifdef AIX
   pos = 40;
#endif

#ifdef PA800R
   pos = 40;
#endif

   if ( pos <= 0 ) {
      fprintf(stderr, "Posicao do numero de serie nao definido\n");
      ret = 3;
      goto fim;
   }

	  	fseek( fp, pos, 0 );
	   if ( fread( &serie, sizeof(LONG), 1, fp) != 1 ) {
         fprintf(stderr, "Erro lendo acessos.dat\n");
         ret = 3;
         goto fim;
      }
      fprintf(stderr, "Numero de serie : %ld\n", serie);
#else
		{
			UTEXT *p;
		  	fseek( fp, 0, 0 );
		   if ( fread( &stru, sizeof(stru), 1, fp) != 1 ) {
	         fprintf(stderr, "Erro lendo acessos.dat\n");
   	      ret = 3;
	         goto fim;
	      }

	      if ( stru.sernum == 0 ) {
		      fprintf(stderr, "Numero de serie : %ld\n", stru.numserie);
				serie = stru.numserie;
	      	sprintf(ser_str, "%6.6ld", serie);
	      }
	      else {
		      p = (UTEXT *)&stru.sernum;
		      serie = stru.numserie;
	      	sprintf(ser_str,"lb%c%c%c%c%05.5ld", *p, *(p+1), *(p+2), *(p+3), stru.numserie - 100000L);
		      fprintf(stderr, "Numero de serie : %s\n", ser_str);
	      }
	   }
#endif


      /* numeros de serie < que 200 sao consideradas bases publicas */
      if ( serie > 200 ) {
         TEXT senha[50];
         fprintf(stderr, "Chave de ativacao para Numero de serie %s ? ", ser_str);
         gets(senha);

         data_val [ 0 ] = 0;

	      if ( stru.sernum != 0 )
	      {
	         strcpy ( serie_aux, ser_str );
		      crypt_key(serie_aux, serie_aux + 11 );

      		if ( stricmp(serie_aux, senha) == 0 )
      		{
		         chaves[ind_chaves++] = serie;
		         goto continua;
      		}

				fprintf ( stderr, "Data de Validade AAMMDD ? " );
				gets ( data_val );

				if ( strlen ( data_val ) == 0  )
				{
					fprintf ( stderr, "Numero de terminais ? ", num_ttys );
					gets ( num_ttys );
				}

				if ( data_val [ 0 ] != 0 )
				{
		      	sprintf ( serie_aux, "%6s%s", data_val, ser_str+6);
				}
				else
				{
					if ( atoi ( num_ttys ) != 999 )
					{
		         	TEXT aux[10];
			        	sprintf(aux, "%03d", atoi ( num_ttys ));
			      	strcpy ( serie_aux, ser_str );
         			memcpy ( serie_aux+2, aux, 3);
					}
					else
			      	strcpy ( serie_aux, ser_str);

				}

		      strlwr(serie_aux);

		      crypt_key(serie_aux, serie_aux + 11 );

      		if ( stricmp(serie_aux, senha) != 0 )
      		{
   	         fprintf(stderr, "Chave de ativacao nao confere. Operacao cancelada\n");
      	      ret = 2;
         	   goto fim;
      		}

	      }
         else /* Esquema anterior */
			{

	         if ( permissao( ser_str, senha ) == 0 ) {
   	         fprintf(stderr, "Chave de ativacao nao confere. Operacao cancelada\n");
      	      ret = 2;
         	   goto fim;
	         }
	      }
         chaves[ind_chaves++] = serie;
      }

continua:

   proximo = pega_proximo(0, fp);

   fprintf(stderr, "\nTratando acessos");

   while (proximo > 0 ) {

      if ( (tipo = ve_acesso(proximo, &tam)) > 0 ) {
         fprintf(stderr, ".", tipo);
      }
      conta++;
      proximo = pega_proximo(proximo+1, fp);
   }

   fprintf(stderr, "\n");

fim:

   if ( fp != NULL )
      fclose(fp);

   if ( fp_out != NULL && fp_out != stderr ) {
      fclose(fp_out);
   }

   if ( fp_log != NULL && fp_log != stderr ) {
      fseek(fp_log, 0, 2);
      if ( ftell(fp_log) != 0 )
         fprintf(CON, "Atencao. Verifique arquivo de log %s.\n", log);
      fclose(fp_log);
   }

   fp = NULL;

   if ( ret == 0 ) {
      if ( conta > 0 && getenv("NOSORT") == NULL) {
         fprintf(CON, "\nOrdenando arquivo de acessos ...");
         system("sort < acessos.txt > acessos.@@@");
#ifdef MSDOS
         system("del acessos.txt");
         system("ren acessos.@@@ acessos.txt");
#else
         system("rm acessos.txt");
         system("mv acessos.@@@ acessos.txt");
#endif
      }
      fprintf(CON, "\nArquivo acessos.txt gerado.\n");
      fprintf(CON, "OK.\n");

   }

   return(ret);
}


COUNT ve_acesso(pos, tam)
LONG pos;
UCOUNT *tam;
{  UCOUNT lido;
   COUNT tipo;
   UTEXT buf[500];

 	fseek( fp, pos, 0 );

   lido = fread( &VHDR, sizeof(struct vrfmt), 1, fp);

	if ( lido != 1 ) {
      fprintf(fp_log, "Erro na estrutura de armazenamento (reg_var). Esperado : %d Lido %d\n", sizeof(struct vrfmt) ,lido);
      goto fim;
      /* cv_exit(4); */
   }

   *tam = VHDR.urclen;

   switch (*tam) {
      case sizeof(Usuario) :
           tipo = 'U';
           lido = fread( &usuario, 1, sizeof(Usuario), fp);
           out_usuario();
           break;
      case sizeof(Grupo)   :
           tipo = 'G';
           lido = fread( &grupo, 1, sizeof(Grupo), fp);
           out_grupo();
           break;
      case sizeof(Base)   :
           tipo = 'B';
           lido = fread( &base, 1, sizeof(Base), fp);
           out_base();
           break;
      case sizeof(Acesso)   :
           tipo = 'A';
           lido = fread( &acesso, 1, sizeof(Acesso), fp);
           out_acesso(pos + sizeof(struct vrfmt));
           break;
      default              :
           tipo = '?';
           if ( *tam < 500 )
              lido = fread( buf, 1, *tam, fp);
           fprintf(fp_log, "Tamanho de informacao desconhecido : %d\n", *tam);
           break;
   }

fim :

   return(tipo);
}

VOID out_usuario()
{  TEXT buf[500], aux[20];

   *buf = '\0';
   concat(buf, usuario.us_ident_usu );
   concat(buf, usuario.us_full_name );
   concat(buf, usuario.us_grupo );
   concat(buf, usuario.us_senha );
   concat(buf, usuario.us_loc_trab);
   concat(buf, usuario.us_fone );
   concat(buf, usuario.us_idioma );
   concat(buf, usuario.us_menu );
   concat(buf, usuario.us_status );
   sprintf(aux, "%d", usuario.us_tmsenha); concat(buf, aux );

   fprintf(fp_out, "T1 - Usuario : [%s]\n", buf);
}

VOID out_grupo()
{  TEXT buf[500];

   *buf = '\0';
   concat(buf, grupo.gr_ident_grp );
   concat(buf, grupo.gr_full_name );
   concat(buf, grupo.gr_loc_trab);
   concat(buf, grupo.gr_fone );

   fprintf(fp_out, "T2 - Grupo   : [%s]\n", buf);
}

VOID out_base()
{  TEXT buf[500], aux[20];

   *buf = '\0';
   concat(buf, base.bs_nome);
   concat(buf, base.bs_path);
   concat(buf, base.bs_desc);
   concat(buf, base.bs_dono);
   concat(buf, base.bs_grupo);
   sprintf(aux, "%d", base.bs_idbase); concat(buf, aux );
   sprintf(aux, "%ld", base.bs_nserie); concat(buf, aux );
   sprintf(aux, "%ld", base.bs_xor); concat(buf, aux );

   fprintf(fp_out, "T3 - Base    : [%s]\n", buf);
}

VOID out_acesso(pos)
LONG pos;
{  TEXT buf[500], *p;
   TEXT str_vis[150], str_rel[150], aux[20];
   COUNT i;

   *buf = '\0';
   str_vis[0] = str_rel[0] = '\0';

   concat(buf, acesso.da_ugnome);

   if ( (p = nome_base(acesso.da_ugnome, pos)) != NULL )
      concat(buf, p);
   else
      concat(buf, "");


   if ( acesso.da_inclui   == 'X')   concat(buf, "in");
   if ( acesso.da_altera   == 'X')   concat(buf, "al");
   if ( acesso.da_exclui   == 'X')   concat(buf, "ex");
   if ( acesso.da_pesquisa == 'X')   concat(buf, "pe");
   if ( acesso.da_emite    == 'X')   concat(buf, "em");
   if ( acesso.da_exporta  == 'X')   concat(buf, "ep");
   if ( acesso.da_importa  == 'X')   concat(buf, "im");
   if ( acesso.da_parcial  == 'X')   concat(buf, "pa");
   if ( acesso.da_total    == 'X')   concat(buf, "to");
   if ( acesso.da_backup   == 'X')   concat(buf, "ba");
   if ( acesso.da_restore  == 'X')   concat(buf, "re");
   if ( acesso.da_tabela   == 'X')   concat(buf, "ta");

   for ( i = 0; i < MAX_VISTAS; i++) {
      if ( acesso.da_vistas[i] != 0 ){
         sprintf(aux, "%d,", acesso.da_vistas[i]);
         strcat(str_vis, aux);
      }

      if ( acesso.da_rel[i] != 0 ) {
         sprintf(aux, "%d,", acesso.da_rel[i]);
         strcat(str_rel, aux);
      }
   }

   if ( strlen(str_vis) > 0 ) {
      p = strchr(str_vis, '\0');
      *--p = '\0';
   }
   if ( strlen(str_rel) > 0 ) {
      p = strchr(str_rel, '\0');
      *--p = '\0';
   }

   concat(buf, str_vis);
   concat(buf, str_rel);

   fprintf(fp_out, "T4 - Acesso  : [%s]\n", buf);
}

TEXT *nome_base(nome,pos)
TEXT *nome;
LONG pos;
{  FILE *fa;
   UCOUNT i, vez=0;
   TEXT procura[30], *p, *arq;

   static TEXT *buf=NULL;
   static UCOUNT ja_tentou=0, tam=0;

   if ( ja_tentou == 0 ) {
      LONG l;

      if ( vai_indice == 1 )
         arq = "acessos.@@@";
      else
         arq = "acessos.idx";

#ifdef MSDOS
      fa = fopen( arq, "rb");
#else
      fa = fopen( arq, "r");
#endif

      if ( fa != NULL ) {
    	   fseek( fa, 0, 2 );
         l = ftell(fa);

         if ( l > 0 && l < 65000L ) {
            buf = malloc((UCOUNT)l);
            if ( buf != NULL ) {
          	   fseek( fa, 0, 0);
               tam = fread( buf, 1, (UCOUNT)l, fa);
            }
         }
         fclose(fa);
      }
      for ( i = 0; i < tam; i++ ) {
         if ( buf[i] == '\r' || buf[i] == '\n' || buf[i] == ' ') {
            buf[i] = '\0';
         }
      }

   }

   /* devido a um bug na versao anterior a 1.8r4.0d pode nao existir "2" */

volta :

   if ( vez == 0 )
      strcpy(procura, "2");
   else
      procura[0] = '\0';

   strcat(procura, nome);

   p = procura;


   for ( i = 0; i < tam; i++ ) {
      if ( buf[i] == *p ) {
         if ( *(p+1) == '\0' ) {
            if ( buf[i+1] == '!' ) {
               LONG la;
               UTEXT *p2;
               TEXT *pn;
               pn = (TEXT *) &buf[i+2];
               p2 = (UTEXT *)strchr(pn, '\0') + 1;

               if ( vai_indice == 0 ) {
                  pega_long((UTEXT *)&la, p2);
               }
               else {
                  la = atol((TEXT *)p2);
               }


               if ( la == pos )
                  return(pn);
               else
                  p = procura;
            }
            else
               p = procura;
         }
         p++;
      }
      else
         p = procura;
   }

   if ( vez++ == 0 )
      goto volta;

   return(NULL);
}

VOID concat(buf, str)
TEXT *buf, *str;
{  TEXT *p;

   p = strchr(buf, '\0');
   if ( p != buf )
      *p++ = ',';
   *p++ = '"';
   while ( *str ) {
      if ( *str == '"' )
         *p++='\\';
      *p++ = *str++;
   }
   *p++ = '"';
   *p++ = '\0';
}


