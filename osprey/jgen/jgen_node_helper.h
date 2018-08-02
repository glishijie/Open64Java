#ifndef JGEN_NODE_HELPER
#define JGEN_NODE_HELPER`

#include "json/json.h"
#include "jgen_include.h"

#include <iostream>

class Json_IR;

namespace JGEN {

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

    enum JGenNodeCategory {
        JGenCodeCat,
        JGenSymbolCat,
        JGenTypeCat
    };

    class JGenNode {
        public:
        explicit JGenNode(Json_IR &_jsonIR, Json::Value &_node, JGenNodeCategory _cate):jsonIR(_jsonIR), node(_node), cate(_cate) {
        }

        JGenNodeCategory getCate() {
            return cate;
        }

        Json_IR &getJsonIR() {
            return jsonIR;
        }

        public:
        Json::Value &node;

        private:
        Json_IR &jsonIR;
        JGenNodeCategory cate;
    };

    class JGenTypeNode: public JGenNode {
        public:
        explicit JGenTypeNode(Json_IR &_jsonIR, Json::Value &_node): JGenNode(_jsonIR, _node, JGenTypeCat) {
        }

        const char *getName() {
            FmtAssert(node.isMember("name"), ("type node don't have key: name."));
            return node["name"].asCString();
        }

        mUINT32 getKind() {
            std::cout << node.toStyledString() << std::endl;
            FmtAssert(node.isMember("kind"), ("type node don't have key: kind."));
            return node["kind"].asUInt();
        }

        std::string getKindName() {
            FmtAssert(node.isMember("kindname"), ("type node don't have key: kindname."));
            return node["kindname"].asString();
        }

        bool isPrimitive() {
            return getKind() <= jVOID;
        }

        virtual mUINT32 getAlign() = 0;
        
    };

    class JGenPrimitiveTypeNode : public JGenTypeNode {
        public:
        explicit JGenPrimitiveTypeNode(Json_IR &_jsonIR, Json::Value &node): JGenTypeNode(_jsonIR, node) {
        }

        mUINT32 getAlign() {
            return 0;
        }
        
    };

    class JGenClassTypeNode : public JGenTypeNode {
        public:
        explicit JGenClassTypeNode(Json_IR &_jsonIR, Json::Value &node): JGenTypeNode(_jsonIR, node) {
        }

        bool isAnonymous() {
            return false;
        }

        mUINT32 getAlign() {
            return 4;
        }
    };

    class JGenNodeHelper {
        public:
        static void init(Json_IR *ir) {
            jsonIR = ir;
        }

        static JGenTypeNode *createTypeNode(Json::Value &node) {

            switch(node["kind"].asUInt()) {
                case jBYTE:
                case jCHAR:
                case jSHORT:
                case jLONG:
                case jFLOAT:
                case jINT:
                case jDOUBLE:
                case jBOOLEAN:
                case jVOID:
                    return new JGenPrimitiveTypeNode(*jsonIR, node);
                case jCLASS:
                    return new JGenClassTypeNode(*jsonIR, node);
                default:
                    FmtAssert(0, ("not handle other type."));
            }
        }

        static void deallocTypeNode(JGenNode *node) {
            delete node;
        }

        static Json_IR *getIR() {
            return jsonIR;
        }

        private:
        static Json_IR *jsonIR;
    };
}

#endif