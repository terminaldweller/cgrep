
namespace fielddecl_ns{
#define fieldmacro fieldthree

struct testStruct {
  int fieldone;
  float fieldtwo;
  int fieldmacro;
};

union testUnion {
  int fieldone;
  bool fieldtwo;
};

class testClass {
public:
  testClass(void);
  virtual ~testClass();

  void myMethod(void);

private:
  int an_arg;
  int another_arg;
};
} // namespace
