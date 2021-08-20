build: bin menu

clean:
	rm -rf bin menu

install:
	sudo mv menu /usr/local/bin

.PHONY: build clean install

##################################################

# creates bin directory if it does not exist
bin:
	mkdir -p bin

bin/hashmap.o: src/hashmap/hashmap.c src/hashmap/hashmap.h
	(cd src/hashmap; make build)
	mv src/hashmap/hashmap.o bin/

bin/menu.o: src/menu.c
	gcc -Wall -c $< -o $@ -O3 -g

menu: bin/menu.o bin/hashmap.o
	gcc -Wall $^ -o $@ -O3 -g
	strip menu
