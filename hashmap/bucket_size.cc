#include <cstdio>
#include <iostream>
#include <string.h>
using namespace std;

static void print_UserId(const unsigned char *userid) { // must use unsigned char *
  char user_id_hex[257] = {0};
  for (int i = 0; i < 128; i++) {
    sprintf(user_id_hex + 2 * i, "%02x", *(userid+i));
  }
  user_id_hex[256] = 0;
  printf("%s\n", user_id_hex);
}


int main() {
    char str[128];
    for (int i = 0; i < 128; i++) {
        str[i] = i;
    }
    print_UserId((const unsigned char *)str);
}
