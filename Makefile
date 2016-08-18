CXX  = g++ -std=c++14
OPTS = -O2 -Wall -Werror -pthread
OUTPUT = metrics

OBJS = main.o vm.o cppshell/cppshell.o cppshell/subprocess.o
LIBS = perfdata.a

all: $(OBJS)
	$(CXX) $(OPTS) $(OBJS) $(LIB) -o $(OUTPUT)

%.o: %.cpp
	$(CXX) -c $(OPTS) $< -o $@

clean:
	$(RM) $(OBJS) $(OUTPUT)

