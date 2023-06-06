CC = g++
PROJECT = server
SRC = server.cpp
LIBS = `pkg-config --cflags --libs opencv4`
$(PROJECT): $(SRC)
	$(CC) $(SRC) -o $(PROJECT) $(LIBS)

test: yolo_test.cpp
	$(CC) yolo_test.cpp -o test $(LIBS)

clean:
	rm $(PROJECT)