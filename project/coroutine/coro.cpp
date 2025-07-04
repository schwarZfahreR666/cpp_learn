#include "coro.h"
// g++ coro.cpp -std=c++20 -fcoroutines -O3 -o coro.o
//这是一个协程函数
task<int> func2(){
    std::cout << "begin func2" << std::endl;
    std::cout << "end func2" << std::endl;
    co_return 2;
}

task<> func() {
  std::cout << "begin func" << std::endl;
  auto res = co_await func2();
  std::cout << "get res from func2: " << res << std::endl;
  std::cout << "end func" << std::endl;
}

int main(int argc, char const *argv[]) {
  auto h = func();

  bool can_resume = true;
  do {
    can_resume = h.resume();
    std::cout << "back to main" << std::endl;
  } while (can_resume);
  return 0;
}