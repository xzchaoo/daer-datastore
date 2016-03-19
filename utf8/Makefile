PROGRAM = project

CXX = g++

CPP_FILES = $(wildcard *.cpp)
H_FILES = $(wildcard *.h)

all: $(PROGRAM)

$(PROGRAM): $(CPP_FILES) $(H_FILES)
	$(CXX) -o $(PROGRAM) $(CPP_FILES)

clean:
	rm -f *.o *.a $(PROGRAM) *.key *.data
