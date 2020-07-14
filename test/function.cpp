
namespace function_ns{
void testMyFunc(void) { return; }

class yolo {
public:
  yolo() = default;
  yolo(int a) : a(a) {}
  virtual ~yolo();

  void testyolofunc(void) { return; }

private:
  int a;
};

#define MFunc macroedFunc

void testFunc(void) { return; }
} // namespace
