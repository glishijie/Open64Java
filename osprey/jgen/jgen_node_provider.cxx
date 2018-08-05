#include "jgen_node_provider.h"
#include "jgen_type.h"
#include "jgen_decl.h"
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
    switch(node["tag"].asUInt()) {
        case JGEN_TOPLEVEL:
            jgenNode = new JGenTopLevelNode(*jsonIR, node);
            break;
        case JGEN_IMPORT:
            jgenNode =  new JGenImportNode(*jsonIR, node);
            break;
        case JGEN_CLASSDEF:
            jgenNode =  new JGenClassDefNode(*jsonIR, node);
            break;
        default:
            FmtAssert(0, ("not handle tag: %d.", node["tag"]));
    }
    nodeCache->insert(make_pair<Value *, JGenNode *>(&node, jgenNode));
    return static_cast<JGenCodeNode *>(jgenNode);
}

JGenTypeNode *JGenNodeProvider::getTypeNode(Value &node) {
    JGenNode *jgenNode = getNode(node);
    if(jgenNode != NULL) {
        return static_cast<JGenTypeNode *>(jgenNode);
    }
    switch(node["kind"].asUInt()) {
        case jBYTE:
        case jCHAR:
        case jSHORT:
        case jLONG:
        case jFLOAT:
        case jINT:
        case jDOUBLE:
        case jBOOLEAN:
        case jVOID:
            jgenNode = new JGenPrimitiveTypeNode(*jsonIR, node);
            break;
        case jCLASS:
            jgenNode = new JGenClassTypeNode(*jsonIR, node);
            break;
        default:
            FmtAssert(0, ("not handle other type."));
    }
    nodeCache->insert(make_pair<Value *, JGenNode *>(&node, jgenNode));
    return static_cast<JGenTypeNode *>(jgenNode);
}

JGenSymbolNode *JGenNodeProvider::getSymbolNode(Value &node) {

}