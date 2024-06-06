#include "C_APICheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::misc {

void C_API::registerMatchers(MatchFinder *Finder) {
  auto cvSmooth = declRefExpr(to(functionDecl(anyOf(hasName("cvSmooth"), hasName("add"))))).bind("funcCall");
  auto cvAcc = declRefExpr(to(functionDecl(hasName("cvAcc")))).bind("funcCall");
  auto cvSquareAcc = declRefExpr(to(functionDecl(hasName("cvSquareAcc")))).bind("funcCall");
  auto cvRemap = declRefExpr(to(functionDecl(hasName("cvRemap")))).bind("funcCall");
  auto cvResize = declRefExpr(to(functionDecl(hasName("cvResize")))).bind("funcCall");
  auto cvCvtColor = declRefExpr(to(functionDecl(hasName("cvCvtColor")))).bind("funcCall");

  Finder->addMatcher(cvSmooth, this);
  Finder->addMatcher(cvAcc, this);
  Finder->addMatcher(cvSquareAcc, this);
  Finder->addMatcher(cvRemap, this);
  Finder->addMatcher(cvResize, this);
  Finder->addMatcher(cvCvtColor, this);
}

void C_API::check(const MatchFinder::MatchResult &Result) {
  const auto *c_api = Result.Nodes.getNodeAs<DeclRefExpr>("funcCall");
  
  diag(c_api->getLocation(), "function '%0' is old C_API")
      << c_api->getDecl()->getName()
      << FixItHint::CreateInsertion(c_api->getLocation(), "use the OpenCV 4.x API");
}
}