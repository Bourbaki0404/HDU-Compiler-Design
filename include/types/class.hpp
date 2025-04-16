#include "common/common.hpp"
#include "types/types.hpp"
// Simplified Class Metadata

enum AccessSpecifier {
    PUBLIC, PRIVATE
};

struct ClassInfo {
    std::string name;
    std::string baseClass;                  // Single parent (empty if none)
    std::vector<FieldInfo> fields;          // Member variables
    std::vector<MethodInfo> methods;        // Member functions
    AccessSpecifier defaultAccess;          // Default: private in C++
    bool isPolymorphic = false;             // Has virtual methods?
};

// Field & Method Metadata
struct FieldInfo {
    std::string name;
    TypePtr type;
    AccessSpecifier access;
};

struct MethodInfo {
    std::string name;
    TypePtr type;
    bool isVirtual;
    bool isOverride;    // Marks if it overrides a base method
    AccessSpecifier access;
};