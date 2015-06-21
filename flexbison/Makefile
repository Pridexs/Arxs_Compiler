LEX=flex
BIS=bison
ODIR=../obj

all: $(ODIR)/lex.yy.c $(ODIR)/expr.tab.c

$(ODIR)/lex.yy.c: expr.l
	$(LEX) -o $(ODIR)/lex.yy.c expr.l 

$(ODIR)/expr.tab.c: expr.y
	$(BIS) -o $(ODIR)/expr.tab.c expr.y