# C Type Revisit

### Understand Array Type and Pointer Type of C
```cpp
int foo(int a[][2][3]) { // what is the type of a?
    int *h = a[0][0]; // what actually happens there?

    int b[2][3];
    int c = *(*(b + 1) + 1); // how many load instruction will be yield? 1, 2, or 3?

    int **d;
    int e = *(*(e + 2) + 1); // what about this？
}
```

- Since the type declarator `a[][2][3]` appearing in the argument is <b>incomplete</b>, it actually is of type int (*)[2][3], which is a pointer to an array!

- Since a is of type int(*)[2][3], the result of a[0] is of type int[2][3]. The functionailty of subscript operator is to treat the underlying expression as an array whose element is either the element of array or the type that a pointer points to. So in a[0], the int[2][3] is indeed treated as the element of the temporary array a.

- Then a[0][0] is simply of type int[3], which can be decayed to int*. The principle of array decaying is to cast the array type to the pointer that points to the element of array.

- So how to understand *(*(b + 1) + 1)? Firstly since b is of type int[2][3], when it meets binary operator '+', it will be decayed to type int(*)[3], which is a pointer to the array of type int[3], thus the value of b + 1 is the a pointer that points to the array located at the address by offsetting b by the size of int[3].

### Understand Abstraction and Implementation on Machine

- Then the operator '*' see that b + 1 is of type `int(*)[3]`. The functionality of `*` operator is to <b>compute the value that the pointer points to</b>, even the value is an array. It is important to keep in mind that in the current abstraction level we don't care about machine-specific details about how different values are actually loaded and stored from and to the memory, but focus on the process of computation itself. In implementation the whole array will not be loaded into registers, but simply its base address, but abstraction is different from implementation! We can simple think that we obtain the whole array despite the fact that only the base address is computed.

- So in an abstract view the value of dereferecing `b + 1` is exactly the array indicated by the type int[3], though in implementation the value is computed by offseting b, and no load is in fact generated.

- Then we can see that int[3] will decay to int*, which will yield a load instruction when being dereferenced. We can conclude that a load will be generated if and only if the computed (in physical machine, not about abstraction) value is of non-array type.

- In fact the declaration of c will generate only 2 loads, while e will generate 3 loads.

# Representation of Types

We categorize simple types that are used in C++ into two kinds (we ignore more advanced types), one is atomic types (or primitives) like int, char, bool, the other is compound types which is constructed using type operators. Since common compound types are defined inductively, tree representation of those types seems plausible and easy to implement.

For example, the pointer type can be represented as a tree with a single subtree, which is the representation of its element type. The array type can be deemed as being consisting a subtree and a non-negative integer. The type `int[2][3]` can be portraited as a tree of height 3 and should be understood as an array of `int[3]`, or array of int arrays. For the ease of the bottom-up parsing, we adopt a design that treats the type of array of arrays (nestling arrays) as a vector whose elements are the corresponding dimensions, like representing `int[2][3]` as `{elementType: int, dims: {2, 3}}` and treat the pointer to pointer as a single node in which the depth of reference is indicated by an integer, like representing `int **` as `{elementType: int, depth: 2}`. To maintain the invariance, when a pointer points to another pointer, the elementType cannot be set as a pointer type, instead we simply increase the depth field to express the same information.

### Case Study: Array Type
```cpp
struct ArrayType : public Type {
    ArrayType();
    void setBaseTypeAndReverse(Type *ptr);
    void addDim(expPtr p);
    // evaluate all pending indexes into integers
    void evaluate(TypeChecker *ptr);
    std::string to_string() override;
    Type *degrade();
    void printUnevaludatedType(std::string prefix, std::string info_prefix);
    bool equals(Type* other) override;
    void setConst();
    TypeKind kind;
    std::vector<expPtr> pendingDims;
    std::vector<size_t> dims;
    Type *element_type;
};
```

- Some types like array type or constructed from array type may contain unevaluetd computations in the array index, which cannot be resolved until semantics analysis. Since those pending computations are directed by the structure of types, I choose to violate the modularity principle and insert those unevaluated expression into the type node instead of putting them in the astnode (maybe there are better options). The `evaluate` method try to evaluate those dimensions at compile time and if failed (value has type error or is not compile-time constant) then push error messages into the node which is inherited from the base type `Type`. 

# TypeChecking


### Case Analysis: Addition Expression

```cpp
analyzeInfo TypeChecker::analyze(binary_expr* node) {
    auto info1 = node->left->dispatch(this);
    auto info2 = node->right->dispatch(this);
    if(info1.type == nullptr || info2.type == nullptr) {
        std::cout << "binary_expr analyze fail, the fault is at:\n";
        node->printAST("","");
        exit(1);
    }
    if(info1.type->kind == TypeKind::Array) {
        ArrayType *array = dynamic_cast<ArrayType*>(info1.type);
        Type *degrade_array = array->degrade();
        type_cast *tc = new type_cast(std::move(node->left), degrade_array);
        tc->inferred_type = degrade_array;
        node->left = expPtr(tc);
        if(array->dims.size() > 1) {
            std::stringstream ss;
            ss << "Operator '" << node->op << "' cannot apply on type " 
            << degrade_array->to_string() 
            << " and " << node->right->inferred_type->to_string() << "";
            TypeError(node, ss.str());
            node->inferred_type = HASERROR.type;
        } else {
            node->inferred_type = degrade_array;
        }
    }
    if(info2.type->kind == TypeKind::Array) {
        ArrayType *array = dynamic_cast<ArrayType*>(info2.type);
        Type *degrade_array = array->degrade();
        type_cast *tc = new type_cast(std::move(node->right), degrade_array);
        tc->inferred_type = degrade_array;
        node->right = expPtr(tc);
        if(array->dims.size() > 1) {
            std::stringstream ss;
            ss << "Operator '" << node->op << "' cannot apply on type " 
            << node->left->inferred_type->to_string() 
            << " and " << degrade_array->to_string() << "";
            TypeError(node, ss.str());
            node->inferred_type = HASERROR.type;
        } else {
            node->inferred_type = degrade_array;
        }
    }
    if(node->op == "+" || node->op == "-") {
        analyzeAdd(node);
    }
```
- The `dispatch` typecheck the subtree of the binary operator.

- If the operand is of array type, then it should decay to the pointer type with the same element type as of the original type. We handle this by inserting an implicit typecasting operator and set the inferred type of its type properly.

```cpp
void TypeChecker::analyzeAdd(binary_expr *node) {
    if(node->left->inferred_type->kind == TypeKind::Int && node->right->inferred_type->kind == TypeKind::Int) {
        node->inferred_type = node->left->inferred_type;
    } else if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Int) {
        node->inferred_type = node->left->inferred_type;
    } else if(node->right->inferred_type->kind == TypeKind::Pointer && node->left->inferred_type->kind == TypeKind::Int) {
        auto ptr = std::move(node->left);
        node->left = std::move(node->right);
        node->right = std::move(ptr);
        node->inferred_type = node->left->inferred_type;
    } else if(node->left->inferred_type->kind == TypeKind::Pointer && node->right->inferred_type->kind == TypeKind::Pointer && node->op == "-") {
        node->inferred_type = TypeFactory::getInt();
    } 
}
```
- By looking into the content of the type we can decide whether the addition expression is well-typed easily.