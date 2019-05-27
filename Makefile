CC = gcc
PY = python
TARGET = main
SRC = flmap.c main.c
LIBS = -lrt

all:
	$(CC) -O3 -o $(TARGET) $(SRC) $(LIBS)
	$(PY) ./setup.py build_ext -i

clean:
	rm -rf *.so *.o $(TARGET)
 
