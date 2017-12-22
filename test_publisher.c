#include "common.h"
#include "publisher.h"

int main(void) {
  int ret = Publish(SERVER_SOCK_FILE, "foo", "bar");
  return (ret < 0) ? -ret : 0;
}
