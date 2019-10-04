
/*first line intentionally left blank.*/
/*************************************************************************************************/
//-*-c++-*-
/*Copyright (C) 2018 Farzad Sadeghi
 * Licensed under GPL-3.0
 * */
/*************************************************************************************************/
/*included modules*/
#include "./cfe-extra/cfe_extra.h"
#include "./pch.hpp"
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
cl::opt<std::string> CO_DIRECTORY(
    "dir",
    cl::desc("recursively goes through all the files and directories. assumes "
             "compilation databases are present for all source files."),
    cl::init(""), cl::cat(CGrepCat), cl::Optional);
cl::opt<std::string> CO_REGEX("regex", cl::desc("the regex to match against"),
                              cl::init(""), cl::cat(CGrepCat),
                              cl::Required); // done
cl::opt<bool> CO_FUNCTION("func", cl::desc("match functions only"),
                          cl::init(false), cl::cat(CGrepCat),
                          cl::Optional); // done
cl::opt<bool> CO_MEM_FUNCTION("memfunc",
                              cl::desc("match member functions only"),
                              cl::init(false), cl::cat(CGrepCat),
                              cl::Optional); // done
cl::opt<bool> CO_VAR("var", cl::desc("match variables only"), cl::init(false),
                     cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_CALL("call", cl::desc("match function calls only"),
                      cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_CXXCALL("cxxcall",
                         cl::desc("match member function calls only"),
                         cl::init(false), cl::cat(CGrepCat),
                         cl::Optional); // done
cl::opt<bool> CO_MEMVAR("memvar", cl::desc("match member variables only"),
                        cl::init(false), cl::cat(CGrepCat),
                        cl::Optional); // done
cl::opt<bool> CO_CLASS("class", cl::desc("match class declrations only"),
                       cl::init(false), cl::cat(CGrepCat),
                       cl::Optional); // done
cl::opt<bool> CO_STRUCT("struct", cl::desc("match structures only"),
                        cl::init(false), cl::cat(CGrepCat),
                        cl::Optional); // done
cl::opt<bool> CO_UNION("union", cl::desc("match unions only"), cl::init(false),
                       cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_MACRO("macro", cl::desc("match macro definitions"),
                       cl::init(false), cl::cat(CGrepCat),
                       cl::Optional); // done
cl::opt<bool> CO_HEADER("header",
                        cl::desc("match headers in header inclusions"),
                        cl::init(false), cl::cat(CGrepCat),
                        cl::Optional); // done
cl::opt<bool> CO_ALL("all",
                     cl::desc("turns on all switches other than nameddecl"),
                     cl::init(false), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<bool> CO_NAMEDDECL("nameddecl",
                           cl::desc("matches all named declrations"),
                           cl::init(false), cl::cat(CGrepCat),
                           cl::Optional); // done
cl::opt<bool> CO_DECLREFEXPR("declrefexpr", cl::desc("matches declrefexpr"),
                             cl::init(false), cl::cat(CGrepCat),
                             cl::Optional); // done
cl::opt<bool> CO_AWK("awk",
                     cl::desc("outputs location in a gawk freidnly format"),
                     cl::init(false), cl::cat(CGrepCat), cl::Optional);
cl::opt<bool> CO_SYSHDR("syshdr",
                        cl::desc("match identifiers in system header as well"),
                        cl::init(false), cl::cat(CGrepCat),
                        cl::Optional); // done
cl::opt<bool> CO_MAINFILE("mainfile",
                          cl::desc("match identifiers in the main file only"),
                          cl::init(true), cl::cat(CGrepCat),
                          cl::Optional); // done
cl::opt<int> CO_A(
    "A",
    cl::desc("same as grep, how many lines after the matched line to print"),
    cl::init(0), cl::cat(CGrepCat), cl::Optional); // done
cl::opt<int> CO_B(
    "B",
    cl::desc("same as grep, howm many lines before the matched line to print"),
    cl::init(0), cl::cat(CGrepCat), cl::Optional); // done
} // namespace
/*************************************************************************************************/
#if 1
#define REGEX_PP(RX_STR) RX_STR
#endif
#if 0
#define REGEX_PP(RX_STR) regex_preprocessor(RX_STR)
#endif

#if __clang_major__ <= 6
#define DEVI_GETLOCSTART getLocStart
#define DEVI_GETLOCEND getLocEnd
#elif __clang_major__ >= 8
#define DEVI_GETLOCSTART getBeginLoc
#define DEVI_GETLOCEND getEndLoc
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

void output_handler(MatchFinder::MatchResult &MR, SourceLocation SL,
                    SourceManager &SM, bool isdecl) {}

void output_handler(const MatchFinder::MatchResult &MR, SourceRange SR,
                    SourceManager &SM, bool isdecl) {
  std::ifstream mainfile;
  mainfile.open(MR.SourceManager->getFilename(SR.getBegin()).str());
  auto linenumber = MR.SourceManager->getSpellingLineNumber(SR.getBegin());
  auto columnnumber_start =
      MR.SourceManager->getSpellingColumnNumber(SR.getBegin()) - 1;
  auto columnnumber_end =
      MR.SourceManager->getSpellingColumnNumber(SR.getEnd()) - 1 -
      columnnumber_start;
  // std::cout << MAGENTA << columnnumber_start << ":" << columnnumber_end <<
  // NORMAL << "\n";
  std::cout << MAGENTA << SR.getBegin().printToString(SM) << ":"
            << SR.getEnd().printToString(SM) << NORMAL << "\n";
  unsigned line_range_begin = linenumber - CO_B;
  unsigned line_range_end = linenumber + CO_A;
  std::string line;
  unsigned line_nu = 0;
  while (getline(mainfile, line)) {
    line_nu++;
    if (line_nu >= line_range_begin && line_nu <= line_range_end) {
      if (line_nu == linenumber) {
        std::cout << RED << MR.SourceManager->getFilename(SR.getBegin()).str()
                  << ":" << linenumber << ":" << columnnumber_start << ":"
                  << NORMAL;
        for (unsigned i = 0; i < line.length(); ++i) {
          if (i >= columnnumber_start && i <= columnnumber_end) {
            std::cout << RED << line[i] << NORMAL;
          } else {
            std::cout << line[i];
          }
        }
        if (isdecl)
          std::cout << GREEN << "\t<---defined here" << NORMAL << "\n";
      } else {
        std::cout << line << "\n";
      }
    }
  }
  std::cout << "\n";
  mainfile.close();
}

/**
 * @brief Gets the list of all directories and sub-directories starting from a
 * base directory.
 * @param _path where the the base directory is.
 * @return Returns the list of all found dirs.
 */
std::vector<std::string> listDirs(std::string _path) {
  std::vector<std::string> dummy_;
  DIR *dir_;
  struct dirent *ent_;
  if ((dir_ = opendir(_path.c_str())) != nullptr) {
    while ((ent_ = readdir(dir_)) != nullptr) {
      std::cout << "name: " << ent_->d_name << "\ttype:" << int(ent_->d_type)
                << "\n";
      if (ent_->d_type == DT_DIR) {
        std::cout << ent_->d_name << "\n";
      }
      dummy_.push_back(ent_->d_name);
    }
  } else {
    perror("could not open directory.");
  }
  return dummy_;
}
/*************************************************************************************************/
class FunctionHandler : public MatchFinder::MatchCallback {
public:
  FunctionHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

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
        std::ifstream mainfile;
        mainfile.open(MR.SourceManager->getFilename(SL).str());
        auto linenumber = MR.SourceManager->getSpellingLineNumber(SL);
        auto columnnumber_start =
            MR.SourceManager->getSpellingColumnNumber(SR.getBegin()) - 1;
        auto columnnumber_end =
            columnnumber_start + DNI.getAsString().length() - 1;
        unsigned line_range_begin = linenumber - CO_B;
        unsigned line_range_end = linenumber + CO_A;
        std::string line;
        unsigned line_nu = 0;
        while (getline(mainfile, line)) {
          line_nu++;
          if (line_nu >= line_range_begin && line_nu <= line_range_end) {
            if (line_nu == linenumber) {
              std::cout << RED << MR.SourceManager->getFilename(SL).str() << ":"
                        << linenumber << ":" << columnnumber_start << ":"
                        << NORMAL;
              for (unsigned i = 1; i < line.length(); ++i) {
                if (i >= columnnumber_start && i <= columnnumber_end) {
                  std::cout << RED << line[i] << NORMAL;
                } else {
                  std::cout << line[i];
                }
              }
              std::cout << GREEN << "\t<---defined here" << NORMAL << "\n";
            } else {
              std::cout << line << "\n";
            }
          }
        }
        std::cout << "\n";
        mainfile.close();
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
      // IdentifierInfo* ID = VD->getIdentifier();
      // SourceRange SR = VD->getSourceRange();
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
        std::cout
            << YELLOW
            << MR.SourceManager->getSpellingColumnNumber(VD->getLocation())
            << ":"
            << MR.SourceManager->getSpellingColumnNumber(VD->DEVI_GETLOCEND())
            << NORMAL << "\n";
        // std::cout << BLUE << ID->getLength() << NORMAL << "\n";
        output_handler(MR, SR, *MR.SourceManager, true);
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
        std::ifstream mainfile;
        mainfile.open(MR.SourceManager->getFilename(SL).str());
        auto linenumber = MR.SourceManager->getSpellingLineNumber(SL);
        auto columnnumber_start =
            MR.SourceManager->getSpellingColumnNumber(SR.getBegin()) - 1;
        auto columnnumber_end =
            columnnumber_start + DNI.getAsString().length() - 1;
        unsigned line_range_begin = linenumber - CO_B;
        unsigned line_range_end = linenumber + CO_A;
        std::string line;
        unsigned line_nu = 0;
        while (getline(mainfile, line)) {
          line_nu++;
          if (line_nu >= line_range_begin && line_nu <= line_range_end) {
            if (line_nu == linenumber) {
              std::cout << RED << MR.SourceManager->getFilename(SL).str() << ":"
                        << linenumber << ":" << columnnumber_start << ":"
                        << NORMAL;
              for (unsigned i = 1; i < line.length(); ++i) {
                if (i >= columnnumber_start && i <= columnnumber_end) {
                  std::cout << RED << line[i] << NORMAL;
                } else {
                  std::cout << line[i];
                }
              }
              std::cout << GREEN << "\t<---defined here" << NORMAL << "\n";
            } else {
              std::cout << line << "\n";
            }
          }
        }
        std::cout << "\n";
        mainfile.close();
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
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = VD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        output_handler(MR, SR, *MR.SourceManager, true);
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
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        output_handler(
            MR, SourceRange(RD->DEVI_GETLOCSTART(), RD->DEVI_GETLOCEND()),
            *MR.SourceManager, true);
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
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        output_handler(MR, SR, *MR.SourceManager, true);
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
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = RD->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        output_handler(MR, SR, *MR.SourceManager, true);
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
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      std::string name = ND->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        output_handler(MR, SR, *MR.SourceManager, true);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class DeclRefExprHandler : public MatchFinder::MatchCallback {
public:
  DeclRefExprHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &MR) {
    const DeclRefExpr *DRE =
        MR.Nodes.getNodeAs<clang::DeclRefExpr>("declrefexpr");
    if (DRE) {
      SourceLocation SL = DRE->DEVI_GETLOCSTART();
      SourceLocation SLE = DRE->DEVI_GETLOCEND();
      CheckSLValidity(SL);
      SL = Devi::SourceLocationHasMacro(SL, Rewrite, "start");
      if (Devi::IsTheMatchInSysHeader(CO_SYSHDR, MR, SL))
        return void();
      if (!Devi::IsTheMatchInMainFile(CO_MAINFILE, MR, SL))
        return void();
      const NamedDecl *ND = DRE->getFoundDecl();
      std::string name = ND->getNameAsString();
      std::cout << BLUE << name << NORMAL << "\n";
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        output_handler(MR, SourceRange(SL, SLE), *MR.SourceManager, false);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class CallExprHandler : public MatchFinder::MatchCallback {
public:
  CallExprHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

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
      if (ND)
        return void();
      std::string name = ND->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        output_handler(MR, SourceRange(SL, SLE), *MR.SourceManager, false);
      }
    }
  }

private:
  Rewriter &Rewrite [[maybe_unused]];
};
/*************************************************************************************************/
class CXXCallExprHandler : public MatchFinder::MatchCallback {
public:
  CXXCallExprHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

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
      if (ND)
        return void();
      std::string name = ND->getNameAsString();
      if (regex_handler(REGEX_PP(CO_REGEX), name)) {
        output_handler(MR, SR, *MR.SourceManager, true);
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
#elif __clang_major__ >= 8
  virtual void InclusionDirective(SourceLocation HashLoc,
                                  const Token &IncludeTok, StringRef FileName,
                                  bool IsAngled, CharSourceRange FilenameRange,
                                  const FileEntry *File, StringRef SearchPath,
                                  StringRef RelativePath,
                                  const clang::Module *Imported,
                                  SrcMgr::CharacteristicKind FileType) {
#endif
    // FIXME-we are not checking whether the header has been found.
    // this callback will be called when there is a header inclusion directive
    // in the source file we are running through, not when the header is found.
    // if the header is not there, we'll be dereferencing a null pointer
    // somewhere and segfault.
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
/*************************************************************************************************/
/// @brief A Clang Diagnostic Consumer that does nothing since we don't want
/// clang to print out diag info.
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
      : HandlerForVar(R), HandlerForClass(R), HandlerForCalledFunc(R),
        HandlerForCXXMethod(R), HandlerForField(R), HandlerForStruct(R),
        HandlerForUnion(R), HandlerForNamedDecl(R), HandlerForDeclRefExpr(R),
        HandlerForCallExpr(R), HandlerForCXXCallExpr(R) {
#if 1
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
    if (CO_MEMVAR || CO_ALL) {
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
  DeclRefExprHandler HandlerForDeclRefExpr;
  CallExprHandler HandlerForCallExpr;
  CXXCallExprHandler HandlerForCXXCallExpr;
  MatchFinder Matcher;
};
/*************************************************************************************************/
class AppFrontendAction : public ASTFrontendAction {
public:
  AppFrontendAction() {}
  ~AppFrontendAction() { delete BDCProto; }

  void EndSourceFileAction() override {
    std::error_code EC;
    // TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID()).write(llvm::outs());
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
  const std::vector<std::string> &SourcePathList [[maybe_unused]] =
      op.getSourcePathList();
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());
  int ret = Tool.run(newFrontendActionFactory<AppFrontendAction>().get());
  return ret;
}
/*************************************************************************************************/
