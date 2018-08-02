#ifndef JGEN_TYPE_H
#define JGEN_TYPE_H

#include <json/json.h>
#include <map>
#include <string>

#include "jgen_include.h"
#include "symtab_idx.h"

namespace JGEN {

    class TypeHandler {
        public:
        static void init();
        static TY_IDX addType(Json::Value &type);
        static mUINT32 getType(Json::Value &type);
        
        private:
        static TY_IDX addClassType(Json::Value &type, TY_IDX idx);

        private:
        static std::map<Json::Value *, TY_IDX> *typeCache;
    };
}

#endif