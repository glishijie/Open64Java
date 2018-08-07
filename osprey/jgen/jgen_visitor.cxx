//
// Created by xc5 on 27/7/2018.
//

#include "jgen_visitor.h"
#include "jgen_decl.h"
#include "jgen_type.h"
#include "jgen_symbol.h"

#include <vector>

using namespace JGEN;
using namespace std;

// keep pu and pu_info
extern PU_Info *PU_Tree_Root;
static PU_Info *PU_Info_Table[258] = {0};

static ST *Return_Address_ST[258] = {0};
static BOOL map_mempool_initialized = FALSE;
static MEM_POOL Map_Mem_Pool;

// to manage the entry_wn, current function's entry_wn always on top
static std::vector<WN *> curr_entry_wn;
static void Push_Current_Entry_WN(WN *wn) { curr_entry_wn.push_back(wn); }
static void Pop_Current_Entry_WN() { curr_entry_wn.pop_back(); }
static WN *Current_Entry_WN(void) { return curr_entry_wn.back(); }

int JGEN_Visitor::visit(JGenCodeNode *node) {
    switch(node->getTag()) {
        case JGEN_TOPLEVEL:
            return visit_toplevel(static_cast<JGenTopLevelNode *>(node));
        case JGEN_CLASSDEF:
            return visit_classdef(static_cast<JGenClassDefNode *>(node));
        case JGEN_METHODDEF:
            return visit_methoddef(static_cast<JGenMethodDefNode *>(node));
        default:
            FmtAssert(0, ("not handle tag : %d.", node->getTag()));
    }
}

int JGEN_Visitor::visit_toplevel(JGenTopLevelNode *node) {
    vector<JGenCodeNode *> *clazzDefs = node->getClazzDefs();
    for(vector<JGenCodeNode *>::iterator I = clazzDefs->begin(); I != clazzDefs->end(); ++I) {
        visit(*I);
    }
    return 0;
}

int JGEN_Visitor::visit_classdef(JGenClassDefNode *node) {
    TypeHandler::getType(node->getType());
    CodeVector *methodDefs = node->getMethodDefs();
    for(CodeVectorIter I = methodDefs->begin(); I != methodDefs->end(); ++I) {
        visit(*I);
    }
    return 0;
}

int JGEN_Visitor::visit_methoddef(JGenMethodDefNode *node) {
    
    TypeHandler::getType(node->getType());
    #if 0
    /* set up the mem pool for the map table and predefined mappings */
    if (!map_mempool_initialized) {
        MEM_POOL_Initialize(&Map_Mem_Pool, "Map_Mem_Pool", FALSE);
        map_mempool_initialized = TRUE;
    } else {
        MEM_POOL_Pop(&Map_Mem_Pool);
    }
    MEM_POOL_Push(&Map_Mem_Pool);
    /* create the map table for the next PU */
    (void)WN_MAP_TAB_Create(&Map_Mem_Pool);
    New_Scope(CURRENT_SYMTAB + 1, Malloc_Mem_Pool, TRUE);
    // handle VLAs in the declaration
    // WN *vla_block = WN_CreateBlock();

    ST_EXPORT eclass = EXPORT_LOCAL;
    
    ST *func_st =  SymbolHandler::getSymbol(node->getSymbol());
    Set_ST_sclass(func_st, SCLASS_TEXT);
    Set_PU_lexical_level(Pu_Table[ST_pu(func_st)], CURRENT_SYMTAB);
    Set_PU_java_lang(Pu_Table[ST_pu(func_st)]);
    Set_ST_export(func_st, eclass);

    Scope_tab[Current_scope].st = func_st;

  WN *body, *wn;
  body = WN_CreateBlock();
  WN *entry_wn;
  entry_wn = WN_CreateEntry(0, func_st, body, NULL, NULL);

    PU_Info *pu_info;
      Set_PU_Info_tree_ptr(pu_info, entry_wn);
  PU_Info_maptab(pu_info) = Current_Map_Tab;
  PU_Info_proc_sym(pu_info) = ST_st_idx(func_st);
    Set_PU_Info_state(pu_info, WT_SYMTAB, Subsect_InMem);
  Set_PU_Info_state(pu_info, WT_TREE, Subsect_InMem);
  Set_PU_Info_state(pu_info, WT_PROC_SYM, Subsect_InMem);

  Set_PU_Info_flags(pu_info, PU_IS_COMPILER_GENERATED);

    if (PU_Info_Table[CURRENT_SYMTAB])
    PU_Info_next(PU_Info_Table[CURRENT_SYMTAB]) = pu_info;
  else if (CURRENT_SYMTAB == GLOBAL_SYMTAB + 1)
    PU_Tree_Root = pu_info;
  else
    PU_Info_child(PU_Info_Table[CURRENT_SYMTAB - 1]) = pu_info;

  PU_Info_Table[CURRENT_SYMTAB] = pu_info;

  Push_Current_Entry_WN(entry_wn);


    if (Current_Map_Tab) {
    WN_MAP_TAB_Delete(Current_Map_Tab);
    Current_Map_Tab = NULL;
  }

    Write_PU_Info(pu_info);

  PU_Info_Table[CURRENT_SYMTAB + 1] = NULL;

    // dump_tree(Current_Entry_WN());
  // Restore the previous entry wn, if any.
  Pop_Current_Entry_WN();

  Delete_Scope(CURRENT_SYMTAB);
  --CURRENT_SYMTAB;
  #endif
    return 0;
}