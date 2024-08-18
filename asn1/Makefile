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

ifdef OPTIMIZE
  CXX := g++-fsf-4.5 # Specific to fink
  CXXFLAGS := ${CXXOPTIMIZE}
else
  CXXFLAGS := ${DEBUG} ${CXXWARNINGS}
endif

cpp: asn1-cpp
	./asn1-cpp ../0018_20050728_153458_Heron.all

# -m32 for 32 bit old school
# -m64 for 64 bit modern code
asn1-cpp: asn1.cpp
	${CXX} -o asn1-cpp asn1.cpp ${CXXFLAGS}

clean:
	rm -f asn1-cpp
