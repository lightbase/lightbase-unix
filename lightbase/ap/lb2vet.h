#define   FIXO    'F'
#define   EDIT_OK   'E'
#define   VET_EDIT  0x40
#define   VET_INC   0x01
#define   VET_ALT   0x02
#define   VET_DEL   0x04



typedef struct {
   TEXT tipo;
   COUNT ident;
   UCOUNT tamanho;
   COUNT num_elementos;
   TEXT  marca_fim[2];  /* deve ser sempre 0x6 e 0x4 */
}  Hdr_bloco;

#define TAMANHO_HDR ((((sizeof(Hdr_bloco)-1) / sizeof(LONG)) + 1) * sizeof(LONG))
#define TAM_ITEM(t) (((  (t==0) ? 0 : (t-1) / sizeof(LONG)) + 1) * sizeof(LONG))
#define TAM_TAM     (sizeof(LONG))

#ifdef MSDOS
   Vet_itens *load_vet(TEXT tipo, COUNT ident, TEXT modo, 
                     POINTER file_pos, COUNT max_elem, UCOUNT max_tam);
   POINTER save_vet(Vet_itens *, POINTER pos_file);  /* nao chama release */
   VOID rlse_vet(Vet_itens *);
   VOID *fst_item(Vet_itens *);
   VOID *nxt_item(Vet_itens *);
   VOID *pos_item(Vet_itens *, COUNT);
   VOID *add_item(Vet_itens *, COUNT pos, VOID *item, UCOUNT tam_item, COUNT shift);
   COUNT del_item(Vet_itens *, COUNT shift);
#else
   Vet_itens *load_vet( );
   POINTER save_vet( );
   VOID rlse_vet( );
   VOID *fst_item( );
   VOID *nxt_item( );
   VOID *pos_item( );
   VOID *add_item( );
   COUNT del_item( );
#endif



/*
TEXT *mod_item(Vet_itens *, COUNT pos, VOID *item, UCOUNT tam_item);
COUNT get_itlen(Vet_itens *);
*/
