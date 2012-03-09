
#ifdef comout

This module handles the printing of screens

function fwprscrn( win )            /* prints the supplied window,
                                       may print win_main for screeprint */

#endif

#include <stdio.h>
#include <window.h>
#ifdef MSDOS
#include <process.h>
#endif

#ifdef VAXVMS
#define  GR_BOX( x )  (((win->type == 1) ? pieces[(x)] : pieces[(x) + 12]) + (win->w_boxatr << 8))
#else
#define  GR_BOX( x )  (((win->type == 1) ? pieces[(x)] : pieces[(x) + 12]) \
                      + (win->w_boxatr << 8))
#endif

#define  GR_CH(  x )  ( pieces[(x)] + (win->w_boxatr << 8) )

#ifdef     PROTO_ANSI

ST( VOID ) sendlin( UC * );
ST( VOID ) send( US );

#else

ST( VOID ) sendlin( );
ST( VOID ) send( );

#endif

static FILE *fd;

VOID fwprscrn( win )
struct Window *win;
{
  SS x,y;
  register US *p;
  US val;
  SS box;
  UC pieces[ 127 ];
#ifdef MSDOS
  SC dos_tmp[30];
#endif

  /* verify the window */
  if( win_stak[ win->w_num ] != win )
        if( win != main_win ) return;
  if( win->w_flags & W_BOXED ) box = 1; else box = 0;
  if( win == main_win ) box = 0;

#ifdef VAXVMS
  sp_rp = 0;
#endif

  /* open the file/pipe */
  if( sp_rp == '0' ) /* redirect */
        { fd = fopen( sp_dest, MODO_WO );
          if( ! fd ) return;
        }
#ifdef MSDOS
  else  { sprintf( dos_tmp,"PR00.TMP" );
          fd = fopen( dos_tmp, MODO_WO );
          if( ! fd ) return;
        }
#else
#ifdef  VAXVMS
  /*** NADA ***/
#else
  else  { fd = popen( sp_dest,MODO_WO );
          if( ! fd ) return;
        }
#endif
#endif

   /* establish graphic values */

   /*** Assuma que nao tem grafico ***/

   for ( x = 0;  x < 127;  x++ )    pieces[ x ] = ' ';

   pieces[0]   = ' ';
   pieces[1]   = ' ';
   pieces[2]   = ' ';
   pieces[3]   = ' ';
   pieces[4]   = ' ';
   pieces[5]   = ' ';
   pieces[6]   = ' ';
   pieces[7]   = ' ';
   pieces[8]   = ' ';
   pieces[9]   = ' ';
   pieces[10]  = ' ';
   pieces[11]  = ' ';
   pieces[12]  = ' ';
   pieces[13]  = ' ';
   pieces[14]  = ' ';
   pieces[15]  = ' ';
   pieces[16]  = ' ';
   pieces[17]  = ' ';
   pieces[18]  = ' ';
   pieces[19]  = ' ';
   pieces[20]  = ' ';
   pieces[21]  = ' ';
   pieces[22]  = ' ';
   pieces[23]  = ' ';
   pieces[24]  = ' ';
   pieces[25]  = ' ';
   pieces[26]  = ' ';
   pieces[27]  = ' ';
   pieces[28]  = ' ';
   pieces[29]  = ' ';
   pieces[30]  = ' ';
   pieces[31]  = ' ';
   pieces[32]  = ' ';
   pieces[33]  = ' ';
   pieces[34]  = ' ';
   pieces[35]  = ' ';
   pieces[36]  = ' ';
   pieces[37]  = ' ';
   pieces[38]  = ' ';
   pieces[39]  = ' ';
   pieces[40]  = ' ';
   pieces[41]  = ' ';
   pieces[42]  = ' ';
   pieces[43]  = ' ';
   pieces[44]  = ' ';
   pieces[45]  = ' ';
   pieces[46]  = ' ';
   pieces[47]  = ' ';
   pieces[48]  = ' ';
   pieces[49]  = ' ';
   pieces[50]  = ' ';
   pieces[51]  = ' ';
   pieces[52]  = ' ';
   pieces[53]  = ' ';
   pieces[54]  = ' ';
   pieces[55]  = ' ';
   pieces[56]  = ' ';
   pieces[57]  = ' ';
   pieces[58]  = ' ';
   pieces[59]  = ' ';
   pieces[60]  = ' ';
   pieces[61]  = ' ';
   pieces[62]  = ' ';
   pieces[63]  = 'A';
   pieces[64]  = 'A';
   pieces[65]  = 'A';
   pieces[66]  = 'A';
   pieces[67]  = ' ';
   pieces[68]  = ' ';
   pieces[69]  = ' ';
   pieces[70]  = 'C';
   pieces[71]  = ' ';
   pieces[72]  = 'E';
   pieces[73]  = 'E';
   pieces[74]  = ' ';
   pieces[75]  = ' ';
   pieces[76]  = 'I';
   pieces[77]  = ' ';
   pieces[78]  = ' ';
   pieces[79]  = ' ';
   pieces[80]  = 'N';
   pieces[81]  = ' ';
   pieces[82]  = 'O';
   pieces[83]  = 'O';
   pieces[84]  = 'O';
   pieces[85]  = ' ';
   pieces[86]  = ' ';
   pieces[87]  = ' ';
   pieces[88]  = ' ';
   pieces[89]  = 'U';
   pieces[90]  = ' ';
   pieces[91]  = ' ';
   pieces[92]  = ' ';
   pieces[93]  = ' ';
   pieces[94]  = ' ';
   pieces[95]  = 'a';
   pieces[96]  = 'a';
   pieces[97]  = 'a';
   pieces[98]  = 'a';
   pieces[99]  = ' ';
   pieces[100] = ' ';
   pieces[101] = ' ';
   pieces[102] = 'c';
   pieces[103] = ' ';
   pieces[104] = 'e';
   pieces[105] = 'e';
   pieces[106] = ' ';
   pieces[107] = ' ';
   pieces[108] = 'i';
   pieces[109] = ' ';
   pieces[110] = ' ';
   pieces[111] = ' ';
   pieces[112] = 'n';
   pieces[113] = ' ';
   pieces[114] = 'o';
   pieces[115] = 'o';
   pieces[116] = 'o';
   pieces[117] = ' ';
   pieces[118] = ' ';
   pieces[119] = ' ';
   pieces[120] = ' ';
   pieces[121] = 'u';

   switch( sp_graph )

   {
   case '0':   /*** Padrao igual ao TERMINAL ***/

               pieces[0]   = UG_VER;
               pieces[1]   = UG_HOR;
               pieces[2]   = UG_UL;
               pieces[3]   = UG_UR;
               pieces[4]   = UG_LR;
               pieces[5]   = UG_LL;

               pieces[6]   = UG_TU;
               pieces[7]   = UG_TR;
               pieces[8]   = UG_TD;
               pieces[9]   = UG_TL;
               pieces[10]  = UG_JUN;
               pieces[11]  = UG_DOT;
               pieces[12]  = UG_VER2;
               pieces[13]  = UG_HOR2;
               pieces[14]  = UG_UL2;
               pieces[15]  = UG_UR2;
               pieces[16]  = UG_LR2;
               pieces[17]  = UG_LL2;
               pieces[18]  = UG_TU2;
               pieces[19]  = UG_TR2;
               pieces[20]  = UG_TD2;
               pieces[21]  = UG_TL2;
               pieces[22]  = UG_JUN2;
               pieces[23]  = UG_SET_D;
               pieces[24]  = UG_SET_E;
               pieces[25]  = UG_SET_C;
               pieces[26]  = UG_SET_B;

               pieces[31]  = UG_SP;
               pieces[32]  = UG_EXCLA_INV;
               pieces[55]  = UG_INTER_INV;
               pieces[63]  = UG_GRA_A;
               pieces[64]  = UG_AGU_A;
               pieces[65]  = UG_CIR_A;
               pieces[66]  = UG_TIL_A;
               pieces[70]  = UG_CED_C;
               pieces[72]  = UG_AGU_E;
               pieces[73]  = UG_CIR_E;
               pieces[76]  = UG_AGU_I;
               pieces[80]  = UG_TIL_N;
               pieces[82]  = UG_AGU_O;
               pieces[83]  = UG_CIR_O;
               pieces[84]  = UG_TIL_O;
               pieces[89]  = UG_AGU_U;
               pieces[95]  = UG_GRA_a;
               pieces[96]  = UG_AGU_a;
               pieces[97]  = UG_CIR_a;
               pieces[98]  = UG_TIL_a;
               pieces[102] = UG_CED_c;
               pieces[104] = UG_AGU_e;
               pieces[105] = UG_CIR_e;
               pieces[108] = UG_AGU_i;
               pieces[112] = UG_TIL_n;
               pieces[114] = UG_AGU_o;
               pieces[115] = UG_CIR_o;
               pieces[116] = UG_TIL_o;
               pieces[121] = UG_AGU_u;

               break;

   case '1':   /*** Padrao PC ***/

               pieces[0]   = 179;
               pieces[1]   = 196;
               pieces[2]   = 218;
               pieces[3]   = 191;
               pieces[4]   = 217;
               pieces[5]   = 192;

               pieces[6]   = 193;
               pieces[7]   = 195;
               pieces[8]   = 194;
               pieces[9]   = 180;
               pieces[10]  = 197;
               pieces[11]  = 219;
               pieces[12]  = 186;
               pieces[13]  = 205;
               pieces[14]  = 201;
               pieces[15]  = 187;
               pieces[16]  = 188;
               pieces[17]  = 200;
               pieces[18]  = 202;
               pieces[19]  = 204;
               pieces[20]  = 203;
               pieces[21]  = 185;
               pieces[22]  = 206;
               pieces[23]  = 196;
               pieces[24]  = 196;
               pieces[25]  = 179;
               pieces[26]  = 179;

               pieces[31]  = 250;
               pieces[32]  = 32;
               pieces[55]  = 32;
               pieces[63]  = 'A';
               pieces[64]  = 'A';
               pieces[65]  = 'A';
               pieces[66]  = 'A';
               pieces[70]  = 128;
               pieces[72]  = 144;
               pieces[73]  = 'E';
               pieces[76]  = 'I';
               pieces[80]  = 165;
               pieces[82]  = 162;
               pieces[83]  = 'O';
               pieces[84]  = 'O';
               pieces[89]  = 'U';
               pieces[95]  = 133;
               pieces[96]  = 160;
               pieces[97]  = 131;
               pieces[98]  = 134;
               pieces[102] = 135;
               pieces[104] = 130;
               pieces[105] = 136;
               pieces[108] = 161;
               pieces[112] = 164;
               pieces[114] = 162;
               pieces[115] = 147;
               pieces[116] = 'o';
               pieces[121] = 163;

               break;

   case '2':   /*** SEM Semi Graficos porem com '+' ***/

               for( x = 2;  x < 6;  x++ )
               {
                  pieces[ x ]      = '+';
                  pieces[ x + 12 ] = '+';
               }

               pieces[ 0 ]  = '|';
               pieces[ 1 ]  = '-';
               pieces[ 12 ] = '|';
               pieces[ 13 ] = '-';

               break;

   default:    /*** Assume branco ' ' que ja esta setado ***/

               break;
   }

   /* if atributes, send size */

   if( sp_atrib == '1' )
   {
      fputc( win->w_alen + (( box ) ? 2 : 0),fd );
      fputc( win->w_aheight + (( box ) ? 2 : 0),fd );
   }

   /*** output the window ***/
   /*** Para colocar TITULO e RODAPE veja algoritmos em winmain.c ***/

   if( box )  /* top box line */
   {
      val = GR_BOX( 2 );

      send( val );

      for( x = 0;  x < win->w_alen;  x++ )
      {
         val = GR_BOX( 1 );
         send( val );
      }

      val = GR_BOX( 3 );
      send( val );
      sendlin( sp_eol );
   }

   /* window body */
   for( y = 0;  y < win->w_aheight;  y++ )
   {
      p = win->w_ary + ( ( y + win->w_ay ) * win->w_len ) + win->w_ax;

      if( box )
      {
         val = GR_BOX( 0 );
         send( val );
      }

      for( x = 0;  x < win->w_alen;  x++, p++ )
      {
         val = *p & 255;

         if( val >= UG_VER )
         {
            /***  para que isto ?
            val   = pieces[ val - UG_VER ];
            val2  = *p / 256;
            val2 *= 256;
            val2 += val;
            ***/

            val   = GR_CH( val - UG_VER );

            send( val );

            continue;
         }

         send( *p );
      }

      if( box )
      {
         val = GR_BOX( 0 );
         send( val );
      }

      sendlin( sp_eol );
   }

   if( box )  /* bottom box line */
   {
      val = GR_BOX( 5 );

      send( val );

      for( x = 0;  x < win->w_alen;  x++ )
      {
         val = GR_BOX( 1 );
         send( val );
      }

      val = GR_BOX( 4 );

      send( val );
      sendlin( sp_eol );
   }

   sendlin( sp_eop );

   if( sp_rp == '0' )   fclose( fd );

#ifdef MSDOS
   else {

      fclose( fd );
      if( spawnlpe( P_WAIT,sp_dest,dos_tmp,NULL ))  return;
      unlink( dos_tmp );
   }
#else
#ifdef   VAXVMS
  /*** NADA ***/
#else
   else  pclose( fd );
#endif
#endif
}

ST( VOID ) sendlin( p )                 /* send a string */
UC *p;
{
  for( ;*p;p++ ) send( *p );
}

ST( VOID ) send( x )                    /* output a value to fd */
US x;
{ US y;
  if( sp_atrib == '1' )
        { y = x / 256;
          fputc( y,fd );
        }
  y = x & 255;
  fputc( y,fd );
}

