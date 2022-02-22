#include <cstdio>
#include <chrono>
#include <iostream>
#include <thread>

int main() {
  int val = std::rand() % 100;
  std::chrono::milliseconds delay{50};
  while (1) {
    std::cout << &val << " " << val << std::endl;
    std::this_thread::sleep_for(delay);
    val++;
  }
}
