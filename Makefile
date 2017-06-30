SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
NAME=dzen++
FLAGS_FILE=.compiler_flags
DEP_FILE=.depend

CXXFLAGS+=-pedantic -Wall -O3 -std=c++14

all: depend $(NAME)

depend: $(DEP_FILE)

$(DEP_FILE): $(SRC)
	$(RM) $(DEP_FILE)
	$(CXX) $(CXXFLAGS) -MM $^>> $(DEP_FILE);

include $(DEP_FILE)

.PHONY: force
$(FLAGS_FILE): force
	@echo '$(CXXFLAGS)' | cmp -s - $@ || echo '$(CXXFLAGS)' > $@

%.o: %.cpp $(FLAGS_FILE)

$(OBJ): $(FLAGS_FILE)

$(NAME): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ)

debug: CXXFLAGS+=-g -DDEBUG
debug: all

.PHONY: test
test: all
	make -C test

.PHONY: debugtest
debugtest: debug
	make -C test debug

.PHONY: clean
clean:
	@$(RM) -v $(OBJ) $(BIN)
	@$(RM) -v $(FLAGS_FILE)
	make -C test clean

.PHONY: mrproper
mrproper: clean
	@$(RM) -v $(NAME)
	make -C test mrproper
