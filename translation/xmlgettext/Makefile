# executable file for project
RUN = xmlgettext

# flags (c-compiler)
CFLAGS =

# flags (c++-compiler)
CXXFLAGS = -Wall -I./src

# libs (linker)
LDLIBS =

# object files
OBJS = $(shell cat ./Makefile.objs )



build: compile $(RUN)

compile: $(OBJS)

$(RUN): compile
	$(CXX) $(CXXFLAGS) -o $(RUN) $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

run: $(RUN)
	./$(RUN)

clean:
	rm -f $(OBJS) 

