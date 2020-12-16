DESTDIR ?=
CFLAGS ?=
LDFLAGS ?=-lncurses
prefix ?= /usr

all: SSS Safety-app Control-app

SSS: SSS.o
	$(CC) Safety-signal-source.o $(LDFLAGS) -o Signalsource

Safety-app: Safety-app.o
	$(CC) safety-app.o $(LDFLAGS) -o safety-app

Control-app: Control-app.o
	$(CC) control-app.o $(LDFLAGS) -o Signalsource-control-panel	

SSS.o:
	$(CC) $(CFLAGS) -c Safety-signal-source.c

Safety-app.o:
	$(CC) $(CFLAGS) -c safety-app.c

Control-app.o:
	$(CC) $(CFLAGS) -c control-app.c

install: all
	install -m 0755 -d $(DESTDIR)/$(prefix)/bin/
	install -m 0755 Signalsource $(DESTDIR)/$(prefix)/bin/
	install -m 0755 safety-app $(DESTDIR)/$(prefix)/bin/
	install -m 0755 Signalsource-control-panel $(DESTDIR)/$(prefix)/bin/
clean:
	rm Safety-signal-source.o
	rm safety-app.o
	rm Signalsource
	rm safety-app
	rm control-app.o
	rm Signalsource-control-panel