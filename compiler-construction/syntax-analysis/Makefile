#
CC=gcc
LEX=flex
YFLAGS=-Wcounterexamples
PLANG=simula

% : %.tab.o %.o 
	$(CC) $< $*.o $(LDFLAGS) -o $@

%.tab.c %.tab.h: %.y
	bison $(YFLAGS) -d $<

%.c: %.l %.tab.h
	$(LEX) -t $< > $@

$(PLANG): $(PLANG).y $(PLANG).l

