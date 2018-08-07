#include "jgen_symbol.h"
#include "jgen_type.h"
#include "jgen_node_provider.h"

using namespace JGEN;
using namespace Json;
using namespace std;

map<JGenSymbolNode *, ST *> *SymbolHandler::cache;

void SymbolHandler::init() {
    cache = new map<JGenSymbolNode *, ST *>();
}

JGenTypeNode *JGenSymbolNode::getType() {
    FmtAssert(node.isMember("type"), ("node don't have key: type."));
    mINT32 jIndex = node["type"].asInt();
    return JGenNodeProvider::getTypeNode(jsonIR.get_type(jIndex));
}

ST *SymbolHandler::addVarSymbol(JGenVarSymbolNode *symbol) {

}

ST *SymbolHandler::addMethodSymbol(JGenMethodSymbolNode *symbol) {
    ST *st = NULL;
    ST_SCLASS sclass;
    ST_EXPORT eclass;
    SYMTAB_IDX level;
    TY_IDX func_ty_idx = TypeHandler::getType(symbol->getType());

    sclass = SCLASS_EXTERN;
    eclass = EXPORT_LOCAL;
    level = GLOBAL_SYMTAB + 1;

    PU_IDX pu_idx;
    PU &pu = New_PU(pu_idx);

    PU_Init(pu, func_ty_idx, level);

    st = New_ST(GLOBAL_SYMTAB);

    const char *p;
    // TODO: implement name mangling, get mangling name
    p = symbol->getName();
    // if (Is_Java_Undeletable_Function(p)) // czw
    // {
    //   Set_PU_no_delete(pu);
    // }
    ST_Init(st, Save_Str(p), CLASS_FUNC, sclass, eclass, TY_IDX(pu_idx));
    return st;
}

ST *SymbolHandler::getSymbol(JGenSymbolNode *symbol) {
    if(cache->find(symbol) != cache->end()) {
      return cache->find(symbol)->second;
    }
    switch(symbol->getKind()) {
        case JGEN_SYMBOL_VAR:
            return SymbolHandler::addVarSymbol(static_cast<JGenVarSymbolNode *>(symbol));
        case JGEN_SYMBOL_MTH:
            return SymbolHandler::addMethodSymbol(static_cast<JGenMethodSymbolNode *>(symbol));
        default:
            FmtAssert(0, ("not handle symbol : %d.", symbol->getKind()));
    }
}
