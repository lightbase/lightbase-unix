#ifdef VAXVMS
#include  "entrada.h"
#else
#include  "../mt/entrada.h"
#endif

#define   ACEITA    0
#define   REJEITA  -1
#define   ACT_REIMP  1



#define   STR_XUPPER(n)       &str_XX80[80 - n]
#define   STR_XLOWER(n)       &str_x80[80 - n]

#define   X1             STR_XUPPER(1)
#define   X8         	 STR_XUPPER(8)
#define   X14        	 STR_XUPPER(14)
#define   X40            STR_XUPPER(40)

#define   x14        	 STR_XLOWER(14)
#define   x20        	 STR_XLOWER(20)
#define   x30            STR_XLOWER(30)
#define   x40            STR_XLOWER(40)
#define   x50            STR_XLOWER(50)
#define   x80            STR_XLOWER(80)


#define   S14            "**************"
#define   xARQ           "xxxxxxxxxxxx"
#define   XP5_35         "PPPPPxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define   xTM            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"


#define   INSERT_OFF()   /**/

#define XPATH     x40
