#include <json/json.h>
#include <map>

#include "defs.h"
#include "symtab_idx.h"

namespace JGEN {
class TypeHandler {
    public:
    static void init();
    static TY_IDX addType(Json::Value& jTypeTree, mUINT32 jIndex);
    static mUINT32 getType(mUINT32 jIndex);
    
    private:
    static TY_IDX addPrimitiveType(Json::Value& jTypeTree, mUINT32 jIndex);
    static TY_IDX addClassType(Json::Value& jTypeTree, mUINT32 jIndex);

    private:
    static std::map<mUINT32, TY_IDX> *typeCache;
};
}

