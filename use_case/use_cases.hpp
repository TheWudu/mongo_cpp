#pragma once 

#include <iostream>
#include <string>
#include <map>

void import_weights();

void import_sessions();

void import_cities(std::map<std::string, std::string> const args);

void list_sessions(std::map<std::string, std::string> const args);

void show_session(std::map<std::string, std::string> const args);

void delete_sessions(std::map<std::string, std::string> const args);

void show_statistics(std::map<std::string, std::string> const args);
