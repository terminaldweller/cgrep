
namespace structdecl_ns{
struct testStruct {
  int a;
  float b;
  char c[10];
};

#define structdeclmacro structDeclMacroExpanded
struct structdeclmacro {
  int d;
  double e;
};
} // namespace
