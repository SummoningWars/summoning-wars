DEFINES =
LIBS = OGRE OIS
CXX = g++
CXXFLAGS = $(shell pkg-config --cflags $(LIBS)) $(DEFINES) -c
LD = g++
LDFLAGS = $(shell pkg-config --libs $(LIBS))

all:
	$(CXX) $(CXXFLAGS)  -o sumwarsframelistener.o sumwarsframelistener.cpp
	$(CXX) $(CXXFLAGS)  -o main.o main.cpp	
	$(CXX) $(LDFLAGS) -o meshtest main.o sumwarsframelistener.o

clean:
	rm -f meshtest main.o sumwarsframelistener.o main.o

