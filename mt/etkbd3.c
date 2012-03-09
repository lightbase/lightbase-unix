
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  "window.h"
#include  "ethead.h"


SS setdefaults()
{
   SS  I;

   for ( I = 0;   I < COM_TOTAL;   I++ )
   {
      vet_com[ I ].prim_tecla  = 0;
      vet_com[ I ].seg_tecla   = 0;
      vet_com[ I ].num_comando = 0;
      vet_com[ I ].escape      = 0;
   }

   I = 0;

   vet_com[ I ].num_comando = VOLTAESPACO;
   vet_com[ I ].prim_tecla  = K_BS;
   I++;

   vet_com[ I ].num_comando = RETORNO;
   vet_com[ I ].prim_tecla  = K_ENTER;
   I++;

   vet_com[ I ].num_comando = SOBE;
   vet_com[ I ].prim_tecla  = K_UP;
   I++;

   vet_com[ I ].num_comando = SOBE;
   vet_com[ I ].prim_tecla  = CTRLE;
   I++;

   vet_com[ I ].num_comando = DESCE;
   vet_com[ I ].prim_tecla  = K_DN;
   I++;

   vet_com[ I ].num_comando = DESCE;
   vet_com[ I ].prim_tecla  = CTRLX;
   I++;

   vet_com[ I ].num_comando = ESQUERDA;
   vet_com[ I ].prim_tecla  = K_LFT;
   I++;

   vet_com[ I ].num_comando = ESQUERDA;
   vet_com[ I ].prim_tecla  = CTRLS;
   I++;

   vet_com[ I ].num_comando = DIREITA;
   vet_com[ I ].prim_tecla  = K_RT;
   I++;

   vet_com[ I ].num_comando = DIREITA;
   vet_com[ I ].prim_tecla  = CTRLD;
   I++;

   vet_com[ I ].num_comando = INILINHA;
   vet_com[ I ].prim_tecla  = K_HOME;
   I++;

   vet_com[ I ].num_comando = INILINHA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'S';
   I++;

   vet_com[ I ].num_comando = INILINHA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 's';
   I++;

   vet_com[ I ].num_comando = INILINHA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLS;
   I++;

   vet_com[ I ].num_comando = PLINTELA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'E';
   I++;

   vet_com[ I ].num_comando = PLINTELA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'e';
   I++;

   vet_com[ I ].num_comando = PLINTELA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLE;
   I++;

   vet_com[ I ].num_comando = ULINTELA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'X';
   I++;

   vet_com[ I ].num_comando = ULINTELA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'x';
   I++;

   vet_com[ I ].num_comando = ULINTELA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLX;
   I++;

   vet_com[ I ].num_comando = ROLAESQ;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'T';
   I++;

   vet_com[ I ].num_comando = ROLAESQ;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 't';
   I++;

   vet_com[ I ].num_comando = ROLAESQ;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLT;
   I++;

   vet_com[ I ].num_comando = ROLADIR;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'Q';
   I++;

   vet_com[ I ].num_comando = ROLADIR;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'q';
   I++;

   vet_com[ I ].num_comando = ROLADIR;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLQ;
   I++;

   vet_com[ I ].num_comando = FIMLINHA;
   vet_com[ I ].prim_tecla  = K_END;
   I++;

   vet_com[ I ].num_comando = FIMLINHA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'D';
   I++;

   vet_com[ I ].num_comando = FIMLINHA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'd';
   I++;

   vet_com[ I ].num_comando = FIMLINHA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLD;
   I++;

   vet_com[ I ].num_comando = ROLACIMA;
   vet_com[ I ].prim_tecla  = CTRLZ;
   I++;

   vet_com[ I ].num_comando = ROLABAIXO;
   vet_com[ I ].prim_tecla  = CTRLW;
   I++;

   vet_com[ I ].num_comando = PROPAGINA;
   vet_com[ I ].prim_tecla  = K_PGDN;
   I++;

   vet_com[ I ].num_comando = PROPAGINA;
   vet_com[ I ].prim_tecla  = CTRLC;
   I++;

   vet_com[ I ].num_comando = ANTPAGINA;
   vet_com[ I ].prim_tecla  = K_PGUP;
   I++;

   vet_com[ I ].num_comando = ANTPAGINA;
   vet_com[ I ].prim_tecla  = CTRLR;
   I++;

   vet_com[ I ].num_comando = INIARQUIVO;
   vet_com[ I ].prim_tecla  = K_CPGUP;
   I++;

   vet_com[ I ].num_comando = INIARQUIVO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'R';
   I++;

   vet_com[ I ].num_comando = INIARQUIVO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'r';
   I++;

   vet_com[ I ].num_comando = INIARQUIVO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLR;
   I++;

   vet_com[ I ].num_comando = FIMARQUIVO;
   vet_com[ I ].prim_tecla  = K_CPGDN;
   I++;

   vet_com[ I ].num_comando = FIMARQUIVO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'C';
   I++;

   vet_com[ I ].num_comando = FIMARQUIVO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'c';
   I++;

   vet_com[ I ].num_comando = FIMARQUIVO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLC;
   I++;

   vet_com[ I ].num_comando = PROPALAVRA;
   vet_com[ I ].prim_tecla  = K_WORDRIGHT;
   I++;

   vet_com[ I ].num_comando = PROPALAVRA;
   vet_com[ I ].prim_tecla  = CTRLF;
   I++;

   vet_com[ I ].num_comando = ANTPALAVRA;
   vet_com[ I ].prim_tecla  = K_WORDLEFT;
   I++;

   vet_com[ I ].num_comando = ANTPALAVRA;
   vet_com[ I ].prim_tecla  = CTRLA;
   I++;

   vet_com[ I ].num_comando = PROXTAB;
   vet_com[ I ].prim_tecla  = K_TAB;
   I++;

   vet_com[ I ].num_comando = TABANT;
   vet_com[ I ].prim_tecla  = K_BTAB;
   I++;

   vet_com[ I ].num_comando = TROCATEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'A';
   I++;

   vet_com[ I ].num_comando = TROCATEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'a';
   I++;

   vet_com[ I ].num_comando = TROCATEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLA;
   I++;

   vet_com[ I ].num_comando = TROCATEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = K_WORDLEFT;
   I++;

   vet_com[ I ].num_comando = DUPLINHA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = '=';
   I++;

   vet_com[ I ].num_comando = PROCTEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'F';
   I++;

   vet_com[ I ].num_comando = PROCTEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'f';
   I++;

   vet_com[ I ].num_comando = PROCTEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLF;
   I++;

   vet_com[ I ].num_comando = PROCTEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = K_WORDRIGHT;
   I++;

   vet_com[ I ].num_comando = INSLIDEPO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'J';
   I++;

   vet_com[ I ].num_comando = INSLIDEPO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'j';
   I++;

   vet_com[ I ].num_comando = INSLIDEPO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLJ;
   I++;

   vet_com[ I ].num_comando = INSLIANTES;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'K';
   I++;

   vet_com[ I ].num_comando = INSLIANTES;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'k';
   I++;

   vet_com[ I ].num_comando = INSLIANTES;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLK;
   I++;

   vet_com[ I ].num_comando = JUNTAPROXL;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'N';
   I++;

   vet_com[ I ].num_comando = JUNTAPROXL;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'n';
   I++;

   vet_com[ I ].num_comando = JUNTAPROXL;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLN;
   I++;

   vet_com[ I ].num_comando = TAMTEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'M';
   I++;

   vet_com[ I ].num_comando = TAMTEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'm';
   I++;

   vet_com[ I ].num_comando = TAMTEXTO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLM;
   I++;

   vet_com[ I ].num_comando = VAIPRALIN;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'L';
   I++;

   vet_com[ I ].num_comando = VAIPRALIN;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'l';
   I++;

   vet_com[ I ].num_comando = VAIPRALIN;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLL;
   I++;

   vet_com[ I ].num_comando = APAGAFIMLIN;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'Y';
   I++;

   vet_com[ I ].num_comando = APAGAFIMLIN;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'y';
   I++;

   vet_com[ I ].num_comando = APAGAFIMLIN;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLY;
   I++;

   vet_com[ I ].num_comando = MUDATRANS;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'T';
   I++;

   vet_com[ I ].num_comando = MUDATRANS;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 't';
   I++;

   vet_com[ I ].num_comando = MUDATRANS;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLT;
   I++;

   vet_com[ I ].num_comando = INIBLCHAR;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'B';
   I++;

   vet_com[ I ].num_comando = INIBLCHAR;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'b';
   I++;

   vet_com[ I ].num_comando = INIBLCHAR;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLB;
   I++;

   vet_com[ I ].num_comando = INIBLCOL;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'N';
   I++;

   vet_com[ I ].num_comando = INIBLCOL;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'n';
   I++;

   vet_com[ I ].num_comando = INIBLCOL;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLN;
   I++;

   vet_com[ I ].num_comando = INIBLLIN;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'L';
   I++;

   vet_com[ I ].num_comando = INIBLLIN;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'l';
   I++;

   vet_com[ I ].num_comando = INIBLLIN;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLL;
   I++;

   vet_com[ I ].num_comando = BLOCOCOPIA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'K';
   I++;

   vet_com[ I ].num_comando = BLOCOCOPIA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'k';
   I++;

   vet_com[ I ].num_comando = BLOCOCOPIA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLK;
   I++;

   vet_com[ I ].num_comando = BLOCOINSERE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'C';
   I++;

   vet_com[ I ].num_comando = BLOCOINSERE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'c';
   I++;

   vet_com[ I ].num_comando = BLOCOINSERE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLC;
   I++;

   vet_com[ I ].num_comando = BLOCOMOVE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'V';
   I++;

   vet_com[ I ].num_comando = BLOCOMOVE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'v';
   I++;

   vet_com[ I ].num_comando = BLOCOMOVE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLV;
   I++;

   vet_com[ I ].num_comando = DESMARCABLO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'H';
   I++;

   vet_com[ I ].num_comando = DESMARCABLO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'h';
   I++;

   vet_com[ I ].num_comando = DESMARCABLO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLH;
   I++;

   vet_com[ I ].num_comando = APAGABLOCO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'Y';
   I++;

   vet_com[ I ].num_comando = APAGABLOCO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'y';
   I++;

   vet_com[ I ].num_comando = APAGABLOCO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLY;
   I++;

   vet_com[ I ].num_comando = EXPORTE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'W';
   I++;

   vet_com[ I ].num_comando = EXPORTE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'w';
   I++;

   vet_com[ I ].num_comando = EXPORTE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLW;
   I++;

   vet_com[ I ].num_comando = IMPORTE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'R';
   I++;

   vet_com[ I ].num_comando = IMPORTE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'r';
   I++;

   vet_com[ I ].num_comando = IMPORTE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLR;
   I++;

   vet_com[ I ].num_comando = QUEBRALINHA;
   vet_com[ I ].prim_tecla  = CTRLN;
   I++;

   vet_com[ I ].num_comando = APAGALINHA;
   vet_com[ I ].prim_tecla  = CTRLY;
   I++;

   vet_com[ I ].num_comando = APAGACAR;
   vet_com[ I ].prim_tecla  = K_DEL;
   I++;

   vet_com[ I ].num_comando = APAGACAR;
   vet_com[ I ].prim_tecla  = CTRLG;
   I++;

   vet_com[ I ].num_comando = APAGAPAL;
   vet_com[ I ].prim_tecla  = CTRLT;
   I++;

   vet_com[ I ].num_comando = INSCTRLE;
   vet_com[ I ].prim_tecla  = CTRLP;
   I++;

   vet_com[ I ].num_comando = PROCPAR;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = '[';
   I++;

   vet_com[ I ].num_comando = PROCPAR;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = ']';
   I++;

#ifdef   MEDIT

   vet_com[ I ].num_comando = SAICONTEXTO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = '!';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = ZOOM;
   vet_com[ I ].prim_tecla  = K_F5;
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = BUSCAPOS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = '.';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = MARCAPOS;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = '.';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = TROCAARQ;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'F';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = TROCAARQ;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'f';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = ORDEMENU;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'O';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = ORDEMENU;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'o';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = MUDAJANELA;
   vet_com[ I ].prim_tecla  = K_F4;
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = DIVIDEJANELA;
   vet_com[ I ].prim_tecla  = K_F7;
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = EXPJANELA;
   vet_com[ I ].prim_tecla  = K_F8;
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = MENUARQS;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'M';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = MENUARQS;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'm';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = PROXARQUIVO;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = '+';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = ANTARQUIVO;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = '-';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = PROXCARREGA;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = '>';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = ANTCARREGA;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = '<';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = NOVOARQUIVO;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'E';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = NOVOARQUIVO;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'e';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = EDITANOTAS;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'N';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = EDITANOTAS;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'n';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = VERARQUIVO;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'V';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = VERARQUIVO;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = 'v';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = EDITAVER;
   vet_com[ I ].prim_tecla  = K_F6;
   vet_com[ I ].seg_tecla   = '!';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = TIRALISTA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = '-';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = TERMINA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'X';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = TERMINA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'x';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = TERMINA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLX;
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = SAISALVA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'D';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = SAISALVA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'd';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = SAISALVA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLD;
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = COMSYSTEM;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = 'S';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = COMSYSTEM;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = 's';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = JANSYSOUT;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = '1';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = JANSYSERRO;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = '2';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = COMPILA;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = 'C';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = COMPILA;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = 'c';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = RECARREGRA;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = 'R';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = RECARREGRA;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = 'r';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = JANERRO;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = 'E';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = JANERRO;
   vet_com[ I ].prim_tecla  = K_F9;
   vet_com[ I ].seg_tecla   = 'e';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = SALVARQ;
   vet_com[ I ].prim_tecla  = K_F2;
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = SALVARQ;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'S';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = SALVARQ;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 's';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = SALVARQ;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLS;
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = ABANDONA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'Q';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = ABANDONA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'q';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = ABANDONA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLQ;
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = EXECUTA;
   vet_com[ I ].prim_tecla  = F9;
   vet_com[ I ].seg_tecla   = 'X';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = EXECUTA;
   vet_com[ I ].prim_tecla  = F9;
   vet_com[ I ].seg_tecla   = 'x';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = IMPRIMEARQ;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'P';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = IMPRIMEARQ;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'p';
   vet_com[ I ].escape      = 1;
   I++;

   vet_com[ I ].num_comando = IMPRIMEARQ;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLP;
   vet_com[ I ].escape      = 1;
   I++;

#else

   vet_com[ I ].num_comando = ZOOM;
   vet_com[ I ].prim_tecla  = K_F5;
   I++;

   vet_com[ I ].num_comando = IMPRIMEARQ;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'P';
   I++;

   vet_com[ I ].num_comando = IMPRIMEARQ;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'p';
   I++;

   vet_com[ I ].num_comando = IMPRIMEARQ;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLP;
   I++;

   vet_com[ I ].num_comando = SALVARQ;
   vet_com[ I ].prim_tecla  = K_F2;
   I++;

   vet_com[ I ].num_comando = ABANDONA;
   vet_com[ I ].prim_tecla  = K_ESC;
   I++;

#endif

   vet_com[ I ].num_comando = INSEREOUNAO;
   vet_com[ I ].prim_tecla  = K_INS;
   I++;

   vet_com[ I ].num_comando = INSEREOUNAO;
   vet_com[ I ].prim_tecla  = CTRLV;
   I++;

   vet_com[ I ].num_comando = GRAVAMACRO;
   vet_com[ I ].prim_tecla  = F9;
   vet_com[ I ].seg_tecla   = 'G';
   I++;

   vet_com[ I ].num_comando = GRAVAMACRO;
   vet_com[ I ].prim_tecla  = F9;
   vet_com[ I ].seg_tecla   = 'g';
   I++;

   vet_com[ I ].num_comando = EXEMACRO;
   vet_com[ I ].prim_tecla  = F9;
   vet_com[ I ].seg_tecla   = 'M';
   I++;

   vet_com[ I ].num_comando = EXEMACRO;
   vet_com[ I ].prim_tecla  = F9;
   vet_com[ I ].seg_tecla   = 'm';
   I++;

   vet_com[ I ].num_comando = ARQMACRO;
   vet_com[ I ].prim_tecla  = F9;
   vet_com[ I ].seg_tecla   = 'A';
   I++;

   vet_com[ I ].num_comando = ARQMACRO;
   vet_com[ I ].prim_tecla  = F9;
   vet_com[ I ].seg_tecla   = 'a';
   I++;

   vet_com[ I ].num_comando = REPITABUSCA;
   vet_com[ I ].prim_tecla  = CTRLL;
   I++;

   vet_com[ I ].num_comando = DESFAZ;
   vet_com[ I ].prim_tecla  = CTRLU;
   I++;

   vet_com[ I ].num_comando = REFAZ;
   vet_com[ I ].prim_tecla  = CTRLB;
   I++;

   vet_com[ I ].num_comando = LINHACIMA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'W';
   I++;

   vet_com[ I ].num_comando = LINHACIMA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'w';
   I++;

   vet_com[ I ].num_comando = LINHACIMA;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLW;
   I++;

   vet_com[ I ].num_comando = LINHABAIXO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'Z';
   I++;

   vet_com[ I ].num_comando = LINHABAIXO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'z';
   I++;

   vet_com[ I ].num_comando = LINHABAIXO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLZ;
   I++;

   vet_com[ I ].num_comando = LINHAMEIO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'G';
   I++;

   vet_com[ I ].num_comando = LINHAMEIO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'g';
   I++;

   vet_com[ I ].num_comando = LINHAMEIO;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLG;
   I++;

   vet_com[ I ].num_comando = APPBLCOPIA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'A';
   I++;

   vet_com[ I ].num_comando = APPBLCOPIA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'a';
   I++;

   vet_com[ I ].num_comando = APPBLCOPIA;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLA;
   I++;

   vet_com[ I ].num_comando = APPBLMOVE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'E';
   I++;

   vet_com[ I ].num_comando = APPBLMOVE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'e';
   I++;

   vet_com[ I ].num_comando = APPBLMOVE;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLE;
   I++;

   vet_com[ I ].num_comando = BLOCOUPPER;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'M';
   I++;

   vet_com[ I ].num_comando = BLOCOUPPER;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'm';
   I++;

   vet_com[ I ].num_comando = BLOCOUPPER;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLM;
   I++;

   vet_com[ I ].num_comando = BLOCOLOWER;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'O';
   I++;

   vet_com[ I ].num_comando = BLOCOLOWER;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'o';
   I++;

   vet_com[ I ].num_comando = BLOCOLOWER;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLO;
   I++;

   vet_com[ I ].num_comando = INDENTBLOCO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'I';
   I++;

   vet_com[ I ].num_comando = INDENTBLOCO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = 'i';
   I++;

   vet_com[ I ].num_comando = INDENTBLOCO;
   vet_com[ I ].prim_tecla  = CTRLK;
   vet_com[ I ].seg_tecla   = CTRLI;
   I++;

   vet_com[ I ].num_comando = MARGEMPARA;
   vet_com[ I ].prim_tecla  = CTRLO;
   vet_com[ I ].seg_tecla   = 'M';
   I++;

   vet_com[ I ].num_comando = MARGEMPARA;
   vet_com[ I ].prim_tecla  = CTRLO;
   vet_com[ I ].seg_tecla   = 'm';
   I++;

   vet_com[ I ].num_comando = MARGEMPARA;
   vet_com[ I ].prim_tecla  = CTRLO;
   vet_com[ I ].seg_tecla   = CTRLM;
   I++;

   vet_com[ I ].num_comando = TROCATRAS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'V';
   I++;

   vet_com[ I ].num_comando = TROCATRAS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'v';
   I++;

   vet_com[ I ].num_comando = TROCATRAS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLV;
   I++;

   vet_com[ I ].num_comando = PROCTRAS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'B';
   I++;

   vet_com[ I ].num_comando = PROCTRAS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'b';
   I++;

   vet_com[ I ].num_comando = PROCTRAS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLB;
   I++;

   vet_com[ I ].num_comando = REPITAFRENTE;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'P';
   I++;

   vet_com[ I ].num_comando = REPITAFRENTE;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'p';
   I++;

   vet_com[ I ].num_comando = REPITAFRENTE;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLP;
   I++;

   vet_com[ I ].num_comando = REPITATRAS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'O';
   I++;

   vet_com[ I ].num_comando = REPITATRAS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = 'o';
   I++;

   vet_com[ I ].num_comando = REPITATRAS;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = CTRLO;
   I++;

   vet_com[ I ].num_comando = EXPRECHAR;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = '#';
   I++;

   vet_com[ I ].num_comando = CODCHAR;
   vet_com[ I ].prim_tecla  = CTRLQ;
   vet_com[ I ].seg_tecla   = '?';
   I++;

   return I;
}

