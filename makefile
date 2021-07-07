# c++ --std=c++17 models/weight.cpp mongo_db.cpp file_list.cpp json_parser.cpp weights.cpp main.cpp $(pkg-config --cflags --libs libmongocxx) -o mongo

CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS= -g -Wall --std=c++17
MONGOFLAGS=$(shell pkg-config --cflags --libs libmongocxx)

FILES = mongo_db.cpp use_case/weight_import.cpp use_case/run_session_import.cpp file_list.cpp json_parser.cpp models/weight.cpp models/run_session.cpp main.cpp

# The build target 
TARGET = run

build: $(TARGET)

$(TARGET): $(FILES)
	$(CC) $(FILES) $(CFLAGS) $(MONGOFLAGS) -o $(TARGET) 

clean:
	$(RM) $(TARGET) *.o

rebuild: clean build 
