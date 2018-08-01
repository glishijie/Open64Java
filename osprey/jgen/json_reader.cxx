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

int JSON_READING_STATE = 0;

namespace JGEN {

    using namespace std;

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
        Json::Value val = root["code_table"];
        return val;
    }

    Json::Value& Json_IR::get_sym_tree() {
        return root["sym_table"];
    }

    Json::Value& Json_IR::get_type_tree() {
        return root["type_table"];
    }

    void Json_Typetree_Simple::init(Json::Value &tree) {
        // Take whatever needed form tree
        // Mark the length, and current cursot to zero.
        _tree = tree;
    };

    // Reading another node (next) (traverse)
    // @return whether there is one to be read(1), or none (0)
    int Json_Typetree_Simple::next() {
        // Move cursor
    };

    // read Kind from Node
    unsigned long long Json_Typetree_Simple::getKind() {

    };

    // read Name from Node
    std::string &Json_Typetree_Simple::getKindName() {

    };

    // read Kind from Node
    unsigned long long Json_Typetree_Simple::getFlag() {

    };

    // get DefId form Node
    int Json_Typetree_Simple::getJsonRefId() {

    };

    // Json_Typetree_Simple::get (Symbol) Name from Node
    std::string &Json_Typetree_Simple::getJsonName() {

    };

    // retrieve the previously bound Idx
    int Json_Typetree_Simple::getIdx() {

    };

    // bind Idx to the tree node
    void Json_Typetree_Simple::setTypeIdx(int idx) {

    };

    // bind Idx to the tree node
    Json_MemberFields &Json_Typetree_Simple::getMemberFields(int idx) {

    };


}
