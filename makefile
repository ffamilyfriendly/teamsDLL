NAME = teams
ifndef OS
    $(error Specify OS=[win32|linux])
endif

ifeq (${OS}, win32)
	FILEEND = dll
else
	FILEEND = so
endif

FLAGS = \
	-shared \
	-lcurl \
	-fPIC

FILES = \
	./src/main.cpp

all:
	g++ ${FILES} ${FLAGS} -o lib${NAME}.${FILEEND}