all: bin/adquisidor

bin/adquisidor: src/adquisidor.c
	gcc -o bin/adquisidor -lpcap src/adquisidor.c

clean:
	rm -rf bin/adquisidor
