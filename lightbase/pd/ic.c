:
#	ic 1.00 22/08/90
#	ic 1.01 19/11/91
#	ic 1.02 19/05/92
#
#	Todos os direitos reservados a INFOCON TECNOLOGIA.
#	Este Script Contem Informacoes de Propriedade da
#	INFOCON TECNOLOGIA e eh Confidencial.
#
# INFOCUSTOM - Instalador de Produtos INFOCON
#
# Dependencias:
#	cat df expr getopt mv rm rmdir sed sh tr tar grep
#

# valores de retorno
: ${OK=0} ${FALHA=1} ${SAI=3} ${STOP=10}

# variaveis iniciais
CMDSHELL=/bin/sh	# comando shell
CMDWHO="who am i"	# comando who
[ "`uname`" = "ULTRIX" ] && {
	CMDSHELL=/bin/sh5
	CMDWHO="whoami"
}
[ "`uname`" = "AIX" ] && {
	CMDWHO="whoami"
}
# [ "`uname`" = "SunOS" ] && {
#	CMDWHO="whoami"
# }

# inicializa mensagens a serem mostradas
# langprod eh substituido na preparacao do master
if [ "LANGPROD" = "PORTUGUES" ]
then
	msg0="Erro: "
	msg1="Diretorio nao existe"
	msg2=" ou <a> para abortar: \c"
	msg3="tecle <s> ou <n>"
	msg4="Inclui Novo Produto"
	msg5="\nCaso deseje altera-lo, digite o percurso completo do diretorio e tecle <ENTER>,\n\
caso contrario,"
	msg6="numero de produtos invalido"
	msg7="nao encontrado configurador para"
	msg8="nao encontrado configurador, no diretorio '/tmp/perms'."
	msg9="Escolha opcao"
	msg10="nao consegui encontrar o arquivo "
	msg11="usa o seguinte diretorio para instalacao:\n"
	msg12="\nTodos os produtos INFOCON deixarao de funcionar, desinstalando o \n"
	msg13="digite o percurso completo do diretorio"
	msg14="arquivo de inicializacao com problemas"
	msg15="FATAL, nao consegui criar diretorio de configuradores (/etc/perms)"
	msg16="Existe instalacao anterior, sobrepoe? \c"
	msg17="# vago"
	msg18="\nEste eh o espaco disponivel no(s) seu(s) sistema(s) de arquivos:"
	msg19="\nHa espaco suficiente para instalacao? \c"
	msg20="Coloque disco "
	msg21="Checando instalacao ..."
	msg22="nao consegui desinstalar completamente"
	msg23="FATAL, uso exclusivo para super-usuario"
	msg24="Uso: ic [-s] [-l dirloc] [[-idvp] produto]\n
s	- espaco suficiente para manipulacao do produto\n
l	- diretorio de localizacao do produto\n
i,d,v,p	- instala, desinstala, verifica e inicializa produto (respectivamente)"
	msg25="\n\n\t1. Instala\n\t2. Desinstala\n\t3. Verifica\n\t4. Inicializa\n\t5. Finaliza\n\n"
	msg26="tecle <ENTER> para continuar"
	msg27="modulo invalido para este produto"
	msg28="Extraindo arquivos ..."
	msg29="Verificando disco ..."
	msg30="InfoCustom - Instalador de Produtos Infocon"
	msg31="Instalacao"
	msg32="Desinstalacao"
	msg33="Verificacao"
	msg34="Inicializacao"
	msg35="completamente instalado"
	msg36="nao instalado"
	msg37="parcialmente instalado"
	msg38="\n\tModulos Disponiveis para o Produto\n\n\
\tIdent.	Tamanho	Pacotes"
	msg39="Produto(s) Instalado(s)"
	msg40="Diretorio de Manipulacao do Produto"
	msg41="\n\t------------------------------------------------------------------"
	msg42="\nFinalize a execucao do InfoCustom e esta sessao do sistema.\n\
Entre numa nova sessao do sistema para que as variaveis de ambiente\n\
sejam setadas corretamente\n"
	msg43="Confirma desinstalacao? \c"
	msg44="\nTecle <s> para continuar, <n> para retornar"
	msg45="\nDigite o identificador do modulo desejado, <TODOS> para selecionar todos os\n\
modulos"
	msg46="\nConfirma interrupcao?\n<s> confirma, qualquer tecla continua: \c"
	msg47="\n\t\t\t\t    AJUDA"
	msg48="\nFuncionamento:\n\
\ta ou A\t- aborta;\n\
\tENTER\t- continua (usa default);\n\
\tcmd\t- executa o comando 'cmd' no shell;\n\
\t?\t- ajuda."
	msg49="\nMenu de Produto(s) Instalado(s):\n\
\tEscolha  a opcao do produto a ser manipulado pelo InfoCustom ou a opcao\n\
'Inclui Novo Produto', para incluir um produto a ser manipulado,  a  partir  do\n\
diretorio /tmp/perms ou do diretorio corrente (nesta ordem).\n"
	msg50="Extraindo arquivos de "
	msg51="\nDiretorio de Manipulacao do Produto:\n\
\tDigite o nome do diretorio onde o produto sera  manipulado  (instalado,\n\
desinstalado,  verificado  ou  inicializado) ou tecle <ENTER> para manipular o\n\
produto no diretorio default.\n"
	msg52="\nInstalacao do Produto:\n\
\tVerifica se o produto esta instalado, caso  esteja  solicita  permissao\n\
para sobrepor, tecle <s> para sobrepor ou <n> retornar ao menu principal.\n\
\tMostra  o espaco necessario para instalar o produto, e o disponivel nos\n\
sistemas de arquivos do usuario. Solicita confirmacao de espaco para instalacao\n\
tecle <s> para continuar instalacao ou <n> retornar ao menu principal.\n\
\tPara cada disco do produto, solicita sua colocacao  na  unidade,  e  em\n\
seguida, tecle <s> para continuar instalacao ou <n> retornar ao menu principal.\n\
\tExecuta o arquivo de inicializacao do produto.\n\
\tVerifica  se  o  produto  e  a  descricao  de terminal (terminfo) estam\n\
instalados corretamente. Caso nao estejam mostra mensagem de erro.\n\
\tDurante a inicializacao do produto algumas variaveis  de  ambiente  sao\n\
modificadas.  Para  que  estas  sejam setadas corretamente, eh solicitado que o\n\
usuario finalize a execucao do InfoCustom e da sessao do sistema em que ele  se\n\
encontra, e abra uma nova sessao do sistema.\n"
	msg53="\nDesinstalacao do Produto:\n\
\tVerifica se o produto esta instalado, caso  esteja  solicita  permissao\n\
para  desinstalar  o  produto,  tecle <s> para confirmar a desinstalacao ou <n>\n\
retornar ao menu principal. Caso contrario mostra uma mensagem de erro.\n\
\tVerifica se o produto foi desinstalado  corretamente,  caso  tenha sido\n\
solicita  permissao  para  retornar  ao  menu  principal,  tecle  <ENTER>  para\n\
confirmar.\n\
\tOBS: esta opcao nao desinstala descricao de terminais (terminfo).\n"
	msg54="\nVerificacao do Produto:\n\
\tVerifica se existe produto instalado, caso exista, mostra o  estado  em\n\
o produto esta (instalado completamente ou parcialmente), caso contrario mostra\n\
uma mensagem de erro. Solicita permissao para retornar ao menu principal, tecle\n\
<ENTER> para confirmar.\n"
	msg55="\nInicializao do Produto:\n\
\tVerifica  se existe produto instalado, caso exista, procura arquivo  de\n\
inicializacao do produto, caso contrario mostra uma mensagem de erro.  Se  este\n\
estiver nos sistemas de arquivos do usuario, ele sera executado, caso contrario\n\
sera solicitado que o usuario coloque na unidade o disco do produto que  contem\n\
o  inicializador, e em seguida tecle <s> para continuar a inicializacao ou <n>\n\
para retornar ao menu principal. Caso seja teclado <s>, apos extraido do disco,\n\
o arquivo de inicializacao sera executado.\n"
	msg56="\nMenu Principal:\n\
\tEscolha  a  opcao  de  operacao  desejada,  instalacao,  desinstalacao,\n\
verificacao, inicializacao ou finalizacao.\n\
\tPara as operacoes de instalacao, desintalacao, verificacao e\n\
inicializacao, sera solicitado o nome do modulo que deve ser manipulado, ou\n\
TODOS para selecionar todos os modulos\n"
	msg57="Deseja cria-lo? \c "
	msg58="\nAguarde ..."
	msg59="\nAguarde verificacao de instalacao anterior ..."
	msg60="\nAguarde verificacao de instalacao ..."
	msg61="USO: getopt -fiwcDl -d pckg perms\n"
	msg62="TODOS"
	msg63="limpa tela"
else
	msg0="Error: "
	msg1="Invalid pathname"
	msg2=" or <a> to abort: \c"
	msg3="press <y> or <n>"
	msg4="Include new product"
	msg5="\nIn case you want to alter it, enter a new pathname\n"
	msg6="bad product number"
	msg7="configuration file not found for"
	msg8="configuration file not found in directory '/tmp/perms'."
	msg9="Select an option"
	msg10="cannot find file "
	msg11="uses the following installation directory:\n"
	msg12="\nAll INFOCON products will stop running, unistalling \n"
	msg13="enter complete pathname"
	msg14="bad initialization file"
	msg15="FATAL ERROR: cannot create configuration directory (/etc/perms)"
	msg16="Overwrite previous installation? \c"
	msg17= # empty
	msg18="\nFree disk blocks available in your file system(s) :"
	msg19="\nAre there enough disk blocks to install? \c"
	msg20="Insert disk "
	msg21="Checking installation ..."
	msg22="could not uninstall package completely"
	msg23="FATAL ERROR: execute permission denied"
	msg24="Usage: ic [-s] [-l dir] [[-idvp] product]\n
s	- disk blocks required\n
l       - installation directory\n
i,u,c,p - install, uninstall, verify and initialize product (respectively)"  
	msg25="\n\n\t1. Install\n\t2. Uninstall\n\t3. Verify\n\t4. Initialize\n\t5. End\n\n"
	msg26="press <ENTER> to continue"
	msg27="package not available"
	msg28="Extracting files ..."
	msg29="Checking disk ..."
	msg30="InfoCustom - Infocon's Installation Utility"
	msg31="Installation"
	msg32="De-installation"
	msg33="Verification"
	msg34="Initialization"
	msg35="installation successful"
	msg36="not installed"
	msg37="partially installed"
	msg38="\n\t Packages Available for this Product\n\n\
\tIdent.	Package Size"
	msg39="Product(s) installed"
	msg40="Installation Directory"
	msg41="\n\t------------------------------------------------------------------"
	msg42="\nQuit InfoCustom and end your login session now.\n\
Environment variables will be set correctly in subsequent logins\n"
	msg43="Confirm de-installation? \c"
	msg44="\ntype <y> to continue or <n> to return"
	msg45="\nEnter package name or "ALL" to select all packages\n"
	msg46="\nConfirm interruption?\nPres <y> to confirm or any other key to continue: \c"
	msg47="\n\t\t\t\t    HELP"
	msg48="\nUsage:\n\
\ta or A\t- abort;\n\
\tENTER\t- continue (use default);\n\
\tcmd\t- run shell command 'cmd';\n\
\t?\t- help."
	msg49="\n Products Currently Installed\n\
\tSelect a product or the 'Include New Product' option to add a new one\n"
	msg50="Extracting files from "
	msg51="\nInstallation Directory:\n\
\tPress <ENTER> to select default directory or type in complete pathname.\n"
	msg52="\nProduct Installation:\n\
\tSearches for previous installations, and requests permission to overwrite them.\n\
Press <y> to overwrite or <n> to return to main menu.\n\
\tDisplays disk blocks required to install product and free blocks available\n\
in the file system(s). Prompts for confirmation to continue installation\n\
type <y> to continue installation or <n> to return to main menu.\n\
\tPrompts for the insertion of volume diskettes in floppy drive, \n\
type <y> to continue installation or <n>to return to main menu.\n\
\tExecutes product initialization file.\n\
\tChecks if product and terminal description (terminfo) are\n\
correctly installed. If not ok, displays error message .\n\
\tDuring product initialization some environment variables are \n\
modified. At the end of the installation,\n\
you will be asked to end your working session\n\
and login again in order to update these variables.\n"
	msg53="\nProduct De-installation:\n\
\tChecks if the product is installed, and then requests permission\n\
to de-install it,  type <y> to confirm de-installation or <n>\n\
to return to main menu.\n\
\tChecks de-installation, if ok\n\
requires permission to return to main menu, press  <ENTER>  to\n\
confirm.\n\
\tP.S.: this option does not uninstall terminal descriptions (terminfo).\n"
	msg54="\nVerify Product:\n\
\tVerifies the installed product, and displays its status (if any)\n\ 
(e.g., fully or partly installed), otherwise displays an\n\
error message. Requests permission to return to main menu, press\n\
<ENTER> to confirm.\n"
	msg55="\nProduct Initialization:\n\
\tChecks if installed product exists, executes product\n\
initialization file, otherwise displays an error message. You will \n\
asked to insert a product volume containing the initialization script if it \
is not found in your file system\n\
, type <y> to continue initialization <n>\n\
to return to main menu.\n"
	msg56="\nMain Menu:\n\
\tSelect one of the available options: installation, de-installation,\n\
verification, initialization, or end.\n\
\tThe installation, de-installation, verification and initialization\n\
operations will ask you to select the desired product package(s).\n"
	msg57="Do you want to create it? \c "
	msg58="\nWait ..."
	msg59="\nSearching for previous installations ..."
	msg60="\nChecking installation ... "
	msg61="USAGE: getopt -fiwcDl -d pckg perms\n"
	msg62="ALL"
	msg63="clear screen"
fi

CFGLOC=/etc/perms	# diretorio default dos arquivos de configuracao
ICINIT=/tmp/init.	# script executado apos a instalacao do produto
null=/dev/null		# diretorio destino de mensagens indesejaveis
tmp=./ic$$		# mascara dos arquivos temporarios criados
pfx=/tmp/_lbl		# mascara de pesquisa p/ label identificador de volume
cfg=$CFGLOC		# local onde esta o arquivo de configuracao
arqdisk=`pwd`/		# arquivo a ser desmontado, quando distribuido em formato disk
TODOS=$msg62		# constante de selecao para todos os modulos do produto
ETC=/etc		# diretorio etc
 

# mkdir diretorio
mkdir() {
	_mdargs=$1
	case "$_mdargs" in
	/*)	_mddir=$_mdargs
		;;
	*)
		_mddir=`pwd`/$_mdargs
		;;
	esac
	(cd /;
	for _mdloop in `echo $_mddir | tr "/" " "`
	do
		[ -d $_mdloop ] || /bin/mkdir $_mdloop
		cd $_mdloop
	done)
	unset _mdargs _mddir _mdloop
	return 0
}

# tela()
# echo 20 linhas em branco (limpa a tela), e coloca nome do produto e acao
# atualmente em execucao. Caso estejamos no modo linha de comando nao faz nada.
tela() {
	# se estamos executando linha de comando nao mostra tela
	[ $lcmd ] && return $OK
	# so devemos limpar a tela se ela modou
	[ "$acao" = "$*" ] && return $OK
	acao=$*
	echo "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" >&2
	# mostra titulo e sub-titulo
	echo "$msg30\n\n$acao\n"
}	

# ajuda()
# usa o valor da variavel stat, para identificar qual o tipo de auxilio
# que deve mostrar ao usuario
ajuda() {
	mens "$msg47 $msg48"	# mostra mensagens gerais
	case $stat in
	0)	mens "$msg56"	;;  # menu principal
	1)	mens "$msg49"	;;  # menu de produtos instalados
	3)	mens "$msg51"	;;  # diretorio de manipulacao do produto
	4|8)	mens "$msg52"	;;  # instalacao do produto
	5|9)	mens "$msg53"	;;  # desinstalacao do produto
	6)	mens "$msg54"	;;  # verificacao do produto
	7)	mens "$msg55"	;;  # inicializacao do produto
	esac
	mens "$msg26 ...\c"	# espera ENTER
	read resp
}

# leia()
# le resp e,
#	entra/sai modo debug se +x/-x
#	executa um comando do shell se !resp
#	finaliza $SAI se "a"
#	chama ajuda se "?"
#	retorna $OK e o valor lido em resp caso contrario
leia() {
	# guardamos argumento, para nao perde-lo na chamada de outra rotina
	argleia=$*
	while	echo ${argleia}$msg2 >&2
		read resp
	do	case $resp in
		+x|-x)	set $resp	;;
		!*)	eval `expr "$resp" : "!\(.*\)"`	;;
		A|a)	fim $SAI	;;
		\?*)	ajuda		;;
		*)	# se houve interrupcao, vamos mostrar mensagem novamente
			[ $int ] && {
				int=
				continue
			}
			return $OK	;;
		esac
	done
}

# getsn()
# pede escolha do usuario (s/n) -	retorna 0 caso escolha <s>
# 					retorna 1 caso escolha <n>
getsn() {
	# guardamos argumento, para nao perde-lo na chamada de outra rotina
	argsn=$*
	while	leia $argsn
	do	case $resp in
		[SsYy])	return $OK			;;
		[Nn])	return $FALHA			;;
		   *)	erro "$msg0$msg3" 0		;;
		esac
	done
}

# erro()
# envia mensagem de erro, passada como argumento, para stderr
# se a mensagem vier seguida de 1, espere permissao do usuario
# para continuar, caso contrario retorne para rotina chamadora
erro() {
	echo "\n$1" >&2
	[ "$2" -eq 1 ] && leia $msg26
}

# mens()
# envia mensagens, passada como argumento, para stderr
mens() {
	# se estamos executando linha de comando nao mostra tela
	[ $lcmd ] && return $OK
	echo "$*" >&2
}

# permroot()
# retorna 0 se e super-usuario
# retorna 1 caso contrario
permroot() {
	$CMDWHO | grep root >/dev/null 2>&1 && return $OK
#	set -- `$CMDWHO`
#	[ "$1" = root ] && return $OK
	return $FALHA
}

# aborta()
# verifica se deseja realmente abortar, caso afirmativo vai para fim()
aborta() {
	# aqui nao usamos funcao, para nao perder o ambiente antes da interrupcao
	echo $msg46 $msg26 >&2
	read resp
	case $resp in
	[SsYy])	fim 1		;;
	*)	int=s
		return $FALHA	;;
	esac
}

# coleta()
# coleta eh um coletor de lixo, para ser usado quando interrompemos 
# no meio de uma instalacao ou desinstalacao. Em ambos os casos o resultado 
# eh a desinstalacao do produto
coleta() {
	echo "$msg58"
	verpckg $PRD
	# so devemos remover se estiver parcialmente instalado
	[ $? -ne 5 ] && return $OK
	# remove arquivos e diretorios de PRD (mantem TERM)
	arqprd=`fixperm -fuo -d$PRD $cfg/$prd`
	rm -f $arqprd 2> $null
	arqprd=`fixperm -Duo -d$PRD $cfg/$prd`
	for dirrm in $arqprd
	do
		case "$dirrm" in
		/*)
			continue;
			;;
		*)
			rmdir $dirrm 2> $null
			;;
		esac
	done
	unset dirrm
	verpckg $PRD
#	[ $? -eq 1 ] && rmcfg=s
}

# fim()
# deve remover todos os arquivos temporarios, e depois sair com o status
# que foi passado como argumento
fim() {
	arg=$1
	trap '' 1 2 3 15
	rm -fr ${tmp}* $pfx $ICINIT* 2> $null
	# se estamos no meio de uma instalacao ou desinstalacao
	[ "$stat" -eq 4 -o "$stat" -eq 5 ] && coleta
	# caso de incluir produto
	[ $CFGLOC != $cfg -a "$typ" != disk ] && rm -rf $cfg 2> $null
	# desinstalamos um produto, devemos remover seu arquivo de configuracao
	[ $rmcfg ] && rm -f $cfg/$prd 2> $null
	exit $arg
}

:
#
# Modulo de rotinas de tratamento do arquivo de configuracao
#
# lstln() : executa a listagem do arquivo de configuracao para as opcoes: -f,
#	-D, -l e -w.
# 	    Variaveis:
#		fxstat - indica se deve imprimir as informacoes da linha atual;
# 		fxout - informacoes a serem impressas.
# 		mfxlnk - numero de links do arquivo
#		fxu - indica se eh para lista os links
#		fxo - inibe a listagem de arquivos e diretorios opcionais
# execln() : checa e seta as permissoes do arquivo ou diretorio, caso opcao -c
#	tenha sido dada, cria arquivos vazios.
#	     Variaveis:
#		fxnam - nome do arquivo ou diretorio;
#		fxmod - permisssoes;
#		fxdon - dono;
#		fxgrp - grupo.
# 		fxlnk - numero de links do arquivo
# setln() : seta os valores de variaveis de dono e grupo, respectivamente
#	uid* e gid*.
# fixperm() : trata opcoes e executa as rotinas necessarias. Opcoes:
#		-f : lista apenas os arquivos;
#		-D : lista apenas os diretorios;
#		-l : lista arquivos e diretorios;
#		-o : inibe a listagem de arquivos e diretorios opcionais
#		     (letras maiusculas), usado principalmente para a
#		     desintalacao ou abandono nao remover estes arquivos e 
#		     diretorios, que podem ser do sistema
#		-u : lista os link dos arquivos e diretorios usada em conjunto
#		     com -D ou -f.
#		-w : lista o volume onde se encontra o arquivo ou diretorio;
#		-i : checa os arquivos do produto, e retorna:
#			0 - produto instalado;
#			1 - produto nao instalado;
#			5 - produto nao instalado corretamente;
#		-dnome : onde nome, identifica produto a manipular.
#	      Variaveis:
#		fxchk - valor de retorno da rotina execln, usado com opcao -i;
#		fxf - eh true, se opcao -f ou -l;
#		fxD - eh true, se opcao -D ou -l;
#		fxi - eh true, se opcao -i;
#		fxu - eh true, se opcao -u;
#		fxo - eh true, se opcao -o;
#		fxc - eh true, se opcao -c;
#		fxid - contem o identificador do produto, passado com opcao -d;
#		fxarg - nome do arquivo de configuracao;
#


lstln() {
	while	[ "$1" ]
	do	fxstat=
		case $2 in
		[dD]*)	[ $fxD ] || fxstat=1	;;
		*)	[ $fxf ] || fxstat=1	;;
		esac
		[ $fxstat ] || {
			case $2 in
			[A-Z]*)	[ $fxo ] && { shift 6 2> $null
					      continue
					    };;
			esac
			fxout=$5
			mfxlnk=$4
			[ $fxw ] && fxout=$fxout"\t"$6
			echo $fxout
			[ $fxu ] && [ "$mfxlnk" -gt 1 ] && {
				mfxln=`sed -n -e "\?^$1[ 	]*$2[ 	]*$3*[ 	]*$4[ 	]*$5[ 	]*$6?=" $fxarg`
				sed -n -e "`expr $mfxln + 1`,`expr $mfxln + $mfxlnk - 1`p" $fxarg | while read mfxnamlk mvol_lixo
				do
					fxout=$mfxnamlk
					[ $fxw ] && fxout=$fxout"\t"$mvol_lixo
					echo $fxout
				done
				unset mfxlnk mfxln mfxnamlk mvol_lixo
			}
		}
		shift 6 2> $null
	done
	return $OK
}

execln() {
	while	[ "$1" ]
	do	fxnam=$5
		# so trata arquivos opcionais ou vazios caso tenha opcao -c
		case $2 in
		[A-Ze]*) [ -z "$fxc" -o -s $fxnam ] && {
				shift 6 2> $null
				continue
			 }
			 # se eh vazio temos que cria-lo
			 case $2 in
			 e*)	> $fxnam	;;
			 esac
		esac	
		fxmod=`expr "$2" : ".\(.*\)"`
		fxdon=`expr "$3" : "\(.*\)/.*"`
		fxgrp=`expr "$3" : ".*/\(.*\)"`
		fxlnk=$4
		fxstat=1
		[ -f $fxnam -o -d $fxnam ] && {
			# se existe arquivo ou diretorio mudamos as permisoes
			fxstat=$?
			chmod $fxmod $fxnam 2> $null
			chown $fxdon $fxnam 2> $null || eval chown \$uid$fxdon $fxnam 2> $null
			chgrp $fxgrp $fxnam 2> $null || eval chgrp \$gid$fxgrp $fxnam 2> $null
			[ "$fxlnk" -gt 1 ] && {
				fxln=`sed -n -e "\?^$1[ 	]*$2[ 	]*$3*[ 	]*$4[ 	]*$5[ 	]*$6?=" $fxarg`
				sed -n -e "`expr $fxln + 1`,`expr $fxln + $fxlnk - 1`p" $fxarg | while read fxnamlk vol_lixo
				do
					ln $fxnam $fxnamlk 2>$null || cp $fxnam $fxnamlk 2>$null
				done
			}
		}
		[ $fxi ] && {
			# opcao -i, chk igual: 0 - istalado, 1 - nao instalado,
			# 		       5 - instalado incorretamente
			[ $fxchk ] || fxchk=$fxstat
			[ $fxchk -ne $fxstat ] && fxchk=5
		}
		shift 6 2> $null
	done
}

setln() {
	while	[ "$1" ]
	do	[ -z "$2" -o -z "$3" ] || eval $1$2=$3
		shift 3
	done
	return $OK
}


fixperm() {
	fxchk= fxi=
	# checa argumentos
	set -- `getopt "fwioucDld:" $* 2> $null` || {
		echo $msg61
		exit 1
	}
	while  case $1 in
		-f) fxf=true		;;
		-l) fxf=true; fxD=true	;;
		-w) fxw=true		;;
		-D) fxD=true		;;
		-u) fxu=true		;;
		-o) fxo=true		;;
		-i) fxi=true		;;
		-c) fxc=true		;;
		-d) fxid=$2 ; shift	;;
		--) shift; break	;;
		esac
	do	shift
	done
	fxarg="$*"
	# seta gid e uid
	setln `grep "^[ug]id" $fxarg`
	# seleciona lista ou executa
	if [ -n "$fxf" -o -n "$fxD" ]
	then	lstln `grep "^$fxid" $fxarg`
	else	execln `grep "^$fxid" $fxarg`
	fi
	unset fxstat fxout fxnam fxmod fxdon fxgrp fxf fxw fxo fxu fxD fxc fxid fxarq fxmak
	[ $fxi ] && return $fxchk
	unset fxi fxchk
	return $OK
}

# inicprdval()
# inicializa as variaveis de configuracao de produtos
inicprdval() {
	set=  prd=  ver=  typ=  rel=  mnt=  dev=  rdev=  lbl=  upd=  dir=  
	rcmd="tar x0"
}

# getprdvals()
# recebe como argumento o arquivo de configuracao e coleta todas as
# variaveis que identificao o produto (set, prd, ver, typ, etc)
getprdvals() {
	sed -n '/^#set=/s/#//p
		/^#prd=/s/#//p
		/^#ver=/s/#//p
		/^#typ=/s/#//p
		/^#rel=/s/#//p
		/^#mnt=/s/#//p
		/^#dev=/s/#//p
		/^#rdev=/s/#//p
		/^#lbl=/s/#//p
		/^#upd=/s/#//p
		/^#rcmd=/s/#//p
		/^#dir=/s/#//p' $*
}

# argprd()
# se foi passado um produto na linha de comando, devemos tentar localizar
# o seu arquivo de configuracao.
argprd() {
	# guardamos argumento, para nao perde-lo na chamada de outra rotina
	arg=$*
	[ -n "$arg" ] || return $FALHA
	# nao aceita mais de um produto
	[ $# -gt 1 ] && {
		tela $msg39
		erro "$msg0$msg6" 0
		return $FALHA
	}
	[ -f $cfg/$arg ] && {
		# verifica se existe arquivo de configuracao para produto
		prd0=`getprdvals $cfg/$arg`
		eval $prd0
		[ "$set" -a "$prd" -a "$rel" ] && return $OK
	}
	tela $msg39
	erro "$msg0$msg7 $arg"
	return $FALHA
}

# getcfg()
# recebe como argumento os arquivos (diretorio /tmp/perms) da midia
getcfg() {
	# guardamos argumento, para nao perde-lo na chamada de outra rotina
	arg=$*
	for i in $arg/*
	do	[ -f $i ] || continue
		inicprdval
		prd0=`getprdvals $i`
		eval $prd0
		[ "$set" -a "$prd" -a "$rel" ] && {
			# setamos a nova localizacao do configurador
			cfg=$arg
			return $OK
		}
	done
	return $FALHA
}

# getmenprd()
# procura em cfg os possiveis arquivos de configuracao instalados,
# coloca-os num menu de opcoes e apos a escolha do usuario, seta as 
# variaveis de configaracao do produto (prd, set, ver, rel, etc)
getmenprd() {
	menu=  ult=0
	# procura os arquivos de configuracao
	for i in $cfg/*
	do	[ -f $i ] || continue
		inicprdval
		prd0=`getprdvals $i`
		eval $prd0
		[ "$set" -a "$prd" -a "$rel" ] || continue
		ult=`expr $ult + 1`
		eval prd$ult='$prd0'
		menu="$menu\n\t$ult. $set"
	done
	# seta a opcao para incluir produto
	ultimo=`expr $ult + 1`
	menu="$menu\n\t$ultimo. $msg4"
	case $ult in
	0)	tela $msg39
		menu="$menu\n\n${msg9} (1)" 	;;
	*)	menu="$menu\n\n${msg9} (1-$ultimo)" 	;;
	esac
	while	tela $msg39
		leia $menu	
	do	case $resp in
		$ultimo)  getcfg /tmp/perms && return $OK
			  erro "$msg0$msg8" 0			;;
		*)       [ "$resp" -ge 1 -a "$resp" -lt "$ultimo" ] && {
				inicprdval
			  	eval prd0=\$prd$resp
			  	eval $prd0
			  	return $OK
			  }
			  erro "$msg0$msg9 (1-$ultimo)" 0
			  continue
		          ;;
		esac
	done
}

# getprd()
# vamos tentar pegar o produto como argumento, caso nao consigamos,
# vamos mostrar o menu de produtos instalados
getprd() {
	stat=1	# estamos no menu de produtos
	argprd $* && return $OK
	getmenprd
	return $?
}

# argdir()
# utiliza o diretorio passado como argumento como localizacao do produto
argdir() {
	# guardamos argumento, para nao perde-lo na chamada de outra rotina
	arg=$*
	case $arg in
	/*)	# so aceitamos percurso completo
		[ -d $arg ] || {
			tela $msg40
			erro "$msg1 $arg" 0
			return $FALHA
		}		;;
	*)	tela $msg40
		erro "$msg0$msg13 $arg" 0
		return $FALHA	;;
	esac
	dir=$arg
	cd $dir
	return $OK
}

# mudadir()
# mostra o diretorio default, usado como raiz pelo produto, e solicita 
# novo diretorio de localizacao do produto:
#	<ENTER>		- mantem o mesmo diretorio
#	<a> ou <A>	- abandona ic
#	diretorio 	- muda diretorio default p/ "diretorio"
mudadir() {
	tela $msg40
	# diretorio default eh /
	[ $dir ] || dir="/"
	while	leia "\n\n$set $msg11\n$dir\n$msg5 $msg26"
	do	# mantem default
		_mdresp=$resp
		[ $_mdresp ] || _mdresp=$dir
		case $_mdresp in
		/*)	# so aceitamos percurso completo
			;;
		*)	erro "$msg0$msg13 $_mdresp" 0
			continue	;;
		esac
		[ -d $_mdresp ] || {
			erro "$msg1 $_mdresp" 0
			getsn "$msg57" || continue
			mkdir $_mdresp
		}
		dir=$_mdresp
		break
	done
	cd $dir
	unset _mdresp
	return $OK
}

# subsdir()
# substitui (cria se nao existir) o valor de 'dir', dentro do arquivo 
# de configuracao, pelo valor escolhido em argdir ou mudadir
subsdir() {
	sed -n "/^#dir=/p" $cfg/$prd > $tmp
	# se existe modifica
	if [ -s $tmp ]
	then	sed "s?^#dir=\(.*\)?#dir=$dir?p" $cfg/$prd > $tmp
		mv $tmp $cfg/$prd
	# se nao existe cria apos variavel prd
	else	echo '/^#prd=/a\
#dir='$dir'
\' > $tmp
		sed -f $tmp $cfg/$prd > ${tmp}1
		mv ${tmp}1 $cfg/$prd
	fi
	return $?
}

# getdir()
# seta variaveis de diretorio para local de instalacao do produto
getdir() {
	ok=$FALHA
	[ $* ] && {
		argdir $*
		ok=$?
	}
	# se argdir retorna OK, nao precisamos de mudadir
	[ $ok -ne $OK ] && {
		stat=3	# estamos na manipulacao do diretorio do produto
		mudadir
		ok=$?
	}
	# se mudadir ou argdir retornou OK podemos substituir
	[ $ok -eq $OK ] && subsdir && ok=$?
	echo "$msg38 $set$msg41" > ${tmp}2
	sed -n "/^#!/s/#!/	/p" $cfg/$prd >> ${tmp}2
	return $ok
}

# getpckg()
# procura no argumento passado o possivel modulo selecionado ou TODOS para
# selecionar todos eles. Seta a variavei mod com o identificador deles e a
# variavel set$pckg com o nome de cada um deles (pckg eh o ident. do modulo)
getpckg() {
	arg=`echo $* | tr '[a-z]' '[A-Z]'`
	mod=
	[ "$arg" ] || return $OK
	if [ "$arg" = $TODOS ]
	then mod=`sed -n "s/^#!\([A-Z]*\)[ 	]*[0-9]*[ 	]*.*/\1/p" $cfg/$prd`
	else	for pckg in $arg
		do	[ `grep -c "^#!$pckg" $cfg/$prd` -ne 1 ] && mod= && break
			mod="$pckg $mod"
		done
	fi
	[ "$mod" ] && {
		for pckg in $mod
		do eval set$pckg=\"`sed -n "s/^#!$pckg[ 	]*[0-9]*[ 	]*\(.*\)/\1/p" $cfg/$prd`\"
		done
		return $OK
	}
	erro "$msg27" 1
	return $OK
}

# init()
# executa um arquivo de comandos que inicializa o produto
init() {
	for pckg in `echo $mod | tr '[A-Z]' '[a-z]'`
	do	[ -x $ICINIT$pckg ] || continue
		$CMDSHELL -c $ICINIT$pckg
		case $? in
		$OK|$STOP) return $OK	;;
		*)	eval mens \$set$pckg
			erro "$msg0$msg14" 0
			continue	;;
		esac
	done
}

# getarqvol()
# retorna o nome dos arquivos contidos no volume atual (vol),
# que sao passados como argumento
getarqvol() {
	sed -n "s/\([^	]*\)[ 	][ 	]*${vol}/\1/p" $1
#	sed -n "s/\([^	]*\)[ 	][ 	]*${vol}/\1/p" $1 | sed "s/[.][/]//"
}

# chkvol()
# monta e verifica rotulo para volume atual
chkvol() {
	mens $msg29
	LABEL=$pfx/prd=$prd/typ=$typ/rel=$rel/vol=$vol
	$rcmd $rdev $LABEL 2> $null || {
		erro "$msg0$msg20 $vol." 0
		return $FALHA
	}
	return $OK
}

# nvol()
# retorna os volumes existentes no arquivo de configuracao
nvol() {
	for pckg in $mod
	do	for resp in `sed -n "s/^[^ 	]*[ 	]*\([0-9]*\).*/\1/p" $tmp$pckg`
		do	eval [ \$V$resp ] && continue
			eval V$resp=true
			echo $resp
		done
	done
}

# selecpck()
# verifica se necessitamos escolher o pacote
selecpckg() {
	[ "$mod" ] && return $OK
	while	cat ${tmp}2
		leia $msg45
	do	getpckg $resp
		[ "$mod" ] && break
	done
}

# instprd()
# instala produto:
#	- verifica se exite versao anterior;
#	- mostra o espaco necessario para instalacao, e o disponivel 
#	  no sistema de seu arquivos;
#	- extrai os arquivos do produto, de cada um dos volumes;
#	- inicializa produto;
#	- verifica instalacao do produto.
instprd() {
	stat=8	# estamos na instalacao do produto
	tela $msg31
	selecpckg
	# verifica se existe versao instalada (parcial ou completa)
	mens $msg59
	for pckg in $mod
	do	verpckg $pckg
		[ $? -ne 1 ] && {
			echo
			eval mens \$set$pckg
			getsn $msg16$msg44 || return $OK
		}
	done
	# verifica espaco disponivel
	[ "$spc" -eq 1 ] || {
		erro "$msg18" 0
		df
		getsn $msg19$msg44 || return $OK
	}
	# cria um arquivo, para cada modulo, contendo os arquivos do modulo e 
	# o volume onde estes se encontram
	for pckg in $mod
	do	fixperm -fw -d$pckg $cfg/$prd > $tmp$pckg
	done
	stat=4	# estamos REALMENTE na instalacao do produto
	for vol in `nvol`
	do	for pckg in $mod
		do	eval $pckg=\`getarqvol $tmp$pckg\`
		done
		if [ "$typ" = disk ]
		then	rdev=$arqdisk$prd$vol
			[ -f $rdev ] || {
				erro "$msg0$msg10$rdev" 1
				return $FALHA
			}
			mens "\n$msg50$rdev ..."
		else
			if [ "`uname`" = "SunOS" ]
			then
				# vamos soltar o disquete
				eject
			fi
			while	getsn "\n$msg20 $vol - $set.$msg44" || return $OK
			do
				chkvol && break
				if [ "`uname`" = "SunOS" ]
				then
					# vamos soltar o disquete
					eject
				fi
			done
			mens $msg28
		fi
		for pckg in $mod
		do	# se nao existe arquivo pckg pegamos o proximo pckg
			eval [ -n \"\$$pckg\" ] || continue
			eval $rcmd $rdev \$$pckg 2> $null
		done
		eval unset \$V$resp
	done
	# executa inicializador
	init
	stat=8	# passamos da fase critica de instalacao do produto
	mens $msg60
	for pckg in $mod
	do	verpckg $pckg "-c"
	done
	# se inclui produto, mv arquivo cfg para /etc/perms e atualiza path de cfg
	[ $cfg != $CFGLOC ] && {
		[ -d $CFGLOC ] || mkdir $CFGLOC 2> $null || {
			erro "$msg15" 0
			fim $FALHA
		}
		mv $cfg/$prd $CFGLOC 2> $null
		cfg=$CFGLOC
	}
	# nao deve remover arquivo cfg
	rmcfg=
	mens $msg42
	leia $msg26
}

# dinstprd()
# desinstala produtos, do diretorio dir, mantendo o terminfo intacto 
dinstprd() {
	stat=9	# estamos na desinstalacao do produto
	tela $msg32
	selecpckg
	# checa se existe versao instalada
	# remove arquivos e diretorios do modulo
	for pckg in $mod
	do	verpckg $pckg
		[ $? -eq 1 ] && {
			eval erro \"$msg0\$set$pckg $msg36\" 0 
			continue
		}
		echo; eval mens \$set$pckg
		getsn $msg43$msg44 || break
		stat=5	# estamos REALMENTE na desinstalacao do modulo
		arqprd=`fixperm -fuo -d$pckg $cfg/$prd`
		rm -f $arqprd 2> $null
		arqprd=`fixperm -Duo -d$pckg $cfg/$prd`
		for dirrm in $arqprd
		do
			case "$dirrm" in
			/*)
				continue;
				;;
			*)
				rmdir $dirrm 2> $null
				;;
			esac
		done
		unset dirrm
		stat=9	# passamos da fase critica de desinstalacao do modulo
	done
	for pckg in $mod
	do	verpckg $pckg
		[ $? -ne 1 ] && eval erro \"$msg0$msg22 \$set$pckg\" 0
	done
	# deve remover arquivo cfg
	#rmcfg=s
	leia $msg26
	return $OK
}
	
# verpckg()
# checa e emiti mensagem do estado do produto (instalado, instalado 
# parcialmente e nao instalado).
# recebe como argumento:
#	- nome do pacote;
#	- 0 ou 1 se deseja mensagem de estado;
#	- opcao adicional para o fixperm (opcional).
verpckg() {
	pckg=$1; shift
	[ "$1" = "-c" ] && {
		pckg="$pckg -c"
		shift 
	}
	msgarg=$*
	fixperm -i -d$pckg $cfg/$prd
	case $? in
	$OK) [ "$msgarg" ] && erro "$msgarg $msg35" 0
	   return $OK		;;
	$FALHA) [ "$msgarg" ] && erro "$msgarg $msg36" 0
	   return $FALHA	;;
	5) [ "$msgarg" ] && erro "$msgarg $msg37" 0
	   return 5		;;
	esac
}

# verifprd()
# verifica instalacao do produto e do terminfo
verifprd() {
	stat=6	# estamos na verificacao do produto
	tela $msg33
	selecpckg
	# verifica pckg
	for pckg in $mod
	do	eval verpckg $pckg \"\$set$pckg\"
	done
	leia "\n$msg26"
	return $OK
}
	
# persprd()
# inicializa produto utilizando o script de inicializacao, se nao conseguir 
# encontra-lo sistema de arquivos do usuario, solicite volume de distribuicao 
# onde ele se encontra
persprd() {
	stat=7	# estamos na inicializacao de produto
	tela $msg34
	selecpckg
	for pckg in $mod
	do	# checa se existe versao instalada
		verpckg $pckg
		[ $? -eq 1 ] && {
			eval erro \"$msg0\$set$pckg $msg36\" 1
			continue
		}
		[ ! -x $ICINIT$prd ] && {
			# procure no arquivo de configuracao, o volume onde ele esta
			fixperm -fw -d$pckg $cfg/$prd > $tmp
			eval arqprd=\`grep \"init.`echo $pckg | tr '[A-Z]' '[a-z]'`\" $tmp\`
			[ "$arqprd" ] || continue
			vol=`expr "$arqprd" : ".*[	 ][	 ]*\([0-9]*\)"`
			arqprd=`expr "$arqprd" : "\(.*\)[	 ][	 ]*[0-9]*"`
			if [ "$typ" = disk ]
			then	rdev=$arqdisk$prd$vol
				[ -f $rdev ] || {
					erro "$msg0$msg10$rdev" 1
					return $FALHA
				}
			else	while	getsn "\n$msg20 $vol - $set.$msg44" || return $OK
				do	chkvol && break
				done
			fi
			mens $msg28
			eval $rcmd $rdev $arqprd 2> $null
		}
	done
	init
	leia $msg26
	return $OK
}
	

# main()

# inicializa variaveis
mod=  cmd=  arglst=  loc=  lcmd=  ult=0  resp=  spc=  rmcfg=  stat=0

# protecao contra definicoes errada nos arquivos de permissoes
[ -f $CFGLOC/dm ] && grep "/usr[ 	]" $CFGLOC/dm > $null 2>&1 && {
	rm -f $CFGLOC/dm
}
[ -f $ETC/dm.perms ] && grep "/usr[ 	]" $ETC/dm.perms > $null 2>&1 && {
	rm -f $ETC/dm.perms
}

# deve executar fim caso haja interrupcao
trap 'aborta' 1 2 3 15

# avalia argumentos
[ $# != 0 ] && {
	set -- `getopt m:idvpsl: $* 2> $null` || {
		mens $msg24
		exit $FALHA
	}
	while  case $1 in
		-i) cmd='instprd' ; lcmd=s	;;
		-d) cmd='dinstprd' ; lcmd=s	;;
		-v) cmd='verifprd' ; lcmd=s	;;
		-p) cmd='persprd' ; lcmd=s	;;
		-l) loc=`echo $2 | tr : " "`; shift	;;
		-m) mod=`echo $2 | tr '[a-z]' '[A-Z]'`; shift	;;
		-s) spc=1				;;
		--) shift; break			;;
		esac
	do	shift
	done
	arglst="$*"
}

# teste de permissao para uso do ic
permroot || {
	erro "$msg0$msg23" 0
	exit $FALHA
}

# inicializa variaveis do produto
inicprdval

# seleciona produto
# local de manupulacao para o produto (diretorios)
getprd $arglst && getdir $loc && getpckg $mod
[ $? -ne $OK ] && fim $OK

# identificador do produto
PRD=`echo $prd | tr '[a-z]' '[A-Z]'`
# executa linha de comando
[ $cmd ] && {
	eval $cmd
	fim $?
}

# menu principal
stat=0
while	tela
	leia "\n\t$set $msg25 $msg9 (1-5)"
do	case $resp in
	1) instprd	;;
	2) dinstprd	;;
	3) verifprd	;;
	4) persprd	;;
	5) fim $OK	;;
	*) erro "$msg0$msg9 (1-5)" 1	;;
	esac
	acao=$msg63   # vamos sempre limpar a tela
	stat=0	# finalizamos uma acao
	mod=
done
