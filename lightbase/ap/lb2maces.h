#ifdef VAXVMS
#include  "ctoptn.h"
#include  "ctdefs.h"
#include  "cteproto.h"
#include  "cterrc.h"
#else
#include  "../ct/ctoptn.h"
#include  "../ct/ctdefs.h"
#include  "../ct/cteproto.h"
#include  "../ct/cterrc.h"
#endif


#define     HDR_TEXTO      6   /* MARCA + NUMERO REGISTRO + NUM_DO_CAMPO */
#define     HDR_REGISTRO   6   /* MARCA + NUMERO REGISTRO + NUM_DE_CAMPOS */

#define     MARCA_REGISTRO 254
#define     MARCA_TEXTO    255


#define     MA_NBUFERS     6
#define     NU_INICIAL     5  /* 0 e 1 sao acessos   2 e' lb4_dnum   3 e 4 sao tabelas*/
#define     AI_FILES       5
#define     AI_INDICES     7   /* era 6 */
#define     NIND_BASE      (AI_FILES + AI_INDICES)
#define     INI_SELE(n)    (NU_INICIAL + ((n) * NIND_BASE))

#define     NUM_FILES      (NU_INICIAL + (MAX_SELE * NIND_BASE))

extern      VRLEN    ct_tamlast;
extern      TEXT     ct_indkey[];
extern      COUNT    ct_lenkey;
extern      COUNT    uerr_cod;

extern      UCOUNT    ct_finger;
extern      UCOUNT    ct_release;
extern      ULONG     ct_sernum;



