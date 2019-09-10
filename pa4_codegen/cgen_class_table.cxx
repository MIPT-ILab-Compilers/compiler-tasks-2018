
///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//

void StringEntry::code_ref(ostream& s) {
    INFO_IN_AS_EMIT;
    s << STRCONST_PREFIX << index;
    INFO_OUT_AS_EMIT;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream& s, int stringclasstag) {
    INFO_IN_AS;
    IntEntryP lensym = inttable.add_int(len);

    // Add -1 eye catcher
    s << WORD << "-1" << endl;

    code_ref(s);
    s << LABEL // label
            << WORD << stringclasstag << endl // tag
            << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len + 4) / 4) << endl // size
            << WORD;


    /***** Add dispatch information for class String ******/
    s << "String" << DISPTAB_SUFFIX;
    s << endl; // dispatch table
    s << WORD;
    lensym->code_ref(s);
    s << endl; // string length
    emit_string_constant(s, str); // ascii string
    s << ALIGN; // align to word
    INFO_OUT_AS;
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//

void StrTable::code_string_table(ostream& s, int stringclasstag) {
    INFO_IN_AS_EMIT;
    for (List<StringEntry> *l = tbl; l; l = l->tl()) {
        l->hd()->code_def(s, stringclasstag);
    }
    INFO_OUT_AS_EMIT;
}

//
// Ints
//

void IntEntry::code_ref(ostream &s) {
    INFO_IN_AS_EMIT;
    s << INTCONST_PREFIX << index;
    INFO_OUT_AS_EMIT;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag) {
    INFO_IN_AS;
    // Add -1 eye catcher
    s << WORD << "-1" << endl;

    code_ref(s);
    s << LABEL // label
            << WORD << intclasstag << endl // class tag
            << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl // object size
            << WORD;

    /***** Add dispatch information for class Int ******/
    s << "Int" << DISPTAB_SUFFIX;
    s << endl; // dispatch table
    s << WORD << str << endl; // integer value
    INFO_OUT_AS;
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//

void IntTable::code_string_table(ostream &s, int intclasstag) {
    INFO_IN_AS;
    for (List<IntEntry> *l = tbl; l; l = l->tl())
        l->hd()->code_def(s, intclasstag);
    INFO_OUT_AS;
}

//
// Bools
//

BoolConst::BoolConst(int i) : val(i) {
    assert(i == 0 || i == 1);
}

void BoolConst::code_ref(ostream& s) const {
    INFO_IN_AS_EMIT;
    s << BOOLCONST_PREFIX << val;
    INFO_OUT_AS_EMIT;
}

//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream& s, int boolclasstag) {
    INFO_IN_AS;
    // Add -1 eye catcher
    s << WORD << "-1" << endl;

    code_ref(s);
    s << LABEL // label
            << WORD << boolclasstag << endl // class tag
            << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl // object size
            << WORD;

    /***** Add dispatch information for class Bool ******/
    s << "Bool" << DISPTAB_SUFFIX;
    s << endl; // dispatch table
    s << WORD << val << endl; // value (0 or 1)
    INFO_OUT_AS;
}



//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data() {
    INFO_IN;
    s << "# coding global data" << endl;
    Symbol main = idtable.lookup_string(MAINNAME);
    Symbol string = idtable.lookup_string(STRINGNAME);
    Symbol integer = idtable.lookup_string(INTNAME);
    Symbol boolc = idtable.lookup_string(BOOLNAME);

    s << "\t.data\n" << ALIGN;
    //
    // The following global names must be defined first.
    //
    s << GLOBAL << CLASSNAMETAB << endl;
    s << GLOBAL;
    emit_protobj_ref(main, s);
    s << endl;
    s << GLOBAL;
    emit_protobj_ref(integer, s);
    s << endl;
    s << GLOBAL;
    emit_protobj_ref(string, s);
    s << endl;
    s << GLOBAL;
    falsebool.code_ref(s);
    s << endl;
    s << GLOBAL;
    truebool.code_ref(s);
    s << endl;
    s << GLOBAL << INTTAG << endl;
    s << GLOBAL << BOOLTAG << endl;
    s << GLOBAL << STRINGTAG << endl;

    //
    // We also need to know the tag of the Int, String, and Bool classes
    // during code generation.
    //
    assert(intclasstag != -1);
    s << INTTAG << LABEL
        << WORD << intclasstag << endl;
    assert(boolclasstag != -1);
    s << BOOLTAG << LABEL
        << WORD << boolclasstag << endl;
    assert(stringclasstag != -1);
    s << STRINGTAG << LABEL
        << WORD << stringclasstag << endl;
    INFO_OUT;
}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text() {
    INFO_IN;
    s << "# coding global text" << endl;
    s << GLOBAL << HEAP_START << endl
        << HEAP_START << LABEL
        << WORD << 0 << endl
        << "\t.text" << endl
        << GLOBAL;
    emit_init_ref(idtable.add_string("Main"), s);
    s << endl << GLOBAL;
    emit_init_ref(idtable.add_string("Int"), s);
    s << endl << GLOBAL;
    emit_init_ref(idtable.add_string("String"), s);
    s << endl << GLOBAL;
    emit_init_ref(idtable.add_string("Bool"), s);
    s << endl << GLOBAL;
    emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), s);
    s << endl;
    INFO_OUT;
}

void CgenClassTable::code_initCode() {
    s << "  # Initialization code \n";
    for (List<CgenNode> *l = nds; l; l = l->tl())
    {
        CgenNode* node = l->hd();
        curr_node = node;
        init_alloc_temp(); 
        int tmpCurr = node->calc_temp();
        node->code_ref(s);
        s << CLASSINIT_SUFFIX << LABEL;
        node->emit_init(s, tmpCurr);
    };
}

void CgenClassTable::code_bools(int boolclasstag) {
    INFO_IN;
    falsebool.code_def(s, boolclasstag);
    truebool.code_def(s, boolclasstag);
    INFO_OUT;
}

void CgenClassTable::code_select_gc() {
    INFO_IN;
    //
    // Generate GC choice constants (pointers to GC functions)
    //
    s << "# choosing GC" << endl;
    s << GLOBAL << "_MemMgr_INITIALIZER" << endl;
    s << "_MemMgr_INITIALIZER:" << endl;
    s << WORD << gc_init_names[cgen_Memmgr] << endl;
    s << GLOBAL << "_MemMgr_COLLECTOR" << endl;
    s << "_MemMgr_COLLECTOR:" << endl;
    s << WORD << gc_collect_names[cgen_Memmgr] << endl;
    s << GLOBAL << "_MemMgr_TEST" << endl;
    s << "_MemMgr_TEST:" << endl;
    s << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
    INFO_OUT;
}


//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

void CgenClassTable::code_constants() {
    INFO_IN_AS;
    //
    // Add constants that are required by the code generator.
    //
    //str << "# coding constants" << endl;
    stringtable.add_string("");
    inttable.add_string("0");

    stringtable.code_string_table(s, stringclasstag);
    inttable.code_string_table(s, intclasstag);
    code_bools(boolclasstag);
    INFO_OUT_AS;
}

void CgenClassTable::code_classNameTab() {
    INFO_IN_AS;
    //str << "# coding class name Table. \n";
    s << CLASSNAMETAB << LABEL;    
    for (int i = 0; i < currclasstag; ++i)
    {
        // Looking for i id-s
        // std::cout << " looking for " << i << " :";
        List<CgenNode> *l = nds;
        for (;  l && (l->hd()->get_id() != i);
                l = l->tl())
        {
        }
        // We must find this node.
        assert(l);
        // std::cout << " found " << l->hd()->get_name()->get_string() << " \n";
        // Looking for string
        StringEntry* se = stringtable.lookup_string(l->hd()->get_name()->get_string());
        s << WORD; 
        se->code_ref(s);
        s << "\n";        
    }
    INFO_OUT_AS;
}

void CgenClassTable::code_classObjTab() {
    INFO_IN_AS;
    //str << "# coding class object Table. \n";
    s << CLASSOBJTAB << LABEL;
    for(int i = 0; i < currclasstag; ++i)
    {
        for (List<CgenNode> *l = nds; l; l = l->tl()) {
            CgenNode* tmp = l->hd();
            if (tmp->get_id() == i) {
                s << WORD;
                tmp->code_ref(s);
                s << PROTOBJ_SUFFIX << "\n";

                s << WORD;
                tmp->code_ref(s);
                s << CLASSINIT_SUFFIX << "\n";

            }
        }
    }
    INFO_OUT_AS;
}


void CgenClassTable::code_protObjs() {
    INFO_IN_AS;
    //str << "# coding prototype Objects. \n";
    for (List<CgenNode> *l = nds; l; l = l->tl())
    {
        CgenNode* tmp = l->hd();
        tmp->code_prot(s);
    };
    INFO_OUT_AS;
}

void CgenClassTable::code_dispTabs() {
    INFO_IN_AS;
    //str << "# coding disp Tables. \n";
    for (List<CgenNode> *l = nds; l; l = l->tl())
    {
        CgenNode* tmp = l->hd();
        tmp->code_ref(s);
        s << DISPTAB_SUFFIX << LABEL;
        tmp->code_disp(s);
    };
    INFO_OUT_AS;
}

void CgenClassTable::code_methods() {
    INFO_IN_AS
    for (List<CgenNode> *l = nds; l; l = l->tl())
    {
        CgenNode* tmp = l->hd();
        curr_node = tmp;
        if (tmp->basic())
        {
            continue;
        }

        for(int i = tmp->features->first();
                tmp->features->more(i);
                i = tmp->features->next(i))
        {
            Feature_class * f = tmp->features->nth(i);
            method_class * m = dynamic_cast<method_class*>(f);
            if (m)
            {
#ifdef DEBUG_TEMPS
                debug_temps = true;
#endif
                
#ifdef DEBUG_AS_NAMES
                function_name = m->get_name()->get_string();
#endif
                int header_size = m->calc_temp() ;
                tmp->code_ref(s);
                s << METHOD_SEP;
                s << m->name << LABEL;

                emit_push_header(s, header_size);

                m->cgen(s,
                        tmp->get_name(),
                        tmp->get_attr_table(),
                        tmp->get_method_table());

                emit_pop_header(s,  header_size + m->formal_size());

                emit_return(s);
            }
        }
    }
    INFO_OUT_AS
}

CgenClassTable::CgenClassTable(Classes classes, ostream& str) : nds(NULL), s(str) {
    INFO_IN;
    enterscope();
    if (cgen_debug) cout << "Building CgenClassTable" << endl;
    install_basic_classes();
    install_classes(classes);
    build_inheritance_tree();

//    exitscope();
    INFO_OUT;
}

void CgenClassTable::install_basic_classes() {
    INFO_IN;
    // The tree package uses these globals to annotate the classes built below.
    //curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    //
    // A few special class names are installed in the lookup table but not
    // the class list.  Thus, these classes exist, but are not part of the
    // inheritance hierarchy.
    // No_class serves as the parent of Object and the other special classes.
    // SELF_TYPE is the self class; it cannot be redefined or inherited.
    // prim_slot is a class known to the code generator.
    //
    addid(No_class,
          new CgenNode(class_(No_class, No_class, nil_Features(), filename),
                       Basic, this));
    addid(SELF_TYPE,
          new CgenNode(class_(SELF_TYPE, No_class, nil_Features(), filename),
                       Basic, this));
    addid(prim_slot,
          new CgenNode(class_(prim_slot, No_class, nil_Features(), filename),
                       Basic, this));

    // 
    // The Object class has no parent class. Its methods are
    //        cool_abort() : Object    aborts the program
    //        type_name() : Str        returns a string representation of class name
    //        copy() : SELF_TYPE       returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.
    //
    install_class(
                  new CgenNode(
                               class_(Object,
                                      No_class,
                                      append_Features(
                                                      append_Features(
                                                                      single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                                                                      single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
                                                      single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
                                      filename),
                               Basic, this));

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE          writes a string to the output
    //        out_int(Int) : SELF_TYPE               "    an int    "  "     "
    //        in_string() : Str                    reads a string from the input
    //        in_int() : Int                         "   an int     "  "     "
    //
    install_class(
                  new CgenNode(
                               class_(IO,
                                      Object,
                                      append_Features(
                                                      append_Features(
                                                                      append_Features(
                                                                                      single_Features(method(out_string, single_Formals(formal(arg, Str)),
                                                                                                             SELF_TYPE, no_expr())),
                                                                                      single_Features(method(out_int, single_Formals(formal(arg, Int)),
                                                                                                             SELF_TYPE, no_expr()))),
                                                                      single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
                                                      single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
                                      filename),
                               Basic, this));

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    install_class(
                  new CgenNode(
                               class_(Int,
                                      Object,
                                      single_Features(attr(val, prim_slot, no_expr())),
                                      filename),
                               Basic, this));

    //
    // Bool also has only the "val" slot.
    //
    install_class(
                  new CgenNode(
                               class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())), filename),
                               Basic, this));

    //
    // The class Str has a number of slots and operations:
    //       val                                  ???
    //       str_field                            the string itself
    //       length() : Int                       length of the string
    //       concat(arg: Str) : Str               string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring
    //       
    install_class(
                  new CgenNode(
                               class_(Str,
                                      Object,
                                      append_Features(
                                                      append_Features(
                                                                      append_Features(
                                                                                      append_Features(
                                                                                                      single_Features(attr(val, Int, no_expr())),
                                                                                                      single_Features(attr(str_field, prim_slot, no_expr()))),
                                                                                      single_Features(method(length, nil_Formals(), Int, no_expr()))),
                                                                      single_Features(method(concat,
                                                                                             single_Formals(formal(arg, Str)),
                                                                                             Str,
                                                                                             no_expr()))),
                                                      single_Features(method(substr,
                                                                             append_Formals(single_Formals(formal(arg, Int)),
                                                                                            single_Formals(formal(arg2, Int))),
                                                                             Str,
                                                                             no_expr()))),
                                      filename),
                               Basic, this));

    INFO_OUT;
}

// CgenClassTable::install_class
// CgenClassTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//

void CgenClassTable::install_class(CgenNodeP nd) {
    INFO_IN;
    Symbol name = nd->get_name();

    CgenNodeP cl= probe(name);
    if (cl) {
        nd->set_id(cl->get_id());
        INFO_OUT;
        return;
    }
    
//    std::cout << " For " << nd->get_name()->get_string() << " \n";
//    std::cout << " geted id : " <<  currclasstag;
    if (nd->get_name() == Int)
    {
        intclasstag = currclasstag;
    } else if (nd->get_name() == Bool)
    {
        boolclasstag = currclasstag;
    } else if (nd->get_name() == Str)
    {
        stringclasstag = currclasstag;
    }
    nd->set_id(currclasstag);
    currclasstag++;
    
    // The class name is legal, so add it to the list of classes
    // and the symbol table.
    nds = new List<CgenNode>(nd, nds);
    addid(name, nd);
    INFO_OUT;
}

void CgenClassTable::install_classes(Classes cs) {
    INFO_IN;
    for (int i = cs->first(); cs->more(i); i = cs->next(i))
        install_class(new CgenNode(cs->nth(i), NotBasic, this));
    INFO_OUT;
}

//
// CgenClassTable::build_inheritance_tree
//

void CgenClassTable::build_inheritance_tree() {
    INFO_IN;
    for (List<CgenNode> *l = nds; l; l = l->tl())
        set_relations(l->hd());

    for(List<CgenNode> *l = nds; l; l = l->tl())
    {
        l->hd()->fill_table();
    }
    //
    INFO_OUT;
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//

void CgenClassTable::set_relations(CgenNodeP nd) {
    INFO_IN;
    CgenNode *parent_node = probe(nd->get_parent());
    nd->set_parentnd(parent_node);
    parent_node->add_child(nd);
    INFO_OUT;
}

void CgenClassTable::code() {
    INFO_IN;
    code_global_data();
    code_select_gc();
    code_constants();
    
    code_classNameTab();
    code_classObjTab();
    code_dispTabs();
    code_protObjs();

    code_global_text();

    //                 Add your code to emit
    //                   - object initializer
    //                   - the class methods
    //                   - etc...

    code_initCode();
    code_methods();
    INFO_OUT;
}

CgenNodeP CgenClassTable::root() {
    INFO_IN;
    return probe(Object);
//    INFO_OUT;
}


///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP ct) :
    class__class((const class__class &) *nd),
    parentnd(NULL),
    children(NULL),
    basic_status(bstatus) {
    INFO_IN;
    stringtable.add_string(name->get_string()); // Add class name to string table
    INFO_OUT;
}

void CgenNode::add_child(CgenNodeP n) {
    INFO_IN;
    children = new List<CgenNode>(n, children);
    INFO_OUT;
}

void CgenNode::set_parentnd(CgenNodeP p) {
    INFO_IN;
    assert(parentnd == NULL);
    assert(p != NULL);
    parentnd = p;
    INFO_OUT;
}

void CgenNode::code_disp(ostream& s)
{
    INFO_IN_AS;
    for(unsigned i = 0; i < methodTable->size(); ++i)
    {
        s << WORD << methodTable->at(i).self << METHOD_SEP << 
            methodTable->at(i).name << "\n";
    }
    INFO_OUT_AS;
};

void CgenNode::code_ref(ostream& s)
{
    s << this->get_name();
}

int CgenNode::get_attr_num()
{
    INFO_IN;
    int attrnum = 0;
    if (parentnd != NULL && get_name() != Object)
        attrnum = parentnd->get_attr_num();

    for(int i = features->first();
            features->more(i);
            i = features->next(i))
    {
        Feature_class * f = features->nth(i);
        attr_class * a = dynamic_cast<attr_class*>(f);
        if (a) attrnum++;
    }
    INFO_OUT;
    return attrnum;
};

void CgenNode::code_attr_prot(ostream& s)
{
    INFO_IN_AS;
    if (parentnd != NULL && get_name() != Object)
        parentnd->code_attr_prot(s);

    for(int i = features->first();
            features->more(i);
            i = features->next(i))
    {
        Feature_class * f = features->nth(i);
        attr_class * a = dynamic_cast<attr_class*>(f);
        if (a)
        {
            s << WORD;
            // here need get default value for all classes
            if (a->type_decl == Int)
            {
                IntEntry * zero = inttable.lookup_string("0");                    
                zero->code_ref(s);                
            } else if (a->type_decl == Str)
            {
                StringEntry * zero = stringtable.lookup_string("");
                zero->code_ref(s);

            } else if (a->type_decl == Bool)
            {
                s << "bool_const0" << "   # Bool default";
            } else
            {
                s << "0";
                //s << " # " << a->type_decl;
            }
            s << endl;
        }
    }
    INFO_OUT_AS;
}

int CgenNode::calc_temp()
{
    INFO_IN;
    // Calculate temp size for initialization of object
    int temp_size = 0;
    for(int i = features->first();
            features->more(i);
            i = features->next(i))
    {
        Feature_class * f = features->nth(i);
        attr_class * attr = dynamic_cast<attr_class*>(f);
        method_class * method = dynamic_cast<method_class*>(f);
        if (attr)
        {
            // std::cout << "found attr with " << attr->calc_temp() << "\n";
            temp_size = std::max(temp_size, attr->calc_temp());
        }
    }
    INFO_OUT;
    return temp_size;
}

void CgenNode::emit_init(ostream& s, int header_size)
{
    INFO_IN_AS;
    emit_push_header(s, header_size);
    if (parentnd != NULL && get_name() != Object)
    {
        std::string str = parentnd->get_name()->get_string();
        str += CLASSINIT_SUFFIX;
        emit_jal(str.c_str(), s);
    }
    // Fill constants
   	for (int i = features->first(); 
         features->more(i);
         i = features->next(i))
	{
        Feature_class * f = features->nth(i);
        attr_class * attr = dynamic_cast<attr_class*>(f);
		if (attr)
		{
			attr->cgen(s,
                       get_name(),
                       get_attr_table(),
                       get_method_table());
		}
	} 
    // restore self
    emit_move(ACC, SELF, s);
    emit_pop_header(s, header_size);
    emit_return(s);
    INFO_OUT_AS;
}

void CgenNode::code_prot(ostream& s)
{
    INFO_IN_AS;
    // Add -1 eye catcher
    s << WORD << "-1" << endl;

    code_ref(s);
    s << PROTOBJ_SUFFIX << LABEL;
    s << WORD << this->get_id() << endl; // class tag

    s << WORD << DEFAULT_OBJFIELDS + get_attr_num() << endl;

    /***** Add dispatch information for class Int ******/
    s << WORD;
    code_ref(s);
    s << DISPTAB_SUFFIX << endl; // dispatch table
    
    code_attr_prot(s);
    // <- here need put inforormation for all 
    
    INFO_OUT_AS;
}
void CgenNode::fill_table() {
    int attr_offset = FRAME_OFFSET;
    int meth_offset = 0;
    // std::cout << get_name() << "\n";
    if (parentnd) {
        //std::cerr << "call for " << get_name() << "\n";
        parentnd->fill_table();
        attrTable = new std::vector<EnvElement>(
                *parentnd->get_attr_table());
        attr_offset += attrTable->size();
        methodTable = new std::vector<EnvElement>(
                *parentnd->get_method_table());
        meth_offset += methodTable->size();
    } else {
        attrTable = new std::vector<EnvElement>();
        methodTable = new std::vector<EnvElement>();
    }

    for (int i = features->first();
            features->more(i);
            i = features->next(i)) {
        Feature_class * f = features->nth(i);
        method_class * m = dynamic_cast<method_class*> (f);
        attr_class * a = dynamic_cast<attr_class*> (f);
        if (m) {
            auto pred = [ = ](EnvElement a){return a.name == m->get_name();};
            auto prev = std::find_if(
                    methodTable->begin(),
                    methodTable->end(),
                    pred);
            if (prev == methodTable->end()) {
                EnvElement new_elem = EnvElement(get_name(), m->get_name(), meth_offset, Type::METHOD);
                //std::cout << " for " << m->get_name() << "\n";
                methodTable->push_back(new_elem);
                ++meth_offset;
            } else {
                auto m_offset = prev->offset;
                EnvElement new_elem = EnvElement(get_name(), m->get_name(), m_offset, Type::METHOD);
                std::replace_if(methodTable->begin(), methodTable->end(),
                        pred, new_elem);
            }
        }
        if (a) {
            attrTable->push_back(EnvElement(get_name(), a->get_name(), attr_offset, Type::OBJECT));
            ++attr_offset;

        }
    }
    max_inherits_id = id;
    std::vector<CgenNodeP> list;
    for (auto it = children; it; it = it->tl()) {
        list.push_back(&(*it->hd()));
        max_inherits_id = std::max(max_inherits_id, it->hd()->get_id());
    }
    for (; !list.empty();) {
        CgenNodeP currNode = *list.rbegin();
        list.pop_back();
        for (auto it = currNode->get_children(); it; it = it->tl()) {
            list.push_back(&(*it->hd()));
        }
        max_inherits_id = std::max(max_inherits_id, currNode->get_id());
    }
};


int method_class::calc_temp() {
    TEMPS_IN
    int ret = expr->calc_temp();
    TEMPS_OUT;
    return ret;
}

int attr_class::calc_temp() {
    TEMPS_IN
    int ret = init->calc_temp();
    TEMPS_OUT;
    return ret;
}

int assign_class::calc_temp() {
    TEMPS_IN
    int ret = expr->calc_temp();
    TEMPS_OUT;
    return ret;
}

int static_dispatch_class::calc_temp() {
    TEMPS_IN
    int ret = expr->calc_temp();
    for (int i = actual->first();
            actual->more(i);
            i = actual->next(i)) {
        ret = std::max(ret, actual->nth(i)->calc_temp());
    }
    TEMPS_OUT;
    return ret;
}

int dispatch_class::calc_temp() {
    TEMPS_IN
    int ret = expr->calc_temp();
    for (int i = actual->first();
            actual->more(i);
            i = actual->next(i)) {
        ret = std::max(ret, actual->nth(i)->calc_temp());
    }
    TEMPS_OUT;
    return ret;
}

int cond_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(
            then_exp->calc_temp(),
            else_exp->calc_temp());
    ret = std::max(ret, pred->calc_temp());
    TEMPS_OUT;
    return ret;
}

int loop_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(
            pred->calc_temp(),
            body->calc_temp());
    TEMPS_OUT;
    return ret;
}

int block_class::calc_temp() {
    TEMPS_IN
    int ret = 0;
    for (int i = body->first();
            body->more(i);
            i = body->next(i)) {
        ret = std::max(ret, body->nth(i)->calc_temp());
    }
    TEMPS_OUT;
    return ret;
}

int let_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(init->calc_temp(),
            body->calc_temp() + 1);
    TEMPS_OUT;
    return ret;
}

int plus_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(e1->calc_temp(), e2->calc_temp() + 1);
    TEMPS_OUT;
    return ret;
}

int sub_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(e1->calc_temp(), e2->calc_temp() + 1);
    TEMPS_OUT;
    return ret;
}

int mul_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(e1->calc_temp(), e2->calc_temp() + 1);
    TEMPS_OUT;
    return ret;
}

int divide_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(e1->calc_temp(), e2->calc_temp() + 1);
    TEMPS_OUT;
    return ret;
}

int neg_class::calc_temp() {
    TEMPS_IN
    int ret = e1->calc_temp();
    TEMPS_OUT;
    return ret;
}

int lt_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(e1->calc_temp(), e2->calc_temp() + 1);
    TEMPS_OUT;
    return ret;
}

int eq_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(e1->calc_temp(), e2->calc_temp() + 1);
    TEMPS_OUT;
    return ret;
}

int leq_class::calc_temp() {
    TEMPS_IN
    int ret = std::max(e1->calc_temp(), e2->calc_temp() + 1);
    TEMPS_OUT;
    return ret;
}

int comp_class::calc_temp() {
    TEMPS_IN
    int ret = e1->calc_temp();
    TEMPS_OUT;
    return ret;
}

int int_const_class::calc_temp() {
    TEMPS_IN
    int ret = 0;
    TEMPS_OUT;
    return ret;
}

int bool_const_class::calc_temp() {
    TEMPS_IN
    int ret = 0;
    TEMPS_OUT;
    return ret;
}

int string_const_class::calc_temp() {
    TEMPS_IN
    int ret = 0;
    TEMPS_OUT;
    return ret;
}

int new__class::calc_temp() {
    TEMPS_IN
    int ret = 1;
    TEMPS_OUT;
    return ret;
}

int isvoid_class::calc_temp() {
    TEMPS_IN
    int ret = e1->calc_temp();
    TEMPS_OUT;
    return ret;
}

int no_expr_class::calc_temp() {
    TEMPS_IN
    int ret = 0;
    TEMPS_OUT;
    return ret;
}

int object_class::calc_temp() {
    TEMPS_IN
    int ret = 0;
    TEMPS_OUT;
    return ret;
}

int branch_class::calc_temp() {
    TEMPS_IN
    int ret = expr->calc_temp();
    TEMPS_OUT;
    return ret;
}

int typcase_class::calc_temp() {
    TEMPS_IN
    int ret = expr->calc_temp();
    for (int i = cases->first();
            cases->more(i);
            i = cases->next(i)) {
        ret = std::max(ret, cases->nth(i)->calc_temp() + 1);
    }
    TEMPS_OUT;
    return ret;
}
