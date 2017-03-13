.PHONY: all install

all: addrg

addrg: addrg.c
	gcc -o addrg addrg.c -Wall -O2 -lhts -lz -lpthread -I$(PREFIX)/include -L$(PREFIX)/lib

install: addrg
	install addrg $(PREFIX)/bin
