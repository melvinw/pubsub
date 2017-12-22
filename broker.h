#ifndef PUBSUB_BROKE_H_
#define PUBSUB_BROKE_H_

#include "uthash.h"
#include <sys/socket.h>
#include <sys/un.h>

#define KEY_LEN 64
#define VALUE_LEN 128

struct sock_info {
    struct sockaddr_un sub_sock;
    socklen_t sub_sock_len;
    struct sock_info* next;
};

struct store_entry {
    char key[KEY_LEN];
    struct sock_info* sock_ptr;

    UT_hash_handle hh;
};

#endif // PUBSUB_BROKE_H_
