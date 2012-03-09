rm md_tar* desmedit/lib/*.bak desmedit/lib/helpne/*.bak desmedit/lib/helpws/*.bak desmedit/lib/macros/*.bak desmedit/lib/language/*.bak desmedit/padrao/*.bak
tar cvnf md_tar desmedit/gera_md desmedit/padrao/* desmedit/src/*.[ch] desmedit/src/makefile desmedit/lib mt/*.[ch] mt/makefile* mt/makefile port_md.sh
compress md_tar

