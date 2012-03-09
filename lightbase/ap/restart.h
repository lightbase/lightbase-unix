/* OK */
#define   TAM_SERIE     6
#define   TAM_MAXUSERS  3
#define   TAM_CLIENTE   40
#define   TAM_ACTKEY    12


typedef struct {
   char *cadeia;
   char  sep;
   int  achou;
} PT_TOKEN;


#ifdef MSDOS
int      pega_tk( char *, char *, PT_TOKEN *);
void     get_strid( char *, char * );
char    get_nible( char, int );
short    pede_data( char * );
char   *str_nible( char * );
short    pede_serie( char * );
short    pede_maxusers( char * );
short    pede_cliente( char * );
short    get_actkey( char * );
short    validkey( char * );
short    chave_valida( char *, char *, char *, char *, char * );
void  xora_id( char *, char * );
short    grava( char *, char *, char *, char *, char * );
short    carrega( char *, char *, char *, char *, char *, char * );
short    pede_confirma( char * );
char   *calc_chave( char *, char *, char *, char * );
void    Strupr( char * );
long    procura( FILE *, char * );
char   *eliminabr( char * );
char   *tirabr( char * );
int     serie_Ok( char * );
int     maxusers_Ok( char * );
int     data_Ok( char * );
unsigned short   valid_data( short, short, short );
long    data_long( char * );
long    sysdata_long( void );
unsigned short leapyear( unsigned short );

#else

int     pega_tk( );
void    get_strid( );
char   *get_chave( );
char    get_nible( );
short    pede_data( );
char   *str_nible( );
short    pede_serie( );
short    pede_maxusers( );
short    pede_cliente( );
short    get_actkey(  );
short    validkey(  );
short    chave_valida(  );
void  xora_id(  );
short    grava(  );
short    carrega(  );
short    pede_confirma( );
char   *calc_chave(  );
void    Strupr();
long    procura( );
char    *eliminabr( );
char    *tirabr( );
int     serie_Ok( );
int     maxusers_Ok( );
int     data_Ok( );
unsigned short   valid_data( );
long    data_long( );
long    sysdata_long( );
unsigned short leapyear( );
#endif

#define    BELL      printf("\07")

#ifdef ESPANHA

#define    MSG_01    "\n\nLIGHT BASE - Banco de Datos Documental"
#define    MSG_02    "\n======================================"
#define    MSG_03    "\n\nPrograma de Activacion"
#define    MSG_04    "\n\n\nNumero de Serie..: %s "
#define    MSG_05    "(Version de DEMOSTRACON. Valided: %s )"
#define    MSG_06    "\nNombre del Cliente..: %s\n"
#define    MSG_07    "Clave de Activacion: %s\n"
#define    MSG_08    'N'
#define    MSG_09    'S'
#define    MSG_10    "\n\n­Licencia Activada!\n"
#define    MSG_11    "\n\n­Clave de Activacion no es la correcta!!"
#define    MSG_12    "\n\nNumero de Serie(6 digitos):\n"
#define    MSG_13    "¨Seis digitos?"
#define    MSG_14    "Solamente digitos ... "
#define    MSG_15    "\nFecha de Valided (DDMMAA) si es Demostracion:\n"
#define    MSG_16    "¨DDMMAA ?"
#define    MSG_17    "Solamente digitos ... "
#define    MSG_18    "­Fecha anterior a \"hoy\"!"
#define    MSG_19    "\nNombre del Cliente (40 Posicoes como maximo):\n"
#define    MSG_20    "....5...10...15...20...25...30...35...40\n"
#define    MSG_21    "­Maximo 40 posiciones!"
#define    MSG_22    "\nEscriba la clave de activacion (12 digitos/letras):\n"
#define    MSG_23    "....5...9.12\n"
#define    MSG_24    "­Clave Invalida!"
#define    MSG_25    "\n\n¨Confirma datos ?(S/N): "
#define    MSG_26    "\n\nNo consigue crear fichero %s"
#define    MSG_27    "\n\n¨Desea Grabar el Fichero? (S/N): "
#define    MSG_28    "\n\n­ERRO:Fichero ejecutable no localizado!\n"
#define    MSG_29    "\n\n­ERRO:Fichero ejecutable invalido!\n"
#define    MSG_30    "\n\n­ERRO:Sin permiso de escrita sobre ejecutable!\n"
#define    MSG_31    "\n\n­Fichero light.dat generado!\n"
#define    MSG_32    "\n\n­Ejecute como usuario \"root\"!\n"
#define    MSG_33    "\n\nReactivado ... \n"
#define    MSG_34    "\n\nParametros: -i -m\n"
#define    MSG_35    "\n\nMaximo de Usuarios (Maximo 3 digitos):\n"
#define    MSG_36    "­Maximo tres digitos!"
#define    MSG_37    "Numero de Usuarios: %s\n"

#else

#define    MSG_01    "\n\nLIGHT BASE - Banco de Dados Textual"
#define    MSG_02    "\n==================================="
#define    MSG_03    "\n\nPrograma de Ativacao"
#define    MSG_04    "\n\n\nNumero de Serie..: %s "
#define    MSG_05    "(Copia DEMONSTRACAO. Validade: %s )"
#define    MSG_06    "\nNome do Cliente..: %s\n"
#define    MSG_07    "Chave de Ativacao: %s\n"
#define    MSG_08    'N'
#define    MSG_09    'S'
#define    MSG_10    "\n\nCopia Ativada!.\n"
#define    MSG_11    "\n\nChave de Ativacao nao confere!!"
#define    MSG_12    "\n\nNumero de Serie(6 digitos):\n"
#define    MSG_13    "Seis digitos?"
#define    MSG_14    "Somente digitos ... "
#define    MSG_15    "\nData de Validade (DDMMAA) se Demonstracao:\n"
#define    MSG_16    "DDMMAA ?"
#define    MSG_17    "Somente digitos ... "
#define    MSG_18    "Data anterior a \"hoje\"!"
#define    MSG_19    "\nNome do Cliente (40 Posicoes no maximo):\n"
#define    MSG_20    "....5...10...15...20...25...30...35...40\n"
#define    MSG_21    "Maximo 40 posicoes!!"
#define    MSG_22    "\nEntre com a chave de ativacao (12 digitos/letras):\n"
#define    MSG_23    "....5...9.12\n"
#define    MSG_24    "Chave Invalida.!"
#define    MSG_25    "\n\nConfirma dados ?(S/N): "
#define    MSG_26    "\n\nNao consegue criar arquivo %s"
#define    MSG_27    "\n\nDeseja Gravar Arquivo? (S/N): "
#ifdef MSDOS
#define    MSG_28    "\n\nERRO:Arquivo executavel /usr/bin/light nao encontrado!.\n"
#else
#define    MSG_28    "\n\nERRO:Arquivo executavel ./light nao encontrado!.\n"
#endif
#define    MSG_29    "\n\nERRO:Arquivo executavel invalido!\n"
#define    MSG_30    "\n\nERRO:Sem permissao de escrita sobre executavel!\n"
#define    MSG_31    "\n\nArquivo light.dat gerado!\n"
#define    MSG_32    "\n\nExecute sob usuario \"root\"!\n"
#define    MSG_33    "\n\nReativado ... \n"
#define    MSG_34    "\n\nParametros: -i ou -m\n"
#define    MSG_35    "\n\nMaximo de Usuarios (Maximo 3 digitos):\n"
#define    MSG_36    "Maximo tres digitos?"
#define    MSG_37    "Numero de Usuarios: %s\n"

#endif

