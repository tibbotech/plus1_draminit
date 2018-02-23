if [ "$1" != 1 ];then
	vim include/config.h
else
	vim Makefile.in
fi
