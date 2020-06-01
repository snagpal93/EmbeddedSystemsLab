CFLAGS:=-DPC -O0 -g3 -Wall -Wextra -DVERBOSE -std=c99
HEADERS:=$(wildcard *.h)
SOURCES=$(wildcard *.c )
OBJECTS=$(SOURCES:.c=.o)

all: jpg_app

$(OBJECTS): $(HEADERS)

jpg_app: $(OBJECTS)
	$(CC) -o $@ $^

clean:
	rm -f $(OBJECTS) jpg_app
