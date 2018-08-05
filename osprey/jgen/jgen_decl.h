#ifndef JGEN_DECL_H
#define JGEN_DECL_H

#include "jgen_code.h"
#include "jgen_type.h"

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

    CodeVector *getClazzDefs() {
        return clazzDefs;
    }

    CodeVector *getImports() {
        return imports;
    }

    private:
    CodeVector *clazzDefs;
    CodeVector *imports;
};

class JGenImportNode: public JGenDeclNode {
    public:
    explicit JGenImportNode(Json_IR &_jsonIR, Json::Value &_node): JGenDeclNode(_jsonIR, _node) {
    }
};

class JGenClassDefNode: public JGenDeclNode {
    public:
    explicit JGenClassDefNode(Json_IR &_jsonIR, Json::Value &_node);
    ~JGenClassDefNode() {
        delete methodDefs;
    }

    CodeVector *getMethodDefs() {
        return methodDefs;
    }

    JGenTypeNode *getType();

    private:
    CodeVector *methodDefs;
};

class JGenMethodDefNode: public JGenDeclNode {
    public:
    explicit JGenMethodDefNode(Json_IR &_jsonIR, Json::Value &node): JGenDeclNode(_jsonIR, node) {
    }

    ~JGenMethodDefNode() {
    }
};

class JGenVarDeclNode: public JGenDeclNode {
    public:
    explicit JGenVarDeclNode(Json_IR &_jsonIR, Json::Value &_node): JGenDeclNode(_jsonIR, _node) {
    }
};

}
#endif