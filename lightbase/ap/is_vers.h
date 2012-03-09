/*** INFOCON *** %M% *** VERSAO %I% *** %E% ***/

/*
 * para obtermos as versoes das bibliotecas, preciso
 * fazer com que os simbolos _ver* estejam indefinidos
 * ao chegar aa biblioteca desejada (infoscr, infogab, ...)
 * e antes de chegar a libinfocon (onde eu pegaria a versao
 * default chamada "?")
 *
 * As linhas que seguem servem so para forcar uma indefinicao
 */

extern char	*_vercon;
extern char	*_vergab;
extern char	*_verscr;
extern char	*_vermenu;
extern char	*_verhelp;
extern char	*_verpers;
extern char	*_vervar;
extern char	*_verprn;

char	**_lixo1 = &_vercon;
char	**_lixo2 = &_vergab;
char	**_lixo3 = &_verscr;
char	**_lixo4 = &_vermenu;
char	**_lixo5 = &_verhelp;
char	**_lixo6 = &_verpers;
char	**_lixo7 = &_vervar;
char	**_lixo8 = &_verprn;
