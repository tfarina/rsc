#include "download.h"

#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>

int
download_file(char const *downloadurl, char const *destfile)
{
  CURL *curl;
  CURLcode result;
  FILE* fp;

  result = curl_global_init(CURL_GLOBAL_DEFAULT);
  if (result != CURLE_OK) {
    fprintf(stderr, "curl_global_init() failed\n");
    return -1;
  }

  curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "curl_easy_init() failed\n");
    return -1;
  }

  curl_easy_setopt(curl, CURLOPT_URL, downloadurl);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 20L);

  /*
   * It was necessary to set CURLOPT_SSL_VERIFYPEER to fix the following error:
   *
   * curl_easy_perform() failed: Peer certificate cannot be authenticated with given CA certificates
   */
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  
  fp = fopen(destfile, "wb");
  if (fp == NULL) {
    curl_easy_cleanup(curl);
    return -1;
  }

  /*
   * By not setting CURLOPT_WRITEFUNCTION here, libcurl will use 'fwrite'
   * as a callback by default.
   */
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

  result = curl_easy_perform(curl);
  if (result != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
    curl_easy_cleanup(curl);
    return -1;
  }

  fclose(fp);

  curl_easy_cleanup(curl);

  curl_global_cleanup();

  return 0;
}
