CFLAGS = -Wall -lncurses
LDFLAGS = -lncurses

# sudo apt install libncurses5-dev libncursesw5-dev

all:
	@echo "Compiling all files"
	@g++ $(CFLAGS) *.cpp -o main.out $(LDFLAGS)

clean:
	@echo "Removing output files"
	rm main.out
