
class myClass {
  public:
    myClass();
    ~myClass();

    void myMehtod1(void) {}
    void myMehtod2(void) {}
  private:
    int a;
    float b;
};

struct myStruct {
  myStruct();
  ~myStruct();
};

void myFunc1(void) {}
void myFunc2(void) {}

int main (int argc, char** argv) {
  myClass mc;
  mc.myMehtod1();
  mc.myMehtod2();
  int a;
  float b;
  int c,d;
  a = c+d;
  return 0;
}
