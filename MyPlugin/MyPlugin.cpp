//===- MyPlugin.cpp ---------------------------------------------===//
//
// Example clang plugin to check a coding convention for function
// parameters.
//
//===----------------------------------------------------------------------===//

#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
using namespace clang;

namespace {

class MyPluginConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  std::set<std::string> ParsedTemplates;

public:
  MyPluginConsumer(CompilerInstance &Instance,
                         std::set<std::string> ParsedTemplates)
      : Instance(Instance), ParsedTemplates(ParsedTemplates) {}

  bool HandleTopLevelDecl(DeclGroupRef DG) override {
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
      const Decl *D = *i;
      if (const NamedDecl *ND = dyn_cast<NamedDecl>(D))
        llvm::errs() << "top-level-decl: \"" << ND->getNameAsString() << "\"\n";

      // check if this is a function declaration
      if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
        // get function parameters
        auto parms = FD->parameters();

        // check for code standard compliance
        for (auto & PD : parms) {
          auto name = PD->getNameAsString();

          // check variable name length
          if (name.size() < 5) {
            FullSourceLoc full(PD->getLocation(), Instance.getSourceManager());
            unsigned id = Instance.getDiagnostics().getCustomDiagID(DiagnosticsEngine::Warning,
               "variable name too short");
            Instance.getDiagnostics().Report(full, id);
          }

          // check first letter is capital
          if (not isupper(name[0])) {
            FullSourceLoc full(PD->getLocation(), Instance.getSourceManager());
            unsigned id = Instance.getDiagnostics().getCustomDiagID(DiagnosticsEngine::Warning,
               "variable begins with lower case");
            Instance.getDiagnostics().Report(full, id);
          }

          // check variable name for blacklisted words
          if (std::find(std::begin(badwords), std::end(badwords), name) != std::end(badwords)) {
            FullSourceLoc full(PD->getLocation(), Instance.getSourceManager());
            unsigned id = Instance.getDiagnostics().getCustomDiagID(DiagnosticsEngine::Warning,
               "variable uses blacklisted word");
            Instance.getDiagnostics().Report(full, id);
          }

        }
      }
    }

    return true;
  }

  std::vector<std::string> badwords{"crap", "gylle"};
};

class MyPluginAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<MyPluginConsumer>(CI, ParsedTemplates);
  }

  // Boilerplate virtual function needs to be implemented
  // but doesn't have to do anything
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }
};

}

static FrontendPluginRegistry::Add<MyPluginAction>
X("my-plugin", "my plugin");
