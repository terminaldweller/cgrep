
/*first line intentionally left blank.*/
/*************************************************************************************************/
//-*-c++-*-
/*Copyright (C) 2018 Farzad Sadeghi
 * Licensed under GPL-3.0
 * */
/*************************************************************************************************/
/*included modules*/
/*project headers*/
#include "./cfe-extra/cfe_extra.h"
/*standard headers*/
#include <cassert>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
/*LLVM headers*/
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/LLVM.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"
/*************************************************************************************************/
/*used namespaces*/
using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;
/*************************************************************************************************/
namespace {
static llvm::cl::OptionCategory CGrepCat("cgrep options");
cl::opt<std::string> CO_DIRECTORY("dir", cl::desc("recursively goes through all the files and directories. assumes compilation databases are present for all source files."), cl::init(""), cl::cat(CGrepCat), cl::Optional);
cl::opt<std::string> CO_REGEX("regex", cl::desc("the regex to match against"), cl::init(""), cl::cat(CGrepCat), cl::Required); //done
cl::opt<bool> CO_FUNCTION("func", cl::desc("match functions only"), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_MEM_FUNCTION("memfunc", cl::desc("match member functions only"), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_VAR("var", cl::desc("map variables only"), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_MEMVAR("memvar", cl::desc("match member variables only"), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_CLASS("class", cl::desc("match class declrations only"), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_STRUCT("struct", cl::desc("match structures only"), cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_UNION("union", cl::desc("match unions only"), cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_MACRO("macro", cl::desc("match macro definitions"), cl::init(false), cl::cat(CGrepCat), cl::Optional);
cl::opt<bool> CO_HEADER("header", cl::desc("match headers in header inclusions"), cl::init(false), cl::cat(CGrepCat), cl::Optional);
cl::opt<bool> CO_ALL("all", cl::desc("turns on all switches other than nameddecl"), cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_NAMEDDECL("nameddecl", cl::desc("matches all named declrations"), cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_AWK("awk", cl::desc("outputs location in a gawk freidnly format"), cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_SYSHDR("syshdr", cl::desc("match identifiers in system header as well"), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_MAINFILE("mainfile", cl::desc("mathc identifiers in the main file only"), cl::init(true), cl::cat(CGrepCat), cl::Optional); //done
}
/*************************************************************************************************/
#if 1
#define REGEX_PP(RX_STR) RX_STR
#endif
#if 0
#define REGEX_PP(RX_STR) regex_preprocessor(RX_STR)
#endif

/**
 * @brief does some preprocessing on the regex string we get as input
 * @param rx_str
 * @return the preprocessed string
 */
std::string regex_preprocessor(std::string rx_str) {
  std::string ret_rx_str;
  return ret_rx_str;
}

bool regex_handler(std::string rx_str, std::string identifier_name) {
  std::regex rx(rx_str);
  std::smatch result;
  return std::regex_search(identifier_name, result, rx);
}

std::string output_handler(SourceLocation SL, SourceManager &SM) {
  return SL.printToString(SM);
}

std::string output_handler(SourceRange SR, SourceManager &SM) {
  return SR.getBegin().printToString(SM);
}
/*************************************************************************************************/
class FunctionHandler : public MatchFinder::MatchCallback {
public:
  FunctionHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const FunctionDecl *FD = MR.Nodes.getNodeAs<clang::FunctionDecl>("funcdecl");
    if (FD) {
      SourceRange SR = FD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL)) return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL)) return void();
      std::string name = FD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SR.getBegin().printToString(*MR.SourceManager) << "\t";
        std::cout << SR.getEnd().printToString(*MR.SourceManager) << "\n";
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class FieldHandler : public MatchFinder::MatchCallback {
public:
  FieldHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const FieldDecl *VD = MR.Nodes.getNodeAs<clang::FieldDecl>("fielddecl");
    if (VD) {
      SourceRange SR = VD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL)) return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL)) return void();
      std::string name = VD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SR.getBegin().printToString(*MR.SourceManager) << "\t";
        std::cout << SR.getEnd().printToString(*MR.SourceManager) << "\n";
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class CXXMethodHandler : public MatchFinder::MatchCallback {
public:
  CXXMethodHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const CXXMethodDecl *MD = MR.Nodes.getNodeAs<clang::CXXMethodDecl>("cxxmethoddecl");
    if (MD) {
      SourceRange SR = MD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL)) return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL)) return void();
      std::string name = MD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SR.getBegin().printToString(*MR.SourceManager) << "\t";
        std::cout << SR.getEnd().printToString(*MR.SourceManager) << "\n";
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class VDecl : public MatchFinder::MatchCallback {
public:
  VDecl(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const VarDecl *VD = MR.Nodes.getNodeAs<clang::VarDecl>("vardecl");
    if (VD) {
      SourceRange SR = VD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL)) return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL)) return void();
      std::string name = VD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SR.getBegin().printToString(*MR.SourceManager) << "\t";
        std::cout << SR.getEnd().printToString(*MR.SourceManager) << "\n";
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class ClassDecl : public MatchFinder::MatchCallback {
public:
  ClassDecl(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const RecordDecl *RD = MR.Nodes.getNodeAs<clang::RecordDecl>("classdecl");
    if (RD) {
      SourceRange SR = RD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL)) return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL)) return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SR.getBegin().printToString(*MR.SourceManager) << "\t";
        std::cout << SR.getEnd().printToString(*MR.SourceManager) << "\n";
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class StructHandler : public MatchFinder::MatchCallback {
public:
  StructHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const RecordDecl *RD = MR.Nodes.getNodeAs<clang::RecordDecl>("structdecl");
    if (RD) {
      SourceRange SR = RD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL)) return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL)) return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SR.getBegin().printToString(*MR.SourceManager) << "\t";
        std::cout << SR.getEnd().printToString(*MR.SourceManager) << "\n";
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class UnionHandler : public MatchFinder::MatchCallback {
public:
  UnionHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const RecordDecl *RD = MR.Nodes.getNodeAs<clang::RecordDecl>("uniondecl");
    if (RD) {
      SourceRange SR = RD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL)) return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL)) return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SR.getBegin().printToString(*MR.SourceManager) << "\t";
        std::cout << SR.getEnd().printToString(*MR.SourceManager) << "\n";
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class NamedDeclHandler : public MatchFinder::MatchCallback {
public:
  NamedDeclHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const NamedDecl *ND = MR.Nodes.getNodeAs<clang::NamedDecl>("namedecl");
    if (ND) {
      SourceRange SR = ND->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL)) return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL)) return void();
      std::string name = ND->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SR.getBegin().printToString(*MR.SourceManager) << "\t";
        std::cout << SR.getEnd().printToString(*MR.SourceManager) << "\n";
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class PPInclusion : public PPCallbacks {
public:
  explicit PPInclusion(SourceManager *SM, Rewriter *Rewrite)
      : SM(*SM), Rewrite(*Rewrite) {}

  virtual void MacroDefined(const Token &MacroNameTok,
                            const MacroDirective *MD) {}

  virtual void MacroExpands(const Token &MacroNameTok,
                            const MacroDefinition &MD, SourceRange Range,
                            const MacroArgs *Args) {}

#if __clang_major__ <= 6
  virtual void InclusionDirective(SourceLocation HashLoc,
                                  const Token &IncludeTok, StringRef FileName,
                                  bool IsAngled, CharSourceRange FilenameRange,
                                  const FileEntry *File, StringRef SearchPath,
                                  StringRef RelativePath,
                                  const clang::Module *Imported) {
#elif __clang_major__ >= 8
  virtual void InclusionDirective(SourceLocation HashLoc,
                                  const Token &IncludeTok, StringRef FileName,
                                  bool IsAngled, CharSourceRange FilenameRange,
                                  const FileEntry *File, StringRef SearchPath,
                                  StringRef RelativePath,
                                  const clang::Module *Imported,
                                  SrcMgr::CharacteristicKind FileType) {
#endif
  }

private:
  const SourceManager &SM [[maybe_unused]];
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
// brief A Clang Diagnostic Consumer that does nothing.
class BlankDiagConsumer : public clang::DiagnosticConsumer {
public:
  BlankDiagConsumer() = default;
  virtual ~BlankDiagConsumer() {}
  virtual void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                                const Diagnostic &Info) override {}
};
/*************************************************************************************************/
class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(Rewriter &R)
      : HandlerForVar(R), HandlerForClass(R),
        HandlerForCalledFunc(R), HandlerForCXXMethod(R), HandlerForField(R), HandlerForStruct(R), HandlerForUnion(R), HandlerForNamedDecl(R) {
#if 1
    if (CO_FUNCTION || CO_ALL) {
      Matcher.addMatcher(functionDecl().bind("funcdecl"), &HandlerForCalledFunc);}
    if (CO_VAR || CO_ALL) {
      Matcher.addMatcher(varDecl(anyOf(unless(hasDescendant(expr(anything()))), hasDescendant(expr(anything()).bind("expr")))).bind("vardecl"),&HandlerForVar);}
    if (CO_CLASS || CO_ALL) {
      Matcher.addMatcher(recordDecl(isClass()).bind("classdecl"),&HandlerForClass);}
    if (CO_MEM_FUNCTION || CO_ALL) {
      Matcher.addMatcher(cxxMethodDecl().bind("cxxmethoddecl"), &HandlerForCXXMethod);}
    if (CO_MEMVAR || CO_ALL) {
      Matcher.addMatcher(fieldDecl().bind("fielddecl"), &HandlerForField);}
    if (CO_STRUCT || CO_ALL) {
      Matcher.addMatcher(recordDecl(isStruct()).bind("structdecl"), &HandlerForStruct);}
    if (CO_UNION || CO_ALL) {
      Matcher.addMatcher(recordDecl(isUnion()).bind("uniondecl"), &HandlerForUnion);}
    if (CO_NAMEDDECL) {
      Matcher.addMatcher(namedDecl().bind("namedecl"), &HandlerForNamedDecl);}
#endif
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    Matcher.matchAST(Context);
  }

private:
  VDecl HandlerForVar;
  ClassDecl HandlerForClass;
  FunctionHandler HandlerForCalledFunc;
  CXXMethodHandler HandlerForCXXMethod;
  FieldHandler HandlerForField;
  StructHandler HandlerForStruct;
  UnionHandler HandlerForUnion;
  NamedDeclHandler HandlerForNamedDecl;
  MatchFinder Matcher;
};
/*************************************************************************************************/
class AppFrontendAction : public ASTFrontendAction {
public:
  AppFrontendAction() {}
  ~AppFrontendAction() { delete BDCProto; }

  void EndSourceFileAction() override {
    std::error_code EC;
    //TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID()).write(llvm::outs());
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    CI.getPreprocessor().addPPCallbacks(
        llvm::make_unique<PPInclusion>(&CI.getSourceManager(), &TheRewriter));
    DiagnosticsEngine &DE = CI.getPreprocessor().getDiagnostics();
    DE.setClient(BDCProto, false);
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return llvm::make_unique<MyASTConsumer>(TheRewriter);
  }

private:
  BlankDiagConsumer *BDCProto = new BlankDiagConsumer;
  Rewriter TheRewriter;
};
/*************************************************************************************************/
/*************************************************************************************************/
/*Main*/
int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, CGrepCat);
  const std::vector<std::string> &SourcePathList [[maybe_unused]] = op.getSourcePathList();
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  int ret = Tool.run(newFrontendActionFactory<AppFrontendAction>().get());
  return ret;
}
/*************************************************************************************************/
