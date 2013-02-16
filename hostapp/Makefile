all: hostapp

hostapp: hostapp.c
	gcc -o hostapp hostapp.c -lusb

clean:
	rm -f *.o hostapp
