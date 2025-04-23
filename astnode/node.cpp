#include "common/common.hpp"

#include "parser/astnodes/node.hpp"

// Base node implementation
node::node(std::pair<size_t, size_t> pair) : location(pair) {}


std::string locToString(std::pair<size_t, size_t> location, std::string tail) {
    return " <" + std::to_string(location.first) + ":" + std::to_string(location.second) + "> " + tail + "\n";
}


// Program Node
program::program(std::pair<size_t, size_t> loc) : node(loc) {
    kind = ASTKind::Program;
}

std::string program::to_string() {
    return "Program";
}

void program::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    for(size_t i = 0; i < children.size(); i++) {
        if(i != children.size() - 1) {
            children[i]->printAST(prefix + "│   ", prefix + "├── ");
        } else {
            children[i]->printAST(prefix + "    ", prefix + "└── ");
        }
    }
}

analyzeInfo program::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo program::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
} 

// Expression Nodes
expr::expr(std::pair<size_t, size_t> loc, struct Type *ptr) : node(loc), inferred_type(ptr) {}


void expr::setConst(bool is_const) {
    this->is_const = is_const;
}

// Unary Expression
unary_expr::unary_expr(std::pair<size_t, size_t> loc, std::string op, expPtr operand)
    : expr(loc, nullptr), op(op), operand(std::move(operand)) {
        kind = ASTKind::Unary_Op;
    }

std::string unary_expr::to_string() {
    return "unary_expr <op " + op + "> " + (inferred_type ? color::magenta + std::string("inferredType: ") + inferred_type->to_string() + color::reset + " ": "");
}

void unary_expr::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    operand->printAST(prefix + "    ", prefix + "└── ");
}

analyzeInfo unary_expr::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo unary_expr::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

constInfo unary_expr::const_eval(TypeChecker *ptr)
{
    return ptr->const_eval(this);
} 


// Binary Expression
binary_expr::binary_expr(std::pair<size_t, size_t> loc, std::string op, expPtr left, expPtr right)
    : expr(loc, nullptr), op(op), left(std::move(left)), right(std::move(right)) {
        kind = ASTKind::Binary_Op;
    }

std::string binary_expr::to_string() {
    return "binary_expr <op " + op + "> " + (inferred_type ? color::magenta + std::string("inferredType: ") + inferred_type->to_string() + color::reset + " ": "");
}

void binary_expr::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    left->printAST(prefix + "│   ", prefix + "├── ");
    right->printAST(prefix + "    ", prefix + "└── ");
}

analyzeInfo binary_expr::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo binary_expr::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

constInfo binary_expr::const_eval(TypeChecker *ptr)
{
    return ptr->const_eval(this);
}

// LValue Expression
lval_expr::lval_expr() : expr({-1, -1}, nullptr) {
    ASTKind kind = LVAL;
}

std::string lval_expr::to_string() {
    return "LVal <id " + id + (inferred_type != nullptr ? ", type " + inferred_type->to_string() : "") + "> " + 
    (inferred_type ? color::magenta + std::string("inferredType: ") + inferred_type->to_string() + color::reset + " ": "");;
}

void lval_expr::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    for (size_t i = 0; i < dims.size(); ++i) {
        if(i != dims.size() - 1) {
            dims[i]->printAST(prefix + "│   ", prefix + "├── ");
        } else {
            dims[i]->printAST(prefix + "    ", prefix + "└── ");
        }
    }
}

void lval_expr::setIdAndReverseDim(std::string id) {
    this->id = id;
    std::reverse(dims.begin(), dims.end());
}

void lval_expr::addDim(expPtr ptr) {
    this->dims.push_back(std::move(ptr));
}

void lval_expr::setLoc(std::pair<size_t, size_t> loc) {
    this->location = loc;
}

analyzeInfo lval_expr::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo lval_expr::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

constInfo lval_expr::const_eval(TypeChecker *ptr)
{
    return ptr->const_eval(this);
}

// Statement Nodes
stmt::stmt(std::pair<size_t, size_t> loc) : node(loc) {}

// Expression Statement
expr_stmt::expr_stmt(std::pair<size_t, size_t> loc, expPtr ptr)
    : stmt(loc), ptr(std::move(ptr)) {}

std::string expr_stmt::to_string() {
    return "expr_stmt";
}

void expr_stmt::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    ptr->printAST(prefix + "    ", prefix + "└── ");
}

analyzeInfo expr_stmt::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo expr_stmt::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

// If-Else Statement
if_else_stmt::if_else_stmt(std::pair<size_t, size_t> loc, expPtr cond, stmtPtr if_branch, stmtPtr else_branch)
    : stmt(loc), cond(std::move(cond)), if_branch(std::move(if_branch)), else_branch(std::move(else_branch)) {}

std::string if_else_stmt::to_string() {
    return else_branch ? "if_else_stmt" : "if_stmt";
}

void if_else_stmt::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    cond->printAST(prefix + "    ", prefix + "├──(cond) ");
    if_branch->printAST(prefix + (else_branch ? "│   " : "    "), 
                       prefix + (else_branch ? "├──(if) " : "└──(if) "));
    if (else_branch) {
        else_branch->printAST(prefix + "    ", prefix + "└──(else) ");
    }
}

analyzeInfo if_else_stmt::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo if_else_stmt::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

while_stmt::while_stmt(std::pair<size_t, size_t> loc, expPtr cond, stmtPtr body)
    : stmt(loc), cond(std::move(cond)), body(std::move(body)) {}

std::string while_stmt::to_string() {
    return "while_stmt";
}

void while_stmt::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    cond->printAST(prefix + "    ", prefix + "├──(cond) ");
    body->printAST(prefix + "    ", prefix + "└──(body) ");
}

analyzeInfo while_stmt::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo while_stmt::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

// Break Statement
break_stmt::break_stmt(std::pair<size_t, size_t> loc) : stmt(loc) {}

std::string break_stmt::to_string() {
    return "break_stmt";
}

void break_stmt::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
}

analyzeInfo break_stmt::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo break_stmt::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

// Continue Statement
continue_stmt::continue_stmt(std::pair<size_t, size_t> loc) : stmt(loc) {}

std::string continue_stmt::to_string() {
    return "continue_stmt";
}

void continue_stmt::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
}

analyzeInfo continue_stmt::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo continue_stmt::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

// Return Statement
return_stmt::return_stmt(std::pair<size_t, size_t> loc, expPtr value)
    : stmt(loc), value(std::move(value)) {
        kind = ASTKind::Return_Stmt;
    }

std::string return_stmt::to_string() {
    return "return_stmt ";
}

void return_stmt::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    if (value) {
        value->printAST(prefix + "    ", prefix + "└──(value) ");
    }
}

analyzeInfo return_stmt::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo return_stmt::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

// Block Statement
block_stmt::block_stmt(std::pair<size_t, size_t> loc) : stmt(loc) {}

void block_stmt::add_item(nodePtr item) {
    if (item) items.push_back(std::move(item));
}

void block_stmt::setLoc(std::pair<size_t, size_t> loc) {
    this->location = loc;
}

std::string block_stmt::to_string() {
    return "block_stmt";
}

void block_stmt::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    for (size_t i = 0; i < items.size(); ++i) {
        if (i != items.size() - 1) {
            items[i]->printAST(prefix + "│   ", prefix + "├── ");
        } else {
            items[i]->printAST(prefix + "    ", prefix + "└── ");
        }
    }
}

analyzeInfo block_stmt::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo block_stmt::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

// ==================== Expression Nodes ====================

// Literal Base Class
literal::literal(std::pair<size_t, size_t> loc) : expr(loc, nullptr) {}

void literal::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
}

// Integer Literal
int_literal::int_literal(std::pair<size_t, size_t> loc, int val)
    : literal(loc) {
        value = val;
    }

std::string int_literal::to_string() {
    return "int_literal <value " + std::to_string(value) + "> " + (inferred_type ? color::magenta + std::string("inferredType: ") + inferred_type->to_string() + color::reset + " ": "");
}

analyzeInfo int_literal::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo int_literal::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

constInfo int_literal::const_eval(TypeChecker *ptr)
{
    return ptr->const_eval(this);
}

fun_call::fun_call(std::pair<size_t, size_t> loc)
    : expr(loc, nullptr) {}

void fun_call::addParam(expPtr param) {
    args.push_back(std::move(param));
}

void fun_call::setName(std::string name) {
    this->func_name = name;
}

std::string fun_call::to_string() {
    std::string result = "fun_call <id " + func_name + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i != 0) result += ", ";
        result += "argnum " + std::to_string(i + 1);
    }
    result += ")> ";
    result += (inferred_type ? color::magenta + std::string("inferredType: ") + inferred_type->to_string() + color::reset + " ": "");
    return result;
}
void fun_call::setLoc(std::pair<size_t, size_t> loc) {
    location = loc;
}

void fun_call::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    for (size_t i = 0; i < args.size(); ++i) {
        if (i != args.size() - 1) {
            args[i]->printAST(prefix + "│    ", prefix + "├── (arg" + std::to_string(i) + ") ");
        } else {
            args[i]->printAST(prefix + "     ", prefix + "└── (arg" + std::to_string(i) + ") ");
        }
    }
}

analyzeInfo fun_call::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo fun_call::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

constInfo fun_call::const_eval(TypeChecker *ptr)
{
    return ptr->const_eval(this);
}  
// ==================== Function and Variable Nodes ====================


// Function Definition
func_def::func_def(std::pair<size_t, size_t> loc) : node(loc) {
    this->type = new FuncType();
    kind = ASTKind::Func_Def;
}

void func_def::set_body(blockPtr body) {
    for(auto &p : body->items) {
        this->body.push_back(std::move(p));
    }
    body->items.clear();
}

std::string func_def::to_string() {
    if(is_constructor) {
        return "Ctor <id: " + name + ">";
    } else {
        return "FuncDef <ret_type: " + (type ? (type->retType ? type->retType->to_string() : "") : "") + ", id: " + name + ">";
    }
    return {};
}

void func_def::printArgList(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << "paramList\n";
    for (size_t i = 0; i < type->argTypeList.size()  ; ++i) {
        if(i != type->argTypeList.size() - 1) {
            std::cout   << prefix + "├── (arg" + std::to_string(i) + " "
                        << type->bindings[i] 
                        << " "
                        << type->argTypeList[i]->to_string() + "\n";
        } else {
            std::cout   << prefix + "└── (arg" + std::to_string(i) + " " 
                        << type->bindings[i]
                        << " "
                        << type->argTypeList[i]->to_string() + "\n";
        }
    }
}

void func_def::printAST(std::string prefix, std::string info_prefix) {
    if(type == nullptr) return;
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    printArgList(prefix + "│   ", prefix + "├── ");
    std::cout << prefix + "└── " + "block\n";
    for (size_t i = 0; i < body.size(); ++i) {
        if(i != body.size() - 1) {
            body[i]->printAST(prefix + "    │   ", prefix + "    ├── ");
        } else {
            body[i]->printAST(prefix + "        ", prefix + "    └── ");
        }
    }
}

analyzeInfo func_def::dispatch(TypeChecker *ptr)
{
    auto info = ptr->analyze(this); 
    ptr->analyzeFunctionBody(this);
    return info;
}

codeGenInfo func_def::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

void func_def::setCtor() {
    is_constructor = true;
}

// Variable Definition
var_def::var_def(std::pair<size_t, size_t> loc) : node(loc) {}

void var_def::setId(const std::string& name) {
    id = name;
}


void var_def::setConst(bool is_const) {
    this->is_const = is_const;
}

void var_def::setInitVal(nodePtr val) {
    init_val = std::move(val);
}

std::string var_def::to_string() {
    // std::cout << ((PointerType*)type.get())->elementType << "hey\n";
    std::string res = "var_def <id " + id + ", type " + 
                     (is_const ? "const " : "") +
                     (type ? type->to_string() : "unknown") + "> " + error_msg;
    return res;
}

void var_def::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << "\n";
    if(type && type->kind == TypeKind::Array) {
        ArrayType *p = static_cast<ArrayType*>(type.get());
        if(!init_val) {
            p->printUnevaludatedType(prefix + "    ", prefix + "└── ");
        } else {
            p->printUnevaludatedType(prefix + "│   ", prefix + "├── ");
        }
    }
    if(init_val) {
        init_val->printAST(prefix + "    ", prefix + "└── ");
    }
}

analyzeInfo var_def::dispatch(TypeChecker *ptr)
{
    auto info = ptr->analyze(this);
    ptr->analyzeInit(this);
    return info;
}

codeGenInfo var_def::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

// Variable Declaration
var_decl::var_decl(std::pair<size_t, size_t> loc) : node(loc) {
    kind = ASTKind::Var_Decl;
}

void var_decl::addVarDef(vardefPtr def) {
    defs.push_back(std::move(def));
}

void var_decl::setTypeAndReverse(std::string name) {
    this->typeName = name;
    finalize();
}

void var_decl::setLoc(std::pair<size_t, size_t> loc) {
    location = loc;
}

void var_decl::setConst(bool is_const) {
    this->is_const = is_const;
}

std::string var_decl::to_string() {
    return "VarDecl <type: " + std::string(is_const ? "const " : "") + typeName + ">";
}

void var_decl::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    for (size_t i = 0; i < defs.size(); ++i) {
        if (i != defs.size() - 1) {
            defs[i]->printAST(prefix + "│   ", prefix + "├── ");
        } else {
            defs[i]->printAST(prefix + "    ", prefix + "└── ");
        }
    }
}

analyzeInfo var_decl::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo var_decl::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

void var_def::finalizeType(std::string type_name) {
    if(this->type == nullptr) {
        this->type = TypeFactory::getTypeFromName(type_name);
    } else if(this->type->kind == TypeKind::Array) {
        ArrayType* ptr = dynamic_cast<ArrayType*>(this->type.get());
        if(!ptr->element_type) {
            ptr->element_type = TypeFactory::getTypeFromName(type_name);
        } else if(ptr->element_type->kind == TypeKind::Pointer) {
            PointerType *pointer = dynamic_cast<PointerType*>(ptr->element_type.get());
            pointer->elementType = TypeFactory::getTypeFromName(type_name).release();
        }
    } else if(this->type->kind == TypeKind::Pointer) {
        PointerType* ptr = dynamic_cast<PointerType*>(this->type.get());
        ptr->elementType = TypeFactory::getTypeFromName(type_name).release();
    } else if(this->kind == TypeKind::Function) {
        FuncType* fun = dynamic_cast<FuncType*>(this->type.get());
        if(fun->retType == nullptr) {
            fun->retType = TypeFactory::getTypeFromName(type_name);
        } else if(fun->retType->kind == TypeKind::Array) {
            ArrayType* ptr = dynamic_cast<ArrayType*>(fun->retType.get());
            ptr->setBaseTypeAndReverse(TypeFactory::getTypeFromName(type_name));
        } else if(fun->retType->kind == TypeKind::Pointer) {
            PointerType* ptr = dynamic_cast<PointerType*>(fun->retType.get());
            ptr->elementType = TypeFactory::getTypeFromName(type_name).release();
        }
    }
}

void var_decl::finalize() {
    for (auto &def : defs) {
        def->finalizeType(typeName);
        def->setConst(is_const);
    }
    std::reverse(defs.begin(), defs.end());
}

// Initial Value
init_val::init_val(std::pair<size_t, size_t> loc) : node(loc) {}

void init_val::addChild(initValPtr child) {
    children.push_back(std::move(child));
}

void init_val::setLoc(size_t row, size_t col) {
    location = {row, col};
}

void init_val::reverseChildren() {
    std::reverse(children.begin(), children.end());
}

void init_val::setConst(bool is_const) {
    this->is_const = is_const;
}

void init_val::setScalar(expPtr val) {
    scalar = std::move(val);
}

void init_val::setLoc(std::pair<size_t, size_t> loc) {
    location = loc;
}

std::string init_val::to_string() {
    return (is_const ? "ConstInitVal" : "InitVal");
}

void init_val::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    if(scalar) {
        scalar->printAST(prefix + "    ", prefix + "└── ");
    } else {
        for (size_t i = 0; i < children.size(); ++i) {
            if (i != children.size() - 1) {
                children[i]->printAST(prefix + "│   ", prefix + "├── ");
            } else {
                children[i]->printAST(prefix + "    ", prefix + "└── ");
            }
        }
    }
}

analyzeInfo init_val::dispatch(TypeChecker *ptr)
{
    return ptr->analyze(this);
}

codeGenInfo init_val::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

class_def::class_def(std::pair<size_t, size_t> loc)
: node(loc) {
    kind = ASTKind::Class_Def;
}

void class_def::addChild(nodePtr child) {
    children.push_back(std::move(child));
}

void class_def::setName(std::string name) {
    this->name = name;
}

void class_def::setLoc(std::pair<size_t, size_t> loc) {
    location = loc;
}

std::string class_def::to_string() {
    return "ClassDef <id: " + name + ">";
}

void class_def::reverseChildren() {
    std::reverse(children.begin(), children.end());
}

void class_def::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    for (size_t i = 0; i < children.size(); ++i) {
        if (i != children.size() - 1) {
            children[i]->printAST(prefix + "│   ", prefix + "├── ");
        } else {
            children[i]->printAST(prefix + "    ", prefix + "└── ");
        }
    }
}

analyzeInfo class_def::dispatch(TypeChecker *ptr) {
    return ptr->analyze(this);
}

codeGenInfo class_def::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

member_access::member_access(std::pair<size_t, size_t> loc, expPtr exp, const std::string &name)
: expr(loc, nullptr) {
    this->exp = std::move(exp);
    this->name = name;
}

std::string member_access::to_string() {
    return "member_access <kind " + std::string(isFunc ? "method" : "var") + ", member " + name + ">";
}

void member_access::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    if(!isFunc) {
        exp->printAST(prefix + "     ", prefix + "└── objexpr: ");
    } else {
        exp->printAST(prefix + "│    ", prefix + "├── objexpr: ");
        for(size_t i = 0; i < args.size(); i++) {
            if (i != args.size() - 1) {
                args[i]->printAST(prefix + "│    ", prefix + "├── arg " + std::to_string(i) + ") ");
            } else {
                args[i]->printAST(prefix + "     ", prefix + "└── arg " + std::to_string(i) + ") ");
            }
        }
    }
}

analyzeInfo member_access::dispatch(TypeChecker *ptr) {
    return ptr->analyze(this);
}

codeGenInfo member_access::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

constInfo member_access::const_eval(TypeChecker *ptr) {
    return constInfo{
        .is_const = false,
        .value = nullptr,
        .type = nullptr
    };
}

pointer_acc::pointer_acc(std::pair<size_t, size_t> loc, expPtr exp, const std::string &name) 
: expr(loc, nullptr){
    this->exp = std::move(exp);
    this->name = name;
}

// Convert node to string representation
std::string pointer_acc::to_string() {
    return "pointer_acc <kind " + std::string(isFunc ? "method" : "var") + ", member " + name + ">";
}

// Print AST structure
void pointer_acc::printAST(std::string prefix, std::string info_prefix) {
    std::cout << info_prefix << to_string() << locToString(location, error_msg);
    if(!isFunc) {
        exp->printAST(prefix + "     ", prefix + "└── objptr: ");
    } else {
        exp->printAST(prefix + "│    ", prefix + "├── objptr: ");
        for(size_t i = 0; i < args.size(); i++) {
            if (i != args.size() - 1) {
                args[i]->printAST(prefix + "│    ", prefix + "├── arg " + std::to_string(i) + ") ");
            } else {
                args[i]->printAST(prefix + "     ", prefix + "└── arg " + std::to_string(i) + ") ");
            }
        }
    }
}

// Type checking dispatch
analyzeInfo pointer_acc::dispatch(TypeChecker *ptr) {
    return ptr->analyze(this);
}

codeGenInfo pointer_acc::dispatch(codeGen *ptr)
{
    return ptr->analyze(this);
}

// Constant expression evaluation (if applicable)
constInfo pointer_acc::const_eval(TypeChecker *ptr) {
    return constInfo{
        .is_const = false,
        .value = nullptr,
        .type = nullptr
    };
}

type_cast::type_cast(std::pair<size_t, size_t> loc, expPtr exp, Type *target)
: expr(loc, nullptr)
{
    this->exp = std::move(exp);
    this->target = target;
    isImplicit = false;
}

type_cast::type_cast(expPtr exp, Type *target)
: expr({0,0}, nullptr){
    this->exp = std::move(exp);
    this->target = target;
    isImplicit = true;
}

std::string type_cast::to_string()
{
    return "type_cast <target " + (target ? target->to_string() : "") + ">" + 
    (inferred_type ? color::magenta + std::string(" inferredType: ") + inferred_type->to_string() + color::reset + " ": "");
}

void type_cast::printAST(std::string prefix, std::string info_prefix)
{
    std::cout << info_prefix << to_string() << (!isImplicit ? locToString(location, error_msg) : " " + error_msg + "\n");
    exp->printAST(prefix + "    ", prefix + "└── ");
}

analyzeInfo type_cast::dispatch(TypeChecker *ptr)
{
    return analyzeInfo();
}

codeGenInfo type_cast::dispatch(codeGen *ptr)
{
    return codeGenInfo();
}

constInfo type_cast::const_eval(TypeChecker *ptr)
{
    return constInfo();
}
