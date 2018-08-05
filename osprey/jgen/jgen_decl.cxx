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
        if((*I)["tag"].asUInt() == JGEN_CLASSDEF) {
            clazzDefs->push_back(JGenNodeProvider::getCodeNode(*I));
        } else if((*I)["tag"].asUInt() == JGEN_IMPORT) {
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
        FmtAssert((*I)["tag"].asUInt() == JGEN_METHODDEF, ("class node contains node is not METHODDEF."));
        methodDefs->push_back(JGenNodeProvider::getCodeNode(*I));
    }
}

JGenTypeNode *JGenClassDefNode::getType() {
    FmtAssert(node.isMember("type"), ("node don't have key : type."));
    mUINT32 jIndex = node["type"].asUInt();
    FmtAssert(jIndex >= 100 && jsonIR.get_type_tree().size() > (jIndex - 100), ("class type index out of bounds."));
    Value &type = jsonIR.get_type_tree()[jIndex - 100];
    return JGenNodeProvider::getTypeNode(type);
}