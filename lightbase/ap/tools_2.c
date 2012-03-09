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
#include  <alloc.h>
#include  <dos.h>
#endif

#ifdef MSDOS
   LONG ve_registro(LONG, UCOUNT *);
   COUNT acha_textos(FILE *);
   COUNT exporta_reg(LONG, LONG, UCOUNT);
   COUNT sai_texto(LONG, LONG, TEXT *, COUNT);
   VOID load_campo(TEXT *);
   COUNT monta_cripto(TEXT *);
   VOID de_cripta(UTEXT *, COUNT );
#else
   LONG ve_registro( );
   COUNT acha_textos( );
   COUNT exporta_reg( );
   COUNT sai_texto( );
   VOID load_campo( );
   COUNT monta_cripto( );
   VOID de_cripta( );
#endif

#define     MARCA_REGISTRO 254
#define     MARCA_TEXTO    255

#define   OFF_INFO 6

#define     HDR_TEXTO      6   /* MARCA + NUMERO REGISTRO + NUM_DO_CAMPO */
#define     HDR_REGISTRO   6   /* MARCA + NUMERO REGISTRO + NUM_DE_CAMPOS */

static struct vrfmt {	/* beginning of var len records */
	UCOUNT	recmrk;	/* record marker		*/
	UCOUNT	trclen;	   /* total record length		*/
	UCOUNT	urclen;	   /* utilized record length	*/
} VHDR;

static LONG reg_anterior;
static FILE *fp, *fp_log;
static TEXT nome_cp[121][16];
static TEXT tipo_cp[121];
static UTEXT       ap_cripto[11];
static TEXT *nomearq="tools.dat";
static TEXT *nomeind="tools.idx";

static COUNT m_datno, m_keyno;


COUNT trata_lb2()
{  COUNT ret=0;
   TEXT aux[80], log[50], base[50];
   UCOUNT tam;
   LONG proximo, reg=0, total, xxx=4143;

   m_datno = FILE_DATNO;
   m_keyno = FILE_KEYNO;

	if ( param.recup  )
      abre_arq(nomearq, nomeind, 1, m_datno, m_keyno);

   reg_anterior = 0;
   fp = fp_log = NULL;

   fprintf(CON, "\nNome da base : ");
   gets(aux);
   strcpy(log, aux);
   strcpy(base, aux);

   if ( aux[0] == '\0' || aux[0] == '.' )
      return(0);

   strcat(aux, ".lb2");

   if ( access(aux, 0) != 0 ) {
      fprintf(stderr, "Arquivo de dados da base nao existe.\n");
      ret = 1;
      goto fim;
   }

#ifdef MSDOS
   fp = fopen( aux, "rb+");
#else
   fp = fopen( aux, "r+");
#endif

   if ( fp == NULL ) {
      fprintf(stderr, "Arquivo de controle da base nao pode ser aberto.\n");
      ret = 1;
      goto fim;
   }

   if ( param.cripto ) {
      if ( monta_cripto(aux) != 0 ) {
         ret = -1;
         goto fim;
      }
   }


   strcat(log, ".log");

   if ( (fp_log = fopen(log, "w")) == NULL )
      fp_log = stderr;

   load_campo(base);


   /* para varrer o arquivo e achar os campos textos */
   if ( param.recup == 1 && acha_textos(fp) < 0 ) {
      fprintf(stderr, "\nProblema na leitura de textos\n");
   }

   proximo = pega_proximo(param.posicao, fp);

   total = 0;

   while (proximo > 0 ) {

      if ( (reg = ve_registro(proximo, &tam)) > 0 ) {
         if (reg > 1000000L) {
            fprintf(stderr, "\rdescartando reg : %8.8ld", reg);
            goto pproximo;
         }
         if ( param.start != 0 ) {
            if ( reg == param.start )
               param.start = 0;
            else
               fprintf(stderr, "\rdescartando reg : %8.8ld", reg);
            goto pproximo;
         }

         exporta_reg(proximo, reg, tam);
#ifdef MSDOS
         fprintf(stderr, "\rUltimo reg : %8.8ld   Total Acumulado : %ld   prox = %ld mem = %ld ", reg, ++total, proximo, farcoreleft( ));
#else
         fprintf(stderr, "\rUltimo reg : %8.8ld   Total Acumulado : %ld   prox = %ld", reg, ++total, proximo);
#endif
         if ( reg == xxx)
            xxx = xxx;
#ifdef MSDOS
         if ( kbhit() &&  getch() == '.' )
            break;
#endif
         reg_anterior = reg;
      }

pproximo :

      proximo = pega_proximo(proximo+1, fp);
   }


fim:

   if ( fp != NULL )
      fclose(fp);

   if ( fp_log != NULL && fp_log != stderr ) {
      fseek(fp_log, 0, 2);
      if ( ftell(fp_log) != 0 )
         fprintf(CON, "Atencao. Verifique arquivo de log %s.\n", log);
      fclose(fp_log);
   }

   fp = NULL;

	if ( param.recup )
      fecha_arq(m_datno, m_keyno);

   return(ret);
}


COUNT acha_textos(fp)
FILE *fp;
{  COUNT campo, vetcp[150], i, parte;
   UCOUNT t;
   LONG reg=0;
   UTEXT buf[100], *p;
   LONG proximo, pos, achou=0L, xxx=0;
   TEXT chave[30];

   for (i=0; i<150; i++)
      vetcp[i] = -1;

   proximo = pega_proximo(0, fp);

   while (proximo > 0 ) {

      pos = proximo;

 	   fseek( fp, pos, 0 );

	   if ( fread( &VHDR, sizeof(struct vrfmt), 1, fp) != 1 ) {
         fprintf(fp_log, "Erro lendo hdr_var em lb2.\n");
         goto vai;
      }
#ifdef XXS
      if ( VHDR.recmrk == 0XFDFD ) {
         fprintf(fp_log, "Atencao registro marcado como DELETADO em pos = %ld\n", pos);
      }
      else if ( VHDR.recmrk == 0XFAFA ) {
         /* tudo ok */
      }
      else {
         fprintf(fp_log, "Atencao marca de registro desconhecida em pos = %ld\n", pos);
         goto vai;
      }
#endif

      if ( VHDR.urclen < 5 ) {
         fprintf(fp_log, "Atencao registro com menos de 5 bytes em pos = %ld\n", pos);
         goto vai;
      }

      t = VHDR.urclen;

      if ( t > 40000L || t == 0) {
         fprintf(fp_log, "Registro descartado. Apresenta tamanho incompativel(%u). pos = %ld \n",t,  pos);
         goto vai;
      }

      if ( t > 90 )
         t = 90;

	   if ( fread( buf, 1, t, fp) != t ) {
         fprintf(fp_log, "Erro lendo lb2. Nao conseguiu ler 5 bytes de controle\n");
         goto vai;
      }
/*
      if ( param.cripto )
         de_cripta((UTEXT *)buf, t);
*/
      if ( buf[0] == MARCA_REGISTRO )
         goto vai;

      /* pega numero do registro */
/*
      if ( param.estrutura_alinhada )  p = (UTEXT *)&buf[4];
      else                             p = (UTEXT *)&buf[1];
*/
      p = (UTEXT *)&buf[off_reg];


      if ( param.forma_long & 0x02 ) reg = GET_LONG(p);
      else {
            UTEXT buf_aux[10];
            memcpy(buf_aux, p, sizeof(LONG));
            reg = *(LONG *) buf_aux;
      }

/*
      if ( param.estrutura_alinhada )  campo = buf[8];
      else                             campo = buf[5];

      if ( param.estrutura_alinhada )  parte = buf[9];
      else                             parte = buf[6];
*/
      campo = buf[off_campo];
      if (off_parte != 0 ) parte = buf[off_parte];
      else                 parte = 1;

      if ( parte != 1 )
         goto vai;
/*
      if ( vetcp[campo] == -1) {
         TEXT aux[30];
         fprintf(stderr, "Campo %d. Novo numero ? ", campo);
         gets(aux);
         if ( aux[0] != '\0' ) {
            vetcp[campo] = atoi(aux);
         }
      }
*/
      vetcp[campo] = campo;

      pos += sizeof(struct vrfmt);

      sprintf(chave, "%9.9ld,%3.3d,%ld", reg, vetcp[campo], pos);


      ADDKEY(m_keyno, chave, 0L, REGADD, strlen(chave)+1);

      fprintf(stderr, "\r%ld   %ld", ++achou, proximo);

      if ( achou == xxx )
         achou = achou;
vai:
      proximo = pega_proximo(proximo+1, fp);
   }


      fecha_arq(m_datno, m_keyno);
   fprintf(CON, "\nDeseja parar (caso sim responda S e depois N : ");
   gets((TEXT *)buf);

   if (buf[0] == 'S' || buf[0] == 's' )
      exit(1);

      abre_arq(nomearq, nomeind, 1, m_datno, m_keyno);

   return(0);
}


LONG ve_registro(pos, tam)
LONG pos;
UCOUNT *tam;
{  UCOUNT lido;
   LONG reg=0;
   UTEXT buf[10];


 	fseek( fp, pos, 0 );

   lido = fread( &VHDR, sizeof(struct vrfmt), 1, fp);

      if ( VHDR.recmrk == 0XFDFD ) {
         fprintf(fp_log, "Atencao registro marcado como DELETADO em pos = %ld\n", pos);
      }
      else if ( VHDR.recmrk == 0XFAFA ) {
         /* tudo ok */
      }
      else {
         fprintf(fp_log, "Atencao marca de registro desconhecida em pos = %ld\n", pos);
         reg = -1;
         goto fim;
      }

	if ( lido != 1 ) {
      fprintf(fp_log, "Erro lendo hdr_var em lb2. Esperado : %d Lido %d\n  (reg_anterior : %ld)\n", sizeof(struct vrfmt) ,lido, reg_anterior);
      reg = -1;
      goto fim;
   }

   *tam = VHDR.urclen;



   if ( *tam >= 5 ) {
      UCOUNT lido;

      lido = fread( buf, 1, 2 + sizeof(LONG), fp);
      if ( param.cripto )
         de_cripta((UTEXT *)buf, 2 + sizeof(LONG));

	   if ( lido != 2 + sizeof(LONG) ) {
         fprintf(fp_log, "Erro lendo lb2. Nao conseguiu ler 5 bytes de controle (reg_anterior = %ld\n", reg_anterior);
         reg = -1;
         goto fim;
         /* cv_exit(4); */
      }

      if ( buf[0] == MARCA_REGISTRO ) {
         memcpy((TEXT *)&reg, &buf[1], sizeof(LONG));
         if ( param.forma_long & 0x01 ) {
            LONG l1;
            l1 = GET_LONG((UTEXT *)&reg);
            reg = l1;
         }

         else {
            UTEXT buf_aux[10];
            memcpy(buf_aux, &buf[1], sizeof(POINTER));
            reg = *(POINTER *) buf_aux;
         }
      }
   }
   else {
      fprintf(fp_log, "Atencao registro com menos de 5 bytes em pos = %ld\n", pos);
      reg = -1;
      goto fim;
   }

fim :

   return(reg);
}


COUNT exporta_reg(pos, reg, tamanho)
LONG pos, reg;
UCOUNT tamanho;
{  COUNT ret=0;
   COUNT efetivos, tamlido, campolido;
   TEXT *pr, *buf_reg;
   POINTER pos_texto;

   /*fprintf(stderr, "Exportando registro %ld de pos = %ld\n", reg, pos); */

   buf_reg = malloc(tamanho + 1);

   if ( buf_reg == NULL ) {
      fprintf(fp_log, "Erro de alocacao de memoria\n", reg, pos);
      exit(15);
   }

 	fseek( fp, pos + OFF_INFO, 0 );

   if ( fread( buf_reg, 1, tamanho, fp) != tamanho ) {
      fprintf(fp_log, "Erro lendo registro %ld de pos = %ld\n", reg, pos);
      ret = -1;
      goto fim;
   }

   if ( param.cripto )
      de_cripta((UTEXT *)buf_reg, tamanho);


   pr = buf_reg;

   efetivos = *(pr + HDR_REGISTRO - 1 );

   pr      += HDR_REGISTRO;

   while ( efetivos-- ){

      campolido = *pr++;
      tamlido   = *pr++;

      if ( tipo_cp[campolido] == 0 ) {
         if ( tamlido != 4 )
            tipo_cp[campolido] = 'N';
         else {
            if ( strlen(pr) < 4 )
               tipo_cp[campolido] = 'T';
            else {
               COUNT i;
               for ( i = 0; i < 4; i++) {
                  if ( ! isprint(pr[i]) )
                     tipo_cp[campolido] = 'T';
               }

            }
         }

      }


      if ( tipo_cp[campolido] == 'T' ) {
         COUNT rt;

         if ( param.forma_long & 0x01 )
               pos_texto = get_postexto('O', (UTEXT *)pr);
            else {
               pos_texto = get_postexto('L', (UTEXT *)pr);
/* estava assim antes de get_postexto....
               UTEXT buf_aux[10];
               memcpy(buf_aux, pr, sizeof(POINTER));
               pos_texto = *(POINTER *) buf_aux;
               if ( sizeof(POINTER) > 4 )
                  debug("WWW Atencao tratamento de POINTER com tamanho maior que 4 (pos_texto = %ld)", pos_texto);
*/
            }

            rt = -1;
            if ( pos_texto != (POINTER) 0 )
               rt = sai_texto(pos_texto, reg, nome_cp[campolido], 1);

            if ( pos_texto != 0 && rt == -1 && param.recup ) {
               COUNT ret;
               TEXT chave[30], linha[40];
               LONG p;

               sprintf(chave, "%9.9ld,%3.3d,", reg, campolido);

 					ret = GTEKEY(m_keyno, chave, linha, strlen(chave));

					if ( ret > 0 && memcmp(chave, linha, strlen(chave)) == 0 ) {
                  TEXT aux[30];
                  if ( piece(linha, aux, ',', 3, 15) > 0 )
                     p = atol(aux);
                  else
                     p = 0;

                  if ( p > 0 ) {
                     sai_texto(p, reg, nome_cp[campolido], 0);
                  }
               }
            }
      }
      else {
               COUNT c;
               c = pr[tamlido];
               pr[tamlido] = '\0';
               printf("%9.9ld %-15.15s %s\n", reg, nome_cp[campolido], pr);
               pr[tamlido] = c;
      }


      pr += tamlido;
   }

fim :

   if ( buf_reg != NULL )
      free(buf_reg);

   return(ret);
}


VOID load_campo(base)
TEXT *base;
{  COUNT i;
   TEXT aux[80], *p;
   FILE *fpi;

   for ( i = 0; i <= 120; i++ ) {
      sprintf(nome_cp[i], "%3.3d", i);
      tipo_cp[i] = 0;
   }

   strcpy(aux, base);
   strcat(aux, ".map");

   fpi = fopen( aux, "r");

   if ( fpi == NULL ) {
#ifdef ESPANHA
     fprintf(stderr, "Mapa de campos la base no existe.\n");
#else
     fprintf(stderr, "Mapa de campos da base nao existe.\n");
#endif
      return;
   }


   while (fgets(aux, 30, fpi) != NULL ) {
      strip_cr(aux);
      i = atoi(aux);
      if ( i > 0 ) {

         p = strchr(aux, ' ');

         if ( p != NULL ) {
            p++;
            if ( *p == '*' ) {
               tipo_cp[i] = 'T';
               p++;
            }
            strcpy(nome_cp[i], p);
         }
      }
   }

   if ( fpi != NULL )
      fclose (fpi);
}

COUNT sai_texto(pos, reg, campo, flag)
LONG pos, reg;
TEXT *campo;
COUNT flag;
{  UCOUNT tam, lido, ret = -1;
   LONG lreg;
   UTEXT *buf=NULL;
   TEXT  *pi, *pf, *pr;
   struct hdtxt {
      UTEXT txt_marca;
      LONG  txt_record;
      UTEXT txt_campo;
      UTEXT txt_parte;
      POINTER txt_proximo;
   } bufstru;

   if ( param.recup == 0 )
      flag = 0;

 	fseek( fp, pos - 6, 0 );

	if ( fread( &VHDR, sizeof(struct vrfmt), 1, fp) != 1 ) {
      if ( flag == 0 )
         fprintf(fp_log, "Perda de registro tipo TEXTO no registro %ld Campo : %s\n", reg, campo);
      goto fim;
   }

   tam = VHDR.urclen;

   if ( tam == 0 || tam > 40000L )
      goto fim;


   buf = (UTEXT *)malloc(tam+20);

   if ( buf == NULL ) {
      fprintf(fp_log, "Erro de alocacao de memoria\n");
      exit(15);
   }

   lido = fread( buf, 1, tam, fp);

   if ( lido != tam ) {
      if ( flag == 0 )
         fprintf(fp_log, "Perda de registro tipo TEXTO no registro %ld Campo : %s\n", reg, campo);
      goto fim;
   }

   memcpy(&bufstru, buf, sizeof( bufstru ));
   lreg = bufstru.txt_record;


   if ( buf[0] != MARCA_TEXTO || lreg != reg ) {
      if ( flag == 0 )
         fprintf(fp_log, "Inconsistencia no registro %ld campo %s  (lreg = %ld)\n", reg, campo, lreg);
      goto fim;
   }
   else {
      pf = (TEXT *)(buf + tam);
      pi = (TEXT *) ( buf + sizeof(bufstru));

      if ( param.cripto )
         de_cripta((UTEXT *)pi, tam - (pi - (TEXT *)buf) );

      while ( pi < pf ) {
         pr = strchr(pi, 0x0a);
         if ( pr != NULL && pr < pf ) {
            *pr = '\0';
            printf("%9.9ld %-15.15s %s\n", reg, campo, pi);
         }
         else
            break;

         pi = pr + 1;
      }
   }

   ret = 0;

fim :

   free(buf);
   return(ret);
}



COUNT  monta_cripto( arq )
TEXT *arq;
{  UCOUNT B;
   UCOUNT C;
   LONG   D;
   COUNT x;
   Base wbs;
   FILE *f;
   TEXT aux[100];

   strcpy(aux, arq);

   aux[strlen(arq) - 1] = '4';


#ifdef MSDOS
   f = fopen( aux, "rb+");
#else
   f = fopen( aux, "r+");
#endif

   if ( f == NULL ) {
      fprintf(stderr, "Arquivo de descricao(.lb4) da base nao pode ser aberto.\n");
      return(-1);
   }

   x = fseek(f, 262L, 0);


   x = fread( &wbs, sizeof(Base), 1, f);

	   if ( x != 1 ) {
         fprintf(stderr, "Erro lendo lb4.\n");
         return(-1);
      }

   B = wbs.bs_idbase;
   C = wbs.bs_finger;
   D = wbs.bs_cr_data;

   /* se nao for base comercial usa B ao invez de C */
   if ( C == 0 )
      C = B;

   /***********************************************
      Esquema de montagem de semente de critografia
      bs_idbase  = B1  B2
      bs_finger  = C1  C2
      bs_cr_data = D1  D2  D3  D4
      =============================================
      B1 C1 C2 D1 D2 D3 B2 D4 B2 C2
   ************************************************/

   ap_cripto[0] = (B & 0xFF00) >> 8;
   ap_cripto[1] = (C & 0xFF00) >> 8;
   ap_cripto[2] = (C & 0xFF);
   ap_cripto[3] = (D & 0xFF000000L) >> 24;
   ap_cripto[4] = (D & 0x00FF0000L) >> 16;
   ap_cripto[5] = (D & 0x0000FF00L) >> 8;
   ap_cripto[6] = (B & 0xFF);
   ap_cripto[7] = (D & 0xFF);
   ap_cripto[8] = (B & 0xFF);
   ap_cripto[9] = (C & 0xFF);

   return(0);
}

VOID de_cripta(buf, tam)
UTEXT *buf;
UCOUNT tam;
{

   if ( param.cripto == 0 )
      return;

   while(tam) {
      buf[tam] ^= ap_cripto[tam & 0x7];
      tam--;
   }

}


