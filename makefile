# c++ --std=c++17 models/weight.cpp mongo_db.cpp file_list.cpp json_parser.cpp weights.cpp main.cpp $(pkg-config --cflags --libs libmongocxx) -o mongo

CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS= -g -Wall --std=c++17
MONGOFLAGS=$(shell pkg-config --cflags --libs libmongocxx)

SRC = mongo_db.cpp use_cases.cpp output.cpp main.cpp
MODEL_SRC = models/weight.cpp models/session.cpp
JSON_SRC = file_list.cpp json_parser.cpp
HELPER_SRC = helper/time_converter.cpp helper/menu.cpp

helpers_o: $(HELPER_SRC)
	$(CC) $(CFLAGS) -c $(HELPER_SRC) 

models_o: $(MODEL_SRC)
	$(CC) $(CFLAGS) -c $(MODEL_SRC) 

json_o: $(JSON_SRC)
	$(CC) $(CFLAGS) -c $(JSON_SRC) 

USE_CASE_SRC = use_case/session_import.cpp use_case/session_show.cpp use_case/weight_import.cpp

use_case_o: $(USE_CASE_SRC)
	$(CC) $(CFLAGS) $(MONGOFLAGS) -c $(USE_CASE_SRC)

# The build target 
TARGET = run

all: json_o models_o helpers_o use_case_o $(TARGET)

$(TARGET): $(SRC) models_o json_o use_case_o helpers_o
	$(CC) $(SRC) $(CFLAGS) $(MONGOFLAGS) -lboost_date_time *.o -o $(TARGET) 

clean:
	$(RM) $(TARGET) *.o 

rebuild: clean all
