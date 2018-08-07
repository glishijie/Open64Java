#include "jgen_decl.h"
#include "jgen_node_provider.h"

using namespace std;
using namespace Json;
using namespace JGEN;

JGenTopLevelNode::JGenTopLevelNode(Json_IR &_jsonIR, Value &_node): JGenDeclNode(_jsonIR, _node) {
    clazzDefs = new CodeVector();
    imports = new CodeVector();
    FmtAssert(node.isMember("defs"), ("node don't have key: defs."));
    Value &defs = node["defs"];
    for(Value::iterator I = defs.begin(); I != defs.end(); ++I) {
        FmtAssert((*I).isMember("tag"), ("node don't have key : tag."));
        if((*I)["tag"].asInt() == JGEN_CLASSDEF) {
            clazzDefs->push_back(JGenNodeProvider::getCodeNode(*I));
        } else if((*I)["tag"].asInt() == JGEN_IMPORT) {
            imports->push_back(JGenNodeProvider::getCodeNode(*I));
        } else {
            FmtAssert(0, ("toplevel's defs can't contains tag : %s.", node["tag_name"]));
        }
    }
}

JGenClassDefNode::JGenClassDefNode(Json_IR &_jsonIR, Value &_node): JGenDeclNode(_jsonIR, _node) {
    methodDefs = new std::vector<JGenCodeNode *>();
    FmtAssert(node.isMember("defs"), ("node don't have key : defs."));
    Value &defs = node["defs"];
    for(Value::iterator I = defs.begin(); I != defs.end(); ++I) {
        FmtAssert((*I).isMember("tag"), ("node don't have key : tag."));
        FmtAssert((*I)["tag"].asInt() == JGEN_METHODDEF, ("class node contains node is not METHODDEF."));
        methodDefs->push_back(JGenNodeProvider::getCodeNode(*I));
    }
}

JGenTypeNode *JGenClassDefNode::getType() {
    FmtAssert(node.isMember("type"), ("node don't have key : type."));
    mINT32 jIndex = node["type"].asInt();
    JGenTypeNode *type = JGenNodeProvider::getTypeNode(jsonIR.get_type(jIndex));
    FmtAssert(type->getKind() == JGEN_TYPE_CLASS, ("class's type is not CLASS."));
    return type;
}

JGenMethodDefNode::JGenMethodDefNode(Json_IR &_jsonIR, Json::Value &node): JGenDeclNode(_jsonIR, node) {
    if(node.isMember("params")) {
        parameterVector = new CodeVector();
        Value &params = node["params"];
        for(Value::iterator I = params.begin(); I != params.end(); ++I) {
            parameterVector->push_back(JGenNodeProvider::getCodeNode(*I));
        }
    }
    if(node.isMember("body")) {
        body = JGenNodeProvider::getCodeNode(node["body"]);
    }
}

JGenTypeNode *JGenMethodDefNode::getType() {
    FmtAssert(node.isMember("type"), ("node don't have key : type."));
    mINT32 jIndex = node["type"].asInt();
    JGenTypeNode *type = JGenNodeProvider::getTypeNode(jsonIR.get_type(jIndex));
    FmtAssert(type->getKind() == JGEN_TYPE_METHOD, ("method's type is not METHOD, type : %d.", type->getKind()));
    return type;
}

JGenSymbolNode *JGenMethodDefNode::getSymbol() {
    FmtAssert(node.isMember("sym"), ("node don't have key : sym."));
    mINT32 jIndex = node["sym"].asInt();
    JGenSymbolNode *symbol = JGenNodeProvider::getSymbolNode(jsonIR.get_symbol(jIndex));
    FmtAssert(symbol->getKind() == JGEN_SYMBOL_MTH, ("method's symbol is not METHOD, symbol : %d.", symbol->getKind()));
    return symbol;  
}
