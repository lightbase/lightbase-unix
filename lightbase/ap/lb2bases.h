#define   IND_EFETIVA   20     /* Tamanho Maximo da PALAVRA INDEXADA    */

#define   TAM_CPNUM     15
#define   TAM_DECIMAIS   4



#define   TREE_PALAVRAS    0
#define   TREE_NUMEROS     1
#define   TREE_DATAS       2
#define   TREE_HORAS       3
#define   TREE_NAO_INDEX   4
#define   TREE_DELETADOS   5
#define   TREE_UNIQ        6
#define   TREE_BRANCOS     'B'
#define   TREE_CHEIOS      'C'
#define   TREE_FONETICA    'F'

#define     AC_NUM(n)     (c_bases[n]->cb_prfile + 0)
#define     AD_NUM(n)     (c_bases[n]->cb_prfile + 1)
#define     AI_NUM(n)     (c_bases[n]->cb_prfile + 2)



#define   OFF_CMP    1
#define   OFF_TAM    1
#define   OFF_ALL    (OFF_CMP + OFF_TAM)


#define   FLUSH_IDX    0
#define   ACUMULA_IDX  1


#define   NOVO_REGISTRO    '0'
#define   ATIVO_INDEX      'I'
#define   ATIVO_NAO_INDEX  'N'
#define   LIBERADO         'D'
#define   MORTO_INDEX      'X'
#define   MORTO_NAO_INDEX  'M'

#define   NOVO 'N'
#define   MODIFICADO 'M'
#define   REB_WARNING 'W'
#define   REB_OBRIGATORIO 'O'
#define   EXCLUIDO  'E'
#define   NAO_MODIFICADO 'P'


#define   CP_TEXTO 'T'
