# clang-plugin
Demonstrating a plugin for clang on MacOS

The plugin traverses the AST for a source file to find all function
arguments/parameters. Then compliance with a 'code standard' is checked:

* Parameters must be at least 5 characters long
* Parameters must start with Uppercase
* Parameters must not be in list of 'forbidden words' (crap, gylle)

## Building clang
    > git clone [--depth=1] https://github.com/llvm/llvm-project.git
    > mkdir -p llvm-project/build
    > cd llvm-project/build
    > cmake -DLLVM_ENABLE_PROJECTS=clang -DCLANG_BUILD_EXAMPLES=ON -G "Unix Makefiles" ../llvm
    > make

Now go do something else for a while as the build takes a long time.

## Looking at AST for selected code

    > clang -fsyntax-only -Xclang -ast-dump test.cpp

![Output of ast-dump](images/ast.png?)

## Write the plugin code
From the AST dump we see that clang uses the type FunctionDecl for - well -
function declarations.

We use this in the AST traversal by casting the more general DeclGroupRef, which  
is passed to HandleToplevelDecl(), to FunctionDecl. If that is not a nullptr
we can then do whatever is appropriate for FunctionDecl.

    if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
      // do function specific processing here
    }
See [clang documentation](https://clang.llvm.org/doxygen/classclang_1_1FunctionDecl.html)
for the available methods.

For example we use the method *parameters()* which returns an iterable object
to investigate the function parameters.

    auto parms = FD->parameters();

    // check for code standard compliance
    for (auto & PD : parms) {
      auto name = PD->getNameAsString();
      // process parameter name
    }


There is a small bit of boilerplate code that needs to be written but not much. See [MyPlugin.cpp](MyPlugin/MyPlugin.cpp)

## Add the plugin
    > cp -r MyPlugin llvm-project/clang/examples
    > echo "add_subdirectory(MyPlugin)" >> llvm-project/clang/examples/CMakelists.txt
    > cd llvm-project/build
    > make MyPlugin

## Test the plugin
Use or modify the runplugin script

    > ./runplugin test.cpp

![Output of ast-dump](images/result.png?)
