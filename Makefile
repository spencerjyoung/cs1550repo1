CC = gcc
CFLAGS = -std=gnu99

snake: snake.c
	$(CC) $(CFLAGS) -o snake snake.c
library: library.c
	$(CC) $(CFLAGS) -o library library.c
	
clean:
	-rm -f *.o