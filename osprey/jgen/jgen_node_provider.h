#ifndef JGEN_NODE_PROVIDER
#define JGEN_NODE_PROVIDER

#include "json/json.h"
#include "json_reader.h"
#include "jgen_include.h"
#include <map>
#include "jgen_node.h"
#include "jgen_type.h"

namespace JGEN {

class JGenNodeProvider {
    public:
    static void init(Json_IR *ir) {
        jsonIR = ir;
        nodeCache = new std::map<Json::Value *, JGenNode *>();
    }

    static JGenTypeNode *getTypeNode(Json::Value &node);

    static Json_IR *getIR() {
        return jsonIR;
    }

    private:
    static JGenNode *getNode(Json::Value &node) {
        if(nodeCache->find(&node) != nodeCache->end()) {
            return nodeCache->find(&node)->second;
        }
        return NULL;
    }

    private:
    static std::map<Json::Value *, JGenNode *> *nodeCache;
    static Json_IR *jsonIR;
};

}

#endif