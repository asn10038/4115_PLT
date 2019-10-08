#include <iostream>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/StaticAnalyzer/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include "clang/AST/Stmt.h"

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
static cl::OptionCategory FindNamedClassCategory("clang-hw2 options");

#if 0
class FindNamedClassVisitor
  : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
  explicit FindNamedClassVisitor(ASTContext *Context)
    : Context(Context) {}

  bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    //if (Declaration->getQualifiedNameAsString() == "n::m::C") {
      FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
      if (FullLocation.isValid())
        llvm::outs() << "Found declaration at "
                     << FullLocation.getSpellingLineNumber() << ":"
                     << FullLocation.getSpellingColumnNumber() << "\n";
    //}
    return true;
  }

private:
  ASTContext *Context;
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
  explicit FindNamedClassConsumer(ASTContext *Context)
    : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindNamedClassVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new FindNamedClassConsumer(&Compiler.getASTContext()));
  }
};
#endif

class FindIfsVisitor
  : public RecursiveASTVisitor<FindIfsVisitor> {
public:
  explicit FindIfsVisitor(ASTContext *Context)
    : Context(Context) {}

  bool VisitIfStmt(IfStmt *stmt) {
        FullSourceLoc fullLocation = Context->getFullLoc(stmt->getBeginLoc());
        if(!fullLocation.isValid()) {
            std::cerr << "WARNING: statement location not valid" << std::endl;
            return false;
        }
        auto lineNum = fullLocation.getLineNumber();
        auto colNum = fullLocation.getColumnNumber();
        
        std::string condString;
        raw_string_ostream OM(condString);
        stmt->getCond()->printPretty(OM, nullptr, PrintingPolicy(Context->getLangOpts()));
        OM.str();

        std::cout << "Found Ifstmt at " << lineNum << ":" << colNum << " with Condition: " <<  condString << std::endl;

        
        return true;
  }

private:
  ASTContext *Context;
};

class FindIfsConsumer : public clang::ASTConsumer {
public:
  explicit FindIfsConsumer(ASTContext *Context)
    : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindIfsVisitor Visitor;
};

class FindIfsAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new FindIfsConsumer(&Compiler.getASTContext()));
  }
};

int main(int argc, const char **argv) {

    CommonOptionsParser OptionsParser(argc, argv, FindNamedClassCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
    return Tool.run(newFrontendActionFactory<FindIfsAction>().get());

}
