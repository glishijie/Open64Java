#ifndef JGEN_DECL_H
#define JGEN_DECL_H

#include "jgen_code.h"

namespace JGEN {

class JGenDeclNode: public JGenCodeNode {
    public:
    explicit JGenDeclNode(Json_IR &_jsonIR, Json::Value &_node): JGenCodeNode(_jsonIR, _node) {
    }
};

class JGenVarDeclNode: public JGenDeclNode {
    public:
    explicit JGenVarDeclNode(Json_IR &_jsonIR, Json::Value &_node): JGenDeclNode(_jsonIR, _node) {
    }
};

}
#endif