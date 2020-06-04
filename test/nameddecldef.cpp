
namespace nameddecldef_ns{
int testVar;
struct testStruct {
  int a;
  int b;
  char *c;
};

void testFunction(void);
void testFunction(void) { return; }

class testClass {
public:
  testClass();
  virtual ~testClass();

private:
  int myPrivate;
};
}; // namespace
