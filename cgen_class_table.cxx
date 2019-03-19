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
    str << "# coding global data" << endl;
    Symbol main = idtable.lookup_string(MAINNAME);
    Symbol string = idtable.lookup_string(STRINGNAME);
    Symbol integer = idtable.lookup_string(INTNAME);
    Symbol boolc = idtable.lookup_string(BOOLNAME);

    str << "\t.data\n" << ALIGN;
    //
    // The following global names must be defined first.
    //
    str << GLOBAL << CLASSNAMETAB << endl;
    str << GLOBAL;
    emit_protobj_ref(main, str);
    str << endl;
    str << GLOBAL;
    emit_protobj_ref(integer, str);
    str << endl;
    str << GLOBAL;
    emit_protobj_ref(string, str);
    str << endl;
    str << GLOBAL;
    falsebool.code_ref(str);
    str << endl;
    str << GLOBAL;
    truebool.code_ref(str);
    str << endl;
    str << GLOBAL << INTTAG << endl;
    str << GLOBAL << BOOLTAG << endl;
    str << GLOBAL << STRINGTAG << endl;

    //
    // We also need to know the tag of the Int, String, and Bool classes
    // during code generation.
    //
    assert(intclasstag != -1);
    str << INTTAG << LABEL
        << WORD << intclasstag << endl;
    assert(boolclasstag != -1);
    str << BOOLTAG << LABEL
        << WORD << boolclasstag << endl;
    assert(stringclasstag != -1);
    str << STRINGTAG << LABEL
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
    str << "# coding global text" << endl;
    str << GLOBAL << HEAP_START << endl
        << HEAP_START << LABEL
        << WORD << 0 << endl
        << "\t.text" << endl
        << GLOBAL;
    emit_init_ref(idtable.add_string("Main"), str);
    str << endl << GLOBAL;
    emit_init_ref(idtable.add_string("Int"), str);
    str << endl << GLOBAL;
    emit_init_ref(idtable.add_string("String"), str);
    str << endl << GLOBAL;
    emit_init_ref(idtable.add_string("Bool"), str);
    str << endl << GLOBAL;
    emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
    str << endl;
    INFO_OUT;
}

void CgenClassTable::code_initCode() {
    str << "  # Initialization code \n";
    for (List<CgenNode> *l = nds; l; l = l->tl())
    {
        CgenNode* node = l->hd();
        int tmpCurr = node->calc_temp();
        node->code_ref(str);
        str << CLASSINIT_SUFFIX << LABEL;
        node->emit_init(str, tmpCurr);
    };
}

void CgenClassTable::code_bools(int boolclasstag) {
    INFO_IN;
    falsebool.code_def(str, boolclasstag);
    truebool.code_def(str, boolclasstag);
    INFO_OUT;
}

void CgenClassTable::code_select_gc() {
    INFO_IN;
    //
    // Generate GC choice constants (pointers to GC functions)
    //
    str << "# choosing GC" << endl;
    str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
    str << "_MemMgr_INITIALIZER:" << endl;
    str << WORD << gc_init_names[cgen_Memmgr] << endl;
    str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
    str << "_MemMgr_COLLECTOR:" << endl;
    str << WORD << gc_collect_names[cgen_Memmgr] << endl;
    str << GLOBAL << "_MemMgr_TEST" << endl;
    str << "_MemMgr_TEST:" << endl;
    str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
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
    INFO_IN;
    //
    // Add constants that are required by the code generator.
    //
    str << "# coding constants" << endl;
    stringtable.add_string("");
    inttable.add_string("0");

    stringtable.code_string_table(str, stringclasstag);
    inttable.code_string_table(str, intclasstag);
    code_bools(boolclasstag);
    INFO_OUT;
}

void CgenClassTable::code_classNameTab() {
    str << "# coding class name Table. \n";
    str << CLASSNAMETAB << LABEL;    
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
        str << WORD; 
        se->code_ref(str);
        str << "\n";        
    }
}

void CgenClassTable::code_classObjTab() {
    str << "# coding class object Table. \n";
    str << CLASSOBJTAB << LABEL;
    for (List<CgenNode> *l = nds; l; l = l->tl())
    {
        CgenNode* tmp = l->hd();
        str << WORD;
        tmp->code_ref(str);
        str << PROTOBJ_SUFFIX <<"\n";
        
        str << WORD;
        tmp->code_ref(str);
        str << CLASSINIT_SUFFIX <<"\n";
    };
}


void CgenClassTable::code_protObjs() {
    str << "# coding prototype Objects. \n";
    for (List<CgenNode> *l = nds; l; l = l->tl())
    {
        CgenNode* tmp = l->hd();
        tmp->code_prot(str);
    };
}

void CgenClassTable::code_dispTabs() {
    str << "# coding disp Tables. \n";
    for (List<CgenNode> *l = nds; l; l = l->tl())
    {
        CgenNode* tmp = l->hd();
        tmp->code_ref(str);
        str << DISPTAB_SUFFIX << LABEL;
        tmp->code_disp(str);
    };
}

void CgenClassTable::code_methods() {
    INFO_IN
    for (List<CgenNode> *l = nds; l; l = l->tl())
    {
        CgenNode* tmp = l->hd();
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
                int header_size = m->calc_temp();
                tmp->code_ref(str);
                str << METHOD_SEP;
                str << m->name << LABEL;

                emit_push_header(str, header_size);

                m->code(str,
                        tmp->get_name(),
                        tmp->get_attr_table(),
                        tmp->get_method_table());

                emit_pop_header(str, header_size);

                emit_return(str);
            }
        }
    }
    INFO_OUT
}

CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(NULL), str(s) {
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
    for(unsigned i = 0; i < methodTable->size(); ++i)
    {
        s << WORD << methodTable->at(i).self << METHOD_SEP << 
            methodTable->at(i).name << "\n";
    }

};

void CgenNode::code_ref(ostream& s)
{
    s << this->get_name();
}

int CgenNode::get_attr_num()
{
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
                s << "0" << "   # Bool default";
            } else
            {
                s << "0";
                s << " # " << a->type_decl;
            }
            s << endl;
        }
    }
    INFO_OUT_AS;
}

int CgenNode::calc_temp()
{
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
    return temp_size;
}

void CgenNode::emit_init(ostream& s, int header_size)
{
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
			attr->code(s,
                       get_name(),
                       get_attr_table(),
                       get_method_table());
		}
	} 
    emit_pop_header(s, header_size);
    emit_return(s);
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


