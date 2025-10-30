#
CC=gcc
LEX=flex
YFLAGS=
PLANG=simula
# -Wcounterexamples
% : %.tab.o %.o 
	$(CC) $< $*.o $(LDFLAGS) -o $@

%.tab.c %.tab.h: %.y
	bison $(YFLAGS) -d $<

%.c: %.l %.tab.h
	$(LEX) -t $< > $@

$(PLANG): $(PLANG).y $(PLANG).l

