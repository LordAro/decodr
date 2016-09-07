
OBJDIR=obj
TESTDIR=test

CC=clang
CXX=clang++
CXXFLAGS=-Wall -Wextra -pedantic -g -std=c++14

CXXFILES=decodr.cpp binary.cpp binaryreader.cpp

OBJFILES=$(addprefix $(OBJDIR)/,$(CXXFILES:.cpp=.o))
TARGET=decodr

TESTBIN=true helloworld count
TESTTARGETS=$(addprefix $(TESTDIR)/,$(TESTBIN))

all: $(TARGET)

tests: $(TESTTARGETS)

$(TESTDIR)/%: $(TESTDIR)/%.c
	$(CC) -o $@ $@.c

$(TARGET): $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJFILES)

run: $(TESTTARGETS) $(TARGET)
	@for t in $(TESTTARGETS);\
	do ./$(TARGET) $$t;\
	done

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	for i in $(OBJFILES) $(TARGET) $(TESTTARGETS);\
	do if test -f $$i; then rm $$i; fi;\
	done

.PHONY: all tests run clean
