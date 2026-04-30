build:
	gcc -Wall -std=c99 -I/opt/homebrew/opt/sdl2/include ./src/*c -L/opt/homebrew/opt/sdl2/lib -lSDL2 -o see-renderer

run:
	./see-renderer

clean:
	rm see-renderer
