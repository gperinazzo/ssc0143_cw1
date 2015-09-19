LINK=-lm
ODIR=./obj
OUT=./bin/Trab1Conc
SRC=./src
INC=-Iinc

_OBJS = mainSequential.o matrix_reader.o

OBJS = $(patsubst %, $(ODIR)/%, $(_OBJS))


all: $(OBJS)
	gcc -o $(OUT) $(OBJS) $(LINK)  

$(ODIR):
	mkdir -p $(ODIR)


$(ODIR)/%.o: $(SRC)/%.c | $(ODIR) 
	gcc -Wall -c $< $(INC) -o $@

run:
	$(OUT)

clean:
	rm -f $(ODIR)/*.o $(OUT)

zip:
	zip -r Trab1-7153362 *