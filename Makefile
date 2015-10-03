LINK=-lm -lpthread
ODIR=./obj
OUT=./bin/Trab1Conc
SRC=./src
INC=-Iinc
OPT=-O2 -fopenmp

_OBJS_SEQ = mainSequential.o matrix_reader.o
_OBJS_PAR = mainParallel.o matrix_reader.o threadpool.o
_OBJS_OMP = mainOMP.o matrix_reader.o threadpool.o

OBJS_SEQ = $(patsubst %, $(ODIR)/%, $(_OBJS_SEQ))
OBJS_PAR = $(patsubst %, $(ODIR)/%, $(_OBJS_PAR))
OBJS_OMP = $(patsubst %, $(ODIR)/%, $(_OBJS_OMP))


all: sequential parallel omp
	
sequential: $(OBJS_SEQ)
	gcc $(OPT) -o $(OUT)_seq $(OBJS_SEQ) $(LINK)

parallel: $(OBJS_PAR)
	gcc $(OPT) -o $(OUT)_par $(OBJS_PAR) $(LINK)

omp: $(OBJS_OMP)
	gcc $(OPT) -o $(OUT)_omp $(OBJS_OMP) $(LINK)  

$(ODIR):
	mkdir -p $(ODIR)


$(ODIR)/%.o: $(SRC)/%.c | $(ODIR) 
	gcc $(OPT) -Wall -c $< $(INC) -o $@

test:
	sh ./script/gen_data.sh

run:
	$(OUT)

clean:
	rm -f $(ODIR)/*.o $(OUT)

zip:
	zip -r Trab1-7153362 *
