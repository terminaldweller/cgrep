
namespace vardecl_ns{
int gtesta;

int testFunction(int a, int b);
int testFunction(int a, int b) { return a + b; }

class testClass {
public:
  testClass(int a) : testa(a) {}
  virtual ~testClass();

  void cxxMethod(double a) {}

private:
  int testa;
};

#define vardeclmacro varDeclMacroExpanded
int testvardeclmacro;
}; // namespace
