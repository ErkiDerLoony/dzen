FLAGS_FILE=.compiler_flags

all: fuzzy_clock
	./fuzzy_clock || exit 1
	@echo -e '\033[32;1mAll tests successful.\033[0m'

.PHONY: force
$(FLAGS_FILE): force
	@echo '$(CXXFLAGS)' | cmp -s - $@ || echo '$(CXXFLAGS)' > $@

debug: CXXFLAGS+=-g -DDEBUG
debug: all

fuzzy_clock.o: fuzzy_clock.cpp $(FLAGS_FILE)

fuzzy_clock: fuzzy_clock.o ../fuzzy_clock.o ../module.o
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) ../fuzzy_clock.o ../module.o $<

.PHONY: clean
clean:
	@$(RM) -v fuzzy_clock.o

.PHONY: mrproper
mrproper: clean
	@$(RM) -v fuzzy_clock
