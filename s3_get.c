/**
 * s3_get.c
 * command to get HTTP content on Amazon S3
 *
 * Usage:
 *  s3_get bucketname key/dir/file.txt > file.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include "s3_client.h"

void usage()
{
    printf("Usage:\n  s3_get bucketname key/dir/file.txt > file.txt\n");
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        usage();
        return 1;
    }
    char *bucket = argv[1];
    char *key = argv[2];

    struct Credential *pcrd;
    struct Credential crd;

    crd.accesskeyid = getenv("AWS_ACCESS_KEY_ID");
    crd.secretaccesskey = getenv("AWS_SECRET_ACCESS_KEY");

    if (crd.accesskeyid == NULL) {
        pcrd = NULL;
    } else {
        pcrd = &crd;
    }

    HTTPResponse *response = s3_get_object(bucket, key, pcrd);
    if (!response) {
        return 1;
    }

    fwrite(response->body, response->size, 1, stdout);
    return 0;
}
