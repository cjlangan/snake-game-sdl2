all: game

.PHONY: clean

game: game.c
	clang -Wall game.c -lSDL2 -lSDL2main -o game

clean:
	rm -f game
