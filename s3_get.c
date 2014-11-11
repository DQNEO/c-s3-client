/**
 * s3_get.c
 * command to get HTTP content on Amazon S3
 *
 * Usage:
 *  s3_get bucketname key/dir/file.txt > file.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define COUNT_FAILED_MAX 5

typedef struct {
    char* memory;
    size_t size;
} Memory;


void usage()
{
    printf("Usage:\n  s3_get bucketname key/dir/file.txt > file.txt\n");
}

size_t _write_memory_callback(void* ptr, size_t size, size_t nmemb, void* data) {

    if (size * nmemb == 0) {
        return 0;
    }

    size_t realsize = size * nmemb;
    Memory* mem = (Memory*) data;
    mem->memory = (char*) realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }

    return realsize;
}

Memory *http_get_content(const char *url, struct curl_slist *slist) {

    Memory *mem = malloc(sizeof(Memory*));
    mem->size = 0;
    mem->memory = NULL;
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
        free(mem->memory);
        mem->memory = NULL;
        count_download_failed++;
    }

    if (count_download_failed == COUNT_FAILED_MAX) {
        free(mem);
        return NULL;
    }

    return mem;
}

#define S3_TOKYO_ENDPOINT "s3-ap-northeast-1.amazonaws.com"

int main(int argc, char **argv)
{
    if (argc < 3) {
        usage();
        return 1;
    }
    char *bucket = argv[1];
    char *key = argv[2];

    char url[512];

    sprintf(url,"https://%s/%s/%s", S3_TOKYO_ENDPOINT, bucket ,key);

    Memory *image_blob = http_get_content(url, NULL);
    if (!image_blob) {
        return 1;
    }

    fwrite(image_blob->memory, image_blob->size, 1,stdout);
    return 0;
}
