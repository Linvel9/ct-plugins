#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_VOPERATORSREPLACEMENTCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_VOPERATORSREPLACEMENTCHECK_H

#include "../ClangTidy.h"

namespace clang::tidy::misc {

class VOperatorsReplacementCheck : public ClangTidyCheck {
public:
  VOperatorsReplacementCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  std::string replacementCode(const Expr *expr, ASTContext *context);
};

}

#endif
