#include  "lb2all.h"

#include <string.h>

#define   MAIN MAIN


#include "tools.h"

#ifdef MSDOS
#include <io.h>
#endif

#ifdef MSDOS
   VOID  sintaxe( TEXT *);
   COUNT trata_lb1(VOID);
   COUNT trata_lb2(VOID);
   COUNT trata_acessos(VOID);
   COUNT refaz_acessos(VOID);
   COUNT manut_tab(VOID);
#else
   VOID  sintaxe( );
   COUNT trata_lb1( );
   COUNT trata_lb2( );
   COUNT trata_acessos( );
   COUNT refaz_acessos( );
   COUNT manut_tab( );
#endif




main(argc, argv)
int argc;
char *argv[];
{  COUNT i, erro=0, testa_ctree=0, t;
   TEXT aux[50];

   ct_maxuser = 2;

	if ( INTREE(12,15,8) ) {
		printf("Erro \n");
		exit(1);
	}

   memset(&param, sizeof(param), '\0');

   param.maximo = 9999;
#ifdef DECALFA
	param.forma_long = 1;
#endif

   fprintf(stderr, "\n\n\n");
   fprintf(stderr, "Ferramenta de manipulacao de arquivos LIGHT\n");
   fprintf(stderr, "Versao 2.3b\n");
   fprintf(stderr, "Copyright 1993 Light Software LTDA\n");
   fprintf(stderr, "Copyright 1995 - Light-Infocon\n\n");
   fprintf(stderr, "Copyright 1996 - Light-Infocon S/A\n\n");

#ifdef MSDOS
   off_reg = 1;    off_campo = 5;    off_parte = 6;    off_info=7;
#else
   off_reg = 4;    off_campo = 8;    off_parte = 9;    off_info=16;
#endif


   if ( argc > 1 ) {

      for ( i = 1; i < argc; i++ ) {
         if (strnicmp(argv[i], "-D", 2) == 0 ) {
            param.delimita_key = 1;
         }
         else if (strnicmp(argv[i], "-M", 2) == 0 ) {
            param.modomanut = 1;
         }
         else if (strnicmp(argv[i], "-C", 2) == 0 ) {
            param.so_conta = 1;
         }
         else if (strnicmp(argv[i], "-A", 2) == 0 ) {
            param.estrutura_alinhada = 1;
         }
         else if (strnicmp(argv[i], "-T", 2) == 0 ) {
            param.totaliza = 1;
            if ( argv[i][2] == '-' )
               param.maximo = atoi(&argv[i][3]);
            if ( argv[i][2] == '+' )
               param.minimo = atoi(&argv[i][3]);
            if ( argv[i][2] == '=' )
               param.maximo = param.minimo = atoi(&argv[i][3]);
         }
         else if (strnicmp(argv[i], "-L", 2) == 0 ) {
            /* forma de armazenar LONG */
            param.forma_long = atoi(&argv[i][2]);
         }
         else if (strnicmp(argv[i], "-X", 2) == 0 ) {
            /* habilita funcoes de teste do ctree */
            testa_ctree = 1;
         }
         else if (strnicmp(argv[i], "-R", 2) == 0 ) {
            /* recupera textos perdidos */
            param.recup = atoi(&argv[i][2]);
         }
         else if (strnicmp(argv[i], "-P", 2) == 0 ) {
            /* Arquivo lb2 protegido (criptografado) */
            param.cripto = 1;
         }
         else if (strnicmp(argv[i], "-O", 2) == 0 ) {
            /* posicao inicial para start */
            param.posicao = atol(&argv[i][2]);
         }
         else if (strnicmp(argv[i], "-S", 2) == 0 ) {
            /* posicao inicial para start em registro */
            param.start = atol(&argv[i][2]);
         }
         else if (strnicmp(argv[i], "-E", 2) == 0 ) {
            /* Estrutura de armazenamento de campos textos */
            sscanf(&argv[i][2], "%d,%d,%d,%d", &off_reg, &off_campo, &off_parte, &off_info);
         }
         else {
            erro = 1;
         }
      }

      if ( erro ) {
         sintaxe(argv[0]);
         exit(0);
      }
   }


   while (1) {


      fprintf(stderr, "\n\n\nFuncao a ser executada :\n\n");
      fprintf(stderr, "   1 ) Manutencao de tabelas indexadas\n");     if ( param.modomanut == 0 ) fprintf(stderr, "\n");
      fprintf(stderr, "   2 ) Exportacao de dados de acessos.dat\n");  if ( param.modomanut == 0 ) fprintf(stderr, "\n");
      fprintf(stderr, "   3 ) Regeracao do cadastro de acessos\n");    if ( param.modomanut == 0 ) fprintf(stderr, "\n");
      if ( param.modomanut != 0 ) {
         fprintf(stderr, "   4 ) lista arquivo de indices\n");
         fprintf(stderr, "   5 ) Pesquisa arquivo de indices\n");
         fprintf(stderr, "   6 ) Manipulacao de registros de controle\n");
         fprintf(stderr, "   7 ) Recuperacao de informacoes de LB2\n");
      }
      if ( testa_ctree == 1 )
         fprintf(stderr, "   9 ) Teste do Metodo de acesso\n");

      fprintf(stderr, "   . ) Encerra\n");

      gets(aux);

      if (aux[0] == '.' )
         break;

      t = atoi(aux);

      switch(t) {
         case 1 : manut_tab();
                  break;

         case 2 : trata_acessos();
                  break;

         case 3 : refaz_acessos();
                  break;

         case 4 : lista_ind(NULL, NULL);
                  break;

         case 5 : pesquisa(NULL);
                  break;

         case 6 : trata_lb1();
                  break;

         case 7 : trata_lb2();
                  break;

         case 9 : teste_ctree( );
                  break;

         default: fprintf(stderr, "Opcao invalida\n\n\n");
                  break;
      }
   }

   return;
}


VOID sintaxe(prog)
TEXT *prog;
{
         fprintf(stderr, "\nChame %s -D -T[{+-=}n] -C\n", prog);
         fprintf(stderr, "      Onde : \n");
         fprintf(stderr, "             -D So' trata indice ate 1o branco\n");
         fprintf(stderr, "             -T totaliza ocorrencias de indices\n");
         fprintf(stderr, "                Seta automaticamente opcao -D\n");
         fprintf(stderr, "                Inibe opcao -C\n");
         fprintf(stderr, "                opcionalmente pode ser definido limites\n");
         fprintf(stderr, "                + n so imprime quem for maior que n\n");
         fprintf(stderr, "                - n so imprime quem for menor que n\n");
         fprintf(stderr, "                = n so imprime quem for igual a n\n");
         fprintf(stderr, "             -C So' contabiliza\n\n");

}


