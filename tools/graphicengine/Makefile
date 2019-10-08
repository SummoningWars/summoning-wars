DEFINES = -DDEBUG_LEVEL=5
LIBS = OGRE OIS freealut openal
CXX = g++
CXXFLAGS = -g $(shell pkg-config --cflags $(LIBS)) $(DEFINES) -c -I../../src/gui -I../../src/core -I../../src/tinyxml
LD = g++
LDFLAGS = $(shell pkg-config --libs $(LIBS))
OBS = sumwarsframelistener.o main.o ../../src/gui/graphicobject.o ../../src/gui/graphicmanager.o ../../src/gui/graphicobjectrenderinfo.o ../../src/gui/sound.o tinyxml.o tinyxmlparser.o tinyxmlerror.o tinystr.o ../../src/core/elementattrib.o ../../src/core/logger.o

.SUFFIXES:
.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@

all : $(OBS)
	$(CXX) $(LDFLAGS) $(OBS) -o graphicengine

clean:
	rm -f graphicengine $(OBS) 

