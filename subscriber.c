#include "subscriber.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "common.h"
#include "message.h"

int Subscribe(char *broker_socket, char *key, struct SubscriberContext *ctx) {
  if (ctx == NULL || key == NULL || broker_socket == NULL) {
    return -EINVAL;
  }

  size_t key_len = strlen(key);
  ctx->key = (char *)malloc(key_len);
  strncpy(ctx->key, key, key_len);

  printf("opening...");
  int fd = socket(PF_UNIX, SOCK_DGRAM, 0); 
  if (fd < 0) {
    printf("socket() failed while subscribing to %s/%s (errno = %d)\n",
           broker_socket, key, errno);
    return -errno;
  }
  ctx->socket = fd;

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, CLIENT_SOCK_FILE);
  printf("binding...");
  int ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  unlink(CLIENT_SOCK_FILE);
  if (ret < 0) {
    close(fd);
    ctx->socket = -1;
    printf("bind() failed while subscribing to %s/%s (errno = %d)\n",
           broker_socket, key, errno);
    return -errno;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, broker_socket);
  printf("connecting...");
  ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    close(fd);
    ctx->socket = -1;
    printf("connect() failed while subscribing to %s/%s (errno = %d)\n",
           broker_socket, key, errno);
    return -errno;
  }

  struct Message msg;
  msg.message_type = subscribe;
  msg.key = (char *)malloc(key_len);
  if (msg.key == NULL) {
    close(fd);
    ctx->socket = -1;
    return -ENOMEM;
  }

  char buf[MAX_MSG_LEN];
  printf("serializing...");
  ret = serialize_msg(buf, &msg);
  if (ret < 0) {
    close(fd);
    ctx->socket = -1;
    printf("serialize_msg() failed while subscribing to %s/%s (errno = %d)\n",
           broker_socket, key, -ret);
    return ret;
  }
  printf("serialized: %s\n", buf);

  printf("sending...");
  ret = send(ctx->socket, buf, ret, 0);
  if (ret < 0) {
    close(fd);
    ctx->socket = -1;
    printf("send() failed while subscribing to %s/%s (errno = %d)\n",
           broker_socket, key, errno);
    return -errno;
  }

  return 0;
}

int Unsubscribe(struct SubscriberContext *ctx) {
  if (ctx == NULL || ctx->key == NULL) {
    return -EINVAL;
  }

  struct Message msg;
  msg.message_type = unsubscribe;
  msg.key = ctx->key;

  char buf[MAX_MSG_LEN];
  int ret = serialize_msg(buf, &msg);
  if (ret < 0) {
    printf("serialize_msg() failed while unsubscribing from %s (errno = %d)\n",
           ctx->key, errno);
    return -errno;
  }

  ret = send(ctx->socket, buf, ret, 0);
  if (ret < 0) {
    printf("send() failed while unsubscribing for %s (errno = %d)\n",
           ctx->key, errno);
    return -errno;
  }

  close(ctx->socket);
  ctx->socket = -1;

  return 0;
}

int GetUpdate(const struct SubscriberContext *ctx, struct Message *msg) {
  if (ctx == NULL || ctx->key == NULL || msg == NULL) {
    return -EINVAL;
  }

  char buf[MAX_MSG_LEN];
  int ret = recv(ctx->socket, buf, MAX_MSG_LEN, 0);
  if (ret < 0) {
    printf("recv() failed while getting update for %s (errno = %d)\n",
           ctx->key, errno);
    return -errno;
  } else if (ret == 0) {
    return -EAGAIN;
  }

  struct Message update;
  ret = deserialize_msg(&update, buf);
  if (ret < 0) {
    printf("deserialize_msg() failed while receiving update for %s (errno = %d)\n",
           ctx->key, -ret);
    return ret;
  }

  copy_msg(msg, &update);

  return 0;
}
