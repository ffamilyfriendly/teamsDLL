NAME = teams
ifndef OS
    $(info Specify OS=[win32|linux]. defaults to linux)
	OS = linux
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