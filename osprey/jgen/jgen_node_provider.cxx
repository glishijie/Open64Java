#include "jgen_node_provider.h"
#include "jgen_type.h"

using namespace JGEN;

Json_IR *JGenNodeProvider::jsonIR = NULL;
std::map<Json::Value *, JGenNode *> *JGenNodeProvider::nodeCache = NULL;

JGenTypeNode *JGenNodeProvider::getTypeNode(Json::Value &node) {

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
            return new JGenPrimitiveTypeNode(*jsonIR, node);
        case jCLASS:
            return new JGenClassTypeNode(*jsonIR, node);
        default:
            FmtAssert(0, ("not handle other type."));
    }
}