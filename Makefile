DESTDIR ?=
CFLAGS ?=
LDFLAGS ?=
prefix ?= /usr

all: SSS Safety-app

SSS: SSS.o
	$(CC) Safety-signal-source.o $(LDFLAGS) -o Signalsource

Safety-app: Safety-app.o
	$(CC) safety-app.o $(LDFLAGS) -o safety-app

SSS.o:
	$(CC) $(CFLAGS) -c Safety-signal-source.c

Safety-app.o:
	$(CC) $(CFLAGS) -c safety-app.c

install: all
	install -m 0755 -d $(DESTDIR)/$(prefix)/bin/
	install -m 0755 Signalsource $(DESTDIR)/$(prefix)/bin/
	install -m 0755 safety-app $(DESTDIR)/$(prefix)/bin/
clean:
	rm Safety-signal-source.o
	rm safety-app.o
	rm Signalsource
	rm safety-app