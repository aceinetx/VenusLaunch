#include <utils.h>

int vasprintf(char **strp, const char *fmt, va_list ap)
{
  int len = _vscprintf(fmt, ap);
  if (len == -1)
  {
    return -1;
  }
  size_t size = (size_t)len + 1;
  char *str = new char[size];
  if (!str)
  {
    return -1;
  }
  int r = _vsprintf_p(str, len + 1, fmt, ap);
  if (r == -1)
  {
    free(str);
    return -1;
  }
  *strp = str;
  return r;
}

int mvprintw_center(int y, const char *fmt, ...)
{
  va_list v;
  va_start(v, fmt);

  char *s;
  int result = vasprintf(&s, fmt, v);
  int x = getmaxx(stdscr) / 2 - (int)(strlen(s)) / 2;

  mvprintw(y, x, "%s", s);

  delete s;

  va_end(v);

  return x;
}

void unzip(const char *zipfilename, const char *outputdir, ZipProgressCallback callback)
{
  int err = 0;
  zip_t *zip = zip_open(zipfilename, ZIP_RDONLY, &err);
  if (!zip)
  {
    fprintf(stderr, "Failed to open zip file: %s\n", zipfilename);
    return;
  }

  zip_int64_t num_entries = zip_get_num_entries(zip, 0);
  zip_int64_t total_size = 0;

  // First pass: Calculate total size of all entries
  for (zip_int64_t i = 0; i < num_entries; i++)
  {
    const char *name = zip_get_name(zip, i, 0);
    if (!name)
    {
      fprintf(stderr, "Failed to get name of entry %lld\n", i);
      continue;
    }

    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(zip, name, 0, &st);
    total_size += st.size; // Accumulate total size
  }

  zip_int64_t total_read = 0;

  // Second pass: Extract entries and report progress
  for (zip_int64_t i = 0; i < num_entries; i++)
  {
    const char *name = zip_get_name(zip, i, 0);
    if (!name)
    {
      fprintf(stderr, "Failed to get name of entry %lld\n", i);
      continue;
    }

    // Create full output path
    char output_path[1024];
    snprintf(output_path, sizeof(output_path), "%s/%s", outputdir, name);

    // Create directories if needed
    char *last_slash = strrchr(output_path, '/');
    if (last_slash)
    {
      *last_slash = '\0';  // Temporarily terminate the string
      _mkdir(output_path); // Create directory
      *last_slash = '/';   // Restore the string
    }

    // Open the zip entry
    zip_file_t *zfile = zip_fopen(zip, name, 0);
    if (!zfile)
    {
      // fprintf(stderr, "Failed to open file in zip: %s\n", name);
      continue;
    }

    // Open output file
    FILE *out = fopen(output_path, "wb");
    if (!out)
    {
      // fprintf(stderr, "Failed to open output file: %s\n", output_path);
      zip_fclose(zfile);
      continue;
    }

    // Get the size of the current file
    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(zip, name, 0, &st);
    zip_int64_t current_file_size = st.size;

    // Read from zip and write to output file
    char buffer[8192];
    zip_int64_t n;

    while ((n = zip_fread(zfile, buffer, sizeof(buffer))) > 0)
    {
      fwrite(buffer, 1, n, out);
      total_read += n;

      // Calculate overall progress percentage
      if (callback && total_size > 0) // Ensure total_size is valid
      {
        int progress = (int)((total_read * 100) / total_size);
        callback(zipfilename, name, progress); // Call the progress callback
      }
    }

    fclose(out);
    zip_fclose(zfile);
  }

  zip_close(zip);
}

std::string replaceAll(const std::string &str, const std::string &from, const std::string &to)
{
  if (from.empty())
    return str; // Avoid empty substring case

  std::string result = str; // Create a copy of the original string
  size_t start_pos = 0;
  while ((start_pos = result.find(from, start_pos)) != std::string::npos)
  {
    result.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Move past the replaced part
  }
  return result; // Return the modified string
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  return fwrite(ptr, size, nmemb, stream);
}

int download_file(const char *url, const char *output_file, DownloadProgressCallback callback)
{
  CURL *curl;
  FILE *fp;
  CURLcode res;
  double total, now;

  curl = curl_easy_init();
  if (curl)
  {
    fp = fopen(output_file, "wb");
    if (!fp)
    {
      perror("Failed to open file");
      return -1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE); // Enable progress meter
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Set the progress data
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, NULL);

    // Perform the request
    res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      fclose(fp);
      curl_easy_cleanup(curl);
      return -1;
    }

    fclose(fp);
    curl_easy_cleanup(curl);
  }
  return 0;
}