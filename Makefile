CC=gcc
FILES=tetris.c
EXE=tetris
TEST=test.c
INSTALL=/usr/local/bin/

build:
	$(CC) $(FILES) -o $(EXE)

run:
	$(CC) $(FILES) -o $(EXE) && ./$(EXE)

clean:
	rm -rf $(EXE) a.out

install:
	sudo mv $(EXE) $(INSTALL)

uninstall:
	sudo rm $(INSTALL)$(EXE)
