
namespace callexpr_ns{
int testFunction(int a, int b) { return a + b; }

class testClass {
public:
  testClass() = default;
  int testMemberFunction(int a, int b) { return a + b; }

private:
  /* data */
};

struct testStruct {
public:
  testStruct() = default;
  int a;
  int b;
  char *c;
  int testMemberFunctionn(int a, int b) { return a + b; }
};
} // namespace

int __attribute__((weak)) main(int argc, char *argv[]) {
  int a = 10;
  int b = 10;
  callexpr_ns::testFunction(a, b);
  callexpr_ns::testClass tc;
  tc.testMemberFunction(a, b);
  callexpr_ns::testStruct ts;
  ts.testMemberFunctionn(a, b);

  return 0;
}
