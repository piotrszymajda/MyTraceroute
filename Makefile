CC = clang #g++
FLAGS = -std=c++11 -Wall -Wextra -g -pedantic -pedantic-errors -O2
NAME = MyTraceroute
OBJS = traceroute

all: $(OBJS) 

$(OBJS): %: %.o
	$(CC) $@.o -o $(NAME)

traceroute.o:
	$(CC) -c $(FLAGS) traceroute.cpp

clean:
	rm -f *.o
	
distclean:
	rm -f $(NAME)
	
.PHONY: all clean distclean
