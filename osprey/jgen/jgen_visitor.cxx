//
// Created by xc5 on 27/7/2018.
//

#include "jgen_visitor.h"

using namespace JGEN;

int JGEN_Visitor::visit(JGenCodeNode *node) {
    switch(node->getTag()) {
        case JGEN_TOPLEVEL:
            return visit_toplevel(static_cast<JGenTopLevelNode *>(node));
        case JGEN_CLASSDEF:
            return visit_classdef(static_cast<JGenClassDefNode *>(node));
        default:
            FmtAssert(0, ("not handle tag : %d.", node->getTag()));
    }
}

int JGEN_Visitor::visit_toplevel(JGenTopLevelNode *node) {
    return 0;
}

int JGEN_Visitor::visit_classdef(JGenClassDefNode *node) {
    return 0;
}