CC=gcc
CFLAGS=-I $(IDIR)

IDIR = include
ODIR = obj
SDIR = src

_DEPS = Lista.h comp.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = Lista.o comp.o expr.tab.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all:  
	cd flexbison; make
	cd ./; make arxs

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

arxs: $(OBJ)
	gcc -o $@ $(SDIR)/expr.c $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*.c
