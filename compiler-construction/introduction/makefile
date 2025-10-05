

# Plik Makefile do pierwszego zadania
#
# U W A G A ! ! !
#
# Kopiowanie przez ,,kopiuj i wklej'' = PROBLEMY!!!
#
# W tym pliku znajduja sie znaki tabulacji poziomej.
# Kopiowanie metoda kopiuj i wklej zamieni je na odstepy!!!

# Dolacz biblioteke fleksa
LDFLAGS=-ll

# Kompilatorem jezyka C jest gcc
CC=gcc

# Do kompilacji plikow z roszerzeniem ,,l'' uzyj programu flex
LEX=flex

# Plik w jezyku C skompiluj z uzyciem kompilatora CC (czyli gcc)
# z uzyciem opcji kompilatora C, opcji CPPFLAGS preprocesora CPP
# i opcji LDFLAGS programu laczacego. Opcje te mozna podawac
# w wierszu polecen, np. make CFLAGS=-g (do dolaczenia informacji
# dla programu uruchomieniowego)
% : %.c
	${CC} $< $(CFLAGS) $(CPPFLAGS) ${LDFLAGS} -o $@

# Polecenie make bez parametrow skompiluje plik z1.l
# Do kompilacji innych programow podaj nazwe programu jako parameter,
# np. make z2, lub dopisz nowa, analogiczna regule przed biezaca.
z1: z1.l

