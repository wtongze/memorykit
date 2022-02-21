#include <cstdio>
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  int val = std::rand() % 100;
  std::chrono::seconds delay{1};
  while (1) {
    if (val == -1) {
      exit(0);
    }
    std::cout << &val << " " << val << std::endl;
    std::this_thread::sleep_for(delay);
    val++;
  }
}
