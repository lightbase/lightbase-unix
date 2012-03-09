/*** INFOCON *** pers.h *** VERSAO 1.16 *** 89/06/23 ***/

#define		PE_OK		1
#define		PE_INSTALA	2
#define		PE_CPU		3
#define		PE_DATA		4
#define		PE_PIRATA	5
#define		PE_SEMDISCO	6

#define		PE_PERS		'1'

#ifdef MSDOS
int      pe_map(void),	
    		pe_tty(void),	/* obtem o numero maximo de terminais permitido */
    		pe_ttymax(),	/* obtem o numero maximo de terminais permitido */
         pe_teclou_x(int, char **),
         pe_x(void);


char		*pe_pers(void),
		*pe_data(int *, int *, int *),
		*pe_serie(void);
long		pe_volume();
#else
int		pe_map(),	
    		pe_tty(),	/* obtem o numero maximo de terminais permitido */
   		pe_ttymax(),
         pe_teclou_x(),
         pe_x();


char		*pe_pers(),
		*pe_cpu(),
		*pe_data(),
		*pe_cpu_atual(),
		*pe_serie();

long		pe_volume();
#endif
