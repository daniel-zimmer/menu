build: bin menu

clean:
	rm -rf bin menu

.PHONY: build clean

##################################################

# creates bin directory if it does not exist
bin:
	mkdir -p bin

bin/hashmap.o: src/hashmap.c src/hashmap.h
	gcc -Wall -c $< -o $@

bin/menu.o: src/menu.o
	gcc -Wall -c $< -o $@

menu: bin/menu.o bin/hashmap.o
	gcc -Wall $^ -o $@

