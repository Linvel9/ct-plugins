#include "RunTimeNlanesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang::tidy::misc {

void RunTimeNlanesCheck::registerMatchers(MatchFinder *Finder) {
  auto constNlanes = declRefExpr(hasDeclaration(namedDecl(hasName("nlanes"))),
                          hasAncestor(varDecl(hasType(arrayType())))).bind("max_nlanes");

  auto refConstNlanes = decl(hasDescendant(varDecl(hasInitializer(ignoringImpCasts(
                                declRefExpr(hasDeclaration(namedDecl(hasName("nlanes")))).bind("max_nlanes"))))));

  auto vlanes = declRefExpr(hasDeclaration(namedDecl(hasName("nlanes")))).bind("vlanes");

  Finder->addMatcher(constNlanes, this);
  Finder->addMatcher(refConstNlanes, this);
  Finder->addMatcher(vlanes, this);
}

void RunTimeNlanesCheck::check(const MatchFinder::MatchResult &Result) {
  const DeclRefExpr *matchedExpr = Result.Nodes.getNodeAs<DeclRefExpr>("vlanes");
  const DeclRefExpr *constNlanes = Result.Nodes.getNodeAs<DeclRefExpr>("max_nlanes");

  if (matchedExpr && matchedExpr->getNameInfo().getAsString().compare("nlanes") == 0 &&
      setNlanes.find(matchedExpr) == setNlanes.end()) {
    const auto *vecStructDecl = dyn_cast<CXXRecordDecl>(matchedExpr->getDecl()->getDeclContext()->getParent());
    if (vecStructDecl && vecStructDecl->getName().startswith("v_")) {
      SourceManager &SM = Result.Context->getSourceManager();
      std::string vecTypeName = clang::Lexer::getSourceText(CharSourceRange::getTokenRange(
                                Result.Context->getFullLoc(matchedExpr->getLocStart())),
                                SM, Result.Context->getLangOpts()).str();
      auto parent = Result.Context->getParents(*matchedExpr)[0];
      diag(matchedExpr->getLocation(), "Found nlanes.")
          << FixItHint::CreateReplacement(parent.getSourceRange(),
                                          "VTraits<" + vecTypeName + ">::vlanes()");
    }
  }

  if (constNlanes && constNlanes->getNameInfo().getAsString().compare("nlanes") == 0) {
    setNlanes.insert(constNlanes);
    const auto *vecStructDecl = dyn_cast<CXXRecordDecl>(constNlanes->getDecl()->getDeclContext()->getParent());
    if (vecStructDecl && vecStructDecl->getName().startswith("v_")) {
      SourceManager &SM = Result.Context->getSourceManager();
      constNlanes->getSourceRange().getBegin().dump(SM);
      std::string vecTypeName = clang::Lexer::getSourceText(
              CharSourceRange::getTokenRange(
                  Result.Context->getFullLoc(constNlanes->getLocStart())),
              SM, Result.Context->getLangOpts())
              .str();
      diag(constNlanes->getLocation(), "Found nlanes as constant.")
          << FixItHint::CreateReplacement(constNlanes->getSourceRange(),
                                          "VTraits<" + vecTypeName + ">::max_nlanes");
    }
  }
}

}