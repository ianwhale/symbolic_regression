# Declaration of variables
CC = mpic++
CC_FLAGS = --std=c++11

# File names
EXEC = run.out
SOURCES = $(wildcard ./gp/*.cpp) $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) -fopenmp

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
