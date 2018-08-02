#include "jgen_type.h"
#include "jgen_node_helper.h"

using namespace JGEN;
using namespace Json;
using namespace std;

map<Value *, TY_IDX> *TypeHandler::typeCache;

void TypeHandler::init() {
    typeCache = new map<Value *, TY_IDX>();
}

TY_IDX TypeHandler::addClassType(Value &type, TY_IDX idx) {

    JGenClassTypeNode *typeNode = (JGenClassTypeNode *) JGenNodeHelper::createTypeNode(type);
    TY &ty = (idx == TY_IDX_ZERO) ? New_TY(idx) : Ty_Table[idx];
    int tsize = 4;

    TY_Init(ty, tsize, KIND_STRUCT, MTYPE_M,
            Save_Str(typeNode->getName()));

    if (typeNode->isAnonymous())
      Set_TY_anonymous(ty);

    int align = typeNode->getAlign();
    if (align == 0)
      align = 1; // in case incomplete type
    Set_TY_align(idx, align);
    // set idx now in case recurse thru fields
    // typeCache[&type] = idx;
    typeCache->insert(std::make_pair<Value *, TY_IDX>(&type, idx));

    // TODO: visit extends class
    // Do_Base_Types(type_tree);

    // Process nested structs and static data members first

    for (gs_t field = get_first_real_or_virtual_field(type_tree); field;
         field = next_real_field(type_tree, field)) {
      Set_TY_content_seen(idx); // bug 10851
      if (gs_tree_code(field) == GS_TYPE_DECL ||
          gs_tree_code(field) == GS_FIELD_DECL) {
        gs_t field_type = gs_tree_type(field);
        if ((gs_tree_code(field_type) == GS_RECORD_TYPE ||
             gs_tree_code(field_type) == GS_UNION_TYPE) &&
            field_type != type_tree) {
#ifdef KEY
          // Defer typedefs within class
          // declarations to avoid circular
          // declaration dependences.  See
          // example in bug 5134.
          if (gs_tree_code(field) == GS_TYPE_DECL)
            defer_decl(field_type);
          else
#endif
            Get_TY(field_type);
        }
      }
#ifdef KEY // Defer expansion of static vars until all the fields in
           // _every_ struct are laid out.  Consider this code (see
           // bug 3044):
           //  struct A
           //    struct B *p
           //  struct B
           //    static struct A *q = ...	// static data member with
           //                              // initializer
           // We cannot expand static member vars while expanding the
           // enclosing stuct, for the following reason:  Expansion of
           // struct A leads to expansion of p, which leads to the
           // expansion of struct B, which leads to the expansion of q and
           // q's initializer.  The code that expands the initializer goes
           // through the fields of struct A, but these fields are not yet
           // completely defined, and this will cause kg++fe to die.
           //
           // The solution is the delay all static var expansions until
           // the very end.
      else if (gs_tree_code(field) == GS_VAR_DECL)
        defer_decl(field);
#else
      else if (gs_tree_code(field) == GS_VAR_DECL)
        WGEN_Expand_Decl(field, TRUE);
#endif
      else if (gs_tree_code(field) == GS_TEMPLATE_DECL)
        WGEN_Expand_Decl(field, TRUE);
    }

    Set_TY_fld(ty, FLD_HANDLE());
    FLD_IDX first_field_idx = Fld_Table.Size();
    gs_t field;
    gs_t method = gs_type_methods(type_tree);
    FLD_HANDLE fld;
    INT32 next_field_id = 1;

#ifdef KEY
    // In GCC 4, the same tree node representing a vtable ptr field
    // can appear in different derived classes.  As a result,
    // DECL_FIELD_ID(field) can't be used to map its field ID.  As
    // a fix, always allocate field ID 1 to the vtable ptr field.
    // Do this before allocating IDs to any other field.
    gs_t vfield = get_virtual_field(type_tree);
    if (vfield) {
      Is_True(gs_tree_code(vfield) == GS_FIELD_DECL,
              ("Create_TY_For_Tree: bad vfield code"));
      // czw
      if (lang_cplus)
        Is_True(gs_decl_name(vfield) &&
                    !strncmp(Get_Name(gs_decl_name(vfield)), "_vptr", 5),
                ("Create_TY_For_Tree: bad vfield name"));
      if (lang_java)
        Is_True(gs_decl_name(vfield) &&
                    !strncmp(Get_Name(gs_decl_name(vfield)), "vtable", 6),
                ("Create_TY_For_Tree: bad vfield name"));
      // The vfield field ID is either not set, or was set to 1.
      Is_True(DECL_FIELD_ID(vfield) <= 1,
              ("Create_TY_For_Tree: invalid vfield field ID"));

      DECL_FIELD_ID(vfield) = next_field_id; // must be 1
      next_field_id += TYPE_FIELD_IDS_USED(gs_tree_type(vfield)) + 1;
      fld = New_FLD();
      FLD_Init(fld, Save_Str(Get_Name(gs_decl_name(vfield))),
               0, // type
               gs_get_integer_value(gs_decl_field_offset(vfield)) +
                   gs_get_integer_value(gs_decl_field_bit_offset(vfield)) /
                       BITSPERBYTE);
    }
#endif

    // Generate an anonymous field for every direct, nonempty,
    // nonvirtual base class.

    INT32 offset = 0;
    INT32 anonymous_fields = 0;
#ifndef KEY // g++'s class.c already laid out the base types.  Bug 11622.
    gs_t type_binfo, basetypes;
    if ((type_binfo = gs_type_binfo(type_tree)) != NULL &&
        (basetypes = gs_binfo_base_binfos(type_binfo)) != NULL) {
      gs_t list;
      for (list = basetypes; gs_code(list) != EMPTY;
           list = gs_operand(list, 1)) {
        gs_t binfo = gs_operand(list, 0);
        gs_t basetype = gs_binfo_type(binfo);
        offset = Roundup(offset, gs_type_align(basetype) / BITSPERBYTE);
        if (!is_empty_base_class(basetype) || !gs_binfo_virtual_p(binfo)) {
          ++next_field_id;
          ++anonymous_fields;
          next_field_id += TYPE_FIELD_IDS_USED(basetype);
          fld = New_FLD();
          FLD_Init(fld, Save_Str(Get_Name(0)), Get_TY(basetype), offset);
          offset += Type_Size_Without_Vbases(basetype);
          Set_FLD_is_anonymous(fld);
#ifdef KEY
          // temporary hack for a bug in gcc
          // Details: From layout_class_type(), it turns out that for this
          // type, gcc is apparently sending wrong type info, they have 2 fields
          // each 8 bytes in a 'record', with the type size == 8 bytes also!
          // So we take care of it here...
          if (offset > tsize) {
            tsize = offset;
            Set_TY_size(ty, tsize);
          }
#endif // KEY
        }
      }
    }
#endif // KEY

    // Assign IDs to real fields.  The vtable ptr field is already
    // assigned ID 1.
    for (field = get_first_real_field(type_tree); field;
         field = next_real_field(type_tree, field)) {
      if (gs_tree_code(field) == GS_TYPE_DECL) {
        continue;
      }
      if (gs_tree_code(field) == GS_CONST_DECL) {
        DevWarn("got CONST_DECL in field list");
        continue;
      }
      if (gs_tree_code(field) == GS_VAR_DECL) {
        continue;
      }
      if (gs_tree_code(field) == GS_TEMPLATE_DECL) {
        continue;
      }

      // Either the DECL_FIELD_ID is not yet set, or is
      // already set to the same field ID.  The latter
      // happens when GCC 4 duplicates the type tree and the
      // same field node appears in both type nodes.
      Is_True(DECL_FIELD_ID(field) == 0 ||
                  DECL_FIELD_ID(field) == next_field_id,
              ("Create_TY_For_Tree: field ID already set"));

      DECL_FIELD_ID(field) = next_field_id;
      next_field_id += TYPE_FIELD_IDS_USED(gs_tree_type(field)) + 1;
      fld = New_FLD();
      FLD_Init(fld, Save_Str(Get_Name(gs_decl_name(field))),
               0, // type
                  /*gs_decl_field_offset(field) == NULL ? 0 : */
                   gs_get_integer_value(gs_decl_field_offset(field)) +
                   /*gs_decl_field_bit_offset(field) == NULL ? 0 : */ (
                       gs_get_integer_value(gs_decl_field_bit_offset(field)) /
                       BITSPERBYTE)); // czw
      if (gs_decl_name(field) == NULL)
        Set_FLD_is_anonymous(fld);
    }

    TYPE_FIELD_IDS_USED(type_tree) = next_field_id - 1;
    FLD_IDX last_field_idx = Fld_Table.Size() - 1;
    if (last_field_idx >= first_field_idx) {
      Set_TY_fld(ty, FLD_HANDLE(first_field_idx));
      Set_FLD_last_field(FLD_HANDLE(last_field_idx));
    }

    // now set the fld types.
    fld = TY_fld(ty);
#ifdef KEY
    // Handle the vtable ptr field if it exists.
    if (vfield) {
      Is_True(gs_tree_code(gs_tree_type(vfield)) == GS_POINTER_TYPE,
              ("Create_TY_For_Tree: vtable ptr should be GS_POINTER_TYPE"));

      // As mentioned below, don't expand pointer-type fields to
      // avoid circular dependences.  Defer expanding the field
      // type.
      fld = TY_fld(ty);
      TY_IDX p_idx = Make_Pointer_Type(MTYPE_To_TY(MTYPE_U8), FALSE);
      Set_FLD_type(fld, p_idx);
      defer_field(vfield, fld);
      fld = FLD_next(fld);
    }
#endif
    // first skip the anonymous fields, whose types are already
    // set.
    while (anonymous_fields--)
      fld = FLD_next(fld);

    for (field = get_first_real_field(type_tree);
         /* ugly hack follows; traversing the fields isn't
            the same from run-to-run. fwa? */
         field && fld.Entry(); field = next_real_field(type_tree, field)) {
#ifdef KEY
      const int FLD_BIT_FIELD_SIZE = 64;
#endif
      if (gs_tree_code(field) == GS_TYPE_DECL)
        continue;
      if (gs_tree_code(field) == GS_CONST_DECL)
        continue;
      if (gs_tree_code(field) == GS_VAR_DECL)
        continue;
      if (gs_tree_code(field) == GS_TEMPLATE_DECL)
        continue;
#ifdef KEY
      // Don't expand the field's type if it's a pointer
      // type, in order to avoid circular dependences
      // involving member object types and base types.  See
      // example in bug 4954.
      if (gs_tree_code(gs_tree_type(field)) == GS_POINTER_TYPE) {
        // Defer expanding the field's type.  Put in a
        // generic pointer type for now.
        TY_IDX p_idx = Make_Pointer_Type(MTYPE_To_TY(MTYPE_U8), FALSE);
        Set_FLD_type(fld, p_idx);
        defer_field(field, fld);
        fld = FLD_next(fld);
        continue;
      }
#endif
      TY_IDX fty_idx = Get_TY(gs_tree_type(field));

      if ((TY_align(fty_idx) > align) || (TY_is_packed(fty_idx)))
        Set_TY_is_packed(ty);
#if 1 // wgen bug 10470
      if (!gs_tree_this_volatile(field))
        Clear_TY_is_volatile(fty_idx);
#endif
      Set_FLD_type(fld, fty_idx);

      if (!gs_decl_bit_field(field)
#if 1 // wgen bug 10901
          && gs_tree_code(gs_tree_type(field)) != GS_RECORD_TYPE &&
          gs_tree_code(gs_tree_type(field)) != GS_UNION_TYPE
#endif
          && gs_decl_size(field) // bug 10305
          && gs_get_integer_value(gs_decl_size(field)) > 0
#ifdef KEY
          // We don't handle bit-fields > 64 bits. For an INT field of 128 bits,
          // we make it 64 bits. But then don't set it as FLD_IS_BIT_FIELD.
          && gs_get_integer_value(gs_decl_size(field)) <= FLD_BIT_FIELD_SIZE
          // bug 2401
          && TY_size(Get_TY(gs_tree_type(field))) != 0
#endif
          && gs_get_integer_value(gs_decl_size(field)) !=
                 (TY_size(Get_TY(gs_tree_type(field))) * BITSPERBYTE)) {
#ifdef KEY
        FmtAssert(gs_get_integer_value(gs_decl_size(field)) <=
                      FLD_BIT_FIELD_SIZE,
                  ("field size too big"));
#endif
        // for some reason gnu doesn't set bit field
        // when have bit-field of standard size
        // (e.g. int f: 16;).  But we need it set
        // so we know how to pack it, because
        // otherwise the field type is wrong.
        DevWarn("field size %lld doesn't match type size %lld",
                gs_get_integer_value(gs_decl_size(field)),
                TY_size(Get_TY(gs_tree_type(field))) * BITSPERBYTE);
        gs_set_decl_bit_field(field, 1);
      }
      if (gs_decl_bit_field(field)) {
        Set_FLD_is_bit_field(fld);
        // bofst is remaining bits from byte offset
        Set_FLD_bofst(fld,
                      gs_get_integer_value(gs_decl_field_bit_offset(field)) %
                          BITSPERBYTE);
        Set_FLD_bsize(fld, gs_get_integer_value(gs_decl_size(field)));
      }
      fld = FLD_next(fld);
    }

#ifndef KEY // Don't expand methods by going through TYPE_METHODS,
            // because:
            //   1) It is incorrect to translate all methods in
            //      TYPE_METHODS to WHIRL because some of the methods are
            //      never used, and generating the assembly code for them
            //      might lead to undefined symbol references.  Instead,
            //      consult the gxx_emitted_decls list, which has all the
            //      functions (including methods) that g++ has ever emitted
            //      to assembly.
            //   2) Expanding the methods here will cause error when the
            //      methods are for a class B that appears as a field in an
            //      enclosing class A.  When Get_TY is run for A, it will
            //      call Get_TY for B in order to calculate A's field ID's.
            //      (Need Get_TY to find B's TYPE_FIELD_IDS_USED.)  If
            //      Get_TY uses the code below to expand B's methods, it
            //      will lead to error because the expansion requires the
            //      field ID's of the enclosing record (A), and these field
            //      ID's are not yet defined.

    // process methods
    if (!Enable_WGEN_DFE) {
      if (cp_type_quals(type_tree) == TYPE_UNQUALIFIED) {
        while (method != NULL_TREE) {
          WGEN_Expand_Decl(method, TRUE);
          method = TREE_CHAIN(method);
        }
      }
    }
#endif // KEY

  return idx;
}

TY_IDX TypeHandler::addType(Value &type) {

    FmtAssert(type.isMember("kind"), ("Type node don't have key: kind."));
    if(typeCache->find(&type) != typeCache->end()) {
      return typeCache->find(&type)->second;
    }
    switch(type["kind"].asUInt()) {
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
        case jCLASS:
            return addClassType(type, TY_IDX_ZERO);
        case jPACKAGE:
            return TY_IDX_ZERO;
        default:
            FmtAssert(0, ("Handle type later."));
            return TY_IDX_ZERO;
    }
}

TY_IDX TypeHandler::getType(Value &type) {

}