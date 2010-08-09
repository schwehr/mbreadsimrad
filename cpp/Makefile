# -*- makefile-gmake -*- Tell emacs this is a GNU Makefile

CFLAGS := -Wall
CFLAGS += -Wimplicit 
CFLAGS += -pedantic 
CFLAGS += -W 
CFLAGS += -Wredundant-decls
CFLAGS += -Werror
CFLAGS += -Wstrict-prototypes

DEBUG := -g3 -O0 
#-D_GLIBCXX_DEBUG

CXXOPTIMIZE := -O3 -funroll-loops -fexpensive-optimizations -ffast-math -DNDEBUG
CXXWARNINGS := -pedantic -Wall -W -Wimplicit -Wredundant-decls -Wmissing-prototypes 

# Allow foo = "string"
CXXWARNINGS += -Wno-write-strings

ifdef OPTIMIZE
  CXX := g++-fsf-4.5 # Specific to fink
  CXXFLAGS := ${CXXOPTIMIZE}
else
  CXXFLAGS := ${DEBUG} ${CXXWARNINGS}
endif

test-simrad: test-simrad.o simrad.o 
	${CXX} ${CXXFLAGS} -o $@ $^

libsimrad.a: simrad.o simrad.h
	ar -r $@ simrad.o
	ranlib libsimrad.a

simrad.o: simrad.cpp simrad.h

clean:
	rm -f *.a test-simrad *.o

BASE:=mbreadsimrad-0.1-20100809
TAR:=${BASE}.tar
tar:
	rm -rf ${BASE}
	mkdir ${BASE}
	cp *.h *.cpp Makefile ${BASE}/
	tar cf ${TAR} ${BASE}
	bzip2 -9 ${TAR}
	rm -rf ${BASE}