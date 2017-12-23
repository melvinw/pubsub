#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "message.h"
#include "subscriber.h"

int main(void) {
  struct SubscriberContext ctx;
  int ret = Subscribe(BROKER_SOCK_FILE, "foo", &ctx);
  if (ret) {
    return -ret;
  }

  struct Message msg;
  //todo check malloc whether fail
  msg.key = (char*)malloc(KEY_LEN);
  msg.value = (char*)malloc(VALUE_LEN);
  while ((ret = GetUpdate(&ctx, &msg)) == -EAGAIN) {
    printf("got update: %s\n", msg.value);
    sleep(1);
  }

  if (ret == 0) {
    free(msg.key);
    free(msg.value);
  }

  return (ret < 0) ? -ret : Unsubscribe(&ctx);
}
