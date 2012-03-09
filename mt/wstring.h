
#define   MAX_ARQ       60
#define   MAX_GETS      300 /* Sempre > MAX_CHAR */
#define   MAX_CHAR      120
#define   MAX_STR       500  /* MAX_STR * MAX_CHAR + MAX_STR'\0' < 65000L */
#define   STR_GRUPO     "GRP"
#define   ABRE_STR      '<'
#define   FECHA_STR     '>'

struct GROUP
{
   US     num;  /* Numero do grupo */
   US     max;  /* Quantidade maxima de mensagens */
   US     mem;  /* Quantidade de memoria para as mensagens */
   SS     qup;  /* Indica quantidade de up_str() */
   SL     off;  /* Offset da primeira mensagem no arquivo */
   SC    *str;  /* As mensagens */
   SC   **ptr;  /* Ponteiros para as mensagens em GRP->STR ou VIRTUAL_MEM */
   struct GROUP  *nxt;
};

typedef struct GROUP GRP;

#ifdef   PROTO_ANSI

SS    start_str( SC *arquivo );
VOID  end_str( void );
SS    up_str( SS numero );
SS    down_str( SS numero );
SC    *loc_str( SS grupo, SS item );
SS    stat_str( SS numero );

#else

SS    start_str( );
VOID  end_str( );
SS    up_str( );
SS    down_str( );
SC    *loc_str( );
SS    stat_str( );

#endif

