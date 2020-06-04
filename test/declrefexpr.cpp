
namespace declrefexpr_ns{

class testClass {
public:
  testClass() = default;
  int a;

private:
};
} // namespace

int __attribute__((weak)) main(int argc, char *argv[]) {
  int a;
  int b;
  declrefexpr_ns::testClass tc;
  tc.a = 10;

  return a + b;
}
