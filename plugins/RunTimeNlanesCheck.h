#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_RUNTIMENLANESCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_RUNTIMENLANESCHECK_H

#include "../ClangTidy.h"

namespace clang::tidy::misc {

class RunTimeNlanesCheck : public ClangTidyCheck {
public:
  RunTimeNlanesCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  std::set<const DeclRefExpr*> setNlanes;
};

}

#endif
