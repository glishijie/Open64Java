#ifndef JGEN_DECL_H
#define JGEN_DECL_H

#include "jgen_code.h"

#include <vector>

namespace JGEN {

class JGenDeclNode: public JGenCodeNode {
    public:
    explicit JGenDeclNode(Json_IR &_jsonIR, Json::Value &_node): JGenCodeNode(_jsonIR, _node) {
    }
};

class JGenTopLevelNode: public JGenDeclNode {
    public:
    explicit JGenTopLevelNode(Json_IR &_jsonIR, Json::Value &_node);

    ~JGenTopLevelNode() {
        delete clazzDefs;
        delete imports;
    }

    std::vector<JGenCodeNode *> *getClazzDefs() {
        return clazzDefs;
    }

    std::vector<JGenCodeNode *> *getImports() {
        return imports;
    }

    private:
    std::vector<JGenCodeNode *> *clazzDefs;
    std::vector<JGenCodeNode *> *imports;
};

class JGenImportNode: public JGenDeclNode {
    public:
    explicit JGenImportNode(Json_IR &_jsonIR, Json::Value &_node): JGenDeclNode(_jsonIR, _node) {
    }
};

class JGenClassDefNode: public JGenDeclNode {
    public:
    explicit JGenClassDefNode(Json_IR &_jsonIR, Json::Value &_node): JGenDeclNode(_jsonIR, _node) {
    }
};

class JGenVarDeclNode: public JGenDeclNode {
    public:
    explicit JGenVarDeclNode(Json_IR &_jsonIR, Json::Value &_node): JGenDeclNode(_jsonIR, _node) {
    }
};

}
#endif