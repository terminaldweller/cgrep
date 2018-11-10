
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
cl::opt<std::string> CO_DIRECTORY("dir", cl::desc(""), cl::init(""), cl::cat(CGrepCat), cl::Optional);
cl::opt<std::string> CO_REGEX("regex", cl::desc(""), cl::init(""), cl::cat(CGrepCat), cl::Required); //done
cl::opt<bool> CO_FUNCTION("func", cl::desc(""), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_MEM_FUNCTION("memfunc", cl::desc(""), cl::init(false), cl::cat(CGrepCat), cl::Optional);
cl::opt<bool> CO_VAR("var", cl::desc(""), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_MEMVAR("memvar", cl::desc(""), cl::init(false), cl::cat(CGrepCat), cl::Optional);
cl::opt<bool> CO_CLASS("class", cl::desc(""), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_STRUCT("struct", cl::desc(""), cl::init(false), cl::cat(CGrepCat), cl::Optional);
cl::opt<bool> CO_SYSHDR("syshdr", cl::desc(""), cl::init(false), cl::cat(CGrepCat), cl::Optional); //done
cl::opt<bool> CO_MAINFILE("mainfile", cl::desc(""), cl::init(true), cl::cat(CGrepCat), cl::Optional); //done
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
class VarHandler : public MatchFinder::MatchCallback {
public:
  VarHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class FuncDecl : public MatchFinder::MatchCallback {
public:
  FuncDecl(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {}

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
      : funcDeclHandler(R), HandlerForVar(R), HandlerForClass(R),
        HandlerForCalledFunc(R), HandlerForCalledVar(R) {
#if 1
    if (CO_FUNCTION) {
      Matcher.addMatcher(functionDecl().bind("funcdecl"), &HandlerForCalledFunc);
    }
    if (CO_VAR) {
    Matcher.addMatcher(
        varDecl(anyOf(unless(hasDescendant(expr(anything()))),
                      hasDescendant(expr(anything()).bind("expr"))))
            .bind("vardecl"),
        &HandlerForVar);
    }
    if (CO_CLASS) {
    Matcher.addMatcher(recordDecl(isClass()).bind("classdecl"),
                       &HandlerForClass);
    }
    if (CO_VAR) {
      Matcher.addMatcher(declRefExpr().bind("calledvar"), &HandlerForCalledVar);
    }
#endif
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    Matcher.matchAST(Context);
  }

private:
  FuncDecl funcDeclHandler;
  VDecl HandlerForVar;
  ClassDecl HandlerForClass;
  FunctionHandler HandlerForCalledFunc;
  VarHandler HandlerForCalledVar;
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
