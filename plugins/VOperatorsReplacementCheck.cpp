#include "VOperatorsReplacementCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::misc {

void VOperatorsReplacementCheck::registerMatchers(MatchFinder *Finder) {
  auto vecType = "v_(int|uint|float)[0-9]+";
  
  auto expr = cxxOperatorCallExpr(hasDescendant(declRefExpr(to(
    varDecl(hasType(qualType(anyOf(hasDeclaration(namedDecl(matchesName(vecType))),
    references(namedDecl(matchesName(vecType))))))).bind("varDecl"))))).bind("expr");
                                  
  Finder->addMatcher(expr, this);
}

void VOperatorsReplacementCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *matchedExpr = Result.Nodes.getNodeAs<CXXOperatorCallExpr>("expr");
  const auto *varDecl = Result.Nodes.getNodeAs<VarDecl>("varDecl");

  if (matchedExpr && matchedExpr->getOperator() != OverloadedOperatorKind::OO_Equal) {
    std::string code = replacementCode(matchedExpr, Result.Context);
    if (!code.empty()) {
      diag(matchedExpr->getExprLoc(), "Found operator to replace")
        << FixItHint::CreateReplacement(matchedExpr->getSourceRange(), code);
    }
  }
}

std::string VOperatorsReplacementCheck::replacementCode(const Expr *expr, ASTContext *context) {
  if (const auto *cxxOpExpr = dyn_cast<CXXOperatorCallExpr>(expr->IgnoreCasts())) {
    std::string result;
    switch (cxxOpExpr->getOperator()) {
      case OverloadedOperatorKind::OO_PlusEqual:
      case OverloadedOperatorKind::OO_Plus:
        result = "v_add(";
        break;
      case OverloadedOperatorKind::OO_MinusEqual:
      case OverloadedOperatorKind::OO_Minus:
        result = "v_sub(";
        break;
      case OverloadedOperatorKind::OO_StarEqual:
      case OverloadedOperatorKind::OO_Star:
        result = "v_mul(";
        break;
      case OverloadedOperatorKind::OO_SlashEqual:
      case OverloadedOperatorKind::OO_Slash:
        result = "v_div(";
        break;
      default:
        diag(cxxOpExpr->getExprLoc(), "Unknown operator %0", DiagnosticIDs::Error)
          << getOperatorSpelling(cxxOpExpr->getOperator());
        return "";
        break;
    }
    if (cxxOpExpr->isAssignmentOp()) {
      std::string leftSrc = replacementCode(cxxOpExpr->getArg(0)->IgnoreCasts(), context);
      result += leftSrc;
      result += ", ";
      result += replacementCode(cxxOpExpr->getArg(1)->IgnoreCasts(), context);
      result = leftSrc + " = " + result + ")";
    }
    else {
      result += replacementCode(cxxOpExpr->getArg(0)->IgnoreCasts(), context);
      result += ", ";
      result += replacementCode(cxxOpExpr->getArg(1)->IgnoreCasts(), context);
      result += ")";
    }
    return result;
  }
  else if (const auto *declRefExpr = dyn_cast<DeclRefExpr>(expr->IgnoreCasts())) {
    std::string result = declRefExpr->getDecl()->getNameAsString();
    return result;
  }
}

}