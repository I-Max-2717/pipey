#include "download.h"
#include <curl/curl.h>
#include <fstream>

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::ofstream* out = static_cast<std::ofstream*>(stream);
    size_t total = size * nmemb;
    out->write(static_cast<char*>(ptr), total);
    return total;
}

bool downloadFile(const std::string& url, const std::string& outPath) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::ofstream ofs(outPath, std::ios::binary);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    ofs.close();

    return (res == CURLE_OK);
}
