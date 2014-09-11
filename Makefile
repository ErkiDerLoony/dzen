CPP=c++
CPPFLAGS=-pedantic -Wall -O2 -std=c++1y
NAME=dzen++

all: $(NAME)

$(NAME): *.cpp
	$(CPP) -o $(NAME) $(CPPFLAGS) *.cpp

debug: CPPFLAGS+=-g -DDEBUG
debug: all

clean:
	$(RM) -v dzen++
