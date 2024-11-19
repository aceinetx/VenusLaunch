#if !defined(LAUNCH_H)
#define LAUNCH_H

#include <string>
#include <format>
#include <utils.h>

typedef struct
{
  int ram_max;
  int ram_min;
  int window_width;
  int window_height;
  std::string java_path;
  std::string game_path;
  std::string username;
  bool java_logging;
  std::string java_arguments;
  bool output_java_command;
} Settings;

std::string generateJavaCmd(Settings &game_settings);

#endif // LAUNCH_H
