#include  "lb2all.h"

#include <ctype.h>
#include <string.h>


#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2prot1.h"

#include  "lb2maces.h"

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif


#define   DEFINE     'D'
#define   CARREGA    'C'
#define   SELECAO    'S'
#define   FINALIZA   'F'

#ifdef MSDOS
   COUNT bissexto(TEXT *);
#else
   COUNT bissexto( );
#endif


/**--------------------------------------------------------------bissexto
*   nome            bissexto  --  verifica se e' um ano bissexto
*   parametros      c = valdate(str);
*                   COUNT c;        -1 - ano nao bissexto
*                                    0 - ano bissexto
*                   TEXT str[];   data no formato "dd/mm/aa" ou "dd/mm/aaaa"
**/
F__LOC   COUNT bissexto(str)
TEXT *str;
{
    SI aa,bi;
    bi=0;
    aa=0;

    aa = atoi(&str[0]);

    if(aa<100)
		  aa += 1900; /* se ano = 82, 082, ou 0082 assume 1982  */
    if(aa<1000)
		  aa += 2000; /* se ano 182, assume 2182 */
    if(aa == (4*(aa/4)))
		  bi = -1;
    if(aa == (100*(aa/100)) )
        bi = 0;
    if(aa == (400*(aa/400)) )
		  bi = -1;
    return((COUNT)bi);
    }

/**--------------------------------------------------------------valdata
*   nome            valdata  --  verifica se e' uma data valida
*   parametros      c = valdate(str);
*                   COUNT c;        -1 - data invalida
*                                    0 - data valida
*                   TEXT str[];   data no formato "dd/mm/aa" ou "dd/mm/aaaa"
**/
F__GLB   COUNT valdata(str)
TEXT  *str;
{
	 COUNT aa,mm,dd,n;
	 TEXT straa[5];

    aa=mm=dd=0;
    n=0;

	 if( str[0] == '\0' || strcmp("  /  /  ",str) == 0 ) /* data em branco e valido */
        return((COUNT)0);

    if (strlen(str) < 8 )
		 return((COUNT)-1);

    while(str[n]) {
        if( n==2 || n==5 ) {
            if( str[n] != '/' ) {
					 return((COUNT)-1); /* Separador invalido */
            }
        }
        else
				if( !isdigit(str[n]) ) {
					 return((COUNT)-1); /* caracter invalido neste local */
            }
        n++;
    }

    dd = atoi(&str[0]);
    mm = atoi(&str[3]);
    aa = atoi(&str[6]);

    if(aa<100)
        aa += 1900; /* se ano = 82, 082, ou 0082 assume 1982  */
    if(aa<1000)
        aa += 2000; /* se ano e' 182, assume 2182 */
    if(mm<1 || mm>12) {
		  return((COUNT)-1); /* mes invalido */
    }
    if(dd < 1 || dd > 31) {
		  return((COUNT)-1); /* dia invalido */
    }
    if( dd > 30 && (mm == 9 || mm == 4 || mm == 6 || mm == 11) ) {
		  return((COUNT)-1); /* dia invalido */
    }
    sprintf(straa,"%d",aa);
    if(mm == 2 && dd > 28) {
		  if( ! bissexto(straa) ) {
				return((COUNT)-1);      /* dia invalido */
        }
        else
            if(dd > 29) {
					 return((COUNT)-1);  /* dia invalido */
            }
    }
    return((COUNT)0);  /* e' uma data valida */
}



/**--------------------------------------------------------------valhora
*   nome            valhora  --  verifica se e' uma hora valida
*   parametros      c = valhora(str);
*                   COUNT c;        -1 - hora invalida
*                                    0 - hora valida
*                   TEXT str[];   hora no formato "hh:mm"
**/
F__GLB   COUNT valhora(str)
TEXT  *str;
{
	 COUNT hh,mm;
	 hh=mm=0;
	 if(!strcmp("  :  ",str)) /* hora em branco e valida */
        return(0);
	 if (str[2] != ':') {
		 return(-1); /* Separador invalido */
    }

    hh = atoi(&str[0]);
    mm = atoi(&str[3]);

    if( hh < 00 || hh > 24 ) {
		  return(-1);
    }
    if( mm < 00 || mm > 59 ) {
		  return(-1);
    }
    return(0);  /* e' uma hora valida */
}

/**--------------------------------------------------------------valdigito
*   nome            valdigito  --  verificadigito modulo 11
*   parametros      c = valdigito(str);
*                   COUNT c;        -1 - digito invalido
*                                    0 - digito valido
*                   TEXT str[];   numero, sendo o ultimo algarismo o digito
**/
F__GLB   COUNT valdigito(str)
TEXT  *str;
{
	 SI i,n,tam,dig,resto,d;
	 LONG soma;

   n = 1;
   soma = 0;
   tam = strlen(str);
   dig = (SI)(str[tam-1]);
   if ( ap_critica == 'X' && (dig == 'X' || dig == 'x'))
      dig = 'x';
   else
      dig -= '0';

	 for ( i=tam-2 ; i >= 0 ; i--) {
			if( !isdigit(str[i]) ) {    continue; }
         n++;
			soma += ((SI)(str[i]-'0') * n );
    }
    resto = (SI)(soma % 11L);
	 d = 11 - resto;

   if ( ap_critica == 'X' ) {
      if (d == 10)
         d = 'x';
      if (d == 11)
         d = 0;
   }
   else {
      if (d == 10)
         d = 0;
      if (d == 11)
         d = 1;
   }

   if (d != dig)
      return(-1);
   else
      return(0);
}

/**--------------------------------------------------------------valcpf
*   nome            valcpf  --  verifica digito do cpf
*   parametros      c = valcpf(str);
*                   COUNT c;        -1 - digito invalido
*                                    0 - digito valido
*                   TEXT str[];   numero, sendo os 2 ultimos algarismos os digitos
**/
F__GLB   COUNT valcpf(str)
TEXT  *str;
{
	 SI i,n,tam,dig1,dig2,resto,d10,d11;
	 LONG soma1,soma2;
	 n = 1;
    tam = strlen(str);
	 dig1 = (SI)(str[tam-2]-'0');
	 dig2 = (SI)(str[tam-1]-'0');
	 soma1 = 0;

	 for ( i=tam-3 ; i >= 0 ; i--) {
			if( !isdigit(str[i]) ) {    continue; }
         n++;
			soma1 += ( (SI)(str[i]-'0') * n );
    }
    resto = (SI)(soma1 % 11L);
	 if(resto == 0 || resto == 1)		d10 = 0;
    else                            d10 = 11 - resto;

	 n=2;
    soma2 = 0;
	 for ( i=tam-3 ; i >= 0 ; i--) {
			if( !isdigit(str[i]) ) {    continue; }
         n++;
			soma2 += ( (SI)(str[i]-'0') * n );
    }
	 soma2 += ( d10 * 2 );
    resto = (SI)(soma2 % 11L);
	 if(resto == 0 || resto == 1)		d11 = 0;
    else                            d11 = 11 - resto;

	 if( (d10 != dig1) || (d11 != dig2))
		return(-1);
	 else
		return(0);
}


/**--------------------------------------------------------------valcgc
*   nome            valcpf  --  verifica digito do cgc
*   parametros      c = valcgc(str);
*                   SI c;        -1 - digito invalido
*                                    0 - digito valido
*                   TEXT str[];   numero, sendo os 2 ultimos algarismos os digitos
**/
F__GLB COUNT valcgc(str)
TEXT *str;
{
	COUNT i, prod, d1, d2;
   TEXT *p;
   TEXT dig[15], *mult;


	if ( strlen(str) < 14)
      return(-1);

   p = str;
   i = 1;
   while ( *p != '\0' && i <= 14 ) {
		if ( isdigit(*p) ) {
         dig[i++] = *p - '0';
      }
      p++;
   }

   if ( i != 15 ) 
      return(-1);

#ifdef FORM_ANT_CGC
   COUNT pp;
   COUNT soma;
/* nao e mais usado pela receita */
   prod = ((dig[1] * 1000) + (dig[3] * 100) + (dig[5] * 10) + dig[7]) * 2;
   for ( i=1, pp=1, soma=0; i <= 5; i++ ) {
      soma = soma + ((prod / pp ) % 10);
      pp = pp * 10;
   }

   prod = dig[2] + dig[4] +dig[6] + dig[8];

   if ( ((soma + prod) % 10) != 0 )
      return(-1);
#endif

   /* calculo do primeiro digito verificador */
   mult = "543298765432";

   for ( prod=0, i = 1; i <= 12; i++) 
      prod = prod + (dig[i] * (mult[i-1] - '0'));

   
   if ( (d1 = prod % 11) < 2 ) d1 = 0;
   else                        d1 = 11 - d1;

   if ( d1 != dig[13])
      return(-1);


   /* calculo do segundo digito verificador */
   mult = "6543298765432";
   for ( prod=0, i = 1; i <= 13; i++) 
      prod = prod + (dig[i] * (mult[i-1] - '0'));

   if ( (d2 = prod % 11) < 2 ) d2 = 0;
   else                        d2 = 11 - d2;

   if ( d2 != dig[14])
      return(-1);

   return(0);
}




/**--------------------------------------------------------------intervalo
*   nome            intervalo -  verifica se dado esta' contido em intervalo
*   parametros      c = intervalo(inf,val,sup,tipo)
*                   COUNT c;        -1 - dado nao contido no intervalo
*                                    0 - dado no intervalo especificado
*                   TEXT inf      limite inferior do intervalo
*                   TEXT val      valor a ser testado
*                   TEXT sup      limite superior do intervalo
*                   TEXT tipo     tipo de comparacao
*                                      'V' - valor
*                                      'D' - data
**/
F__GLB   COUNT intervalo(inf,val,sup,tipo)
TEXT  *inf;
TEXT  *val;
TEXT  *sup;
COUNT tipo;
{
	TEXT temp1[80];
	TEXT temp2[80];
	TEXT temp3[80];
	COUNT ret;
	ret=0;

   switch(tipo) {

		case 'V' : {
            DOUBLE min, max, val_tst;

            min = str_to_db(inf, sep_dec);
            max = str_to_db(sup, sep_dec);
            val_tst = str_to_db(val, sep_dec);

            if ( val_tst < min || val_tst > max ) ret = -1;

            break;
			}

		case 'D' : {

				memset((TEXT *)temp1,'\0',80);
				memset((TEXT *)temp2,'\0',80);
				memset((TEXT *)temp3,'\0',80);

            temp1[0] = inf[6];
            temp1[1] = inf[7];
            temp1[2] = inf[3];
            temp1[3] = inf[4];
            temp1[4] = inf[0];
            temp1[5] = inf[1];

            temp2[0] = val[6];
            temp2[1] = val[7];
            temp2[2] = val[3];
            temp2[3] = val[4];
            temp2[4] = val[0];
            temp2[5] = val[1];

            temp3[0] = sup[6];
            temp3[1] = sup[7];
            temp3[2] = sup[3];
            temp3[3] = sup[4];
            temp3[4] = sup[0];
				temp3[5] = sup[1];

            temp1[6] = '\0';
            temp2[6] = '\0';
            temp3[6] = '\0';

				if( strcmp(temp2,temp1)<0 || strcmp(temp2,temp3)>0 )   ret = -1;
            break;
         }
   }
   return(ret);
}




/**------------------------------------------------------Tabela ( ananias )
*/

F__GLB   COUNT val_tabela(digitado, pc, flag)
TEXT *digitado;
Campo *pc;
COUNT flag; /* 0 so critica */
{  COUNT ret=0, coluna, i, poderes;
   VOID   *opcoes;
   TEXT   **vet_opcoes;
   SC     titulo_tab[TE+1];
   SC     *item;
   TEXT   str[80], str_item[80];


      if ( digitado[0] == '\0') {
         ret = 0;
         goto fim;
      }

   if ( pc->cp_pttab == NULL ) {

      if ( digitado[0] == '\0') {
         ret = 0;
         goto fim;
      }

      if (pc->cp_2futuro > 0 && pc->cp_2futuro < 100 ) {
         TEXT chave[100];
         sprintf(chave, "%02.2d%s", (COUNT)pc->cp_2futuro, digitado);
   	   if ( EQLKEY(tab_inum, chave, strlen(chave) + 1) > 0 ) 
            ret = 0;
         else
            ret = -1;
         goto fim;
      }
      else {
         debug( "Ponteiro para vetor de opcoes esta NULL em valtab");
         ret = -1;
         goto fim;
      }
   }


   vet_opcoes = opcoes = pc->cp_pttab;

   strcpy(str, digitado);
   prep_palavra(str);


   if ( str[0] != '\0' ) {
      for ( i = 0;   vet_opcoes[ i ]  !=  NULL;   i++ ) {
         strcpy(str_item, vet_opcoes[i]);
         prep_palavra(str_item);
         if ( strcmp(str_item, str) == 0 ) {
            if ( flag ) {
               strcpy(digitado ,vet_opcoes[i]);
               ret = 1;
            }
            else {
               ret = 0;
            }
            goto fim;
         }
      }
   }


   if ( flag == 0 ) {
      ret = -1;
      goto fim;
   }

   sprintf(titulo_tab," %s ", pc->cp_param.tab_auto.tabela);

   coluna = size_x - 1;/*** - 4 - MAX(strlen(titulo_tab), strlen(pc->cp_mascara));***/

   if ( ap_acesso_ativo.da_tabela == 'X' )
      poderes = INSERE | ALTERA | DELETA;
   else
      poderes = 0;

   item = file_tabela( pc->cp_param.tab_auto.tabela, digitado, coluna, 9,
               size_x, 10, pc->cp_mascara,
               MP_CAMPO, MP_BARRA, 1, MP_JANELA, MP_DESTAQUE,
               titulo_tab, str_nada, poderes, SELECAO , &opcoes);

   if ( item == NULL ) {
      digitado[0] = '\0';
      ret = 0;
   }
   else
      ret = 1;

fim :

   return(ret);
}
