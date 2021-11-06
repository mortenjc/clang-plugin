//===- test.cpp ---------------------------------------------===//
//
// File demonstrating noncompliance to a simplified coding standard:
//
// 1) function parameters must begin with an upper case letter
// 2) function parameter names must be at least 5 characters long
// 3) function parameter names must not be in blacklist: crap, gylle
//===----------------------------------------------------------------------===//

long a;

int myfunc(int a, int b, int Longer) {
  return 0;
}

int myotherfunc(long * crap) {
  return 1;
}

int main() {
  myfunc(1, 2, 3);
}
