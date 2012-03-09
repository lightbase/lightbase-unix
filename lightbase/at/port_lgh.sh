rm lgh*
tar cvnf lgh_tar deslight/ap/infomake* deslight/ap/*.lb deslight/ap/makefile deslight/ct/makefile deslight/ct/*.[ch] deslight/padrao/* mt/*.[ch] mt/makefile* mt/makefile port_lg.sh
tar cvnf lgh1_tar deslight/ap/geraconv deslight/ap/*.[ch] deslight/ap/converte/*.[ch] deslight/ap/converte/makefile
compress lgh_tar
compress lgh1_tar
tar cvnf lgh lgh_tar.Z lgh1_tar.Z
rm lgh_* lgh1_*




