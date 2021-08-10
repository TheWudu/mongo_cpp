#pragma once

enum Arguments {
  help,
  menu,
  ls,
  session_import,
  stats,
  show,
  del,
  weight_import,
  cities_import,
  // unknown input
  unknown
};

Arguments argv_to_arg(std::string const arg) {
  if (arg == "menu")           { return menu; }
  if (arg == "ls")             { return ls; }
  if (arg == "session_import") { return session_import; }
  if (arg == "weight_import")  { return weight_import;  }
  if (arg == "cities_import")  { return cities_import; }
  if (arg == "show")           { return show; }
  if (arg == "delete")         { return del; }
  if (arg == "stats")          { return stats; }
  if (arg == "help")           { return help; }
  return unknown;
}
