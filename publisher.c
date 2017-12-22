#include "publisher.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "common.h"
#include "message.h"

int Publish(char *broker_socket, char *key, char *value) {
  if (broker_socket == NULL || key == NULL || value == NULL) {
    return -EINVAL;
  }

  int fd = socket(PF_UNIX, SOCK_DGRAM, 0); 
  if (fd < 0) {
    printf("socket() failed while subscribing to %s/%s (errno = %d)\n",
           broker_socket, key, errno);
    return -errno;
  }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, CLIENT_SOCK_FILE);
  int ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    close(fd);
    printf("bind() failed while publishing to %s/%s (errno = %d)\n",
           broker_socket, key, errno);
    return -errno;
  }
  unlink(CLIENT_SOCK_FILE);

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, SERVER_SOCK_FILE);
  ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    close(fd);
    printf("connect() failed while publishing to %s/%s (errno = %d)\n",
           broker_socket, key, errno);
    return -errno;
  }

  struct Message msg;
  msg.message_type = publish;
  msg.key = key;
  msg.value = value;

  char buf[MAX_MSG_LEN];
  ret = serialize_msg(buf, &msg);
  if (ret < 0) {
    close(fd);
    printf("serialize_msg() failed while publishing to %s/%s (errno = %d)\n",
           broker_socket, key, -ret);
    return ret;
  }

  ret = send(fd, buf, ret, 0);
  if (ret < 0) {
    close(fd);
    printf("send() failed while publishing to %s/%s (errno = %d)\n",
           broker_socket, key, errno);
    return -errno;
  }

  return 0;
}
