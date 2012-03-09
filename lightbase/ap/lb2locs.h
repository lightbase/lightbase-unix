#define   L_IN    0
#define   C_IN    1
#define   L_FI    2
#define   C_FI    3
#define   L_VI    4
#define   C_VI    5
#define   TIPO    6


#define   JANELA_MOR       40


#define     INVALIDA        0
#define     JAN_SEL_BASE    1
#define     SENHA_WIN       2
#define     NUMSE_WIN       3
#define     J_CADUSU        4
#define     J_EXCUSU        5
#define     J_MOSUSU        6
#define     J_CADGRP        7
#define     J_MOSGRP        8
#define     J_UTIL          9
#define     J_NEWBASE      10
#define     J_STOPWORD     11
#define     J_DEFCAMPOS    12
#define     J_DEFCARCP     13
#define     J_ATUDESC      14
#define     J_CARAC1REL    15
#define     J_CARAC2REL    16
#define     J_COLUNADO     17
#define     J_NREG         18
#define     J_POSREG       19
#define     J_PSQARG       20
#define     J_PSQPAL       21
#define     J_REGDISP      22
#define     J_ARQLOGRB     23
#define     J_LOGREPRO     24
#define     J_LOGINPORT    25
#define     J_DEVICE       26
#define     J_CARETIQ      27
#define     J_CARFORM      28
#define     J_NUMWIN       29
#define     J_ACTWIN       30
#define     J_WINREIN      31
#define     J_ACESWIN      32
#define     J_CODPERM      33
#define     J_FINGER       34
#define     J_REFINA       35

/* Valores maior que novecentos seram convertidos em funcao do tamanho do */
/* Video.  Ex :  200 -> sixe_?     201 -> (size_? - 1)   204->(size_? - 4) */

#ifdef MODO_DEFINICAO
UTEXT    ap_winloc[JANELA_MOR][7] = {
/*    li, ci, lf, cf,  l_vis, c_vis, tipo   */
/*  0 */  {   0,  0,   0,   0,      0,    0,  0 },   /* INVALIDA     */
/*  1 */  {  12, 17,  14,  55,      0,    0, JS },   /* JAN_SEL_BASE */
/*  2 */  {   4, 26,   5,  51,      0,    0, JP },   /* SENHA_WIN */
/*  3 */  {  19, 24,  20,  54,      0,    0, JP },   /* NUMSE_WIN */
/*  4 */  {  11,  2,  20,  77,      0,    0, JP },   /* J_CADUSU  */
/*  5 */  {  16,  3,  20,  33,      0,    0, JS },   /* J_EXCUSU  */
/*  6 */  {  10,  5, 199,  75,     10,    0, JS },   /* J_MOSUSU  */
/*  7 */  {  15,  4,  20,  77,      0,    0, JP },   /* J_CADGRP  */
/*  8 */  {  10,  5, 199,  75,     10,    0, JS },   /* J_MOSGRP  */
/*  9 */  {   2,  1, 204, 202,      0,    0, JP },   /* J_UTIL    */
/* 10 */  {   3,  3,   4, 204,      0,    0, JS },   /* J_NEWBASE  */
/* 11 */  {   8, 10,  19,  68,      0,    0, JS },   /* J_STOPWORD */
/* 12 */  {   8, 24,  20,  76,      0,    0, JS },   /* J_DEFCAMPOS*/
/* 13 */  {  13, 26,  20,  74,      0,    0, JS },   /* J_DEFCARCP */
/* 14 */  {  17, 10,  19,  70,      0,    0, JS },   /* J_ATUDESC  */
/* 15 */  {   7,  3,  20,  76,      0,    0, JS },   /* J_CARAC1REL*/
/* 16 */  {   9,  3,  20,  76,      0,    0, JS },   /* J_CARAC2REL*/
/* 17 */  {   8, 24,  20,  76,      0,    0, JS },   /* J_COLUNADO*/
/* 18 */  {  10,  5,  14,  75,      0,    0, JS },   /* J_NREG    */
/* 19 */  {  10, 28,  12,  52,      0,    0, JS },   /* J_POSREG  */
/* 20 */  {   3,  2,  10, 203,      0,    0, JS },   /* J_PSQARG  */
/* 21 */  {  10, 30, 190,  70,     10,   37, JS },   /* J_PSQPAL  */
/* 22 */  {   2,  1, 204, 202,      0,    0, JS },   /* J_REGDISP */
/* 23 */  {  18, 25,  19,  55,      0,    0, JS },   /* J_ARQLOGRB*/
/* 24 */  {  14,  3,  64, 204,      7,    0, JP },   /* J_LOGREPRO  */
/* 25 */  {  16,  2,  64, 203,      5,    0, JP },   /* J_LOGINPORT */
/* 26 */  {  14, 30,  15,  70,      0,    0, JS },   /* J_DEVICE  */
/* 27 */  {  12, 17, 209, 222,      0,    0, JS },   /* J_CARETIQ */
/* 28 */  {  12,  3, 207, 204,      0,    0, JS },   /* J_CARFORM */
/* 29 */  {  10, 10,  10,  35,      0,    0, JS },   /* J_NUMWIN */
/* 30 */  {  10, 44,  11,  69,      0,    0, JS },   /* J_ACTWIN */
/* 31 */  {  18, 15,  19,  65,      0,    0, JP },   /* J_WINREIN */
/* 32 */  {   7, 23,  21,  76,      0,    0, JS },   /* J_ACESWIN */
/* 33 */  {  10, 28,  12,  47,      0,    0, JS },   /* J_CODPERM */
/* 34 */  {  10, 21,  13,  54,      0,    0, JS },   /* J_FINGER */
/* 35 */  {  11, 25,  13,  60,      0,    0, JE },   /* J_REFINA */


};

#else
extern UTEXT    ap_winloc[JANELA_MOR][7];
#endif

