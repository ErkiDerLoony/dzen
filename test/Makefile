FLAGS_FILE=.compiler_flags

all: test_fuzzy_clock
	./test_fuzzy_clock

.PHONY: force
$(FLAGS_FILE): force
	@echo '$(CXXFLAGS)' | cmp -s - $@ || echo '$(CXXFLAGS)' > $@

debug: CXXFLAGS+=-g -DDEBUG
debug: all

test_fuzzy_clock.o: test_fuzzy_clock.cpp $(FLAGS_FILE)

test_fuzzy_clock: test_fuzzy_clock.o ../fuzzy_clock.o ../module.o
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) ../fuzzy_clock.o ../module.o $<

.PHONY: clean
clean:
	@$(RM) -v test_fuzzy_clock.o

.PHONY: mrproper
mrproper: clean
	@$(RM) -v test_fuzzy_clock
