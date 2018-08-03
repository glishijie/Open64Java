#ifndef JGEN_NODE
#define JGEN_NODE

#include "json/json.h"
#include "json_reader.h"

namespace JGEN {

enum JGenNodeCategory {
    JGenSymbolCat,
    JGenTypeCat,
    JGenCodeCat
};

class JGenNode {
    public:
    explicit JGenNode(Json_IR &_jsonIR, Json::Value &_node, JGenNodeCategory _cate):jsonIR(_jsonIR), node(_node), cate(_cate) {
    }

    JGenNodeCategory getCate() {
        return cate;
    }

    public:
    Json::Value &node;
    Json_IR &jsonIR;

    private:
    JGenNodeCategory cate;
};

}

#endif