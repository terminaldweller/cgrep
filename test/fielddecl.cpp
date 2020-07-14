
namespace fielddecl_ns{
#define fieldmacro fieldthree

struct testStruct {
  int fieldone;
  float testfieldtwo;
  int fieldmacro;
};

union testUnion {
  int testfieldone;
  bool fieldtwo;
};

class testClass {
public:
  testClass(void);
  virtual ~testClass();

  void testMethod(void);

private:
  int an_arg;
  int another_arg;
};
} // namespace
