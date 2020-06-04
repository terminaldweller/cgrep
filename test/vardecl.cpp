
namespace vardecl_ns{
int a;

int testFunction(int a, int b);
int testFunction(int a, int b) { return a + b; }

class testClass {
public:
  testClass(int a) : a(a) {}
  virtual ~testClass();

  void cxxMethod(double a) {}

private:
  int a;
};

#define vardeclmacro varDeclMacroExpanded
int vardeclmacro;
}; // namespace
