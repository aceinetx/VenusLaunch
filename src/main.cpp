#include <stdio.h>
#include <stdlib.h>
#include <zip.h>
#include <io.h>
#include <curses.h>
#include <fkYAML/node.hpp>
#include <direct.h>
#include <fstream>
#include <thread>
#include <chrono>
#include <filesystem>

#include <launch.h>
#include <utils.h>

#define resize_terminal(y, x) \
  resize_term(y, x);          \
  resize_window(stdscr, y, x)

typedef enum
{
  ACTION_UNZIP_GAME,
  ACTION_DOWNLOAD_ASSETS,
} Action;

char config_path[1024] = ".venus\\VenusLaunch.yml";
char assets_url[1024];

bool action_performing = false;
int action_progress = 0;
char current_action[1024];

bool pdcurses_initalized = false;

void zip_callback(const char *zip_filename, const char *filename, int progress)
{
  if (strcmp(zip_filename, ".venus\\VenusFree.zip") == 0)
  {
    memset(current_action, 0, sizeof(current_action));
    snprintf(current_action, sizeof(current_action), "Unzipping game (%d%%): %s", progress, filename);
  }
  else if (strcmp(zip_filename, ".venus\\VenusAssets.zip") == 0)
  {
    memset(current_action, 0, sizeof(current_action));
    snprintf(current_action, sizeof(current_action), "Unzipping assets (%d%%): %s", progress, filename);
  }
  action_progress = progress;
}

void download_callback(int percentage, const char *filename)
{
  memset(current_action, 0, sizeof(current_action));
  snprintf(current_action, sizeof(current_action), "Downloading (%d%%): %s", percentage, filename);
  action_progress = percentage;
}

int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
  if (dltotal == 0 || dlnow == 0)
  {
    action_progress = 0;
    memset(current_action, 0, sizeof(current_action));
    snprintf(current_action, sizeof(current_action), "Preparing to download...");
  }
  else
  {
    double fraction = (double)(dlnow) / (double)(dltotal);
    action_progress = (int)(fraction * 100.0f);

    memset(current_action, 0, sizeof(current_action));
    snprintf(current_action, sizeof(current_action), "Downloading (%d%% %lldMb/%lldMb)", action_progress, dlnow / 1000000L, dltotal / 1000000L);
  }
  return 0;
}

void action_thread(Action action)
{
  action_performing = true;
  memset(current_action, 0, sizeof(current_action));

  if (action == ACTION_UNZIP_GAME)
  {
    _mkdir(".venus\\game");
    memset(current_action, 0, sizeof(current_action));
    strcpy(current_action, "Unzipping game");
    unzip(".venus\\VenusFree.zip", ".venus\\game\\", zip_callback);

    memset(current_action, 0, sizeof(current_action));
    strcpy(current_action, "Unzipping assets");
    unzip(".venus\\VenusAssets.zip", ".venus\\game", zip_callback);

    memset(current_action, 0, sizeof(current_action));
    strcpy(current_action, "Unzipping done  ");
    action_progress = 0;
  }
  else if (action == ACTION_DOWNLOAD_ASSETS)
  {
    memset(current_action, 0, sizeof(current_action));
    strcpy(current_action, "Downloading assets");

    download_file(assets_url, ".venus\\VenusAssets.zip", progress_callback);
  }
  action_performing = false;
}

void call_action(Action action)
{
  memset(current_action, 0, sizeof(current_action));
  strcpy(current_action, "Performing...");
  action_progress = 0;

  if (!action_performing)
  {
    std::thread t(action_thread, action);
    t.detach();
  }
}

void update_config(Settings &settings)
{
  if (_access(config_path, 0) != 0)
  {
    std::ofstream _f;
    _f.open(config_path);
    _f << R"(java_path: "C:\\Program Files\\Java\\jdk-17\\bin\\java.exe"
assets_url: "https://www.dropbox.com/scl/fi/09qsh1jnjegaqk6h00i57/VenusAssets.zip?rlkey=kljr2loysxdde88bj2of4e30y&st=sagwdkve&dl=1"
ram_max: 2048
ram_min: 2048
window_width: 925
window_height: 530
username: "Player"
java_logging: true
java_arguments: "-noverify"
output_java_command: true)";
    _f.close();
  }
  std::ifstream f(config_path);

  try
  {
    fkyaml::node root = fkyaml::node::deserialize(f);

    settings.java_path = root["java_path"].get_value<std::string>();
    settings.ram_max = root["ram_max"].get_value<int>();
    settings.ram_min = root["ram_min"].get_value<int>();
    settings.window_width = root["window_width"].get_value<int>();
    settings.window_height = root["window_height"].get_value<int>();
    settings.username = root["username"].get_value<std::string>();
    settings.java_logging = root["java_logging"].get_value<bool>();
    settings.java_arguments = root["java_arguments"].get_value<std::string>();
    settings.output_java_command = root["output_java_command"].get_value<bool>();
    snprintf(assets_url, sizeof(assets_url), "%s", root["assets_url"].get_value<std::string>().c_str());
  }
  catch (const std::exception &e)
  {
    nodelay(stdscr, FALSE);
    clear();
    mvprintw(0, 0, "Config error: %s", e.what());

    refresh();
    getch();
    endwin();
    pdcurses_initalized = false;

    exit(0);
  }

  f.close();
}

void initalize_pdcurses()
{
  initscr();
  pdcurses_initalized = true;
  start_color();
  use_default_colors();

  resize_terminal(19, 92);
  PDC_set_title("VenusLaunch");

  init_pair(1, 18, 0);
  init_pair(2, 19, 0);
  init_pair(3, 20, 0);
  init_pair(4, 57, 0);
  init_pair(5, 56, 0);
  init_pair(6, 69, 0);
  init_pair(7, COLOR_RED, 0);
  init_pair(8, COLOR_WHITE, 60);
  init_pair(9, COLOR_WHITE, 56);

  curs_set(0);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
}

int launcher_main(int argc, char **argv, char **envp)
{
  Settings settings;
  settings.ram_max = 2048;
  settings.ram_min = 2048;
  settings.window_height = 530;
  settings.window_width = 925;
  settings.game_path = "";
  char cwd[1024];
  _getcwd(cwd, sizeof(cwd));
  settings.game_path.append(cwd);
  settings.game_path.append("\\.venus\\game");
  settings.username = "Player";

  initalize_pdcurses();

  _mkdir(".venus");
  std::filesystem::remove(".venus\\temp.bat");

  update_config(settings);

  int wy, wx, menu_sel;
  wy = getmaxy(stdscr);
  wx = getmaxx(stdscr);
  menu_sel = 0;

  std::unique_ptr<char[]> action_buf(new char[wx]);
  memset(action_buf.get(), 0, wx);
  while (true)
  {
    clear();
    wy = getmaxy(stdscr);
    wx = getmaxx(stdscr);

    bool assets_downloaded = _access(".venus\\VenusAssets.zip", 0) == 0;
    bool venus_downloaded = _access(".venus\\VenusFree.zip", 0) == 0;

    attron(COLOR_PAIR(1));
    mvprintw_center(0, " _    __                     _       __              ");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    mvprintw_center(1, "| |  / /__  ____  __  ______| |     / /___ _________ ");
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    mvprintw_center(2, "| | / / _ \\/ __ \\/ / / / ___/ | /| / / __ `/ ___/ _ \\");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(4));
    mvprintw_center(3, "| |/ /  __/ / / / /_/ (__  )| |/ |/ / /_/ / /  /  __/");
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(5));
    mvprintw_center(4, "|___/\\___/_/ /_/\\__,_/____/ |__/|__/\\__,_/_/   \\___/ ");
    attroff(COLOR_PAIR(5));

    if (assets_downloaded)
    {
      int x = mvprintw_center(6, "  Assets:  exists");
      attron(COLOR_PAIR(6));
      mvprintw(6, x + 11, "exists");
      attroff(COLOR_PAIR(6));
    }
    else
    {
      int x = mvprintw_center(6, "! Assets: missing");
      attron(COLOR_PAIR(7));
      mvprintw(6, x + 10, "missing");
      attroff(COLOR_PAIR(7));
    }

    if (venus_downloaded)
    {
      int x = mvprintw_center(7, "  Venus:   exists");
      attron(COLOR_PAIR(6));
      mvprintw(7, x + 11, "exists");
      attroff(COLOR_PAIR(6));
    }
    else
    {
      int x = mvprintw_center(7, "! Venus:  missing");
      attron(COLOR_PAIR(7));
      mvprintw(7, x + 10, "missing");
      attroff(COLOR_PAIR(7));
    }

    if (_access(".venus\\game", 0) == 0 && assets_downloaded && venus_downloaded)
    {
      mvprintw_center(8, "Game state:  Extractable & Launchable");
    }
    else if (assets_downloaded && venus_downloaded)
    {
      mvprintw_center(8, "  Game state: Extractable");
    }
    else if (_access(".venus\\game", 0) == 0)
    {
      mvprintw_center(8, "  Game state:  Launchable");
    }
    else
    {
      mvprintw_center(8, "Game state:  Unlaunchable & Unextractable");
    }

    mvprintw_center(10, "Launch");
    mvprintw_center(11, "Configuration");
    mvprintw_center(12, "Download assets");
    switch (menu_sel)
    {
    case 0:
      attron(COLOR_PAIR(8));
      mvprintw_center(10, "Launch");
      attroff(COLOR_PAIR(8));
      break;
    case 1:
      attron(COLOR_PAIR(8));
      mvprintw_center(11, "Configuration");
      attroff(COLOR_PAIR(8));
      break;
    case 2:
      attron(COLOR_PAIR(8));
      mvprintw_center(12, "Download assets");
      attroff(COLOR_PAIR(8));
      break;
    }

    memset(action_buf.get(), 0, wx);
    memset(action_buf.get(), ' ', wx - 1);
    snprintf(action_buf.get(), wx, "%s", current_action);

    float ratio = wx / 100.0f;
    int color_end = (int)(ratio * (float)(action_progress));
    for (int i = 0; i < wx; i++)
    {
      if (i <= color_end && color_end != 0)
        attron(COLOR_PAIR(9));

      mvprintw(wy - 1, i, "%c", action_buf.get()[i]);
      if (action_buf.get()[i] == '\0')
        mvprintw(wy - 1, i, " ");

      if (i <= color_end && color_end != 0)
        attroff(COLOR_PAIR(9));
    }

    mvprintw(wy - 2, 0, "Launcher made by");

    attron(COLOR_PAIR(4));
    mvprintw(wy - 2, 17, "aceinet");
    attroff(COLOR_PAIR(4));

    int k = getch();
    if (k == 'q')
    {
      break;
    }
    else if (k == '\n')
    {
      switch (menu_sel)
      {
      case 0:
        if (!action_performing)
        {
          if (_access(".venus\\game", 0) != 0)
          {
            if (venus_downloaded && assets_downloaded)
            {
              call_action(ACTION_UNZIP_GAME);
            }
          }
          else
          {
            update_config(settings);
            std::string java_cmd = generateJavaCmd(settings);
            std::string cmd = std::format("@echo off\n");

            if (settings.output_java_command)
              cmd.append(std::format("echo \x1b[38;5;69mVenusLaunch: Java command:\x1b[38;5;146m {}\x1b[0m\n", java_cmd));

            if (!settings.java_logging)
              java_cmd.append(" > nul");

            cmd.append(std::format("{}\ndel .venus\\temp.bat", java_cmd));

            std::ofstream f;
            f.open(".venus\\temp.bat");
            f << cmd;
            f.close();
            system("start cmd /c .venus\\temp.bat");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
        }
        break;
      case 1:
        update_config(settings);
        system(std::format("start notepad {}\\{}", cwd, config_path).c_str());
        break;
      case 2:
        if (!action_performing)
        {
          call_action(ACTION_DOWNLOAD_ASSETS);
        }
        break;
      }
    }
    else if (k == KEY_UP)
    {
      menu_sel--;
      if (menu_sel < 0)
        menu_sel = 2;
    }
    else if (k == KEY_DOWN)
    {
      menu_sel++;
      if (menu_sel > 2)
        menu_sel = 0;
    }
  }

  curs_set(1);
  endwin();
  pdcurses_initalized = false;

  return 0;
}

int main(int argc, char **argv, char **envp)
{
  try
  {
    return launcher_main(argc, argv, envp);
  }
  catch (const std::exception &e)
  {
    if (!pdcurses_initalized)
    {
      initalize_pdcurses();
    }

    nodelay(stdscr, FALSE);

    attron(COLOR_PAIR(4));
    mvprintw(0, 0, "VenusLaunch: C++ Error: ");
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(7));
    mvprintw(1, 0, "%s", e.what());
    attron(COLOR_PAIR(7));

    refresh();

    getch();

    endwin();

    return 0;
  }
}