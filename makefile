CFLAGS = -Wno-int-conversion -O3 -g

build: bin menu
	mkdir -p ~/.cache/menu

clean:
	rm -rf bin menu

uninstall: 
	sudo rm /usr/local/bin/menu

install:
	mkdir -p ~/.cache/menu
	strip menu
	sudo mv menu /usr/local/bin

.PHONY: build clean install uninstall

##################################################

# creates bin directory if it does not exist
bin:
	mkdir -p bin

bin/hashmap.o: src/hashmap/hashmap.c src/hashmap/hashmap.h
	(cd src/hashmap; make build)
	mv src/hashmap/hashmap.o bin/

bin/helper.o: src/helper.c src/helper.h
	gcc -Wall ${CFLAGS} -c $< -o $@

bin/cache.o: src/cache.c src/cache.h
	gcc -Wall ${CFLAGS} -c $< -o $@

bin/menu.o: src/menu.c
	gcc -Wall ${CFLAGS} -c $< -o $@

menu: bin/menu.o bin/hashmap.o bin/cache.o bin/helper.o
	gcc -Wall ${CFLAGS} $^ -o $@
