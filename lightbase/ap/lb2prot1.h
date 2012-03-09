#ifdef MSDOS
   VOID     INIC_TRANS( VOID );
   COUNT    LOCK_RECURSO(TEXT *, TEXT *, COUNT);
   COUNT    checa_copia( COUNT *, COUNT *, LONG *);
   VOID     sem_memoria( VOID );
#else
   VOID     INIC_TRANS( );
   COUNT    LOCK_RECURSO( );
   COUNT    checa_copia( );
   VOID     sem_memoria( );
#endif


/** lb2but1    ***********************************************************/
#ifdef MSDOS
   Campo **pos_vetcmp(COUNT);
   COUNT qtd_vetcmp(COUNT);
   Vet_itens *load_campos(COUNT, COUNT);
   VOID rlse_campos(COUNT);
   COUNT acha_campo(COUNT, COUNT);
   COUNT ver_campo(COUNT, TEXT *, COUNT *);
   TEXT *sel_campo(COUNT, Campo **, COUNT);
   Vet_itens *load_vistas(COUNT, COUNT);
   Vet_itens *load_itvis(Vista *, COUNT);
   TTela *pos_itvis(COUNT);
   COUNT qtd_itvis(VOID);
   Vet_itens *load_rels(COUNT, COUNT);
   Vet_itens *load_itrel(Rel *, COUNT);
   RTela *pos_itrel(COUNT);
   COUNT qtd_itrel(VOID);
#else
   Campo **pos_vetcmp( );
   COUNT qtd_vetcmp( );
   Vet_itens *load_campos( );
   VOID rlse_campos( );
   COUNT acha_campo( );
   COUNT ver_campo( );
   TEXT *sel_campo( );
   Vet_itens *load_vistas( );
   Vet_itens *load_itvis( );
   TTela *pos_itvis( );
   COUNT qtd_itvis( );
   Vet_itens *load_rels( );
   Vet_itens *load_itrel( );
   RTela *pos_itrel( );
   COUNT qtd_itrel( );
#endif




/** lb2tela    ***********************************************************/
#ifdef MSDOS
   VOID     lista_vista( WIN *, SS, SS, SS, SC *);
   VOID     brilha_pal( SS, SS, WIN *, SC **, SS);
   SS       centraliza( SC *, SC *, SS );
   VOID     linha_hor( SS, SS, SS, WIN * );
   VOID     linha_ver( SS, SS, SS, WIN * );
#else
   VOID     lista_vista( );
   VOID     brilha_pal( );
   SS       centraliza( );
   VOID     linha_hor( );
   VOID     linha_ver( );
#endif


/** lb2crit    ***********************************************************/
#ifdef MSDOS
   COUNT valdata(TEXT *);
   COUNT valhora(TEXT *);
   COUNT valdigito(TEXT *);
   COUNT valcpf(TEXT *);
   COUNT valcgc(TEXT *);
   COUNT intervalo(TEXT *, TEXT *, TEXT *, COUNT);
   COUNT val_tabela(TEXT *, Campo *, COUNT);
#else
   COUNT valdata( );
   COUNT valhora( );
   COUNT valdigito( );
   COUNT valcpf( );
   COUNT valcgc( );
   COUNT intervalo( );
   COUNT val_tabela( );
#endif


/** lb2rdisp   ***********************************************************/
#ifdef MSDOS
   COUNT reg_display( COUNT, LONG, WIN *, TEXT *, TEXT **, COUNT, LONG *, WIN *, WIN *, COUNT);
#else
   COUNT reg_display( );
#endif


/** lb2ent1    ***********************************************************/
#ifdef MSDOS
   COUNT ent_dados(COUNT, COUNT);
#else
   COUNT ent_dados( );
#endif


/** lb2ent2   ***********************************************************/
#ifdef MSDOS
   TTela *anda(COUNT, COUNT, COUNT, WIN *, TEXT *, COUNT *);
   VOID  disp_mtela(COUNT, WIN *);
   COUNT rd_campo(TTela *, Campo *, WIN *, WIN *);
   COUNT rd_texto(TTela *, Campo *, WIN *);
   COUNT cp_vazio( Campo *, COUNT);
   COUNT so_navega(WIN *, COUNT, LONG *, WIN *, TEXT *);
   VOID  dn_textos(COUNT, COUNT);
   LONG  pega_num(VOID);
   VOID  trata_outs( COUNT, COUNT );
#else
   TTela *anda( );
   VOID  disp_mtela( );
   COUNT rd_campo( );
   COUNT rd_texto( );
   COUNT cp_vazio( );
   COUNT so_navega( );
   VOID  dn_textos( );
   LONG  pega_num( );
   VOID  trata_outs( );
#endif

/** lb2ent3   ***********************************************************/
#ifdef MSDOS
   COUNT read_record( COUNT , LONG, COUNT);
   VOID  free_record( COUNT );
   LONG  reg_corrente(COUNT, LONG);
   LONG  prox_reg(COUNT, LONG);
   LONG  ante_reg(COUNT, LONG);
#else
   COUNT read_record( );
   VOID  free_record( );
   LONG  reg_corrente( );
   LONG  prox_reg( );
   LONG  ante_reg( );
#endif

/** lb2ent4   ***********************************************************/
#ifdef MSDOS
   COUNT write_record(COUNT, COUNT);
   LONG  new_record( COUNT, COUNT );
   COUNT edit_record(COUNT, LONG, WIN *, COUNT);
   COUNT del_record( COUNT , LONG);
   COUNT atu_ac(COUNT, COUNT, POINTER);
   COUNT varre_vista(COUNT, COUNT);
   COUNT crit_reg(COUNT);
   COUNT critica_campo( Campo *);
   COUNT pal_index(COUNT, COUNT, UTEXT *, COUNT);
   COUNT pal_d_index(COUNT, COUNT, UTEXT *, COUNT);
   COUNT salva_textos(COUNT, LONG, COUNT);
#else
   COUNT write_record( );
   LONG  new_record( );
   COUNT edit_record( );
   COUNT del_record( );
   COUNT atu_ac( );
   COUNT varre_vista( );
   COUNT crit_reg( );
   COUNT critica_campo( );
   COUNT pal_index( );
   COUNT pal_d_index( );
   COUNT salva_textos( );
#endif

/** lb2ent5   ***********************************************************/
#ifdef MSDOS
   COUNT importa( COUNT );
#else
   COUNT importa( );
#endif



/** lb2pesq4   ***********************************************************/
#ifdef MSDOS
   COUNT aloc_brilha(COUNT);
   COUNT push_brilha(COUNT, TEXT *);
#else
   COUNT aloc_brilha( );
   COUNT push_brilha( );
#endif

/** lb2pesq5   ***********************************************************/
#ifdef MSDOS
   COUNT menu_pesq( VOID );
#else
   COUNT menu_pesq( );
#endif


/** lb2rel   ***********************************************************/
#ifdef MSDOS
   COUNT emite( SS, SL ( * )( SS, SL ), SC** );
   SS    col_quebra_reg (COUNT, COUNT);
   COUNT rel_form( SS, SL ( * )( SS, SL ));
   COUNT rel_etiq( SS, SL ( * )( SS, SL ));
   SS carac_form( VOID );
#else
   COUNT emite( );
   SS    col_quebra_reg ( );
   COUNT rel_form( );
   COUNT rel_etiq( );
   SS carac_form( );
#endif

/** lb2carel   ***********************************************************/
#ifdef MSDOS
   COUNT carac_rel(VOID);
#else
   COUNT carac_rel( );
#endif


/** lb2table  ***********************************************************/
#ifdef MSDOS
   COUNT    ld_matriz(COUNT);
   VOID     lib_matriz(VOID);
   COUNT    file_array( SC *, SC **, SS, SS );
   VOID     free_array( SC ** );
   COUNT    write_array( SC *, SC ** );
#else
   COUNT    ld_matriz( );
   VOID     lib_matriz( );
   COUNT    file_array( );
   VOID     free_array( );
   COUNT    write_array( );
#endif


/** lb2tab    ***********************************************************/
#ifdef MSDOS
   TEXT *file_tabela( SC *, SC *, SS, SS, SS, SS, SC *,
                     SS, SS,  SS, SS, SS, SC *, SC *, SS, SS, VOID ** );
#else
   TEXT *file_tabela( );
#endif


/** lb2edind  ***********************************************************/
#ifdef MSDOS
   COUNT trata_indireto(TEXT *, Campo *, COUNT);
   COUNT tab_number(TEXT *, TEXT *);
   VOID  trata_formula(COUNT);
   COUNT calc_token(TEXT *, DOUBLE *);
#else
   COUNT trata_indireto( );
   COUNT tab_number( );
   VOID  trata_formula( );
   COUNT calc_token( );
#endif

/** lb2ger4    ***********************************************************/
#ifdef MSDOS
   COUNT    open_lb4(Base *, TEXT *, COUNT, LONG, UCOUNT);
   COUNT    close_lb4(VOID);
#else
   COUNT    open_lb4( );
   COUNT    close_lb4( );
#endif

/** lb2db00    ***********************************************************/
#ifdef MSDOS
   COUNT    def_base( VOID );
   COUNT    def_seguranca( Base * );
#else
   COUNT    def_base( );
   COUNT    def_seguranca( );
#endif

/** lb2db01    ***********************************************************/
#ifdef MSDOS
   POINTER  lb4_space( POINTER, UCOUNT );
   COUNT    lb4_upd( COUNT );
#else
   POINTER  lb4_space( );
   COUNT    lb4_upd( );
#endif

/** lb2dbsw    ***********************************************************/
#ifdef MSDOS
   COUNT rec_stop(VOID);
#else
   COUNT rec_stop( );
#endif

/** lb2dbcp    ***********************************************************/
#ifdef MSDOS
   VOID  rec_campos(COUNT);
#else
   VOID  rec_campos( );
#endif

/** lb2dbvs0   ***********************************************************/
#ifdef MSDOS
   VOID  rec_vistas(COUNT);
#else
   VOID  rec_vistas( );
#endif


/** lb2dbvs1   ***********************************************************/
#ifdef MSDOS
   COUNT  ed_tela(SS *);
#else
   COUNT  ed_tela( );
#endif

/** lb2dbrl0   ***********************************************************/
#ifdef MSDOS
   VOID rec_relats(COUNT);
#else
   VOID rec_relats( );
#endif

/** lb2dbrl1   ***********************************************************/
#ifdef MSDOS
   COUNT ed_relat(SS *, SS, SS);
#else
   COUNT ed_relat( );
#endif

/** lb2dbrl2   ***********************************************************/
#ifdef MSDOS
   COUNT make_colunado(COUNT *, COUNT, COUNT);
   COUNT prep_f2_colunado( VOID);
#else
   COUNT make_colunado( );
   COUNT prep_f2_colunado( );
#endif


/** lb2dbcb    ***********************************************************/
#ifdef MSDOS
   COUNT cria_base( TEXT * );
#else
   COUNT cria_base( );
#endif

/** lb2dbib    ***********************************************************/
#ifdef MSDOS
   COUNT inport_base( COUNT);
#else
   COUNT inport_base( );
#endif

/** lb2sbase   ***********************************************************/
#ifdef MSDOS
   COUNT    sel_base(COUNT, TEXT *, TEXT *);
   COUNT    seta_acesso( TEXT *);
   TEXT    *uma_base(COUNT, TEXT *, COUNT, COUNT, TEXT *);
#else
   COUNT    sel_base( );
   COUNT    seta_acesso( );
   TEXT    *uma_base( );
#endif


/** lb2main2   ***********************************************************/
#ifdef MSDOS
   COUNT decripta(VOID);
   VOID  imp_logo(COUNT);
   COUNT encerra(VOID);
   COUNT config(VOID);
#else
   COUNT decripta( );
   VOID  imp_logo( );
   COUNT encerra( );
   COUNT config( );
#endif

/** lb2usu0 **************************************************************/
#ifdef MSDOS
   COUNT pega_usu(VOID);
   VOID  lb_encrypt (UTEXT *, UTEXT *, UTEXT);
#else
   COUNT pega_usu( );
   VOID  lb_encrypt ( );
#endif

/** lb2usu1 **************************************************************/
#ifdef MSDOS
   COUNT open_acessos(COUNT, LONG, UCOUNT, COUNT);
   COUNT t_numse(TEXT *, TEXT *, LONG);
#else
   COUNT open_acessos( );
   COUNT t_numse( );
#endif

/** lb2usu2 **************************************************************/
#ifdef MSDOS
   COUNT atu_usuario( COUNT );
   COUNT mos_usuario( VOID );
#else
   COUNT atu_usuario( );
   COUNT mos_usuario( );
#endif

/** lb2grp **************************************************************/
#ifdef MSDOS
   COUNT atu_grupo( COUNT );
   COUNT mos_grupo( VOID );
#else
   COUNT atu_grupo( );
   COUNT mos_grupo( );
#endif

/** lb2musgr ************************************************************/
#ifdef MSDOS
   COUNT cad_usuarios( VOID );
#else
   COUNT cad_usuarios( );
#endif

/** lb2div1   ***********************************************************/
#ifdef MSDOS
   VOID  imp_base(TEXT *);
   VOID  lib_ativa(COUNT);
   VOID  fim_lb(TEXT *);
   VOID  numdebug(COUNT);
   TEXT *data_hora( COUNT, LONG);
   TEXT *conv_data( TEXT *);
   TEXT *conv_hora( TEXT *);
   VOID  free_vet(VOID **, COUNT);
   SI lb_random(COUNT);
#else
   VOID  imp_base( );
   VOID  lib_ativa( );
   VOID  fim_lb( );
   VOID  numdebug( );
   TEXT *data_hora( );
   TEXT *conv_data( );
   TEXT *conv_hora( );
   VOID  free_vet( );
   SI lb_random( );
#endif

/** lb2base0  ***********************************************************/
#ifdef MSDOS
   TEXT    *prep_palavra(TEXT *);
   COUNT    val_indice(TEXT *);
   COUNT    lin_texto(ARG *, COUNT, TEXT *, COUNT);
   TEXT    *field_info(COUNT, COUNT, COUNT, TEXT *, COUNT);
   VOID     en_cripta(UTEXT *, UCOUNT);
   VOID     de_cripta(UTEXT *, UCOUNT);
   WIN     *abre_janela(COUNT);
   WIN     *abre_win( COUNT, COUNT, COUNT, COUNT, COUNT, COUNT, COUNT);
   VOID     set_menu( TEXT **, COUNT, TEXT *, TEXT *, TEXT *);
   LONG     get_postexto(COUNT, UTEXT *);
   VOID     put_postexto(COUNT, UTEXT *, LONG);

#  ifndef PUT_LONG
      VOID PUT_LONG(UTEXT *, LONG);
      VOID PUT_LONG4(UTEXT *, LONG);
#  endif
#  ifndef GET_LONG
      LONG GET_LONG(UTEXT *);
      LONG GET_LONG4(UTEXT *);
#  endif
#else
   TEXT    *prep_palavra( );
   COUNT    val_indice( );
   COUNT    lin_texto( );
   TEXT    *field_info( );
   VOID     en_cripta( );
   VOID     de_cripta( );
   WIN     *abre_janela( );
   WIN     *abre_win( );
   VOID     set_menu( );
   LONG     get_postexto( );
   VOID     put_postexto( );

#  ifndef PUT_LONG
      VOID PUT_LONG( );
      VOID PUT_LONG4( );
#  endif
#  ifndef GET_LONG
      LONG GET_LONG( );
      LONG GET_LONG4( );
#  endif
#endif


/** lb2base1  ***********************************************************/
#ifdef MSDOS
   COUNT    info_base(TEXT *, TEXT *, TEXT *, TEXT *);
   COUNT    look_regs(COUNT, LONG *, LONG *, LONG *, LONG *);
   VOID     wr_log(COUNT, TEXT *);
   COUNT    pode_exportar(UCOUNT);
   UCOUNT   calc_bsxor(Base *);
   COUNT    base_liberada(Base *);
#else
   COUNT    info_base( );
   COUNT    look_regs( );
   VOID     wr_log( );
   COUNT    pode_exportar( );
   UCOUNT   calc_bsxor( );
   COUNT    base_liberada( );
#endif

/** lb2cad1   ***********************************************************/
#ifdef MSDOS
   COUNT open_base(TEXT *, COUNT);
#else
   COUNT open_base( );
#endif

/** lb2cad2   ***********************************************************/
#ifdef MSDOS
   COUNT open_vista(COUNT, COUNT, COUNT);
   VOID  close_vista(VOID);
   COUNT muda_vista(COUNT, COUNT);
#else
   COUNT open_vista( );
   VOID  close_vista( );
   COUNT muda_vista( );
#endif

/** lb2cad3   ***********************************************************/
#ifdef MSDOS
   COUNT open_rel(COUNT, COUNT, COUNT);
   VOID  close_rel(VOID);
#else
   COUNT open_rel( );
   VOID  close_rel( );
#endif

/** lb2ind   ***********************************************************/
#ifdef MSDOS
   COUNT    reg_index(COUNT, COUNT, COUNT, LONG, COUNT);
   COUNT    touch_unica(Campo *, LONG *, TEXT *, COUNT);
   TEXT    *salva_unica(COUNT, COUNT, COUNT);
   VOID     prep_indice( S_INDEX *, UL, US, US, US, US );
   SS       sep_palavra( S_INDEX *, SC *, S_DELIMIT *, SS ( * )() );
   SS       lb_palavra( ARG *, US, S_INDEX *, S_DELIMIT *, SS ( * )( ) );
   US       to_struct( S_INDEX *, UC * );
   US       to_bits( S_INDEX *, UC * );
   COUNT    fim_index( VOID );
#else
   COUNT    reg_index( );
   COUNT    touch_unica( );
   TEXT    *salva_unica( );
   VOID     prep_indice( );
   SS       sep_palavra( );
   SS       lb_palavra( );
   US       to_struct( );
   US       to_bits( );
   COUNT    fim_index( );
#endif


/** lb2gntxt  ***********************************************************/
#ifdef MSDOS
   VOID txt_start(VOID);
   ARG *txt_load(COUNT, POINTER, LONG *, COUNT, LONG);
   POINTER txt_save(COUNT, ARG *, LONG, COUNT);
   COUNT txt_free( COUNT, POINTER);
   POINTER txt_dup( COUNT, COUNT, POINTER, LONG);
#else
   VOID txt_start( );
   ARG *txt_load( );
   POINTER txt_save( );
   COUNT txt_free( );
   POINTER txt_dup( );
#endif

/** lb2perso  ***********************************************************/
#ifdef MSDOS
   COUNT    persona(TEXT *, TEXT *);
   COUNT    tabpsq_inic(COUNT);
   COUNT    move_janela(WIN *);
   COUNT    config_lb(WIN *);
   COUNT    config_save(TEXT *, TEXT *, COUNT);
#else
   COUNT    persona( );
   COUNT    tabpsq_inic( );
   COUNT    move_janela( );
   COUNT    config_lb( );
   COUNT    config_save( );
#endif

/** lb2cadge  ***********************************************************/
#ifdef MSDOS
   POINTER  find_rec(COUNT, COUNT, TEXT *, TEXT *, TEXT);
   TEXT    *load_rec(COUNT, COUNT, TEXT *, TEXT *, VOID_T *, UCOUNT, TEXT);
   COUNT    add_ind (COUNT, COUNT, TEXT *, TEXT *, POINTER);
   POINTER  add_rec (COUNT, COUNT, TEXT *, TEXT *, VOID_T *, UCOUNT);
   POINTER  alt_rec (COUNT, COUNT, TEXT *, POINTER, VOID_T *, UCOUNT);
   POINTER  exc_rec (COUNT, COUNT, TEXT *, TEXT *, COUNT);
   COUNT    file_open(COUNT, TEXT *, COUNT, LONG, UCOUNT);
   COUNT    arq_erro(TEXT *, COUNT);
#else
   POINTER  find_rec( );
   TEXT    *load_rec( );
   COUNT    add_ind ( );
   POINTER  add_rec ( );
   POINTER  alt_rec ( );
   POINTER  exc_rec ( );
   COUNT    file_open( );
   COUNT    arq_erro( );
#endif



/** lb2fergn  ***********************************************************/
#ifdef MSDOS
   COUNT tools_base(VOID);
   COUNT zap_base( Base *, COUNT);
#else
   COUNT tools_base( );
   COUNT zap_base( );
#endif

/** lb2ferrd ***********************************************************/
#ifdef MSDOS
   COUNT tbd_status(COUNT, Base *, LONG *, UCOUNT *);
   COUNT tbd_manut( TEXT *);
#else
   COUNT tbd_status( );
   COUNT tbd_manut( );
#endif


/** lb2ferri ***********************************************************/
#ifdef MSDOS
   COUNT new_base(Base *);
   COUNT tbp_scan( COUNT, TEXT *, LONG *);
   COUNT tbp_index( COUNT, LONG);
#else
   COUNT new_base( );
   COUNT tbp_scan( );
   COUNT tbp_index( );
#endif

/** lb2ferrm ***********************************************************/
#ifdef MSDOS
   COUNT del_base(WIN *);
#else
   COUNT del_base( );
#endif

/** lb2reind ***********************************************************/
#ifdef MSDOS
   COUNT  index_scan(COUNT, COUNT);
#else
   COUNT  index_scan( );
#endif

/** lb2hmenu ***********************************************************/
#ifdef MSDOS
   COUNT  hmenu(TEXT *);
#else
   COUNT  hmenu( );
#endif

/** lb2dummy  ***********************************************************/
#ifdef MSDOS
   COUNT principal(VOID);
   COUNT close_base( VOID );
   VOID  rec_prot(COUNT);
   COUNT pesquisa( COUNT);
   COUNT pre_pesquisa( COUNT, TEXT *);
   COUNT exporta( COUNT, SL ( * )( SS, SL ));
   COUNT  tools_base(VOID);
   VOID   backup(COUNT);
   VOID   restore(COUNT);
#  ifdef ENTRA_TESTE_DATA
   COUNT data_demo_ok( VOID );
#  endif
   LONG  cai_fora( LONG );
#else
   COUNT principal( );
   COUNT close_base( );
   VOID  rec_prot( );
   COUNT pesquisa( );
   COUNT pre_pesquisa( );
   COUNT exporta( );
   COUNT  tools_base( );
   VOID   backup( );
   VOID   restore( );
#  ifdef ENTRA_TESTE_DATA
   COUNT data_demo_ok( );
#  endif
   LONG  cai_fora( );
#endif

