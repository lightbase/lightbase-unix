
#include <setjmp.h>

#define A_CALCULA  "calcula" /*** Arquivo Ajuda ***/
#define PI 3.14159265358979323864
#define   VERB_LEN  30

#ifdef        PROTO_ANSI

extern SS  (*c_verbo)( SC *, DB * );

VOID var_inicio( void );
SS   getass( SC * );
DB   gexpr( SC * );
DB   gterm( SC * );
DB   gpowr( SC * );
DB   garg( SC *, SS );
DB   gtrig( SC *, SC );
DB   gnum( SC *, SS );
DB   fact( DB );
VOID makhex( DB, SC * );
VOID makbin( DB, SC * );
VOID makoct( DB, SC * );
VOID calc_erro( SS, WIN * );
SS   c_valor( SC *, SS );

#else

extern SS  (*c_verbo)( );

VOID var_inicio( );
SS   getass( );
DB   gexpr( );
DB   gterm( );
DB   gpowr( );
DB   garg( );
DB   gtrig( );
DB   gnum( );
DB   fact( );
VOID makhex( );
VOID makbin( );
VOID makoct( );
VOID calc_erro( );
SS   c_valor( );

#endif

#define   C_VARS     26  /* variaveis de 'a' a 'z' */

struct Var
{
	DB lett[ C_VARS ];
	UL     flags;        /* 32 bits for 26 flags */
};

extern struct Var var;

#define   C_RETRO    5

extern SC  vf_buf[ VERB_LEN + 2 ];  /* Verbo ou variavel externa ou funcao */
extern DB  val, vals[ C_RETRO ];

extern SS  hxmod,  paren,  err;
extern SS  ind, len, C_V;
extern SC  c_prompt;
extern UC  dv_zero; /* Tratamento de divisao por zero ... */
                    /* ... 'A'  -> Aproxima a expressao para 1 ... */
                    /* ... 'N'  -> Retorna erro mas nao imprime ... */
                    /* ... '!=' -> Retorna erro e imprime. */

extern jmp_buf  SV;

#define   TAM_EXPR  127


