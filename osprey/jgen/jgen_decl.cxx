#include "jgen_decl.h"
#include "jgen_node_provider.h"

#include <vector>

using namespace std;
using namespace Json;
using namespace JGEN;

JGenTopLevelNode::JGenTopLevelNode(Json_IR &_jsonIR, Json::Value &_node): JGenDeclNode(_jsonIR, _node) {
    clazzDefs = new vector<JGenCodeNode *>();
    imports = new vector<JGenCodeNode *>();
    FmtAssert(node.isMember("defs"), ("node don't have key: defs."));
    Value &defs = node["defs"];
    for(Value::iterator I = defs.begin(); I != defs.end(); ++I) {
        if((*I)["tag"].asUInt() == JGEN_CLASSDEF) {
            clazzDefs->push_back(JGenNodeProvider::getCodeNode(node));
        } else if((*I)["tag"].asUInt() == JGEN_IMPORT) {
            imports->push_back(JGenNodeProvider::getCodeNode(node));
        } else {
            FmtAssert(0, ("toplevel's defs can't contains tag : %s.", node["tag_name"]));
        }
    }
}

