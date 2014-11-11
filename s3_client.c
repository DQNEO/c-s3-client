#include "s3_client.h"

char *generate_authorization(const char *cat_header, const char *secretkey)
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

struct curl_slist *prepare_headers(const char *bucket, const char *key, struct Credential *crd)
{
        char path_style_resource[1024];
        char date[512];
        char cat_header[1024];
        char signature[1024];
        char h_date[512];
        struct curl_slist *slist = NULL;

        sprintf(path_style_resource,"/%s/%s", bucket, key);

        time_t t = time(NULL);
        struct tm *tm = gmtime(&t);
        strftime(date, BUFF_LENGTH, "%a, %d %b %Y %H:%M:%S +0000", tm);

        //sprintf(cat_header, "GET\n\n\n%s\nx-amz-security-token:\n%s\n%s", date, token, path_style_resource);

        sprintf(cat_header, "GET\n\n\n%s\n%s", date, path_style_resource);

        char *authorization = generate_authorization(cat_header, crd->secretaccesskey);
        sprintf(signature, "Authorization: AWS %s:%s", crd->accesskeyid, authorization);

        sprintf(h_date, "Date: %s", date);

        slist = curl_slist_append(slist, h_date);
        slist = curl_slist_append(slist, signature);
        return slist;
}

HTTPResponse *s3_get_object(const char *bucket, const char *key, struct Credential *crd)
{
    char url[512];
    sprintf(url,"https://%s/%s/%s", S3_TOKYO_ENDPOINT, bucket, key);

    struct curl_slist *slist;
    HTTPResponse *response;

    if (crd == NULL) {
        response = http_get_content(url, NULL);
    } else {
        slist = prepare_headers(bucket, key, crd);
        response = http_get_content(url, slist);
    }

    return response;
}


