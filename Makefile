SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
NAME=dzen++
FLAGS_FILE=.compiler_flags

CXXFLAGS+=-pedantic -Wall -O2 -std=c++1y

all: $(NAME)

.PHONY: force
$(FLAGS_FILE): force
	@echo "Checking compiler flags ..."
	@echo '$(CXXFLAGS)' | cmp -s - $@ || echo '$(CXXFLAGS)' > $@

%.o: %.cpp $(FLAGS_FILE)

$(OBJ): $(FLAGS_FILE)

$(NAME): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJ)

debug: CXXFLAGS+=-g -DDEBUG
debug: all

.PHONY: clean
clean:
	@$(RM) -v $(OBJ) $(BIN)
	@$(RM) -v $(FLAGS_FILE)
