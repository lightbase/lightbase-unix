#define   GRP_FER 27

#define   ZAP     'Z'
#define   IMPORT  'I'
#define   EXCLUI  'E'
#define   INFOR   'F'
#define   ESTAT   'S'

#define   MAX_REG     5000

#define   VERSAO_CRIPT  600
#define   VERSAO_LONG   410  /* se mudar mude tambem lb2gntxt.c */

#ifdef MODO_DEF_FER
   Base *rec_base;
   TEXT *buf_reg;
#else
   extern Base *rec_base;
   extern TEXT *buf_reg;
#endif


#define   H_CFZAP    "CF_ZAP"   /* confirmacao para apagar dados da base */
#define   H_ARQRP    "ARQ_RPLOG"
#define   H_CFEXC    "CF_FREXC"    /* confirma exclusao de base */
#define   H_NOPCA    "NOPENRW"     /* nao pode abrir acessos para R/W */
#define   H_MEMREB   "MEMOREB"     /* sem memoria para reprocessar */

#define   MN_TL_TIT     loc_str(GRP_FER, 1 )
#define   MN_TL_ROD     loc_str(GRP_FER, 2 )
#define   MN_TL_T01     loc_str(GRP_FER, 3 )
#define   MN_TL_T02     loc_str(GRP_FER, 4 )
#define   MN_TL_T03     loc_str(GRP_FER, 5 )
#define   MN_TL_T04     loc_str(GRP_FER, 6 )

#define   MN_TL_R01     loc_str(GRP_FER, 7 )
#define   MN_TL_R02     loc_str(GRP_FER, 8 )
#define   MN_TL_R03     loc_str(GRP_FER, 9 )
#define   MN_TL_R04     loc_str(GRP_FER, 10)

#define   C_MN_INFORM   loc_str(GRP_FER, 11)
#define   C_MN_REPROC   loc_str(GRP_FER, 12)
#define   C_MN_ZAP      loc_str(GRP_FER, 13)
#define   C_MN_EXCLUI   loc_str(GRP_FER, 14)

#define   C_IB_TIT      loc_str(GRP_FER, 15)
#define   C_IB_NOME     loc_str(GRP_FER, 16)
#define   C_IB_MAGI     loc_str(GRP_FER, 17)
#define   C_IB_DESC     loc_str(GRP_FER, 18)
#define   C_IB_PATH     loc_str(GRP_FER, 19)
#define   C_IB_DONO     loc_str(GRP_FER, 20)
#define   C_IB_GRUP     loc_str(GRP_FER, 21)
#define   C_IB_NCAM     loc_str(GRP_FER, 22)
#define   C_IB_NVIS     loc_str(GRP_FER, 23)
#define   C_IB_NREL     loc_str(GRP_FER, 24)
#define   C_IB_CRIA     loc_str(GRP_FER, 25)
#define   C_IB_ALTE     loc_str(GRP_FER, 26)
#define   C_IB_BACK     loc_str(GRP_FER, 27)
#define   C_IB_AREG     loc_str(GRP_FER, 28)
#define   C_IB_DREG     loc_str(GRP_FER, 29)
#define   C_IB_IREG     loc_str(GRP_FER, 30)
#define   C_IB_NREG     loc_str(GRP_FER, 31)
#define   C_IB_NOBACK   loc_str(GRP_FER, 32)

#define   C_IB_GO       loc_str(GRP_FER, 33)


#define   TW_LB_D00  loc_str(GRP_FER, 34)
#define   TW_LB_D99  loc_str(GRP_FER, 35)
#define   TW_LB_D01  loc_str(GRP_FER, 36)
#define   TW_LB_D02  loc_str(GRP_FER, 37)

#define   T_JANLOG   loc_str(GRP_FER, 38)


#define   TW_EB_T00  loc_str(GRP_FER, 40)
#define   TW_EB_T99  loc_str(GRP_FER, 41)
#define   TW_EB_T01  loc_str(GRP_FER, 42)
#define   TW_EB_T02  loc_str(GRP_FER, 43)
#define   TW_EB_T03  loc_str(GRP_FER, 44)
#define   TW_EB_T04  loc_str(GRP_FER, 45)

#define   M_DLARQB   loc_str(GRP_FER, 46)
#define   M_DLACEB   loc_str(GRP_FER, 47)

#define   E_MEMREB   loc_str(GRP_FER, 48)
#define   C_RP_RLOG  loc_str(GRP_FER, 49)

#define   C_RB_TIT   loc_str(GRP_FER, 50)

#define   C_RB_F1    loc_str(GRP_FER, 51)
#define   C_RB_F2    loc_str(GRP_FER, 52)
#define   C_RB_F3    loc_str(GRP_FER, 53)

#define   C_RB_F1D   loc_str(GRP_FER, 54)
#define   C_RB_F2D   loc_str(GRP_FER, 55)
#define   C_RB_F3D   loc_str(GRP_FER, 56)

#define   TW_RB_D00  loc_str(GRP_FER, 57)
#define   TW_RB_D99  loc_str(GRP_FER, 58)
#define   TW_RB_D01  loc_str(GRP_FER, 59)
#define   TW_RB_D02  loc_str(GRP_FER, 60)
#define   TW_RB_D03  loc_str(GRP_FER, 61)
#define   TW_RB_D04  loc_str(GRP_FER, 62)



#define   ML_REPRO  loc_str(GRP_FER, 63)
#define   ML_F1BEG  loc_str(GRP_FER, 64)
#define   ML_F1END  loc_str(GRP_FER, 65)
#define   ML_F2BEG  loc_str(GRP_FER, 66)
#define   ML_F2END  loc_str(GRP_FER, 67)
#define   ML_ERRDAD loc_str(GRP_FER, 68)
#define   ML_F3BEG  loc_str(GRP_FER, 69)
#define   ML_F3END  loc_str(GRP_FER, 70)

#define   M_FRB_NAV loc_str(GRP_FER, 71)
#define   M_FRB_DIR loc_str(GRP_FER, 72)

#define   E_REBCLS  loc_str(GRP_FER, 73)
#define   E_RBREN   loc_str(GRP_FER, 74)
#define   E_RBAPG   loc_str(GRP_FER, 75)
#define   E_RBRXY   loc_str(GRP_FER, 76)

#define   E_RDOPN    loc_str(GRP_FER, 77 )

#define   ML_COR4    loc_str(GRP_FER, 78 )
#define   ML_NOREB   loc_str(GRP_FER, 79 )
#define   ML_RDFIN   loc_str(GRP_FER, 80 )

#define   ML_BEGCONS loc_str(GRP_FER, 81 )
#define   ML_EROP4   loc_str(GRP_FER, 82 )
#define   ML_ERCONS  loc_str(GRP_FER, 83 )

#define   ML_LDCPS   loc_str(GRP_FER, 84 )
#define   ML_NOCPS   loc_str(GRP_FER, 85 )
#define   ML_RCCPS   loc_str(GRP_FER, 86 )

#define   ML_LDVIS   loc_str(GRP_FER, 87 )
#define   ML_NOVIS   loc_str(GRP_FER, 88 )
#define   ML_AVVIS   loc_str(GRP_FER, 89 )
#define   ML_DSVIS   loc_str(GRP_FER, 90 )
#define   ML_RCVIS   loc_str(GRP_FER, 91 )

#define   ML_LDEVIS  loc_str(GRP_FER, 92 )
#define   ML_DSITV   loc_str(GRP_FER, 93 )
#define   ML_NOEVIS  loc_str(GRP_FER, 94 )

#define   ML_LDREL   loc_str(GRP_FER, 95 )
#define   ML_NOREL   loc_str(GRP_FER, 96 )
#define   ML_AVREL   loc_str(GRP_FER, 97 )
#define   ML_DSREL   loc_str(GRP_FER, 98 )
#define   ML_RCREL   loc_str(GRP_FER, 99 )

#define   ML_LDEREL  loc_str(GRP_FER, 100)
#define   ML_DSITR   loc_str(GRP_FER, 101)
#define   ML_NOEREL  loc_str(GRP_FER, 102)

#define   ML_AVLB4   loc_str(GRP_FER, 103)

#define   C_IN_JLOG  loc_str(GRP_FER, 104)






#define   ML_EAVREC     loc_str(GRP_FER, 105)
#define   ML_LSTTXT     loc_str(GRP_FER, 106)
#define   ML_KEYDES     loc_str(GRP_FER, 107)
#define   ML_WRREG      loc_str(GRP_FER, 108)
#define   ML_RECREG     loc_str(GRP_FER, 109)
#define   ML_ERNEW      loc_str(GRP_FER, 110)
#define   ML_LSTCTR     loc_str(GRP_FER, 111)
#define   ML_BIGREG     loc_str(GRP_FER, 112)
#define   ML_CPTRUNC    loc_str(GRP_FER, 113)
#define   ML_BIGTXT     loc_str(GRP_FER, 114)
#define   ML_CANIDX     loc_str(GRP_FER, 115)

#define   E_READLOG     loc_str(GRP_FER, 116)
#define   E_RBIND       loc_str(GRP_FER, 117)

#define   C_FER3_AT     loc_str(GRP_FER, 118)
#define   C_FER3_IN     loc_str(GRP_FER, 119)
#define   C_FER3_RE     loc_str(GRP_FER, 120)
#define   C_FER3_PR     loc_str(GRP_FER, 121)
#define   C_FER3_NR     loc_str(GRP_FER, 122)

#define   TW_ZPRB1      loc_str(GRP_FER, 123)
#define   TW_ZPRB2      loc_str(GRP_FER, 124)
#define   TW_ZPRB3      loc_str(GRP_FER, 125)

#define   M_WAITMRK     loc_str(GRP_FER, 126)
#define   M_WAITIND     loc_str(GRP_FER, 127)


#define   T_DELBAS      loc_str(GRP_FER, 128)

#define   ML_REGRUIM    loc_str(GRP_FER, 129)


#define   TW_OT_D00     loc_str(GRP_FER, 130)
#define   TW_OT_D99     loc_str(GRP_FER, 131)
#define   TW_OT_D01     loc_str(GRP_FER, 132)

#define   ML_CTLINV     loc_str(GRP_FER, 133)

#define   E_USECONV     loc_str(GRP_FER, 501)
