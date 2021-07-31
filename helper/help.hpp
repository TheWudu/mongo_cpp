#pragma once

#include "args.hpp"


void show_help() {
  std::cout << "Usage: ./run <command> <opts>" << std::endl
    << "Available commands:" << std::endl
    << "  help [command], shows this help, or the help of the command" << std::endl
    << "  menu,           opens the menu mode" << std::endl
    << "  ls,             list the session" << std::endl
    << "  session_import, import sessions from data/Sport-sessions" << std::endl
    << "  weight_import,  import weights from data/Weight" << std::endl
    << "  show,           shows a single session" << std::endl
    << "  delete,         delete a single session" << std::endl
    << "  stats,          prints statistics" << std::endl
    << std::endl;
}

void show_help_menu() {
  std::cout << "Menu mode, enables to select some of the functionality from \n"
            << "within the application. \n"
            << "No additional parameters available"
            << std::endl;
}

void show_help_session_import() {
  std::cout << "Session import, imports the sessions from the json files in  \n"
            << "data/Sport-sessions folder into the local mongodb. Also checks \n"
            << "if the sessions already exists via session id. \n"
            << "No additional parameters available"
            << std::endl;
}

void show_help_weight_import() {
  std::cout << "Weight import, imports the weights from the json files in  \n"
            << "data/Weight folder into the local mongodb. Also checks \n"
            << "if the weight already exists via id. \n"
            << "No additional parameters available"
            << std::endl;
}

void show_help_ls() {
  std::cout << "List sessions, lists all sessions matching the given filters. \n"
            << "Possible filters are: \n"
            << "  -from=YYYY-MM-DD        e.g. 2021-06-01,      default: 30 days ago. \n"
            << "  -to=YYYY-MM-DD          e.g. 2021-07-31,      default: today. \n"
            << "  -sport_type=type[,type] e.g. running          default: none. \n"
            << "  -notes=<note>           e.g. RunForTheOceans, default: none. \n"
            << std::endl;
}

void show_help_show() {
  std::cout << "Show session, shows a single session by id. \n"
            << "Possible parameters are: \n"
            << "  -id=UID       e.g. 123-127382-12ab-123    no default, shows a prompt to enter it if missing\n"
            << std::endl;
}

void show_help_delete() {
  std::cout << "Delete session, deletes a single session by id. \n"
            << "Possible parameters are: \n"
            << "  -id=UID       e.g. 123-127382-12ab-123    no default\n"
            << std::endl;
}

void show_help_stats() {
  std::cout << "Show some statistics like overall distance, duration, count, \n"
            << "and others. \n"
            << "Possible parameters are: \n"
            << "  -year=YYYY[,YYYY]       e.g. 2021,2020 or all       default: this year. \n"
            << "  -sport_type=type[,type] e.g. running,cycling or all default: running. \n"
            << "  -group=x,y              e.g. year,month,sport_type_id, default: year,sport_type_id \n"
            << std::endl;
}

void show_help(std::string const command) {
  Arguments arg = argv_to_arg(command);
  
  switch(arg) {
    case menu:           show_help_menu(); break;
    case ls:             show_help_ls(); break;
    case session_import: show_help_session_import(); break;
    case weight_import:  show_help_weight_import();  break;
    case show:           show_help_show(); break;
    case del:            show_help_delete(); break;
    case stats:          show_help_stats(); break;
    default:             std::cout << "No additional help available" << std::endl;
  }
}

