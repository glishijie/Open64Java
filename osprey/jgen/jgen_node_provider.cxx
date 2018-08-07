#include "jgen_node_provider.h"
#include "jgen_type.h"
#include "jgen_symbol.h"
#include "jgen_decl.h"
#include "jgen_stmt.h"
#include "jgen_include.h"

using namespace JGEN;
using namespace Json;
using namespace std;

Json_IR *JGenNodeProvider::jsonIR = NULL;
map<Value *, JGenNode *> *JGenNodeProvider::nodeCache = NULL;

JGenCodeNode *JGenNodeProvider::getCodeNode(Value &node) {
    JGenNode *jgenNode = getNode(node);
    if(jgenNode != NULL) {
        return static_cast<JGenCodeNode *>(jgenNode);
    }
    switch(node["tag"].asInt()) {
        case JGEN_TOPLEVEL:
            jgenNode = new JGenTopLevelNode(*jsonIR, node);
            break;
        case JGEN_IMPORT:
            jgenNode =  new JGenImportNode(*jsonIR, node);
            break;
        case JGEN_CLASSDEF:
            jgenNode =  new JGenClassDefNode(*jsonIR, node);
            break;
        case JGEN_METHODDEF:
            jgenNode = new JGenMethodDefNode(*jsonIR, node);
            break;
        case JGEN_BLOCK:
            jgenNode = new JGenBlockStmtNode(*jsonIR, node);
            break;
        default:
            FmtAssert(0, ("not handle tag: %d.", node["tag"].asInt()));
            break;
    }
    nodeCache->insert(make_pair<Value *, JGenNode *>(&node, jgenNode));
    return static_cast<JGenCodeNode *>(jgenNode);
}

JGenTypeNode *JGenNodeProvider::getTypeNode(Value &node) {
    JGenNode *jgenNode = getNode(node);
    if(jgenNode != NULL) {
        return static_cast<JGenTypeNode *>(jgenNode);
    }
    switch(node["kind"].asInt()) {
        case JGEN_TYPE_BYTE:
        case JGEN_TYPE_CHAR:
        case JGEN_TYPE_SHORT:
        case JGEN_TYPE_LONG:
        case JGEN_TYPE_FLOAT:
        case JGEN_TYPE_INT:
        case JGEN_TYPE_DOUBLE:
        case JGEN_TYPE_BOOLEAN:
        case JGEN_TYPE_VOID:
            jgenNode = new JGenPrimitiveTypeNode(*jsonIR, node);
            break;
        case JGEN_TYPE_CLASS:
            jgenNode = new JGenClassTypeNode(*jsonIR, node);
            break;
        case JGEN_TYPE_METHOD:
            jgenNode = new JGenMethodTypeNode(*jsonIR, node);
            break;
        default:
            FmtAssert(0, ("not handle type : %d.", node["kind"].asInt()));
            break;
    }
    nodeCache->insert(make_pair<Value *, JGenNode *>(&node, jgenNode));
    return static_cast<JGenTypeNode *>(jgenNode);
}

JGenSymbolNode *JGenNodeProvider::getSymbolNode(Value &node) {
    JGenNode *jgenNode = getNode(node);
    if(jgenNode != NULL) {
        return static_cast<JGenSymbolNode *>(jgenNode);
    }
    switch(node["kind"].asInt()) {
        case JGEN_SYMBOL_VAR:
            jgenNode = new JGenVarSymbolNode(*jsonIR, node);
            break;
        case JGEN_SYMBOL_MTH:
            jgenNode = new JGenMethodSymbolNode(*jsonIR, node);
            break;
        default:
            FmtAssert(0, ("not handle symbol : %d.", node["kind"].asInt()));
            break;
    }
    nodeCache->insert(make_pair<Value *, JGenNode *>(&node, jgenNode));
    return static_cast<JGenSymbolNode *>(jgenNode);
}