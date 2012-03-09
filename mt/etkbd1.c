
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  "window.h"
#include  "ethead.h"
#include  <mensagem.h>

#ifdef    MSDOS
#include  <io.h>
#else
SI access( );
#endif

#ifdef    XTOOLS
SS        carga_comxt( );
#endif

SS def_cmd( arquivo, mac_arq )

SC *arquivo;
SC *mac_arq;
{
   SS   I, num_tec, num_com;

   static COMANDOS comandos[] =      /* lista de comandos */
   {
      str_vazia,     VOLTAESPACO,     0,
      str_vazia,            SOBE,     0,
      str_vazia,           DESCE,     0,
      str_vazia,        ESQUERDA,     0,
      str_vazia,         DIREITA,     0,
      str_vazia,        INILINHA,     0,
      str_vazia,        FIMLINHA,     0,
      str_vazia,        ROLACIMA,     0,
      str_vazia,       ROLABAIXO,     0,
      str_vazia,       PROPAGINA,     0,
      str_vazia,       ANTPAGINA,     0,
      str_vazia,      INIARQUIVO,     0,
      str_vazia,      FIMARQUIVO,     0,
      str_vazia,      PROPALAVRA,     0,
      str_vazia,      ANTPALAVRA,     0,
      str_vazia,      TROCATEXTO,     0,
      str_vazia,        DUPLINHA,     0,
      str_vazia,       PROCTEXTO,     0,
      str_vazia,       INSLIDEPO,     0,
      str_vazia,      INSLIANTES,     0,
      str_vazia,      JUNTAPROXL,     0,
      str_vazia,        TAMTEXTO,     0,
      str_vazia,       VAIPRALIN,     0,
      str_vazia,     APAGAFIMLIN,     0,
      str_vazia,       INIBLCHAR,     0,
      str_vazia,        INIBLCOL,     0,
      str_vazia,        INIBLLIN,     0,
      str_vazia,      BLOCOCOPIA,     0,
      str_vazia,     BLOCOINSERE,     0,
      str_vazia,       BLOCOMOVE,     0,
      str_vazia,     DESMARCABLO,     0,
      str_vazia,      APAGABLOCO,     0,
      str_vazia,         EXPORTE,     0,
      str_vazia,         IMPORTE,     0,
      str_vazia,     QUEBRALINHA,     0,
      str_vazia,      APAGALINHA,     0,
      str_vazia,        APAGACAR,     0,
      str_vazia,        APAGAPAL,     0,
      str_vazia,        INSCTRLE,     0,

#ifdef   MEDIT
      str_vazia,        ABANDONA,     1,
      str_vazia,         SALVARQ,     1,
#else
      str_vazia,        ABANDONA,     0,
      str_vazia,         SALVARQ,     0,
#endif

      str_vazia,     INSEREOUNAO,     0,

#ifdef   MEDIT
      str_vazia,            ZOOM,     1,

#else
      str_vazia,            ZOOM,     0,

#endif

#ifdef   MEDIT
      str_vazia,      MUDAJANELA,     1,
      str_vazia,    DIVIDEJANELA,     1,
      str_vazia,       EXPJANELA,     1,
      str_vazia,      EDITANOTAS,     1,
      str_vazia,      VERARQUIVO,     1,
      str_vazia,     NOVOARQUIVO,     1,
      str_vazia,         TERMINA,     1,
      str_vazia,        MENUARQS,     1,
      str_vazia,         COMPILA,     1,
#endif

      str_vazia,         PROCPAR,     0,

#ifdef   MEDIT
      str_vazia,         JANERRO,     1,
      str_vazia,       COMSYSTEM,     1,
#endif

      str_vazia,        PLINTELA,     0,
      str_vazia,        ULINTELA,     0,
      str_vazia,         ROLAESQ,     0,
      str_vazia,         ROLADIR,     0,

#ifdef   MEDIT
      str_vazia,        SAISALVA,     1,
      str_vazia,     PROXARQUIVO,     1,
      str_vazia,      ANTARQUIVO,     1,
#endif

      str_vazia,       MUDATRANS,     0,

#ifdef   MEDIT
      str_vazia,       JANSYSOUT,     1,
      str_vazia,      JANSYSERRO,     1,
      str_vazia,         EXECUTA,     1,
#endif

      str_vazia,      GRAVAMACRO,     0,
      str_vazia,        EXEMACRO,     0,
      str_vazia,        ARQMACRO,     0,
      str_vazia,     REPITABUSCA,     0,

#ifdef   MEDIT
      str_vazia,     PROXCARREGA,     1,
      str_vazia,      ANTCARREGA,     1,
#endif

#ifdef   MEDIT
      str_vazia,      IMPRIMEARQ,     1,
#else
      str_vazia,      IMPRIMEARQ,     0,
#endif

#ifdef   MEDIT
      str_vazia,        EDITAVER,     1,
      str_vazia,       TIRALISTA,     1,
#endif

      str_vazia,          DESFAZ,     0,
      str_vazia,           REFAZ,     0,

      str_vazia,     INDENTBLOCO,     0,
      str_vazia,       LINHACIMA,     0,
      str_vazia,      LINHABAIXO,     0,
      str_vazia,       LINHAMEIO,     0,
      str_vazia,      APPBLCOPIA,     0,
      str_vazia,       APPBLMOVE,     0,
      str_vazia,      BLOCOUPPER,     0,
      str_vazia,      BLOCOLOWER,     0,

#ifdef   MEDIT
      str_vazia,        ORDEMENU,     1,
      str_vazia,        TROCAARQ,     1,
      str_vazia,        MARCAPOS,     1,
      str_vazia,        BUSCAPOS,     1,
#endif

      str_vazia,      MARGEMPARA,     0,
      str_vazia,       TROCATRAS,     0,
      str_vazia,        PROCTRAS,     0,
      str_vazia,    REPITAFRENTE,     0,
      str_vazia,      REPITATRAS,     0,

#ifdef   MEDIT
      str_vazia,     SAICONTEXTO,     1,
      str_vazia,     TECLAQUENTE,     1,
#endif

      str_vazia,       EXPRECHAR,     0,
      str_vazia,         CODCHAR,     0,

#ifdef   MEDIT
      str_vazia,      RECARREGRA,     1,
#endif

      NULL,               0,     0,
   };

   static TECLAS teclas[] =
   {
      str_vazia,            NULA,
      str_vazia,           CTRLA,
      str_vazia,           CTRLB,
      str_vazia,           CTRLC,
      str_vazia,           CTRLD,
      str_vazia,           CTRLE,
      str_vazia,           CTRLF,
      str_vazia,           CTRLG,
      str_vazia,           CTRLH,
      str_vazia,           CTRLI,
      str_vazia,           CTRLJ,
      str_vazia,           CTRLK,
      str_vazia,           CTRLL,
      str_vazia,           CTRLM,
      str_vazia,           CTRLN,
      str_vazia,           CTRLO,
      str_vazia,           CTRLP,
      str_vazia,           CTRLQ,
      str_vazia,           CTRLR,
      str_vazia,           CTRLS,
      str_vazia,           CTRLT,
      str_vazia,           CTRLU,
      str_vazia,           CTRLV,
      str_vazia,           CTRLW,
      str_vazia,           CTRLX,
      str_vazia,           CTRLY,
      str_vazia,           CTRLZ,
      str_vazia,          ESCAPE,
      str_vazia,      TECLAVOLTA,

      str_vazia,              F1,
      str_vazia,              F2,
      str_vazia,              F3,
      str_vazia,              F4,
      str_vazia,              F5,
      str_vazia,              F6,
      str_vazia,              F7,
      str_vazia,              F8,
      str_vazia,              F9,
      str_vazia,             F10,
      str_vazia,        SETACIMA,
      str_vazia,       SETABAIXO,
      str_vazia,         SETAESQ,
      str_vazia,         SETADIR,
      str_vazia,        PAGINANT,
      str_vazia,      PAGINAPROX,
      str_vazia,          INICIO,
      str_vazia,             FIM,
      str_vazia,          TECINS,
      str_vazia,           APAGA,
      str_vazia,             TAB,

      str_vazia,           ALTF1,
      str_vazia,           ALTF2,
      str_vazia,           ALTF3,
      str_vazia,           ALTF4,
      str_vazia,           ALTF5,
      str_vazia,           ALTF6,
      str_vazia,           ALTF7,
      str_vazia,           ALTF8,
      str_vazia,           ALTF9,
      str_vazia,          ALTF10,
      str_vazia,         BACKTAB,
      str_vazia,        CTRLPGUP,
      str_vazia,        CTRLPGDN,
      str_vazia,        PALAPROX,
      str_vazia,         PALAANT,
      str_vazia,         ALT_A,
      str_vazia,         ALT_B,
      str_vazia,         ALT_C,
      str_vazia,         ALT_D,
      str_vazia,         ALT_E,
      str_vazia,         ALT_F,
      str_vazia,         ALT_G,
      str_vazia,         ALT_H,
      str_vazia,         ALT_I,
      str_vazia,         ALT_J,
      str_vazia,         ALT_K,
      str_vazia,         ALT_L,
      str_vazia,         ALT_M,
      str_vazia,         ALT_N,
      str_vazia,         ALT_O,
      str_vazia,         ALT_P,
      str_vazia,         ALT_Q,
      str_vazia,         ALT_R,
      str_vazia,         ALT_S,
      str_vazia,         ALT_T,
      str_vazia,         ALT_U,
      str_vazia,         ALT_V,
      str_vazia,         ALT_W,
      str_vazia,         ALT_X,
      str_vazia,         ALT_Y,
      str_vazia,         ALT_Z,

#ifdef    XTOOLS

      str_vazia,         ENTER,
      str_vazia,           TAB,
      str_vazia,       BACKTAB,

#endif

      NULL,               0,
   };

   up_str( STR_COMP );
   
   /*** COMANDOS ***/
   
   I = 0;

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_1  );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_2  );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_3  );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_4  );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_5  );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_6  );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_7  );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_8  );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_9  );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_10 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_11 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_12 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_13 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_14 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_15 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_16 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_17 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_18 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_19 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_20 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_21 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_22 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_23 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_24 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_25 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_26 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_27 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_28 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_29 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_30 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_31 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_32 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_33 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_34 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_35 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_36 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_37 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_38 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_39 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_40 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_41 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_42 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_43 );

#ifdef   MEDIT

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_44 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_45 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_46 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_47 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_48 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_49 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_50 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_51 );
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_52 );

#endif

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_114 );  /* PROCPAR */

#ifdef   MEDIT
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_115 );  /* JANERRO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_116 );  /* COMSYSTEM */
#endif

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_117 );  /* PLINTELA */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_118 );  /* ULINTELA */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_119 );  /* ROLAESQ */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_120 );  /* ROLADIR */

#ifdef   MEDIT
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_121 );  /* SAISALVA */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_126 );  /* PROXARQUIVO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_127 );  /* ANTARQUIVO */
#endif

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_128 );  /* MUDATRANS */

#ifdef   MEDIT
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_129 );  /* JANSYSOUT */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_130 );  /* JANSYSERRO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_131 );  /* EXECUTA */
#endif

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_132 );  /* GRAVAMACRO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_133 );  /* EXEMACRO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_134 );  /* ARQMACRO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_135 );  /* REPITABUSCA */

#ifdef   MEDIT
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_136 );  /* PROXCARREGA */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_137 );  /* ANTCARREGA */
#endif

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_138 );  /* IMPRIMEARQ */

#ifdef   MEDIT
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_140 );  /* EDITAVER */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_141 );  /* TIRALISTA */
#endif

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_142 );  /* DESFAZ */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_143 );  /* REFAZ */

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_170 );  /* INDENTBLOCO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_171 );  /* LINHACIMA */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_172 );  /* LINHABAIXO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_173 );  /* LINHAMEIO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_174 );  /* APPBLCOPIA  */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_175 );  /* APPBLMOVE */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_176 );  /* BLOCOUPPER */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_177 );  /* BLOCOLOWER */

#ifdef   MEDIT
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_178 );  /* ORDEMENU */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_179 );  /* TROCAARQ */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_180 );  /* MARCAPOS */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_181 );  /* BUSCAPOS */
#endif

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_182 );  /* MARGEMPARA */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_183 );  /* TROCATRAS */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_184 );  /* PROCTRAS */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_185 );  /* REPITAFRENTE */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_186 );  /* REPITATRAS */

#ifdef   MEDIT
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_187 );  /* SAICONTEXTO */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_188 );  /* TECLAQUENTE */
#endif

   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_139 );  /* EXPRECHAR */
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_195 );  /* CODCHAR */

#ifdef   MEDIT
   comandos[ I++ ].ident = loc_str( STR_COMP,  COMP_196 );  /* RECARREGRA */
#endif

   /*** TECLAS ***/
   I = 0;
   
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_53  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_54  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_55  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_56  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_57  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_58  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_59  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_60  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_61  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_62  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_63  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_64  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_65  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_66  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_67  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_68  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_69  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_70  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_71  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_72  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_73  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_74  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_75  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_76  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_77  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_78  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_79  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_80  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_81  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_82  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_83  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_84  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_85  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_86  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_87  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_88  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_89  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_90  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_91  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_92  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_93  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_94  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_95  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_96  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_97  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_98  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_99  );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_100 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_101 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_102 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_103 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_104 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_105 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_106 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_107 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_108 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_109 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_110 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_111 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_112 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_113 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_122 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_123 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_124 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_125 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_144 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_145 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_146 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_147 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_148 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_149 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_150 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_151 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_152 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_153 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_154 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_155 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_156 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_157 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_158 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_159 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_160 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_161 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_162 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_163 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_164 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_165 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_166 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_167 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_168 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_169 );

#ifdef    XTOOLS

   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_192 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_193 );
   teclas[ I++ ].ident = loc_str( STR_COMP,  COMP_194 );

#endif

   for ( num_com = 0;   comandos[ num_com ].ident;   ++num_com )
      ;

   for ( num_tec = 0;   teclas[   num_tec ].ident;   ++num_tec )
      ;

   qsort((SC *) comandos, num_com, sizeof( COMANDOS ), comparac );
   qsort((SC *) teclas,   num_tec, sizeof( TECLAS ),   comparat );

   I = carga_coms( arquivo, mac_arq, comandos, num_com, teclas, num_tec );

#ifdef    XTOOLS

   I = carga_comxt( teclas, num_tec );

#endif

   down_str( STR_COMP );

   return ( I );
}

SI comparac( ptr1, ptr2 )

COMANDOS *ptr1,
         *ptr2;
{
   return strcmp( ptr1->ident, ptr2->ident );
}

SI comparat( ptr1, ptr2 )

TECLAS *ptr1,
       *ptr2;
{
   return strcmp( ptr1->ident, ptr2->ident );
}


VOID arq_macro( args )

ARG   *args;
{
   SS    tecla;
   SC    arq[ 80 ], *open_opt;
   FILE  *FA = NULL, *FB = NULL;

   /*** Limpa o resto da chamada do ultimo comando ***/
   if ( wr_macro != NULL )   et_grava_macro( args );

   if ( rd_macro != NULL )   mr_fim( );

   if ((FA = scrap_read( args, MACRO_TMP )) != NULL )
   {
      arq[ 0 ] = '\0';

pede_arquivo:

      tecla = pede_cadeia( loc_str( STR_EDIT, EDIT_18 ), arq, 60,
                           loc_str( STR_EDIT, EDIT_35 ), 'x', NULL );

      insert_mode( args, insercao );

      if ( tecla == K_ESC  ||  arq[ 0 ] == '\0' )   goto fim;

      open_opt = MODO_BWO;

      if ( access( arq, 0 ) == 0 )
      {
         tecla = decida( loc_str( STR_EDIT, EDIT_13 ),
                         loc_str( STR_EDIT, EDIT_22 ), "", 1, 40,
                         loc_str( STR_EDIT, EDIT_23 ), arq );

         if ( tecla < 0   )   goto  pede_arquivo;
         if ( tecla == 1  )   open_opt = MODO_BAO;
      }

      if ((FB = fopen( arq, open_opt )) == NULL )
      {
         errorexists = YES;
         mens_1( args->jm, loc_str( STR_EDIT, EDIT_506 ));
         goto fim;
      }

      while ((tecla = fgetc( FA )) != EOF )   fputc( tecla, FB );
   }

fim:

   if ( FA != NULL )   fclose( FA );
   if ( FB != NULL )   fclose( FB );
}

VOID et_grava_macro( args )

ARG   *args;
{
   SS  qc = 1;

   if ( wr_macro != NULL )
   {
      if ( vet_com[ ult_comando ].seg_tecla )   qc = 2;

      /*** Limpe os residuos da chamada de macro. ***/
      fseek( wr_macro, (SL) qc * -1L, 1 );
      while( qc-- )   fputc( 0, wr_macro );

      mw_fim( );
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_32 ));
   }

   else
   {
      if ((wr_macro = scrap_write( args, MACRO_TMP, 0 )) != NULL )
      {
         errorexists = YES;
         mens_1( args->jm, loc_str( STR_EDIT, EDIT_33 ));
      }
   }
}


VOID et_exec_macro( args )

ARG   *args;
{
   /*** Limpa o resto da chamada do ultimo comando ***/
   if ( wr_macro != NULL )   et_grava_macro( args );

   if ( rd_macro != NULL )   mr_fim( );

   if ((rd_macro = scrap_read( args, MACRO_TMP )) != NULL )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_34 ));
   }
}

VOID exec_macroarq( args, arq )

ARG   *args;
SC    *arq;
{
   /*** Limpa o resto da chamada do ultimo comando ***/
   if ( wr_macro != NULL )   et_grava_macro( args );

   if ( rd_macro != NULL )   mr_fim( );

   if ((rd_macro = fopen( arq, MODO_BRO )) == NULL )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_526 ));
   }
}



