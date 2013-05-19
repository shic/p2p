# commento

# definizione delle variabili
CC=gcc
CFLAGS=-Wall -Wextra -O2
CFILES=$(shell ls *.c)
PROGS=$(CFILES:%.c=%)

# NOME: PREREQUISITO
# [TAB] COMANDO

all: $(PROGS)

# %: %.c
# 	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(PROGS) *.o
