#pragma once 

#include <iostream>
#include <string>
#include <map>

void import_weights();

void import_sessions();

void list_sessions(std::map<std::string, std::string> args);

void show_session(std::map<std::string, std::string> args);

void delete_sessions(std::map<std::string, std::string> args);

void show_statistics(std::map<std::string, std::string> args);
