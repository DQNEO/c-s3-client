/**
 * s3_get.c
 * command to get HTTP content on Amazon S3
 *
 * Usage:
 *  s3_get bucketname key/dir/file.txt > file.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include "http_client.h"
#include <openssl/hmac.h>
#include <glib.h>
#define BUFF_LENGTH 1024

struct Credential
{
    const char *accesskeyid;
    const char *secretaccesskey;
};

void usage()
{
    printf("Usage:\n  s3_get bucketname key/dir/file.txt > file.txt\n");
}


#define S3_TOKYO_ENDPOINT "s3-ap-northeast-1.amazonaws.com"

char *s3_generate_authorization(const char *cat_header, const char *secretkey)
{

    HMAC_CTX ctx;
    unsigned char MD[BUFF_LENGTH];
    unsigned len;
    HMAC_CTX_init(&ctx);
    HMAC_Init(&ctx, secretkey, strlen(secretkey), EVP_sha1());
    HMAC_Update(&ctx, (unsigned char*) cat_header, strlen(cat_header));
    HMAC_Final(&ctx, (unsigned char*) MD, &len);
    HMAC_CTX_cleanup(&ctx);

    return g_base64_encode(MD, len);
}

HTTPResponse *s3_get_object(const char *bucket, const char *key, struct Credential *crd)
{
    char url[512];
    char path_style_resource[1024];
    char date[512];
    char cat_header[1024];
    char signature[1024];

    char h_date[512];
    struct curl_slist *slist = NULL;

    sprintf(url,"https://%s/%s/%s", S3_TOKYO_ENDPOINT, bucket, key);

    if (crd != NULL) {
        sprintf(path_style_resource,"/%s/%s", bucket, key);

        time_t t = time(NULL);
        struct tm *tm = gmtime(&t);
        strftime(date, BUFF_LENGTH, "%a, %d %b %Y %H:%M:%S +0000", tm);

        //sprintf(cat_header, "GET\n\n\n%s\nx-amz-security-token:\n%s\n%s", date, token, path_style_resource);

        sprintf(cat_header, "GET\n\n\n%s\n%s", date, path_style_resource);

        char *authorization = s3_generate_authorization(cat_header, crd->secretaccesskey);
        sprintf(signature, "Authorization: AWS %s:%s", crd->accesskeyid, authorization);

        sprintf(h_date, "Date: %s", date);

        slist = curl_slist_append(slist, h_date);
        slist = curl_slist_append(slist, signature);
    }

    HTTPResponse *response = http_get_content(url, slist);
    return response;
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
