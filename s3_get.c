/**
 * s3_get.c
 * command to get HTTP content on Amazon S3
 *
 * Usage:
 *  s3_get bucketname key/dir/file.txt > file.txt
 */
#include <stdio.h>
#include "http_client.h"

void usage()
{
    printf("Usage:\n  s3_get bucketname key/dir/file.txt > file.txt\n");
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

    HTTPResponse *response = http_get_content(url, NULL);
    if (!response) {
        return 1;
    }

    fwrite(response->body, response->size, 1,stdout);
    return 0;
}
