#ifndef JGEN_STMT_H
#define JGEN_STMT_H

#include "jgen_include.h"
#include "jgen_code.h"

namespace JGEN {

class JGenStmtNode: public JGenCodeNode {
    public:
    explicit JGenStmtNode(Json_IR &_jsonIR, Json::Value &_node): JGenCodeNode(_jsonIR, _node) {
    }
};

class JGenBlockStmtNode: public JGenStmtNode {
    public:
    explicit JGenBlockStmtNode(Json_IR &_jsonIR, Json::Value &_node): JGenStmtNode(_jsonIR, _node) {
    }
};

}

#endif