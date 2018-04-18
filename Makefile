CPP      = g++
#CC       = gcc.exe
#WINDRES  = windres.exe
OBJ      = Allele.o epistasis.o ga.o gaba.o main.o
LINKOBJ  = Allele.o epistasis.o ga.o gaba.o main.o
LIBS     = -lm -lz -lpthread
#INCS     = 
#CXXINCS  = 
BIN      = GIM
CXXFLAGS = 
#CFLAGS   = $(INCS) 
#RM       = rm.exe -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

Allele.o: Allele.cpp
	$(CPP) -c Allele.cpp -o Allele.o $(CXXFLAGS)

epistasis.o: epistasis.cpp
	$(CPP) -c epistasis.cpp -o epistasis.o $(CXXFLAGS)

ga.o: ga.cpp
	$(CPP) -c ga.cpp -o ga.o $(CXXFLAGS)

gaba.o: gaba.cpp
	$(CPP) -c gaba.cpp -o gaba.o $(CXXFLAGS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)
