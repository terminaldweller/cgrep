
namespace uniondecl_ns{
union testUnion {
  int reg;
  bool b1;
  bool b2;
};

#define uniondecdefmacro unionDecDefMacroExpanded
union uniondecdefmacro {
  int reggie;
  int bubu;
};
}; // namespace
