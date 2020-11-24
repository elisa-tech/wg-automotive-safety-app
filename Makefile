DESTDIR ?=
CFLAGS ?=
LDFLAGS ?=
prefix ?= /usr

all: SSS

SSS: SSS.o
	$(CC) Safety-signal-source.o $(LDFLAGS) -o Signalsource

SSS.o:
	$(CC) $(CFLAGS) -c Safety-signal-source.c

install: all
	install -m 0755 -d $(DESTDIR)/$(prefix)/bin/
	install -m 0755 Signalsource $(DESTDIR)/$(prefix)/bin/

clean:
	
