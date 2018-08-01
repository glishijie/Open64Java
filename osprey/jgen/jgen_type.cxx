#include "jgen_type.h"
#include "jgen_include.h"

using namespace JGEN;
using namespace Json;
using namespace std;

map<mUINT32, TY_IDX> *TypeHandler::typeCache;

enum TypeTag {
    /** The tag of the basic type `byte'.
     */
    jBYTE = 0,

    /** The tag of the basic type `char'.
     */
    jCHAR,

    /** The tag of the basic type `short'.
     */
    jSHORT,

    /** The tag of the basic type `long'.
     */
    jLONG,

    /** The tag of the basic type `float'.
     */
    jFLOAT,
    /** The tag of the basic type `int'.
     */
    jINT,
    /** The tag of the basic type `double'.
     */
    jDOUBLE,
    /** The tag of the basic type `boolean'.
     */
    jBOOLEAN,

    /** The tag of the type `void'.
     */
    jVOID,

    /** The tag of all class and interface types.
     */
    jCLASS,

    /** The tag of all array types.
     */
    jARRAY,

    /** The tag of all (monomorphic) method types.
     */
    jMETHOD,

    /** The tag of all package "types".
     */
    jPACKAGE,

    /** The tag of all (source-level) type variables.
     */
    jTYPEVAR,

    /** The tag of all type arguments.
     */
    jWILDCARD,

    /** The tag of all polymorphic (method-) types.
     */
    jFORALL,

    /** The tag of deferred expression types in method context
     */
    jDEFERRED,

    /** The tag of the bottom type {@code <null>}.
     */
    jBOT,

    /** The tag of a missing type.
     */
    jNONE,

    /** The tag of the error type.
     */
    jERROR,

    /** The tag of an unknown type
     */
    jUNKNOWN,

    /** The tag of all instantiatable type variables.
     */
    jUNDETVAR,

    /** Pseudo-types, these are special tags
     */
    jUNINITIALIZED_THIS,

    jUNINITIALIZED_OBJECT
};

void TypeHandler::init() {
    typeCache = new map<mUINT32, TY_IDX>();
}

TY_IDX TypeHandler::addPrimitiveType(Json::Value& jTypeTree, mUINT32 jIndex) {
    switch(jIndex) {
        case jBYTE:
            return MTYPE_To_TY(MTYPE_I1);
        case jCHAR:
            return MTYPE_To_TY(MTYPE_U2);
        case jSHORT:
            return MTYPE_To_TY(MTYPE_I2);
        case jLONG:
            return MTYPE_To_TY(MTYPE_I8);
        case jFLOAT:
            return MTYPE_To_TY(MTYPE_F4);
        case jINT:
            return MTYPE_To_TY(MTYPE_I4);
        case jDOUBLE:
            return MTYPE_To_TY(MTYPE_F8);
        case jBOOLEAN:
            FmtAssert(0, ("deal with this type later."));
            return 0;
        case jVOID:
            return MTYPE_To_TY(MTYPE_V);
        default:
            FmtAssert(0, ("Not primitive type."));
    }
}

TY_IDX TypeHandler::addClassType(Json::Value& jTypeTree, mUINT32 jIndex) {

}

TY_IDX TypeHandler::addType(Json::Value& jTypeTree, mUINT32 jIndex) {

    if(jIndex < 100) {
        return addPrimitiveType(jTypeTree, jIndex);
    }
    mUINT32 realIndex = jIndex - 100;
    FmtAssert(realIndex <= jTypeTree.size(), ("Type index out of type tree array."));
    FmtAssert(jTypeTree[realIndex].isMember("kind"), ("JSON type object don't have key: kind"));
    switch(jTypeTree[realIndex]["kind"].asUInt()) {
        case jCLASS:
            return addClassType(jTypeTree, jIndex);
        case jPACKAGE:
            return TY_IDX_ZERO;
        default:
            FmtAssert(0, ("Handle type later."));
            return TY_IDX_ZERO;
    }
}

TY_IDX TypeHandler::getType(mUINT32 jIndex) {

}