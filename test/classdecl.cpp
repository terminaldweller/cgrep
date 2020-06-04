
namespace classdecl_ns{
class testClass {
public:
  testClass();
  virtual ~testClass();

private:
};

#define classdeclmacro classDeclMacroExpanded
class anotherTestClass {
public:
  anotherTestClass();
  virtual ~anotherTestClass();

private:
};
}; // namespace
