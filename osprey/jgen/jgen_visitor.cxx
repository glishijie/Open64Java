//
// Created by xc5 on 27/7/2018.
//

#include "jgen_visitor.h"
#include "jgen_decl.h"
#include "jgen_type.h"

#include <vector>

using namespace JGEN;
using namespace std;

int JGEN_Visitor::visit(JGenCodeNode *node) {
    switch(node->getTag()) {
        case JGEN_TOPLEVEL:
            return visit_toplevel(static_cast<JGenTopLevelNode *>(node));
        case JGEN_CLASSDEF:
            return visit_classdef(static_cast<JGenClassDefNode *>(node));
        case JGEN_METHODDEF:
            return visit_methoddef(static_cast<JGenMethodDefNode *>(node));
        default:
            FmtAssert(0, ("not handle tag : %d.", node->getTag()));
    }
}

int JGEN_Visitor::visit_toplevel(JGenTopLevelNode *node) {
    vector<JGenCodeNode *> *clazzDefs = node->getClazzDefs();
    for(vector<JGenCodeNode *>::iterator I = clazzDefs->begin(); I != clazzDefs->end(); ++I) {
        visit(*I);
    }
    return 0;
}

int JGEN_Visitor::visit_classdef(JGenClassDefNode *node) {
    TypeHandler::getType(node->getType());
    CodeVector *methodDefs = node->getMethodDefs();
    for(CodeVectorIter I = methodDefs->begin(); I != methodDefs->end(); ++I) {
        visit(*I);
    }
    return 0;
}

int JGEN_Visitor::visit_methoddef(JGenMethodDefNode *node) {
    return 0;
}