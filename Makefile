INC_DIR = include
SRC_DIR = source
SOURCES =  $(SRC_DIR)/main.cc $(SRC_DIR)/game.cc $(SRC_DIR)/state.cc
OBJECTS = $(SOURCES:.cc=.o)
TARGET = Chess_engine
CXX = g++
CFLAGS = -fPIC -Wall -Wextra
CPPFLAGS = -I$(INC_DIR)
LFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LFLAGS)