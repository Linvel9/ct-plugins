#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_TESTPLUGINCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_TESTPLUGINCHECK_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace misc {

class C_API : public ClangTidyCheck {
public:
  C_API(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace misc
} // namespace tidy
} // namespace clang

#endif
