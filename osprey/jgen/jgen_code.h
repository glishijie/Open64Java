#ifndef JGEN_CODE_H
#define JGEN_CODE_H

#include "jgen_node.h"

namespace JGEN {

class JGenCodeNode: public JGenNode {
    public:
    explicit JGenCodeNode(Json_IR &_jsonIR, Json::Value &_node): JGenNode(_jsonIR, _node, JGenCodeCat) {
    }
};

}
#endif