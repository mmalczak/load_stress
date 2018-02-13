all:
	$(CC) lstress.c -o lstress -lm
clean:
	rm lstress