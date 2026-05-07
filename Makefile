SRCS := $(filter-out ./src/upng.c, $(wildcard ./src/*.c))

build:
	@gcc -std=c99 -I/opt/homebrew/opt/sdl2/include -c ./src/upng.c -o upng.o
	@gcc -Wall -std=c99 -I/opt/homebrew/opt/sdl2/include $(SRCS) upng.o \
		-L/opt/homebrew/opt/sdl2/lib -lSDL2 -lm -o gevurah
	@echo "Build complete"
run:
	@./gevurah
clean:
	@rm -f gevurah upng.o
	@echo "Cleaned"
