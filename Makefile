LINK=-lm -lpthread
ODIR=./obj
OUT=./bin/Trab1Conc
SRC=./src
INC=-Iinc

_OBJS_SEQ = mainSequential.o matrix_reader.o
_OBJS_PAR = mainParallel.o matrix_reader.o threadpool.o

OBJS_SEQ = $(patsubst %, $(ODIR)/%, $(_OBJS_SEQ))
OBJS_PAR = $(patsubst %, $(ODIR)/%, $(_OBJS_PAR))


all: sequential parallel
	
sequential: $(OBJS_SEQ)
	gcc -g -o $(OUT)_seq $(OBJS_SEQ) $(LINK)

parallel: $(OBJS_PAR)
	gcc -g -o $(OUT)_par $(OBJS_PAR) $(LINK)

$(ODIR):
	mkdir -p $(ODIR)


$(ODIR)/%.o: $(SRC)/%.c | $(ODIR) 
	gcc -g -Wall -c $< $(INC) -o $@

run:
	$(OUT)

clean:
	rm -f $(ODIR)/*.o $(OUT)

zip:
	zip -r Trab1-7153362 *