#include "http_client.h"

size_t _write_memory_callback(void* ptr, size_t size, size_t nmemb, void* data) {

    if (size * nmemb == 0) {
        return 0;
    }

    size_t realsize = size * nmemb;
    HTTPResponse* mem = (HTTPResponse*) data;
    mem->body = (char*) realloc(mem->body, mem->size + realsize + 1);
    if (mem->body) {
        memcpy(&(mem->body[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->body[mem->size] = 0;
    }

    return realsize;
}

HTTPResponse *http_get_content(const char *url, struct curl_slist *slist) {

    HTTPResponse *mem = malloc(sizeof(HTTPResponse*));
    mem->size = 0;
    mem->body = NULL;
    int count_download_failed = 0;

    while (count_download_failed < COUNT_FAILED_MAX) {

        CURL *curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void* )mem);

        if (curl_easy_perform(curl) == CURLE_OK) {
            break;
        }
        free(mem->body);
        mem->body = NULL;
        count_download_failed++;
    }

    if (count_download_failed == COUNT_FAILED_MAX) {
        free(mem);
        return NULL;
    }

    return mem;
}
