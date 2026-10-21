FLAGS = -g
INCLUDES = -I ./include
OBJECTS = ./build/system_memory.o ./build/PPU.o ./build/Gameboy.o

all: ${OBJECTS}
	gcc ${FLAGS} ${INCLUDES} ./src/main.c ${OBJECTS} -L ./lib -lmingw32 -lSDL3 -o ./bin/main

./build/system_memory.o: ./src/system_memory.c
	gcc ${FLAGS} ${INCLUDES} ./src/system_memory.c -c -o ./build/system_memory.o

./build/PPU.o: ./src/PPU.c
	gcc ${FLAGS} ${INCLUDES} ./src/PPU.c -c -o ./build/PPU.o

./build/Gameboy.o: ./src/Gameboy.c
	gcc ${FLAGS} ${INCLUDES} ./src/Gameboy.c -c -o ./build/Gameboy.o