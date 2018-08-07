/***
 * Json AST Reading Base Class Declaration
 * Author: Jason Lu : lu.gt@163.com
 * Version : V0.1 
 * Usage:
 *  1. Instantiate the JGEN::Json_IR class,
 *  2. JsonIR::open (file_name)
 *  3. JsonIR::read ()
 ***/
#include "json_reader.h"
#include <iostream>
#include <fstream>
#include <ios>
#include <algorithm>
#include <string>
#include "jgen_type.h"

int JSON_READING_STATE = 0;

using namespace std;
using namespace JGEN;
using namespace Json;

Json_IR::Json_IR() {
    primitive_type_vector = new std::vector<Value *>();
    // create primitive type
    Value *jgen_primitive_type;
    // JGEN_TYPE_BYTE
    jgen_primitive_type = new Value(objectValue);
    (*jgen_primitive_type)["name"] = "byte";
    (*jgen_primitive_type)["kind"] = (mINT32) JGEN_TYPE_BYTE;
    (*jgen_primitive_type)["kind_name"] = "BYTE";
    primitive_type_vector->push_back(jgen_primitive_type);
    // JGEN_TYPE_CHAR
    jgen_primitive_type = new Value(objectValue);
    (*jgen_primitive_type)["name"] = "char";
    (*jgen_primitive_type)["kind"] = (mINT32) JGEN_TYPE_CHAR;
    (*jgen_primitive_type)["kind_name"] = "CHAR";
    primitive_type_vector->push_back(jgen_primitive_type);
    // JGEN_TYPE_SHORT
    jgen_primitive_type = new Value(objectValue);
    (*jgen_primitive_type)["name"] = "short";
    (*jgen_primitive_type)["kind"] = (mINT32) JGEN_TYPE_SHORT;
    (*jgen_primitive_type)["kind_name"] = "SHORT";
    primitive_type_vector->push_back(jgen_primitive_type);
    // JGEN_TYPE_LONG
    jgen_primitive_type = new Value(objectValue);
    (*jgen_primitive_type)["name"] = "long";
    (*jgen_primitive_type)["kind"] = (mINT32) JGEN_TYPE_LONG;
    (*jgen_primitive_type)["kind_name"] = "LONG";
    primitive_type_vector->push_back(jgen_primitive_type);
    // JGEN_TYPE_FLOAT
    jgen_primitive_type = new Value(objectValue);
    (*jgen_primitive_type)["name"] = "float";
    (*jgen_primitive_type)["kind"] = (mINT32) JGEN_TYPE_FLOAT;
    (*jgen_primitive_type)["kind_name"] = "FLOAT";
    primitive_type_vector->push_back(jgen_primitive_type);
    // JGEN_TYPE_INT
    jgen_primitive_type = new Value(objectValue);
    (*jgen_primitive_type)["name"] = "int";
    (*jgen_primitive_type)["kind"] = (mINT32) JGEN_TYPE_INT;
    (*jgen_primitive_type)["kind_name"] = "INT";
    primitive_type_vector->push_back(jgen_primitive_type);
    // JGEN_TYPE_DOUBLE
    jgen_primitive_type = new Value(objectValue);
    (*jgen_primitive_type)["name"] = "double";
    (*jgen_primitive_type)["kind"] = (mINT32) JGEN_TYPE_DOUBLE;
    (*jgen_primitive_type)["kind_name"] = "DOUBLE";
    primitive_type_vector->push_back(jgen_primitive_type);
    // JGEN_TYPE_BOOLEAN
    jgen_primitive_type = new Value(objectValue);
    (*jgen_primitive_type)["name"] = "boolean";
    (*jgen_primitive_type)["kind"] = (mINT32) JGEN_TYPE_BOOLEAN;
    (*jgen_primitive_type)["kind_name"] = "BOOLEAN";
    primitive_type_vector->push_back(jgen_primitive_type);
    // JGEN_TYPE_VOID
    jgen_primitive_type = new Value(objectValue);
    (*jgen_primitive_type)["name"] = "void";
    (*jgen_primitive_type)["kind"] = (mINT32) JGEN_TYPE_VOID;
    (*jgen_primitive_type)["kind_name"] = "VOID";
    primitive_type_vector->push_back(jgen_primitive_type);
}

int Json_IR::read(const char *json_file_path) {
    std::ifstream ifs;
    ifs.open(json_file_path);
    if (!ifs.is_open()) {
        return -1;
    }

    if (!reader.parse(ifs, root, false)) {
        cerr << " JGEN::json_parse on file failed \n";
        return -2;
    }


    // root
    // 1. Get All Data Stored In
    Json::Value::Members roots = root.getMemberNames();
    if (roots.size() <= 0) {
        // Error
        cerr << " No Class Object Found in json" << endl;
        return -3;
    } else {
        cout << " selecting to read the class of " << roots[0] << endl;
    }

    if (root["code_table"].isNull()) {
        cerr << " Cannot find code_table in the class object" << endl;
        return -5;
    }

    if (root["sym_table"].isNull()) {
        cerr << " Cannot find symbol_table in the class object" << endl;
        return -6;
    }

    if (root["type_table"].isNull()) {
        cerr << " Cannot find type_table in the class object" << endl;
        return -7;
    }
    return 0;
}

Json::Value& Json_IR::get_code_tree() {
    Json::Value &val = root["code_table"];
    return val;
}

Json::Value& Json_IR::get_sym_tree() {
    return root["sym_table"];
}

Json::Value& Json_IR::get_type_tree() {
    return root["type_table"];
}

Value& Json_IR::get_type(mINT32 jIndex) {
    FmtAssert((jIndex < 100 && jIndex < primitive_type_vector->size()) || (jIndex >= 100 && (jIndex - 100) < root["type_table"].size()), 
        ("JIndex out of bounds, JIndex : %d, primitive type num : %d, type table size : %d", jIndex, primitive_type_vector->size(), root["type_table"].size()));
    if(jIndex < 100) {
        FmtAssert(jIndex < primitive_type_vector->size(), 
            ("jIndex out of primitive type vector bounds, jIndex : %d, primitive type vector size : %d", jIndex, primitive_type_vector->size()));
        return *(primitive_type_vector->at(jIndex));
    } else {
        jIndex -= 100;
        FmtAssert(jIndex < root["type_table"].size(),
            ("jIndex out of type table bounds, jIndex : %d, type table size : %d", jIndex, root["type_table"].size()));
        return root["type_table"][jIndex];
    }
}

Value& Json_IR::get_symbol(mINT32 jIndex) {
    FmtAssert(jIndex >= 100, ("now not contains default symbol."));
    FmtAssert(jIndex - 100 < root["sym_table"].size(), 
        ("jIndex out of symbol table bounds, jIndex : %d, symbol table size : %d", jIndex, root["sym_table"].size()));
    jIndex -= 100;
    return root["sym_table"][jIndex];
}
