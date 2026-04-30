build:
	gcc -Wall -std=c99 ./src/*c -o see-renderer

run:
	./see-renderer

clean:
	rm see-renderer
