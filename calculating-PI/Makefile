TARGET=crunch_exec
DEPS=pi_number_counter
DEPS:=$(addsuffix .o, $(DEPS))

CC=gcc
CFLAGS=-Os -march=native -fomit-frame-pointer -lm -lgmp -lpthread

.PHONY: all clean

all:	$(TARGET)

$(TARGET): $(DEPS)
	$(CC) $(addsuffix .c, $(TARGET)) $(DEPS) $(CFLAGS)  -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	@echo Tidying things up...
#	-rm -f $(TARGET)
#	-rm -f $(DEPS)
	-rm -f *.o $(TARGET)

	


