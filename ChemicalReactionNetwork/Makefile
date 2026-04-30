CXX = clang++
CXXFLAGS = -g -Wall -std=c++17
TARGET = ChemReax
SOURCE_DIR = ./src/
SOURCES = $(shell find $(SOURCE_DIR) -name '*.cpp')
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

LIBS = 
LDFLAGS +=
LIBS += -
IFLAGS += -I$(SOURCE_DIR)
CXXFLAGS += $(IFLAGS)

all: $(TARGET)
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)
