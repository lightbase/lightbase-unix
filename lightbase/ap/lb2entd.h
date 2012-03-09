#define   GRP_ENTD   25

#define   LF13E   "1 3 E"
#define   MASC_NREG   "999.999.999"



#define   H_CUNICA   "C_UNICA"    /* informe chave unica */
#define   H_NREG     "NREG"       /* informe numero do registro */

#define   M_GETNUM   loc_str(GRP_ENTD,  1)
#define   M_GETREG   loc_str(GRP_ENTD,  2)

#define   T_ALTERA   loc_str(GRP_ENTD,  3)
#define   T_EXCLUI   loc_str(GRP_ENTD,  4)

#define   S_REG_ALT  loc_str(GRP_ENTD,  5)
#define   S_REG_EXC  loc_str(GRP_ENTD,  6)

#define   IC_INCLUI  loc_str(GRP_ENTD,  7)
#define   IC_ALTERA  loc_str(GRP_ENTD,  8)
#define   IC_EXCLUI  loc_str(GRP_ENTD,  9)

#define   R_INCLUI   loc_str(GRP_ENTD, 10)
#define   R_ALTERA   loc_str(GRP_ENTD, 11)
#define   R_EXCLUI   loc_str(GRP_ENTD, 12)

#define   R_PROTOC   loc_str(GRP_ENTD, 13)




#define   M_NAVEGA   loc_str(STR_GLB, 45)
#define   M_REGSEL   loc_str(STR_GLB, 46)
#define   T_POSREG   loc_str(STR_GLB, 47)
#define   C_REGLD    loc_str(STR_GLB, 48)
#define   M_WAITCRIT loc_str(STR_GLB, 49)
#define   M_WAITATU  loc_str(STR_GLB, 50)

#define   E_INFOBRIG loc_str(STR_GLB, 51)
#define   E_VLIT1    loc_str(STR_GLB, 52)
#define   E_VLIT2    loc_str(STR_GLB, 53)
#define   E_VLIT4    loc_str(STR_GLB, 55)
#define   E_DVINV    loc_str(STR_GLB, 56)
#define   E_CPFINV   loc_str(STR_GLB, 57)
#define   E_CGCINV   loc_str(STR_GLB, 58)
#define   E_EXUNICA  loc_str(STR_GLB, 59)
#define   E_WRUNICA  loc_str(STR_GLB, 60)
#define   M_SAIEDIT  loc_str(STR_GLB, 61)


#define   H_POSREG      "POSREG"    /* numero de ordem na pesquisa */
#define   H_EDMTEX      "E_EDMTEX"  /* tamanho maximo de edicao atingido */
#define   H_INFOBRIG    "E_INFOBR"    /* Informacao obrigatoria */
#define   H_VLIT1       "DATAINV"     /* Data invalida */
#define   H_VLIT2       "HORAINV"     /* Hora invalida */
#define   H_VLIT4       "FORAINT"     /* Valor fora do intervalo */
#define   H_ITEMTAB     "E_ITEMTA"    /* */
#define   H_DVINV       "E_DVINV"     /* Digito verificador invalido */
#define   H_CPFINV      "E_CPFINV"    /* CPF Invalido */
#define   H_CGCINV      "E_CGCINV"    /* CGC Invalido */
#define   H_EXUNICA     "E_EXUNIC"    /* Chave unica ja existe */
#define   H_WRUNICA     "E_WRUNIC"    /* Chave no campo : %s nao e unica */
#define   H_CFAEDI      "CF_A_EDI"    /* adandona edicao ? */
#define   H_CFWRREG     "CF_WRREG"    /* Retenta gravar registro ? */
#define   H_REGIND      "E_REGIND"    /* Erro na indexacao de registro */




extern UC   fl_erro_on;
#define   set_erro(x)  fl_erro_on = x
