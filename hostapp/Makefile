all: hostapp synergy-hw

hostapp: hostapp.c
	gcc -o hostapp hostapp.c -lusb

synergy-hw: synergy-hw.c uSynergy.c uSynergy.h
	gcc -o synergy-hw synergy-hw.c uSynergy.c -lusb

clean:
	rm -f *.o hostapp synergy-hw
