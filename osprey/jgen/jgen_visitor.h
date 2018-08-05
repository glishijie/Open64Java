//
// Created by xc5 on 27/7/2018.
//

#ifndef OSPREY_JGEN_VISITOR_H
#define OSPREY_JGEN_VISITOR_H

#include "jgen_include.h"
#include "jgen_code.h"
#include "jgen_decl.h"

namespace JGEN {

class JGEN_Visitor {
    public:

    JGEN_Visitor() {};
    ~JGEN_Visitor() {};

    int visit(JGenCodeNode *node);
    int visit_toplevel(JGenTopLevelNode *node);
    int visit_classdef(JGenClassDefNode *node);
    int visit_methoddef(JGenMethodDefNode *node);
};

}

#endif //OSPREY_JGEN_VISITOR_H
