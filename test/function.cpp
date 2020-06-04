
namespace function_ns{
void MyFunc(void) { return; }

class yolo {
public:
  yolo() = default;
  yolo(int a) : a(a) {}
  virtual ~yolo();

  void yolofunc(void) { return; }

private:
  int a;
};

#define MFunc macroedFunc

void MFunc(void) { return; }
} // namespace
