#if !defined(UTILS_H)
#define UTILS_H
#include <stdlib.h>
#include <stdio.h>
#include <zip.h>
#include <curses.h>
#include <string.h>
#include <direct.h>
#include <string>
#include <curl/curl.h>
#include <iostream>

typedef void (*ZipProgressCallback)(const char *zip_filename, const char *filename, int progress);
typedef int (*DownloadProgressCallback)(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

int vasprintf(char **strp, const char *fmt, va_list ap);
int mvprintw_center(int y, const char *fmt, ...);
void unzip(const char *zipfilename, const char *outputdir, ZipProgressCallback callback);
std::string replaceAll(const std::string &str, const std::string &from, const std::string &to);
int download_file(const char *url, const char *output_file, DownloadProgressCallback callback);

#endif // UTILS_H