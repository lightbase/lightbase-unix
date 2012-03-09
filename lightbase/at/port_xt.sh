rm xt_tar*
rm desxtools/lib/*.bak desxtools/lib/help/*.bak desxtools/padrao/*.bak
tar cvnf xt_tar desxtools/gera_xt desxtools/padrao/* desxtools/src/*.[ch] desxtools/src/makefile desxtools/src/instala* desxtools/lib mt/*.[ch] mt/makefile* mt/makefile port_xt.sh
compress xt_tar








