#define   GRP_DEFBASE    16

#define   H_CRBA1    "ERCRDIRB" /* Erro na criacao do diretorio da base */
#define   H_CRBA2    "ERINSTOP" /* Erro na inicializacao de stopword */
#define   H_CRBA3    "ERATUACE" /* Erro na inclusao de base em acessos */
#define   H_NOPCA    "NOPENRW"    /* nao pode abrir acessos para R/W */

#define   OP_CAMPOS   loc_str(GRP_DEFBASE, 36)
#define   OP_CRITICAS loc_str(GRP_DEFBASE, 37)


#define   E_CRBA1    loc_str(GRP_DEFBASE, 510)
#define   E_CRBA2    loc_str(GRP_DEFBASE, 511)
#define   E_CRBA3    loc_str(GRP_DEFBASE, 512)
#define   E_NOPCA    loc_str(GRP_DEFBASE, 513)
#define   E_NOPRD    loc_str(GRP_DEFBASE, 514)

#define   H_MXITEM      "MXITVIRE"
#define   E_MXITEM      loc_str(GRP_DEFBASE, 526)


typedef struct {   /* estrutura permanente para edicao de vistas   */
   TEXT  nome[TE+1];       /* Nome sintetico do campo              */
   TEXT  desc[TD+1];       /* Descricao do campo                   */
   TEXT  tipo_x;           /* Tipo de campo - caracter unico       */
   TEXT  tipo[TD+1];       /* Tipo de campo - string               */
   TEXT  tamanho[5];       /* Tamanho do campo 999                 */
   TEXT  decimais[4];      /* numero de decimais                   */
   TEXT  mascara[TM+1];    /* Mascara de leitura/display -picture  */
   TEXT  crit_x;           /* Tipo de critica caracter unico       */
   TEXT  critica[TD+1];    /* Tipo de critica (string)             */
   TEXT  rodape[52];       /* Mensagem apresentada no rodape       */
   TEXT  help[9];          /* help de campo                        */
   TEXT  unica[TD+1];      /* Nao podera existir outra chave igual */
   TEXT  index[TD+1];      /* Devera ou nao ser incluido no indice */
   TEXT  branco[TD+1];     /* Indexa campos sem informacao         */
   TEXT  fonetica[TD+1];   /* Possibilita pesquisa fonetica        */

   union {
      struct {
         TEXT  tabela[25];   /* Nome do arquivo tabela             */
         TEXT  autonum[25];  /* Mascara para autonum               */
      } tab_auto;
      struct {
         TEXT  minimo[25];  /* Valor minimo .....ver cp_param */
         TEXT  maximo[25];  /* Valor maximo .....ver cp_param */
      } inter;
      TEXT  masc_formula[30];
   } param;
} Cw;


/*---------------------------------------------------------------------*/
/*  Edicao de Relatorios                                               */
/*---------------------------------------------------------------------*/
typedef struct {   /* estrutura permanente para edicao de relatorios   */
	TEXT     r_ident[TD+1];   /* identificacao do relatorio              */
   TEXT     r_tipo_r;        /* tipo de relatorio - caracter unico      */
/*   TEXT     r_tipo[10]; */     /* tipo de relatorio (F)orm. (C)ol. (E)tiq */
   TEXT     r_cabec[10];     /* imprimir cabec SIM/NAO                  */
   TEXT     r_titulo[51];    /* titulo do relatorio                     */
   TEXT     r_rodape[51];    /* Rodape do relatorio                     */
   TEXT     r_linhas[10];    /* numero de linhas da folha               */
   TEXT     r_colunas[4];    /* numero de colunas do papel              */
   TEXT     r_caracter[15];  /* tipo do caracter Normal/Comprimido      */
   TEXT     r_qualid[15];    /* Qualidade: Normal/Carta                 */
   TEXT     r_folha[10];     /* impressao em folha solta                */

   TEXT     r_editdoc[10];   /* editar texto de instrucoes especiais    */

   TEXT     r_elin[4];       /* linhas entre etiquetas */
   TEXT     r_ecol[4];       /* colunas entre etiquetas */
   TEXT     r_netq[4];       /* numero de etiquetas po linha */

   TEXT     r_1campo[TD+1];  /* campo de classificacao                  */
   TEXT     r_1ord[TD+1];    /* ordem Ascendente/Descendente            */
   TEXT     r_1linhas[4];    /* linhas a pular na quebra (99 pagina)    */
   TEXT     r_1total[TD+1];  /* totaliza SIM/NAO                        */

   TEXT     r_2campo[TD+1];  /* campo de classificacao                  */
   TEXT     r_2ord[TD+1];    /* ordem Ascendente/Descendente            */
   TEXT     r_2linhas[4];    /* linhas a pular na quebra (99 pagina)    */
   TEXT     r_2total[TD+1];  /* totaliza SIM/NAO                        */

   TEXT     r_3campo[TD+1];  /* campo de classificacao                  */
   TEXT     r_3ord[TD+1];    /* ordem Ascendente/Descendente            */
   TEXT     r_3linhas[4];    /* linhas a pular na quebra (99 pagina)    */
   TEXT     r_3total[TD+1];  /* totaliza SIM/NAO                        */

   TEXT     *r_campos;
   TEXT     *r_ordcampos;
} Rw;
