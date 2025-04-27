## Abstract Syntax Tree

The abstract syntax tree is a condensed version of the parse tree, which filters out all of the syntatic details that are unnecessary for the semantic checking and code generation and establishes a representation of the program that is easy to manipulate.

The proper design of abstract syntax tree is crucial in frontend construction, and several useful strategies or design patterns have be recognized. Here are some points that one should better consider when designing AST:

- Use class inheritance to organize kinds of astnode is preferred in oop language with not-so-strong type system. In language like SML or Haskell inductive types are more fitting option for representing the nodes.

- The class of node should incrementally collect information for error reporting, typechecking and code generation during parsing and semantic analysis. For example, the node for if statement should collect the location of the statement for debugging purpose, the expression for branching condition and two sub statements for then branch and else branch. The node for binary-operator should collect the kind of the operator and two operands which should be the subclass of expression class so that they will be typed and yield value during compuation. The node for class definition should carry information about its super class as well as its own members.

- Implement visitor pattern for tree traversal to separate algorithms from tree structure provides high modularity and is highly recommended. DO NOT write everything in one single .cpp file or even header file, which might introduce endless dependency issues!

- Non-syntatic node might sometimes be necessary to introduce for attaching important information to the program, for example node for type or complilation error. They might simplifies the parsing and ease semantic analysis significantly, though they may looks "impure" or ugly for their non-syntatic nature.

- Despite the design space of AST is huge, as long as you collect enough information in each node nothing bad will happen.

### Case Analysis: Function Definition

```c++
struct func_def : public node {
    func_def(std::pair<size_t, size_t> loc);
    void set_body(blockPtr body);
    std::string to_string() override;
    void printArgList(std::string prefix, std::string info_prefix);
    void printAST(std::string prefix, std::string info_prefix) override;
    analyzeInfo dispatch(TypeChecker *ptr) ;
    codeGenInfo dispatch(codeGen *ptr);
    void setCtor();
    FuncType *type; //will be evaluated during type checking
    std::string name;
    std::vector<nodePtr> body;
    bool is_constructor = false;
};
```

- The constructor `func_def(std::pair<size_t, size_t> loc)` collect the location of the node.
- When the parsing of function is done, `void set_body(blockPtr body);` will be called to attach the function body to the function definition.
- `void printAST(std::string prefix, std::string info_prefix) override;` provides print method of the node, which is extremely useful for debugging and error diagonsis.
- `dispatch` method follows the visitor pattern, whose detail is implemented by the typechecker and code generator and is seperated from the astnode for better modularity.
- `is_constructor = false;` is used to indicate whether the node is a ctor and is set to true during the parsing of the class member definition.

Let's look into the detail of the construction of the function node:
```cpp

    ruleAction(rule(FuncDef, {Type, Declarator, Block}), [](std::vector<parseInfoPtr>& children) {
        parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
        auto funcDef = new func_def(children[0]->location);
        funcDef->name = children[1]->str_val;
        funcDef->set_body(blockPtr(static_cast<block_stmt*>(children[2]->ptr.release())));
        if(children[1]->type == nullptr || children[1]->type->kind != TypeKind::Function) {
            std::cout << "funcdef error, not a function type\n";
            exit(1);
            return res;
        } 
        FuncType *fun = dynamic_cast<FuncType*>(children[1]->type);
        if(fun->retType == nullptr) {
            fun->retType = TypeFactory::getTypeFromName(children[0]->str_val);
        } else if(fun->retType->kind == TypeKind::Pointer) {
            PointerType *pointer = dynamic_cast<PointerType*>(fun->retType);
            pointer->elementType = TypeFactory::getTypeFromName(children[0]->str_val);
        }
        funcDef->type = fun;
        std::reverse(funcDef->type->argTypeList.begin(), funcDef->type->argTypeList.end());
        std::reverse(funcDef->type->bindings.begin(), funcDef->type->bindings.end());
        res->set_node(nodePtr(funcDef));
        return res;
    }),
```

- The parseInfo carries the all the information from the child node in bottom-up parsing, like the pointer to the subtree, the type descriptor, the location of the token and its content.

```cpp
parseInfoPtr res = std::make_unique<parseInfo>(children[0]->location);
auto funcDef = new func_def(children[0]->location);
funcDef->name = children[1]->str_val;
funcDef->set_body(blockPtr(static_cast<block_stmt*>(children[2]->ptr.release())));
```

- funcDef is created with the location carried by the parseInfo. We then set its body and function name with the block statement extracted from the children[2] and the identifier of the type declarator contained in children[1].

```cpp
FuncType *fun = dynamic_cast<FuncType*>(children[1]->type);
if(fun->retType == nullptr) {
    fun->retType = TypeFactory::getTypeFromName(children[0]->str_val);
} else if(fun->retType->kind == TypeKind::Pointer) {
    PointerType *pointer = dynamic_cast<PointerType*>(fun->retType);
    pointer->elementType = TypeFactory::getTypeFromName(children[0]->str_val);
}
funcDef->type = fun;
std::reverse(funcDef->type->argTypeList.begin(), funcDef->type->argTypeList.end());
std::reverse(funcDef->type->bindings.begin(), funcDef->type->bindings.end());
res->set_node(nodePtr(funcDef));
```

- The code above set the return type of the function and reverse the the vector of its arguments. Due to the constraint of the grammar (however can be resolved, but I don't realize when I crafted the parser), we can only collect the arguments (as well as statements in block or array indexes) backwards, so a reverse call is necessary.

- After setting the node, it is put into the res and is returned and treated as a child node of its parent.