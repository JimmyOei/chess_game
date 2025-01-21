INC_DIR = include
SRC_DIR = source
LIB = /opt/homebrew/lib # For MacOS
INC_HOMEBREW = /opt/homebrew/include # For MacOS
SOURCES =  $(SRC_DIR)/main.cc $(SRC_DIR)/game.cc $(SRC_DIR)/interface.cc
OBJECTS = $(SOURCES:.cc=.o)
TARGET = Chess_game
CXX = g++
CFLAGS = -fPIC -Wall -Wextra
CPPFLAGS = -I$(INC_DIR) -I$(INC_HOMEBREW)
LFLAGS = -L$(LIB) -lSDL2main -lSDL2 -lSDL2_image

all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LFLAGS)

clean:
	rm -f ${SRC_DIR}/*.o $(TARGET)