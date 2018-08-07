#ifndef JGEN_CODE_H
#define JGEN_CODE_H

#include "jgen_node.h"
#include "jgen_include.h"

#include <vector>

namespace JGEN {

enum JGenCodeTag {
    /** For methods that return an invalid tag if a given condition is not met
        */
    JGEN_NO_TAG,

    /** Toplevel nodes, of type TopLevel, representing entire source files.
    */
    JGEN_TOPLEVEL,

    /** Import clauses, of type Import.
        */
    JGEN_IMPORT,

    /** Class definitions, of type ClassDef.
        */
    JGEN_CLASSDEF,

    /** Method definitions, of type MethodDef.
        */
    JGEN_METHODDEF,

    /** Variable definitions, of type VarDef.
        */
    JGEN_VARDEF,

    /** The no-op statement ";", of type Skip
        */
    JGEN_SKIP,

    /** Blocks, of type Block.
        */
    JGEN_BLOCK,

    /** Do-while loops, of type DoLoop.
        */
    JGEN_DOLOOP,

    /** While-loops, of type WhileLoop.
        */
    JGEN_WHILELOOP,

    /** For-loops, of type ForLoop.
        */
    JGEN_FORLOOP,

    /** Foreach-loops, of type ForeachLoop.
        */
    JGEN_FOREACHLOOP,

    /** Labelled statements, of type Labelled.
        */
    JGEN_LABELLED,

    /** Switch statements, of type Switch.
        */
    JGEN_SWITCH,

    /** Case parts in switch statements, of type Case.
        */
    JGEN_CASE,

    /** Synchronized statements, of type Synchonized.
        */
    JGEN_SYNCHRONIZED,

    /** Try statements, of type Try.
        */
    JGEN_TRY,

    /** Catch clauses in try statements, of type Catch.
        */
    JGEN_CATCH,

    /** Conditional expressions, of type Conditional.
        */
    JGEN_CONDEXPR,

    /** Conditional statements, of type If.
        */
    JGEN_IF,

    /** Expression statements, of type Exec.
        */
    JGEN_EXEC,

    /** Break statements, of type Break.
        */
    JGEN_BREAK,

    /** Continue statements, of type Continue.
        */
    JGEN_CONTINUE,

    /** Return statements, of type Return.
        */
    JGEN_RETURN,

    /** Throw statements, of type Throw.
        */
    JGEN_THROW,

    /** Assert statements, of type Assert.
        */
    JGEN_ASSERT,

    /** Method invocation expressions, of type Apply.
        */
    JGEN_APPLY,

    /** Class instance creation expressions, of type NewClass.
        */
    JGEN_NEWCLASS,

    /** Array creation expressions, of type NewArray.
        */
    JGEN_NEWARRAY,

    /** Lambda expression, of type Lambda.
        */
    JGEN_LAMBDA,

    /** Parenthesized subexpressions, of type Parens.
        */
    JGEN_PARENS,

    /** Assignment expressions, of type Assign.
        */
    JGEN_ASSIGN,

    /** Type cast expressions, of type TypeCast.
        */
    JGEN_TYPECAST,

    /** Type test expressions, of type TypeTest.
        */
    JGEN_TYPETEST,

    /** Indexed array expressions, of type Indexed.
        */
    JGEN_INDEXED,

    /** Selections, of type Select.
        */
    JGEN_SELECT,

    /** Member references, of type Reference.
        */
    JGEN_REFERENCE,

    /** Simple identifiers, of type Ident.
        */
    JGEN_IDENT,

    /** Literals, of type Literal.
        */
    JGEN_LITERAL,

    /** Basic type identifiers, of type TypeIdent.
        */
    JGEN_TYPEIDENT,

    /** Array types, of type TypeArray.
        */
    JGEN_TYPEARRAY,

    /** Parameterized types, of type TypeApply.
        */
    JGEN_TYPEAPPLY,

    /** Union types, of type TypeUnion.
        */
    JGEN_TYPEUNION,

    /** Intersection types, of type TypeIntersection.
        */
    JGEN_TYPEINTERSECTION,

    /** Formal type parameters, of type TypeParameter.
        */
    JGEN_TYPEPARAMETER,

    /** Type argument.
        */
    JGEN_WILDCARD,

    /** Bound kind: extends, super, exact, or unbound
        */
    JGEN_TYPEBOUNDKIND,

    /** metadata: Annotation.
        */
    JGEN_ANNOTATION,

    /** metadata: Type annotation.
        */
    JGEN_TYPE_ANNOTATION,

    /** metadata: Modifiers
        */
    JGEN_MODIFIERS,

    /** An annotated type tree.
        */
    JGEN_ANNOTATED_TYPE,

    /** Error trees, of type Erroneous.
        */
    JGEN_ERRONEOUS,

    /** Unary operators, of type Unary.
        */
    JGEN_POS,                             // +
    JGEN_NEG,                             // -
    JGEN_NOT,                             // !
    JGEN_COMPL,                           // ~
    JGEN_PREINC,                          // ++ _
    JGEN_PREDEC,                          // -- _
    JGEN_POSTINC,                         // _ ++
    JGEN_POSTDEC,                         // _ --

    /** unary operator for null reference checks, only used internally.
        */
    JGEN_NULLCHK,

    /** Binary operators, of type Binary.
        */
    JGEN_OR,                              // ||
    JGEN_AND,                             // &&
    JGEN_BITOR,                           // |
    JGEN_BITXOR,                          // ^
    JGEN_BITAND,                          // &
    JGEN_EQ,                              // ==
    JGEN_NE,                              // !=
    JGEN_LT,                              // <
    JGEN_GT,                              // >
    JGEN_LE,                              // <=
    JGEN_GE,                              // >=
    JGEN_SL,                              // <<
    JGEN_SR,                              // >>
    JGEN_USR,                             // >>>
    JGEN_PLUS,                            // +
    JGEN_MINUS,                           // -
    JGEN_MUL,                             // *
    JGEN_DIV,                             // /
    JGEN_MOD,                             // %

    /** Assignment operators, of type Assignop.
        */
    JGEN_BITOR_ASG,                       // |=  BITOR
    JGEN_BITXOR_ASG,                      // ^=  BITXOR
    JGEN_BITAND_ASG,                      // &=  BITAND

    JGEN_SL_ASG,                          // <<=  SL
    JGEN_SR_ASG,                          // >>=  SR
    JGEN_USR_ASG,                         // >>>=  USR
    JGEN_PLUS_ASG,                        // +=  PLUS
    JGEN_MINUS_ASG,                       // -=  MINUS
    JGEN_MUL_ASG,                         // *=  MUL
    JGEN_DIV_ASG,                         // /=  DIV
    JGEN_MOD_ASG,                         // %=  MOD

    /** A synthetic let expression, of type LetExpr.
        */
    JGEN_LETEXPR                          // ala scheme
};

class JGenCodeNode: public JGenNode {
    public:
    explicit JGenCodeNode(Json_IR &_jsonIR, Json::Value &_node): JGenNode(_jsonIR, _node, JGenCodeCat) {
    }

    JGenCodeTag getTag() {
        FmtAssert(node.isMember("tag"), ("node don't have key: tag."));
        return (JGenCodeTag) node["tag"].asInt();
    }
};

typedef std::vector<JGenCodeNode *> CodeVector;
typedef std::vector<JGenCodeNode *>::iterator CodeVectorIter;

}
#endif