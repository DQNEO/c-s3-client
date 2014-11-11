/**
 * s3_get.c
 * command to get HTTP content on Amazon S3
 *
 * Usage:
 *  s3_get bucketname key/dir/file.txt > file.txt
 */
#include <stdio.h>
#include "http_client.h"
#include <openssl/hmac.h>
#include <glib.h>
#define BUFF_LENGTH 1024

void usage()
{
    printf("Usage:\n  s3_get bucketname key/dir/file.txt > file.txt\n");
}


#define S3_TOKYO_ENDPOINT "s3-ap-northeast-1.amazonaws.com"

char *aws_get_authorization_string(const char *cat_header, const char *secretkey) {

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

HTTPResponse *s3_get_object(const char *bucket, const char *key)
{
    char url[512];

    sprintf(url,"https://%s/%s/%s", S3_TOKYO_ENDPOINT, bucket, key);

    char path_style_resource[1024];
    sprintf(path_style_resource,"/%s/%s", bucket, key);

    struct curl_slist *slist = NULL;

    // 取得したcredentialsを使ってsignatureを作成
    char date[512];
    char cat_header[1024];
    char signature[1024];

    char *accesskeyid = "AKIA***********";
    char *secretaccesskey = "1KuIp*************";

    char h_date[512];

    time_t t = time(NULL);
    struct tm *tm = gmtime(&t);
    strftime(date, BUFF_LENGTH, "%a, %d %b %Y %H:%M:%S +0000", tm);

    //sprintf(cat_header, "GET\n\n\n%s\nx-amz-security-token:\n%s\n%s", date, token, path_style_resource);

    sprintf(cat_header, "GET\n\n\n%s\n%s", date, path_style_resource);

    char *authorization = aws_get_authorization_string(cat_header, secretaccesskey);
    sprintf(signature, "Authorization: AWS %s:%s", accesskeyid, authorization);

    sprintf(h_date, "Date: %s", date);

    slist = curl_slist_append(slist, h_date);
    slist = curl_slist_append(slist, signature);

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

    HTTPResponse *response = s3_get_object(bucket, key);
    if (!response) {
        return 1;
    }

    fwrite(response->body, response->size, 1, stdout);
    return 0;
}
