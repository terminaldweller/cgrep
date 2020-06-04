
namespace cxxmethoddecl_ns{
#define cxxmethoddeclmacro cxxMethodTwo
class testClass {
public:
  testClass();
  virtual ~testClass();

  void cxxMethodOne(void);
  void cxxMethodOne(int a);
  void cxxmethoddeclmacro(void);

private:
};
}; // namespace
