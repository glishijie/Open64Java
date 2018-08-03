#include "jgen_symbol.h"
#include "jgen_node_provider.h"

using namespace JGEN;
using namespace Json;
using namespace std;


void SymbolHandler::init() {
}

JGenTypeNode *JGenSymbolNode::getType() {
    FmtAssert(node.isMember("type"), ("node don't have key: type."));
    mUINT32 jTypeId = node["type"].asUInt();
    if(jTypeId < 100) {
        FmtAssert(0, ("json reader will create primitive type, and provide api to get it."));
    }
    return JGenNodeProvider::getTypeNode(jsonIR.get_type_tree()[jTypeId - 100]);
}

ST *SymbolHandler::addVarSymbol(JGenVarDeclNode *symbol) {

}

ST *SymbolHandler::getSymbol(JGenDeclNode *symbol) {

}
