/**
 * Json AST Reading Base Class Declaration
 * Author: Jason Lu : lu.gt@163.com
 * Version : V0.1 
 * Usage:
 *  1. Instantiate the JGEN::Json_IR class,
 *  2. JsonIR::open (file_name)
 *  3. JsonIR::read ()
 ***/

#ifndef JSON_READER_H_
#define JSON_READER_H_

#include <json/json.h>
#include <vector>
#include "jgen_include.h"

extern int JSON_READING_STATE;

namespace JGEN {

class Json_IR {
private:
    Json::Reader reader;
    Json::Value root;
    std::vector<Json::Value *> *primitive_type_vector;

public:
    Json_IR();

    ~Json_IR() {
        delete primitive_type_vector;
    }

    int read(const char *json_file_path);

    Json::Value& get_code_tree();

    Json::Value& get_type_tree();

    Json::Value& get_sym_tree();

    Json::Value& get_type(mINT32 jIndex);

    Json::Value& get_symbol(mINT32 jIndex);
};

}


#endif
