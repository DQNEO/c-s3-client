#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#define COUNT_FAILED_MAX 5

typedef struct {
    char* body;
    size_t size;
} HTTPResponse;

HTTPResponse *http_get_content(const char *url, struct curl_slist *slist);

