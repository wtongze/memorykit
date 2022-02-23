#include <chrono>
#include <cstdio>
#include <iostream>
#include <thread>

int main() {
  int val = std::rand() % 100;
  char a[] = "abcd";
  std::chrono::milliseconds delay{30};
  while (1) {
    printf("%p %d %p %s \r\n", &val, val, &a, a);
    fflush(stdout);
    std::this_thread::sleep_for(delay);
    val++;
  }
}
