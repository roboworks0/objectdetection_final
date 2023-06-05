CC = g++
PROJECT = server
SRC = server.cpp
LIBS = `pkg-config --cflags --libs opencv4`
$(PROJECT): $(SRC)
	$(CC) $(SRC) -o $(PROJECT) $(LIBS)

clean:
	rm $(PROJECT)