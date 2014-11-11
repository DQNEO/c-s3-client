#include <openssl/hmac.h>
#include <glib.h>
#include "http_client.h"

#define S3_TOKYO_ENDPOINT "s3-ap-northeast-1.amazonaws.com"
#define BUFF_LENGTH 1024

struct Credential
{
    const char *accesskeyid;
    const char *secretaccesskey;
};

HTTPResponse *s3_get_object(const char *bucket, const char *key, struct Credential *crd);


