#ifndef JGEN_SYMBOL_H
#define JGEN_SYMBOL_H

#include "jgen_include.h"
#include "jgen_node.h"
#include "jgen_decl.h"
#include "jgen_type.h"

namespace JGEN {

static int NIL = 0;

/** The kind of package symbols.
 */
static int PCK = 1 << 0;

/** The kind of type symbols (classes, interfaces and type variables).
 */
static int TYP = 1 << 1;

/** The kind of variable symbols.
 */
static int VAR = 1 << 2;

/** The kind of values (variables or non-variable expressions), includes VAR.
 */
static int VAL = (1 << 3) | VAR;

/** The kind of methods.
 */
static int MTH = 1 << 4;

/** Poly kind, for deferred types.
 */
static int POLY = 1 << 5;

/** The error kind, which includes all other kinds.
 */
static int ERR = (1 << 6) - 1;

class JGenSymbolNode: public JGenNode {
    public:
    explicit JGenSymbolNode(Json_IR &_jsonIR, Json::Value &_node): JGenNode(_jsonIR, _node, JGenSymbolCat) {
    }

    const char *getName() {
        FmtAssert(node.isMember("name"), ("node don't have key: name."));
        return node["name"].asCString();
    }

    mUINT32 getKind() {
        FmtAssert(node.isMember("kind"), ("node don't have key: kind."));
        return node["kind"].asUInt();
    }

    std::string getKindName() {
        FmtAssert(node.isMember("kindname"), ("node don't have key: kindname."));
        return node["kindname"].asString();
    }

    mINT64 getFlag() {
        FmtAssert(node.isMember("flag"), ("node don't have key: flag."));
        return node["flag"].asInt64();
    }

    JGenTypeNode *getType();
};

class JGenVarSymbolNode: public JGenSymbolNode {
    public:
    explicit JGenVarSymbolNode(Json_IR &_jsonIR, Json::Value &_node): JGenSymbolNode(_jsonIR, _node) {
    }
};

class SymbolHandler {
    public:
    static void init();
    static ST *getSymbol(JGenDeclNode *decl);
    
    private:
    static ST *addVarSymbol(JGenVarDeclNode *decl);
};

};

#endif
