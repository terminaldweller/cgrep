
namespace cxxmembercallexpr_ns{
class testClass {
public:
  testClass() = default;
  int testFunction(int a, int b) { return a + b; }

private:
};

struct testStruct {
public:
  testStruct() = default;
  int testFunction(int a, int b) {
    return a + b;
  };
};
} // namespace

int __attribute__((weak)) main(int argc, char *argv[]) {
  int a = 10;
  int b = 10;
  cxxmembercallexpr_ns::testClass tc;
  tc.testFunction(a, b);
  cxxmembercallexpr_ns::testStruct ts;
  ts.testFunction(a, b);
  return 0;
}
