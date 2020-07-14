
namespace declrefexpr_ns{

class testClass {
public:
  testClass() = default;
  int testa;

private:
};
} // namespace

int __attribute__((weak)) main(int argc, char *argv[]) {
  int a;
  int testb;
  declrefexpr_ns::testClass tc;
  tc.testa = 10;

  return a + testb;
}
