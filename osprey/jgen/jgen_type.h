#ifndef JGEN_TYPE_H
#define JGEN_TYPE_H

#include <map>
#include <string>
#include <vector>

#include "jgen_include.h"
#include "symtab_idx.h"
#include "jgen_node.h"

namespace JGEN {

enum TypeTag {
    /** The tag of the basic type `byte'.
     */
    JGEN_TYPE_BYTE = 0,

    /** The tag of the basic type `char'.
     */
    JGEN_TYPE_CHAR,

    /** The tag of the basic type `short'.
     */
    JGEN_TYPE_SHORT,

    /** The tag of the basic type `long'.
     */
    JGEN_TYPE_LONG,

    /** The tag of the basic type `float'.
     */
    JGEN_TYPE_FLOAT,
    /** The tag of the basic type `int'.
     */
    JGEN_TYPE_INT,
    /** The tag of the basic type `double'.
     */
    JGEN_TYPE_DOUBLE,
    /** The tag of the basic type `boolean'.
     */
    JGEN_TYPE_BOOLEAN,

    /** The tag of the type `void'.
     */
    JGEN_TYPE_VOID,

    /** The tag of all class and interface types.
     */
    JGEN_TYPE_CLASS,

    /** The tag of all array types.
     */
    JGEN_TYPE_ARRAY,

    /** The tag of all (monomorphic) method types.
     */
    JGEN_TYPE_METHOD,

    /** The tag of all package "types".
     */
    JGEN_TYPE_PACKAGE,

    /** The tag of all (source-level) type variables.
     */
    JGEN_TYPE_TYPEVAR,

    /** The tag of all type arguments.
     */
    JGEN_TYPE_WILDCARD,

    /** The tag of all polymorphic (method-) types.
     */
    JGEN_TYPE_FORALL,

    /** The tag of deferred expression types in method context
     */
    JGEN_TYPE_DEFERRED,

    /** The tag of the bottom type {@code <null>}.
     */
    JGEN_TYPE_BOT,

    /** The tag of a missing type.
     */
    JGEN_TYPE_NONE,

    /** The tag of the error type.
     */
    JGEN_TYPE_ERROR,

    /** The tag of an unknown type
     */
    JGEN_TYPE_UNKNOWN,

    /** The tag of all instantiatable type variables.
     */
    JGEN_TYPE_UNDETVAR,

    /** Pseudo-types, these are special tags
     */
    JGEN_TYPE_UNINITIALIZED_THIS,

    JGEN_TYPE_UNINITIALIZED_OBJECT

};

class JGenTypeNode: public JGenNode {
    public:
    explicit JGenTypeNode(Json_IR &_jsonIR, Json::Value &_node): JGenNode(_jsonIR, _node, JGenTypeCat) {
    }

    const char *getName() {
        FmtAssert(node.isMember("name"), ("node don't have key: name."));
        return node["name"].asCString();
    }

    mINT32 getKind() {
        FmtAssert(node.isMember("kind"), ("node don't have key: kind."));
        return node["kind"].asInt();
    }

    std::string getKindName() {
        FmtAssert(node.isMember("kindname"), ("node don't have key: kindname."));
        return node["kindname"].asString();
    }

    bool isPrimitive() {
        return getKind() <= JGEN_TYPE_VOID;
    }

    virtual mINT32 getAlign() = 0;
    
};

typedef std::vector<JGenTypeNode *> TypeVector;
typedef std::vector<JGenTypeNode *>::iterator TypeVectorIter;

class JGenPrimitiveTypeNode : public JGenTypeNode {
    public:
    explicit JGenPrimitiveTypeNode(Json_IR &_jsonIR, Json::Value &node): JGenTypeNode(_jsonIR, node) {
    }

    mINT32 getAlign() {
        return 0;
    }
    
};

class JGenClassTypeNode : public JGenTypeNode {
    public:
    explicit JGenClassTypeNode(Json_IR &_jsonIR, Json::Value &node): JGenTypeNode(_jsonIR, node) {
    }

    bool isAnonymous() {
        return false;
    }

    mINT32 getAlign() {
        return 4;
    }
};

class JGenMethodTypeNode: public JGenTypeNode {
    public:
    explicit JGenMethodTypeNode(Json_IR &_jsonIR, Json::Value &node);

    JGenTypeNode *getReturnType() {
        return resType;
    }

    TypeVector *getParameterTypeVector() {
        return parameter_type_vector;
    }

    mINT32 getAlign() {
        return 1;
    }

    private:
    TypeVector *parameter_type_vector;
    JGenTypeNode *resType;
};

class TypeHandler {
    public:
    static void init();
    static TY_IDX getType(JGenTypeNode *type);
    
    private:
    static TY_IDX addClassType(JGenClassTypeNode *type, TY_IDX idx);
    static TY_IDX addMethodType(JGenMethodTypeNode *type, TY_IDX idx);

    private:
    static std::map<JGenTypeNode *, TY_IDX> *cache;
};
}

#endif