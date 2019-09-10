

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"
#include <cassert>
#include <map>
#include <set>

#define DUMP(x) 
//#define DUMP(x) std::cerr << x

extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//////////////////////////////////////////////////////////////////////
static Symbol
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;

Symbol Expression_class::get_type() {
    return No_type;
}

Symbol tree_node::get_type() {
    return No_type;
};

Symbol tree_node::get_name() {
    return NULL;
};


// Global variable -> current class
Class__class *curent = NULL;
ClassTable *handler = NULL;

// class_list_type *class_list,attr_list_type* attr_list,method_list_type* method

Symbol lub(class_list_type *class_list, Symbol A, Symbol B) {
    std::map<Symbol, Class__class*> a_list;
    Class__class* a = class_list->lookup(A);
    for (; a != NULL;) {
        a_list[a->get_name()] = a;
        if (a->get_name() == Object)
            break;
        a = class_list->lookup(a->get_parent());
    }
    Class__class* b = class_list->lookup(B);
    for (; b != NULL && !a_list.empty();) {
        if (a_list.find(b->get_name()) != a_list.end()) {
            return b->get_name();
        }
        b = class_list->lookup(b->get_parent());
    }
    return Object;
}
// lub(class_list , A, B)
// algorithm -> fill one map
//          in second -> if match - it is lub

tree_node* find_method(class_list_type *class_list, Symbol A, Symbol method) {
    Class__class* a = class_list->lookup(A);
    if (a == NULL)
        return NULL;
    method_list_type* list = a->get_methods();
    if (list == NULL)
        return NULL;
    for (;;) {
        DUMP("  find_method 1:" << a->get_name()->get_string() << "\n");
        DUMP("  find_method 2:" << method->get_string() << "\n");
        tree_node* node = list->lookup(method);
        if (node != NULL) {
            DUMP("  find_method 3:" << node->get_type()->get_string() << "\n");
            return node;
        }
        if (a->get_name() == Object)
            break;
        a = class_list->lookup(a->get_parent());
        if (a == NULL) {
            return NULL;
        }
        list = a->get_methods();
    }
    return NULL;
}
// find_method(class_list, A, name)
// for every node -> 

attr_class* find_attr(class_list_type *class_list, Symbol A, Symbol attr) {
    Class__class* a = class_list->lookup(A);
    if (a == NULL)
        return NULL;
    attr_list_type* list = a->get_attr();
    if (list == NULL)
        return NULL;
    for (;;) {
        DUMP("  find_attr " << a->get_name()->get_string() << "\n");
        DUMP("  find_attr " << attr->get_string() << "\n");
        tree_node* node = list->lookup(attr);
        if (node != NULL)
            return dynamic_cast<attr_class*> (node);
        if (a->get_name() == Object)
            break;
        a = class_list->lookup(a->get_parent());
        if (a == NULL) {
            return NULL;
        }
        list = a->get_attr();
    }
    return NULL;
}
//
// Initializing the predefined symbols.
//

static void initialize_constants(void) {
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}

void class__class::fill_table(class_list_type* class_list) {
    attr_list = new attr_list_type;
    method_list = new method_list_type;
    attr_list->enterscope();
    method_list->enterscope();
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (dynamic_cast<method_class*> (feature) != NULL) {
            DUMP(curent->get_name() << "\n");
            method_list->addid(feature->get_name(), feature);
        }
        if (dynamic_cast<attr_class*> (feature) != NULL) {
            Symbol name = (dynamic_cast<attr_class*> (feature))->get_name();
            if (find_attr(class_list, curent->get_name(), name) != NULL) {
                handler->semant_error(curent->get_filename(), this) << " Attribute " <<
                    name << "is an attribute of an inherited class.\n";
                return;
            }

            attr_list->addid(feature->get_name(), feature);
        }
    }
    // method_class
    // table lay in current object
}

ClassTable::ClassTable(Classes classes) : semant_errors(0), error_stream(cerr) {
    handler = this;
    DUMP("Init Class table");
    // Init class table list
    // Need to save Name && Pointer to node 
    class_list = new class_list_type();
    class_list->enterscope();
    install_basic_classes();
    // fill class table
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ class_ = classes->nth(i);
        curent = class_;
        // Fill class table
        if (class_list->lookup(class_->get_name()) != NULL || class_->get_name() == SELF_TYPE) {
            semant_error(class_) << " Redefinition class " <<
                class_->get_name() << "\n";
            return;
        }
        class_list->addid(class_->get_name(), class_);

        class_->fill_table(class_list);
        // Fill environment-class
    }
    if (class_list->lookup(Main) == NULL) {
        semant_error() << "Class Main is not defined.\n";
        return;
    }
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ class_ = classes->nth(i);
        // For every class - there are uniq environment
        curent = class_;
        class_->semant(class_list,
              class_->get_attr(),
              class_->get_methods());
    }
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
    // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.

    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
        class_(Object,
               No_class,
               append_Features(
                               append_Features(
                                               single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                                               single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
                               single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
               filename);

    curent     = Object_class;
    Object_class->fill_table(class_list);
    class_list->addid(Object_class->get_name(), Object_class);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class =
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
               filename);

    curent     = IO_class;
    IO_class->fill_table(class_list);
    class_list->addid(IO_class->get_name(), IO_class);

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
        class_(Int,
               Object,
               single_Features(attr(val, prim_slot, no_expr())),
               filename);

    curent     = Int_class;
    Int_class->fill_table(class_list);
    class_list->addid(Int_class->get_name(), Int_class);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
        class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())), filename);

    curent     = Bool_class;
    Bool_class->fill_table(class_list);
    class_list->addid(Bool_class->get_name(), Bool_class);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
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
               filename);

    curent     = Str_class;
    Str_class->fill_table(class_list);
    class_list->addid(Str_class->get_name(), Str_class);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c) {
    return semant_error(c->get_filename(), c);
}

ostream& ClassTable::semant_error(Symbol filename, tree_node *t) {
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error() {
    semant_errors++;
    return error_stream;
}

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant() {
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
}

void class__class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Symbol name;
       Symbol parent;
       Features features;
       Symbol filename;
     */
    DUMP("class__class \n");
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature f = features->nth(i);
        f->semant(class_list, attr_list, method_list);
    }
    if (class_list->lookup(parent) == NULL) {
        handler->semant_error(curent->get_filename(), this) << "Class " << name <<
            "inherits from an undefined class " << parent << "\n";
        return;
    }

}

void method_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Symbol name;
       Formals formals;
       Symbol return_type;
       Expression expr;
     */
    DUMP("method_class \n");
    attr_list->enterscope();
    std::set<Symbol> formal_list;
    for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
        Formal f = formals->nth(i);
        f->semant(class_list, attr_list, method);
        if (formal_list.find(f->get_name()) != formal_list.end()) {
            handler->semant_error(curent->get_filename(), this) <<
                "Formal parameter " << f->get_name() <<
                " is multiply defined. \n";
            return;
        }
        formal_list.insert(f->get_name());
        attr_list->addid(f->get_name(), f);
    }
    expr->semant(class_list, attr_list, method);
    if (class_list->lookup(return_type) == NULL && !(return_type == SELF_TYPE && expr->get_type() == SELF_TYPE)) {
        handler->semant_error(curent->get_filename(), this) << "Undefined return type " << return_type
            << " in method " << name << " expected  " << expr->get_type() << "\n";
        return;
    }
    if ((lub(class_list, return_type, expr->get_type()) == expr->get_type()) &&
        (return_type != expr->get_type())) {
        handler->semant_error(curent->get_filename(), this) <<
            "Inferred return type " << return_type <<
            " of method " << name << " does not conform to declared return type " << expr->get_type() << "\n";
        return;
    }

    tree_node* node = find_method(class_list, curent->get_parent(), name);
    if (node != NULL && dynamic_cast<method_class*> (node)) {
        method_class* last_method = dynamic_cast<method_class*> (node);
        Formals fmls = last_method->get_formals();
        for (int i = fmls->first(); fmls->more(i); i = fmls->next(i)) {
            Formal orig = fmls->nth(i);
            if (!formals->more(i)) {
                handler->semant_error(curent->get_filename(), this) << " wrong number of formals \n";
                return;
            }
            Formal test = formals->nth(i);
            if (orig->get_type() != test->get_type()) {
                handler->semant_error(curent->get_filename(), this) << " Different calls \n";
                return;
            }
        }
        for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
            if (!fmls->more(i)) {
                handler->semant_error(curent->get_filename(), this) << " wrong number of formals \n";
                return;
            }
        }
    }
    attr_list->exitscope();
}

void attr_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
   Symbol name;
   Symbol type_decl;
   Expression init;
     */
    DUMP("attr_class \n");
    /* FILL ME */
}

void formal_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
   Symbol name;
   Symbol type_decl;
     */
    DUMP("formal_class \n");
   /* FILL ME */
}

void branch_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
   Symbol name;
   Symbol type_decl;
   Expression expr;
     */

    DUMP("branch_class \n");
    type = type_decl;
    attr_list->addid(name, this);
    expr->semant(class_list, attr_list, method);
    type = type_decl;
}

Symbol branch_class::get_type() {
    return type;
}

void assign_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Symbol name;
       Expression expr;
     */
    DUMP("assign_class \n");
    expr->semant(class_list, attr_list, method);
    if (attr_list->lookup(name) != NULL &&
        attr_list->lookup(name)->get_type() != expr->get_type() &&
        expr->get_type() != SELF_TYPE) {
        handler->semant_error(curent->get_filename(), this) << " assign != name  val named" << name <<
            " type: " << attr_list->lookup(name)->get_type() <<
            "  assigned to : " << expr->get_type() << "\n";
        return;
    }
    if (name == self) {
        handler->semant_error(curent->get_filename(), this) <<
            " Can't assign to self \n";
        return;
    }
    type = expr->get_type();
}

void static_dispatch_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression expr;
       Symbol type_name;
       Symbol name;
       Expressions actual;
     */
    DUMP("static_dispatch_class \n");
    /* FILL ME */
}

void dispatch_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression expr;
       Symbol name;
       Expressions actual;
     */
    DUMP("dispatch_class \n");
    attr_list->enterscope();

    for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
        Expression f = actual->nth(i);
        f->semant(class_list, attr_list, method);
        attr_list->addid(f->get_name(), f);
    }
    expr->semant(class_list, attr_list, method);
    attr_list->exitscope();

    DUMP("dispatch_class finish semant \n");
    Symbol class_type = expr->get_type();
    tree_node *node;
    Class__class *call_class = curent;
    //    std::cout << " > " << class_type->get_string() <<"\n";
    if (class_type == SELF_TYPE) {
        node = method->lookup(name);
    } else {
        call_class = class_list->lookup(class_type);
        DUMP("dispatch_class get methods \n");
    }

    method_list_type * methods_ = call_class->get_methods();
    node = methods_->lookup(name);
    if (node == NULL)
        node = find_method(class_list, call_class->get_name(), name);
    if (node == NULL) {
        handler->semant_error(curent->get_filename(), this) << " Dispatch to undefined method  " << name << "\n";
        return;
    }

    if (class_type == SELF_TYPE && node->get_type() == SELF_TYPE) {
        type = SELF_TYPE;
    } else
        if (node->get_type() == SELF_TYPE) {
        type = call_class->get_name();
    } else {
        type = node->get_type();
    }

    // check for types of arguments
    method_class* method_ = dynamic_cast<method_class*> (node);
    for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
        Formal orig = method_->get_formals()->nth(i);
        Expression test = actual->nth(i);
        Symbol orig_type = orig->get_type();
        Symbol test_type = test->get_type();
        if (test_type == SELF_TYPE) {
            test_type = curent->get_name();
        }
        Symbol lub_type = lub(class_list, test_type, orig_type);
        DUMP(" In type " << type <<" test " << test_type << " orig " << orig_type << " lub_type " << lub_type << "\n");
        if ((test_type != orig_type) &&
            (lub_type == test_type)) {
            handler->semant_error(curent->get_filename(), this) << " ﻿Wrong call for " << name <<
                " method  tested :" << test->get_type() << " orig :" << orig->get_type() << "\n";
            return;
        }
    }

}

void cond_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression pred;
       Expression then_exp;
       Expression else_exp;
     */
    DUMP("cond_class \n");
    /* FILL ME */
}

void loop_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression pred;
       Expression body;
     */
    DUMP("loop_class \n");
    /* FILL ME */
}

void typcase_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression expr;
       Cases cases;
       Symbol type;
     */
    DUMP("typcase_class \n");
    Symbol last_type = NULL;
    expr->semant(class_list, attr_list, method);
    std::set<Symbol> full;

    for (int i = cases->first(); cases->more(i); i = cases->next(i)) {
        Case f = cases->nth(i);
        DUMP("typcase_class call semant for  \n");
        f->semant(class_list, attr_list, method);
        if (full.find(f->get_type()) != full.end()) {
            handler->semant_error(curent->get_filename(), this)
                << "Duplicate branch " << f->get_type()
                << " in case statement.\n";
            return;
        }
        full.insert(f->get_type());
        /// > GET EXPRESION RETURN TYPE <
        Symbol test_type = f->get_expr();
        if (test_type == SELF_TYPE) {
            //            test_type = curent->get_name();
            continue;
        }
        if (last_type != NULL) {
            DUMP(" call lub for " << last_type << " and " << test_type << " \n");
            last_type = lub(class_list, last_type, test_type);
        } else {
            DUMP(" first type " << test_type << "\n");
            last_type = test_type;
        }
    }
    type = last_type;
}

void block_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
      Expressions body;
     */
    DUMP("block_class \n");
    /* FILL ME */
}

void let_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Symbol identifier;
       Symbol type_decl;
       Expression init;
       Expression body;
     */
    DUMP("let_class \n");
    /* FILL ME */
}

void plus_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression e1;
       Expression e2;
     */
    DUMP("plus_class \n");
    /* FILL ME */
}

void sub_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression e1;
       Expression e2;
     */
     DUMP("sub_class \n");
    /* FILL ME */
}

void mul_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
     /*
       Expression e1;
       Expression e2;
     */

    DUMP("mul_class \n");
    /* FILL ME */
}

void divide_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression e1;
       Expression e2;
     */
    DUMP("divide_class \n");
    /* FILL ME */
}

void neg_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression e1;
     */
    DUMP("neg_class \n");
    /* FILL ME */
}

void lt_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression e1;
       Expression e2;
     */
    DUMP("lt_class \n");
    /* FILL ME */
}

void eq_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression e1;
       Expression e2;
     */
    DUMP("eq_class \n");
    /* FILL ME */
}

void leq_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression e1;
       Expression e2;
     */
    DUMP("leq_class \n");
    /* FILL ME */
}

void comp_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
       Expression e1;
     */
    DUMP("comp_class \n");
    /* FILL ME */
}

void int_const_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
      Symbol token;
     */
    DUMP("int_const_class \n");
    type = Int;
}

void bool_const_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
      Boolean val;
     */
    DUMP("bool_const_class \n");
    type = Bool;
}

void string_const_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
      Symbol token;
     */
    DUMP("string_const_class \n");
    type = Str;
}

void new__class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
      Symbol type_name;
     */
    DUMP("new__class \n");
    type = type_name;

    if (class_list->lookup(type_name) == NULL && type_name != SELF_TYPE) {
        handler->semant_error(curent->get_filename(), this) << "'new' used with undefined class " << type_name << "\n";
        return;
    }

}

void isvoid_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
      Expression e1;
     */
    DUMP("isvoid_class \n");
    e1->semant(class_list, attr_list, method);
    type = Bool;
}

void no_expr_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
     */
    DUMP("no_expr_class \n");
    type = No_type;
}

void object_class::semant(class_list_type *class_list, attr_list_type* attr_list, method_list_type* method) {
    /*
      Symbol name;
     */
    DUMP("object_class \n");
    if (name == self) {
        type = SELF_TYPE;
    } else {
        if (attr_list->lookup(name) != NULL) {
            DUMP("object_class lookup \n");
            DUMP(attr_list->lookup(name)->get_type());
            type = attr_list->lookup(name)->get_type();
        } else {
            DUMP("object_class + find\n");
            tree_node *node = find_attr(class_list, curent->get_name(), name);
            if (node == NULL) {
                handler->semant_error(curent->get_filename(), this) << "Undeclared identifier " << name << "\n";
                return;
            }
            type = node->get_type();
        }
    }
}
