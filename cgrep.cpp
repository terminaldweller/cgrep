
/*first line intentionally left blank.*/
/***********************************************************************************************/
//-*-c++-*-
/*Copyright (C) 2018 Farzad Sadeghi
 * Licensed under GPL-3.0
 * */
/***********************************************************************************************/
/*included modules*/
#include "./cfe-extra/cfe_extra.h"
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
#include <cassert>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
/***********************************************************************************************/
/*used namespaces*/
using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;
/***********************************************************************************************/
namespace {
static llvm::cl::OptionCategory CGrepCat("cgrep options");
cl::opt<std::string> CO_RECURSIVE(
    "dir",
    cl::desc("Recursively goes through all the files and directories. Assumes "
             "compilation databases are present for all source files."),
    cl::init(""), cl::cat(CGrepCat), cl::Optional);
cl::opt<std::string> CO_REGEX("regex", cl::desc("The regex to match against."),
                              cl::init(""), cl::cat(CGrepCat),
                              cl::Optional); // done
cl::opt<std::string>
    CO_TRACE("trace", cl::desc("The variable that's going to be traced"),
             cl::init(""), cl::cat(CGrepCat),
             cl::Optional); // done
cl::opt<bool> CO_FUNCTION("func", cl::desc("Match functions."), cl::init(false),
                          cl::cat(CGrepCat),
                          cl::Optional); // done
cl::opt<bool> CO_MEM_FUNCTION("memfunc", cl::desc("Match member functions."),
                              cl::init(false), cl::cat(CGrepCat),
                              cl::Optional); // done
cl::opt<bool> CO_VAR("var", cl::desc("Match variables."), cl::init(false),
                     cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_CALL("call", cl::desc("Match function calls."),
                      cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_CXXCALL("cxxcall", cl::desc("Match member function calls."),
                         cl::init(false), cl::cat(CGrepCat),
                         cl::Optional); // done
cl::opt<bool> CO_CFIELD("cfield", cl::desc("Match C field declarations."),
                        cl::init(false), cl::cat(CGrepCat),
                        cl::Optional); // done
cl::opt<bool> CO_CLASS("class", cl::desc("Match class declarations."),
                       cl::init(false), cl::cat(CGrepCat),
                       cl::Optional); // done
cl::opt<bool> CO_STRUCT("struct", cl::desc("Match structures."),
                        cl::init(false), cl::cat(CGrepCat),
                        cl::Optional); // done
cl::opt<bool> CO_CXXFIELD("cxxfield",
                          cl::desc("Match CXX field member declarations."),
                          cl::init(false), cl::cat(CGrepCat),
                          cl::Optional); // done
cl::opt<bool> CO_RECORD("recorddecl", cl::desc("Match a record declaration."),
                        cl::init(false), cl::cat(CGrepCat),
                        cl::Optional); // done
cl::opt<bool> CO_UNION("union", cl::desc("Match unions."), cl::init(false),
                       cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_MACRO("macro", cl::desc("Match macro definitions."),
                       cl::init(false), cl::cat(CGrepCat),
                       cl::Optional); // done
cl::opt<bool> CO_CLANGDIAG("clangdiag",
                           cl::desc("use clang's diagnostic consumer instead "
                                    "of the one that cgrep provide."),
                           cl::init(false), cl::cat(CGrepCat),
                           cl::Optional); // done
cl::opt<bool> CO_HEADER("header",
                        cl::desc("Match headers in header inclusions."),
                        cl::init(false), cl::cat(CGrepCat),
                        cl::Optional); // done
cl::opt<bool> CO_ALL("all",
                     cl::desc("Turns on all switches other than nameddecl."),
                     cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_NAMEDDECL("nameddecl",
                           cl::desc("Matches all named declarations."),
                           cl::init(false), cl::cat(CGrepCat),
                           cl::Optional); // done
cl::opt<bool> CO_DECLREFEXPR("declrefexpr", cl::desc("Matches declrefexpr."),
                             cl::init(false), cl::cat(CGrepCat),
                             cl::Optional); // done
cl::opt<bool>
    CO_AWK("awk",
           cl::desc("Outputs location in a gawk friendly format, not meant for "
                    "human consumption. Defaults to false."),
           cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool>
    CO_NOCOLOR("nocolor",
               cl::desc("For terminals that don't support ANSI escape "
                        "sequences. The default is to false."),
               cl::init(false), cl::cat(CGrepCat),
               cl::Optional); // done
cl::opt<bool>
    CO_NODECL("nodecl",
              cl::desc("For switches that are not declarations, don't print "
                       "declarations. Defaults to false."),
              cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_SYSHDR(
    "syshdr",
    cl::desc("Match identifiers in system header as well. Defaults to true."),
    cl::init(false), cl::cat(CGrepCat),
    cl::Optional); // done
cl::opt<bool> CO_MAINFILE(
    "mainfile",
    cl::desc("Match identifiers in the main file only. Defaults to true."),
    cl::init(true), cl::cat(CGrepCat),
    cl::Optional); // done
cl::opt<int> CO_A("A",
                  cl::desc("Same as grep, how many lines after the matched "
                           "line to print. Defaults to 0."),
                  cl::init(0), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<int> CO_B("B",
                  cl::desc("Same as grep, how many lines before the matched "
                           "line to print. Defaults to 0."),
                  cl::init(0), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<int>
    CO_C("C",
         cl::desc("Same as grep, how many lines before and after the matched "
                  "line to print. Defaults to 0."),
         cl::init(0), cl::cat(CGrepCat), cl::Optional); // done
} // namespace
/***********************************************************************************************/
#if 1
#define REGEX_PP(RX_STR) RX_STR
#endif
#if 0
#define REGEX_PP(RX_STR) regex_preprocessor(RX_STR)
#endif

#if __clang_major__ <= 6
#define DEVI_GETLOCSTART getLocStart
#define DEVI_GETLOCEND getLocEnd
#elif __clang_major__ >= 7
#define DEVI_GETLOCSTART getBeginLoc
#define DEVI_GETLOCEND getEndLoc
#endif

#if __clang_major__ >= 12
#define AST_TYPE_TRAITS clang
#else
#define AST_TYPE_TRAITS clang::ast_type_traits
#endif

#define RED "\033[1;31m"
#define CYAN "\033[1;36m"
#define GREEN "\033[1;32m"
#define BLUE "\033[1;34m"
#define BLACK "\033[1;30m"
#define BROWN "\033[1;33m"
#define MAGENTA "\033[1;35m"
#define GRAY "\033[1;37m"
#define DARKGRAY "\033[1;30m"
#define YELLOW "\033[1;33m"
#define NORMAL "\033[0m"
#define CLEAR "\033[2J"

#define CC_RED (CO_NOCOLOR == true ? "" : RED)
#define CC_CYAN (CO_NOCOLOR == true ? "" : CYAN)
#define CC_GREEN (CO_NOCOLOR == true ? "" : GREEN)
#define CC_BLUE (CO_NOCOLOR == true ? "" : BLUE)
#define CC_BLACK (CO_NOCOLOR == true ? "" : BLACK)
#define CC_BROWN (CO_NOCOLOR == true ? "" : BROWN)
#define CC_MAGENTA (CO_NOCOLOR == true ? "" : MAGENTA)
#define CC_GRAY (CO_NOCOLOR == true ? "" : GRAY)
#define CC_DARKGRAY (CO_NOCOLOR == true ? "" : DARKGRAY)
#define CC_YELLOW (CO_NOCOLOR == true ? "" : YELLOW)
#define CC_NORMAL (CO_NOCOLOR == true ? "" : NORMAL)
#define CC_CLEAR (CO_NOCOLOR == true ? "" : CLEAR)
/***********************************************************************************************/
static std::string get_line_from_file(SourceManager &SM,
                                      const MatchFinder::MatchResult &MR,
                                      SourceRange SR) {
  std::string Result = "";

  std::ifstream mainfile;
  std::string mainfile_str = MR.SourceManager->getFilename(SR.getBegin()).str();
  mainfile.open(mainfile_str);
  auto linenumber = MR.SourceManager->getSpellingLineNumber(SR.getBegin());

  std::string line;
  unsigned line_nu = 0;

  while (getline(mainfile, line)) {
    line_nu++;
    if (line_nu == linenumber) {
      Result = line;
      std::cout << CC_GREEN << "\n"
                << mainfile_str << ":" << linenumber << ":" << line
                << "\t <---declared here" << CC_NORMAL << "\n";
    }
  }

  return Result;
}

/**
 * @brief does some preprocessing on the regex string we get as input
 * @param rx_str
 * @return the preprocessed string
 */
std::string regex_preprocessor(const std::string &rx_str) {
  std::string ret_rx_str;
  return rx_str;
}

bool regex_handler(std::string rx_str, std::string identifier_name) {
  std::regex rx(rx_str);
  std::smatch result;
  return std::regex_search(identifier_name, result, rx);
}

/**
 * @brief all print outs pass through here
 *
 * @param MR match result
 * @param SR source range for the matched result
 * @param SM sourcemanager
 * @param isdecl is the matched result a declaration
 * @param DTN the matched result cast to a dynamically typed node
 */
void output_handler(const MatchFinder::MatchResult &MR, SourceRange SR,
                    SourceManager &SM, bool isdecl,
                    AST_TYPE_TRAITS::DynTypedNode &DTN) {
  std::ifstream mainfile;
  mainfile.open(MR.SourceManager->getFilename(SR.getBegin()).str());
  auto linenumber = MR.SourceManager->getSpellingLineNumber(SR.getBegin());
  auto columnnumber_start =
      MR.SourceManager->getSpellingColumnNumber(SR.getBegin()) - 1;
  auto columnnumber_end =
      MR.SourceManager->getSpellingColumnNumber(SR.getEnd()) - 1;
  if (CO_AWK) {
    std::cout << CC_MAGENTA << SR.getBegin().printToString(SM) << ":"
              << SR.getEnd().printToString(SM) << CC_NORMAL << "\n";
    std::cout << CC_RED << MR.SourceManager->getFilename(SR.getBegin()).str()
              << ":" << linenumber << ":" << columnnumber_start << CC_NORMAL;
  } else {
    unsigned line_range_begin;
    unsigned line_range_end;
    if (0 >= CO_C) {
      line_range_begin = linenumber - CO_C;
      line_range_end = linenumber + CO_C;
    }
    line_range_begin = linenumber - CO_B;
    line_range_end = linenumber + CO_A;
    std::string line;
    unsigned line_nu = 0;
    while (getline(mainfile, line)) {
      line_nu++;
      if (line_nu >= line_range_begin && line_nu <= line_range_end) {
        if (line_nu == linenumber) {
          std::cout << CC_RED
                    << MR.SourceManager->getFilename(SR.getBegin()).str() << ":"
                    << linenumber << ":" << columnnumber_start << ":"
                    << CC_NORMAL;
          for (unsigned i = 0; i < line.length(); ++i) {
            if (i >= columnnumber_start && i <= columnnumber_end) {
              std::cout << CC_RED << line[i] << CC_NORMAL;
            } else {
              std::cout << line[i];
            }
          }
          if (!CO_NODECL) {
            if (!isdecl) {
              const NamedDecl *ND = DTN.get<NamedDecl>();
              if (nullptr != ND) {
                SourceRange ND_SR = ND->getSourceRange();
                get_line_from_file(SM, MR, ND_SR);
              }

              const CallExpr *CE = DTN.get<CallExpr>();
              if (nullptr != CE) {
                SourceRange CE_SR = CE->getDirectCallee()->getSourceRange();
                get_line_from_file(SM, MR, CE_SR);
              }
            }
          } else {
            std::cout << line << "\n";
          }
        } else {
          std::cout << line << "\n";
        }
      }
    }
  }
  std::cout << "\n";
  mainfile.close();
}
/**
 * @brief Gets the list of all directories and sub-directories starting from a
 * base directory.
 * @param _path where the base directory is.
 * @return Returns the list of all found dirs.
 */
std::vector<std::string> listDirs(std::string path) {
  std::vector<std::string> dummy;
  DIR *dir;
  if ((dir = opendir(path.c_str())) != nullptr) {
    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
      std::cout << "name: " << ent->d_name << "\ttype:" << int(ent->d_type)
                << "\n";
      if (ent->d_type == DT_DIR) {
        std::cout << ent->d_name << "\n";
      }
      dummy.push_back(ent->d_name);
    }
  } else {
    perror("could not open directory.");
  }
  return dummy;
}
/***********************************************************************************************/
class FunctionHandler : public MatchFinder::MatchCallback {
public:
  explicit FunctionHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const FunctionDecl *FD =
        MR.Nodes.getNodeAs<clang::FunctionDecl>("funcdecl");
    if (FD) {
      DeclarationNameInfo DNI = FD->getNameInfo();
      SourceRange SR = DNI.getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = FD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*FD);
        auto StartLocation = FD->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager,
                       FD->isThisDeclarationADefinition(), DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class FieldHandler : public MatchFinder::MatchCallback {
public:
  explicit FieldHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const FieldDecl *FD = MR.Nodes.getNodeAs<clang::FieldDecl>("fielddecl");
    if (FD) {
      SourceRange SR = FD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = FD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*FD);
        auto StartLocation = FD->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, true, DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class CXXMethodHandler : public MatchFinder::MatchCallback {
public:
  explicit CXXMethodHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const CXXMethodDecl *MD =
        MR.Nodes.getNodeAs<clang::CXXMethodDecl>("cxxmethoddecl");
    if (MD) {
      DeclarationNameInfo DNI = MD->getNameInfo();
      SourceRange SR = DNI.getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = MD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*MD);
        auto StartLocation = MD->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager,
                       MD->isThisDeclarationADefinition(), DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class VDecl : public MatchFinder::MatchCallback {
public:
  explicit VDecl(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const VarDecl *VD = MR.Nodes.getNodeAs<clang::VarDecl>("vardecl");
    if (VD) {
      SourceRange SR = VD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = VD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*VD);
        auto StartLocation = VD->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, true, DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class ClassDecl : public MatchFinder::MatchCallback {
public:
  explicit ClassDecl(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const RecordDecl *RD = MR.Nodes.getNodeAs<clang::RecordDecl>("classdecl");
    if (RD) {
      SourceRange SR = RD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*RD);
        auto StartLocation = RD->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, true, DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class StructHandler : public MatchFinder::MatchCallback {
public:
  explicit StructHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const RecordDecl *RD = MR.Nodes.getNodeAs<clang::RecordDecl>("structdecl");
    if (RD) {
      SourceRange SR = RD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*RD);
        auto StartLocation = RD->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, true, DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class UnionHandler : public MatchFinder::MatchCallback {
public:
  explicit UnionHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const RecordDecl *RD = MR.Nodes.getNodeAs<clang::RecordDecl>("uniondecl");
    if (RD) {
      SourceRange SR = RD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*RD);
        auto StartLocation = RD->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, true, DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class NamedDeclHandler : public MatchFinder::MatchCallback {
public:
  explicit NamedDeclHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const NamedDecl *ND = MR.Nodes.getNodeAs<clang::NamedDecl>("namedecl");
    if (ND) {
      SourceRange SR = ND->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = ND->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*ND);
        auto StartLocation = ND->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, true, DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class DeclRefExprHandler : public MatchFinder::MatchCallback {
public:
  explicit DeclRefExprHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const DeclRefExpr *DRE =
        MR.Nodes.getNodeAs<clang::DeclRefExpr>("declrefexpr");
    if (DRE) {
      const NamedDecl *ND = DRE->getFoundDecl();
      std::string name = ND->getNameAsString();
      SourceLocation SL = DRE->DEVI_GETLOCSTART();
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      SourceLocation SLE = SL.getLocWithOffset(name.length() - 1);
      // SourceLocation SLE = DRE->DEVI_GETLOCEND();
      CheckSLValidity(SL);
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DTN =
            AST_TYPE_TRAITS::DynTypedNode::create(*ND);
        auto StartLocation = ND->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, SourceRange(SL, SLE), *MR.SourceManager, false, DTN);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class CallExprHandler : public MatchFinder::MatchCallback {
public:
  explicit CallExprHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const CallExpr *CE = MR.Nodes.getNodeAs<clang::CallExpr>("callexpr");
    if (CE) {
      SourceLocation SL = CE->DEVI_GETLOCSTART();
      SourceLocation SLE = CE->DEVI_GETLOCEND();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      const NamedDecl *ND = CE->getDirectCallee();
      if (ND == nullptr) {
        return void();
      }
      std::string name = ND->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DTN =
            AST_TYPE_TRAITS::DynTypedNode::create(*CE);
        auto StartLocation = CE->getExprLoc();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, false, DTN);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class CXXCallExprHandler : public MatchFinder::MatchCallback {
public:
  explicit CXXCallExprHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const CXXMemberCallExpr *CE =
        MR.Nodes.getNodeAs<clang::CXXMemberCallExpr>("cxxcallexpr");
    if (CE) {
      SourceRange SR = CE->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      const NamedDecl *ND = CE->getDirectCallee();
      if (ND == nullptr) {
        return void();
      }
      std::string name = ND->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*CE);
        auto StartLocation = CE->getExprLoc();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, false, DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class RecordFieldHandler : public MatchFinder::MatchCallback {
public:
  explicit RecordFieldHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const FieldDecl *FD =
        MR.Nodes.getNodeAs<clang::FieldDecl>("recordfielddecl");
    if (FD) {
      SourceRange SR = FD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = FD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*FD);
        auto StartLocation = FD->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, true, DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class RecordHandler : public MatchFinder::MatchCallback {
public:
  explicit RecordHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const RecordDecl *RD = MR.Nodes.getNodeAs<clang::RecordDecl>("recorddecl");
    if (RD) {
      SourceRange SR = RD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        AST_TYPE_TRAITS::DynTypedNode DNode =
            AST_TYPE_TRAITS::DynTypedNode::create(*RD);
        auto StartLocation = RD->getLocation();
        auto EndLocation = StartLocation.getLocWithOffset(name.size() - 1);
        auto Range = SourceRange(StartLocation, EndLocation);
        output_handler(MR, Range, *MR.SourceManager, true, DNode);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class TraceVarHandlerSub : public MatchFinder::MatchCallback {
public:
  explicit TraceVarHandlerSub(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const DeclRefExpr *DRE =
        MR.Nodes.getNodeAs<clang::DeclRefExpr>("tracevardeclrefexpr");
    if (DRE) {
      if (DRE->getFoundDecl() == ND) {
        std::cout << DRE->getLocation().printToString(*(MR.SourceManager))
                  << "\n";
      }
    }
  }

  void setND(NamedDecl const *Original_Declaration) {
    ND = Original_Declaration;
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
  NamedDecl const *ND;
};
/***********************************************************************************************/
class TraceVarHandler : public MatchFinder::MatchCallback {
public:
  explicit TraceVarHandler(Rewriter &Rewrite)
      : Rewrite(Rewrite), SubMatcher(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) override {
    const VarDecl *VD = MR.Nodes.getNodeAs<clang::VarDecl>("tracevar");
    if (VD) {
      SourceRange SR = VD->getSourceRange();
      SourceLocation SL = SR.getBegin();
      CheckSLValidity(SL);
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      auto NameRef = VD->getName();
      if (CO_TRACE == NameRef.str()) {
        SubMatcher.setND(VD->getCanonicalDecl()->getUnderlyingDecl());
        Matcher.addMatcher(declRefExpr(to(varDecl(hasName(VD->getName()))))
                               .bind("tracevardeclrefexpr"),
                           &SubMatcher);
        Matcher.matchAST(*(MR.Context));
      }
    }
  }

private:
  MatchFinder Matcher;
  Rewriter &Rewrite [[maybe_unused]];
  TraceVarHandlerSub SubMatcher;
};
/***********************************************************************************************/
class SubDynamicMatcher : public MatchFinder::MatchCallback {
public:
  explicit SubDynamicMatcher(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) override {}

private:
  MatchFinder Matcher;
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class DynamicMatcher : public MatchFinder::MatchCallback {
public:
  explicit DynamicMatcher(Rewriter &Rewrite)
      : Rewrite(Rewrite), SubDynamicHandler(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) override {}

private:
  MatchFinder Matcher;
  Rewriter &Rewrite [[maybe_unused]];
  SubDynamicMatcher SubDynamicHandler;
};
/***********************************************************************************************/
class PPInclusion : public PPCallbacks {
public:
  explicit PPInclusion(SourceManager *SM, Rewriter *Rewrite)
      : SM(*SM), Rewrite(*Rewrite) {}

  virtual bool FileNotFound(StringRef FileName,
                            SmallVectorImpl<char> &RecoveryPath) {
    std::cerr << CC_RED << "Header not found: " << FileName.str() << CC_NORMAL
              << "\n";
    exit(1);
  }

  virtual void MacroDefined(const Token &MacroNameTok,
                            const MacroDirective *MD) {
    if (CO_MACRO) {
      SourceLocation SL = MD->getLocation();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, SM, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, SM, SL))
        return void();
      std::string name = MacroNameTok.getIdentifierInfo()->getName().str();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SL.printToString(SM) << "\n";
      }
    }
  }

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
#elif __clang_major__ >= 7
  virtual void InclusionDirective(SourceLocation HashLoc,
                                  const Token &IncludeTok, StringRef FileName,
                                  bool IsAngled, CharSourceRange FilenameRange,
                                  const FileEntry *File, StringRef SearchPath,
                                  StringRef RelativePath,
                                  const clang::Module *Imported,
                                  SrcMgr::CharacteristicKind FileType) {
#endif
    if (CO_HEADER) {
      CheckSLValidity(HashLoc);
      SourceLocation SL =
          Devi::SourceLocationHasMacro(HashLoc, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, SM, SL)) {
        return void();
      }
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, SM, SL)) {
        return void();
      }
      std::string name = FileName.str();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        std::cout << name << "\t";
        std::cout << SL.printToString(SM) << "\n";
      }
    }
  }

private:
  const SourceManager &SM [[maybe_unused]];
  Rewriter &Rewrite [[maybe_unused]];
};
/***********************************************************************************************/
class CgrepDiagConsumer : public clang::DiagnosticConsumer {
public:
  CgrepDiagConsumer() = default;
  virtual ~CgrepDiagConsumer() {}
  virtual void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                                const Diagnostic &Info) override {
    if ((clang::DiagnosticsEngine::Level::Error == DiagLevel) ||
        (clang::DiagnosticsEngine::Level::Fatal == DiagLevel)) {
      SmallVector<char, 16> OutStr;
      Info.FormatDiagnostic(OutStr);
      std::cout << "Error:";
      for (auto &iter : OutStr)
        std::cout << iter;
      ArrayRef<CharSourceRange> SourceRanges = Info.getRanges();
      for (auto &iter : SourceRanges) {
        if (Info.hasSourceManager()) {
          std::cout << iter.getBegin().printToString(Info.getSourceManager())
                    << ":"
                    << iter.getEnd().printToString(Info.getSourceManager())
                    << "\n";
        }
      }
      ArrayRef<FixItHint> FixItHints [[maybe_unused]] = Info.getFixItHints();
      std::cout << "\n";
    }
  }
};
/***********************************************************************************************/
class TraceASTConsumer : public ASTConsumer {
public:
  explicit TraceASTConsumer(Rewriter &R) : HandlerForTraceVar(R) {
    Matcher.addMatcher(varDecl().bind("tracevar"), &HandlerForTraceVar);
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    Matcher.matchAST(Context);
  }

private:
  TraceVarHandler HandlerForTraceVar;
  MatchFinder Matcher;
};
/***********************************************************************************************/
class DynamicASTConsumer : public ASTConsumer {
public:
  explicit DynamicASTConsumer(Rewriter &R) : DynamicHandler(R) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Matcher.matchAST(Context);
  }

private:
  DynamicMatcher DynamicHandler;
  MatchFinder Matcher;
};
/***********************************************************************************************/
class CgrepASTConsumer : public ASTConsumer {
public:
  explicit CgrepASTConsumer(Rewriter &R)
      : HandlerForVar(R), HandlerForClass(R), HandlerForCalledFunc(R),
        HandlerForCXXMethod(R), HandlerForField(R), HandlerForStruct(R),
        HandlerForUnion(R), HandlerForNamedDecl(R), HandlerForDeclRefExpr(R),
        HandlerForCallExpr(R), HandlerForCXXCallExpr(R),
        HandlerForRecordField(R), HandlerForRecord(R) {
    if (CO_FUNCTION || CO_ALL) {
      Matcher.addMatcher(functionDecl().bind("funcdecl"),
                         &HandlerForCalledFunc);
    }
    if (CO_VAR || CO_ALL) {
      Matcher.addMatcher(
          varDecl(anyOf(unless(hasDescendant(expr(anything()))),
                        hasDescendant(expr(anything()).bind("expr"))))
              .bind("vardecl"),
          &HandlerForVar);
    }
    if (CO_CLASS || CO_ALL) {
      // we are excluding the definitions here, since class declarations and
      // definitions will match separately, so for a class that is declared and
      // defined in the same location, we'll get two matches. A declaration can
      // happen without a definition but the other way around cannot be true.
      Matcher.addMatcher(recordDecl(allOf(isClass(), unless(isDefinition())))
                             .bind("classdecl"),
                         &HandlerForClass);
    }
    if (CO_MEM_FUNCTION || CO_ALL) {
      Matcher.addMatcher(cxxMethodDecl().bind("cxxmethoddecl"),
                         &HandlerForCXXMethod);
    }
    if (CO_CFIELD || CO_ALL) {
      Matcher.addMatcher(fieldDecl().bind("fielddecl"), &HandlerForField);
    }
    if (CO_STRUCT || CO_ALL) {
      Matcher.addMatcher(recordDecl(allOf(isStruct(), unless(isDefinition())))
                             .bind("structdecl"),
                         &HandlerForStruct);
    }
    if (CO_UNION || CO_ALL) {
      Matcher.addMatcher(recordDecl(isUnion()).bind("uniondecl"),
                         &HandlerForUnion);
    }
    if (CO_NAMEDDECL) {
      Matcher.addMatcher(namedDecl().bind("namedecl"), &HandlerForNamedDecl);
    }
    if (CO_DECLREFEXPR || CO_ALL) {
      Matcher.addMatcher(declRefExpr().bind("declrefexpr"),
                         &HandlerForDeclRefExpr);
    }
    if (CO_CALL || CO_ALL) {
      Matcher.addMatcher(callExpr().bind("callexpr"), &HandlerForCallExpr);
    }
    if (CO_CXXCALL || CO_ALL) {
      Matcher.addMatcher(cxxMemberCallExpr().bind("cxxcallexpr"),
                         &HandlerForCXXCallExpr);
    }
    if (CO_CXXFIELD || CO_ALL) {
      Matcher.addMatcher(
          fieldDecl(hasParent(cxxRecordDecl())).bind("recordfielddecl"),
          &HandlerForRecordField);
    }
    if (CO_RECORD || CO_ALL) {
      Matcher.addMatcher(recordDecl().bind("recorddecl"), &HandlerForRecord);
    }
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
  DeclRefExprHandler HandlerForDeclRefExpr;
  CallExprHandler HandlerForCallExpr;
  CXXCallExprHandler HandlerForCXXCallExpr;
  RecordFieldHandler HandlerForRecordField;
  RecordHandler HandlerForRecord;
  MatchFinder Matcher;
};
/***********************************************************************************************/
class TraceFrontendAction : public ASTFrontendAction {
public:
  TraceFrontendAction() {}
  ~TraceFrontendAction() {}

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    if (!CO_CLANGDIAG) {
      DiagnosticsEngine &DE = CI.getPreprocessor().getDiagnostics();
      DE.setClient(BDCProto, false);
    }
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
#if __clang_major__ <= 9
    return llvm::make_unique<TraceASTConsumer>(TheRewriter);
#endif
#if __clang_major__ >= 10
    return std::make_unique<TraceASTConsumer>(TheRewriter);
#endif
  }

private:
  CgrepDiagConsumer *BDCProto = new CgrepDiagConsumer;
  Rewriter TheRewriter;
};
/***********************************************************************************************/
class CgrepFrontendAction : public ASTFrontendAction {
public:
  CgrepFrontendAction() {}
  ~CgrepFrontendAction() { delete BDCProto; }

  void EndSourceFileAction() override { std::error_code EC; }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
#if __clang_major__ <= 9
    CI.getPreprocessor().addPPCallbacks(
        llvm::make_unique<PPInclusion>(&CI.getSourceManager(), &TheRewriter));
#endif
#if __clang_major__ >= 10
    CI.getPreprocessor().addPPCallbacks(
        std::make_unique<PPInclusion>(&CI.getSourceManager(), &TheRewriter));
#endif
    if (!CO_CLANGDIAG) {
      DiagnosticsEngine &DE = CI.getPreprocessor().getDiagnostics();
      DE.setClient(BDCProto, false);
    }
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
#if __clang_major__ <= 9
    return llvm::make_unique<CgrepASTConsumer>(TheRewriter);
#endif
#if __clang_major__ >= 10
    return std::make_unique<CgrepASTConsumer>(TheRewriter);
#endif
  }

private:
  CgrepDiagConsumer *BDCProto = new CgrepDiagConsumer;
  Rewriter TheRewriter;
};
/***********************************************************************************************/
/*Main*/
int main(int argc, const char **argv) {
#if __clang_major__ >= 13
  auto op = CommonOptionsParser::create(argc, argv, CGrepCat);
  if (auto error = op.takeError()) {
    errs() << toString(std::move(error)) << "\n";
    return 1;
  }
  ClangTool Tool(op->getCompilations(), op->getSourcePathList());
#else
  CommonOptionsParser op(argc, argv, CGrepCat);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
#endif
  int ret = 0;

  if ("" != CO_TRACE) {
    ret = Tool.run(newFrontendActionFactory<TraceFrontendAction>().get());
  } else {
    ret = Tool.run(newFrontendActionFactory<CgrepFrontendAction>().get());
  }

  return ret;
}
/***********************************************************************************************/
