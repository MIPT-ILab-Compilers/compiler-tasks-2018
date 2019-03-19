
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"
#include <string>
#include <algorithm>

long __offset = 0;

//#define DEBUG_PRINT
//#define DEBUG_AS

#ifdef DEBUG_AS
#  define INFO_IN_AS { std::string tmp_in; for(long i_in =0; i_in < __offset; ++i_in) tmp_in += " ";\
    s << "\n # " << tmp_in << " " << __PRETTY_FUNCTION__ << " in\n"; }
#  define INFO_OUT_AS { std::string tmp_out; for(long i_out =0; i_out < __offset; ++i_out) tmp_out += " ";\
    s << "\n # " << tmp_out << " " << __PRETTY_FUNCTION__ << " out\n"; }
#else
#  define INFO_IN_AS
#  define INFO_OUT_AS
#endif

#ifdef DEBUG_PRINT
#  define INFO_IN { std::string tmp_in; for(long i_in =0; i_in < __offset; ++i_in) tmp_in += " ";\
    fprintf(stderr, "%s %s: in\n", tmp_in.c_str(), __PRETTY_FUNCTION__); ++__offset; }
#  define INFO_OUT { --__offset; std::string tmp_out; for(long i_out =0; i_out < __offset; ++i_out) tmp_out += " ";\
    fprintf(stderr, "%s %s: out\n", tmp_out.c_str(), __PRETTY_FUNCTION__);}
#else
#  define INFO_IN
#  define INFO_OUT
#endif

extern void emit_string_constant(ostream& str, char *s);
extern int cgen_debug;

//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
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

static char *gc_init_names[] = {"_NoGC_Init", "_GenGC_Init", "_ScnGC_Init"};
static char *gc_collect_names[] = {"_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect"};

// All static initialization function

#include "cgen_static_init.cxx"


//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

CgenClassTableP codegen_classtable;

void program_class::cgen(ostream &s) {
    INFO_IN_AS;
    // spim wants comments to start with '#'
    s << "# start of generated code\n";

    initialize_constants();
    codegen_classtable = new CgenClassTable(classes, s);
    codegen_classtable->code();

    s << "\n# end of generated code\n";
    INFO_OUT_AS;
}


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
    INFO_IN_AS;
    s << STRCONST_PREFIX << index;
    INFO_OUT_AS;
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
    INFO_IN_AS;
    for (List<StringEntry> *l = tbl; l; l = l->tl())
    {
        l->hd()->code_def(s, stringclasstag);
    }
    INFO_OUT_AS;
}

//
// Ints
//

void IntEntry::code_ref(ostream &s) {
    INFO_IN_AS;
    s << INTCONST_PREFIX << index;
    INFO_OUT_AS;
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
    INFO_IN_AS;
    s << BOOLCONST_PREFIX << val;
    INFO_OUT_AS;
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


// CgenClassTable && CgenNode classes defines

#include "cgen_class_table.cxx"


int method_class::calc_temp() {
	return expr->calc_temp();
}

int attr_class::calc_temp() {
	return init->calc_temp();
}

int assign_class::calc_temp() {
	return expr->calc_temp();
}

int static_dispatch_class::calc_temp() {
    int ret = expr->calc_temp();
    for (int i = actual->first();
            actual->more(i); 
            i = actual->next(i))
    {
        ret = std::max(ret, actual->nth(i)->calc_temp());
    }
    return ret;
}

int dispatch_class::calc_temp() {
	int ret = expr->calc_temp();
	for (int i = actual->first();
         actual->more(i);
         i = actual->next(i))
	{
		ret = std::max(ret, actual->nth(i)->calc_temp());
	}
	return ret;
}

int cond_class::calc_temp() {
    int ret = std::max(
        then_exp->calc_temp(),
        else_exp->calc_temp());
    ret = std::max(ret, pred->calc_temp());
	return ret;
}

int loop_class::calc_temp() {
    return std::max(
        pred->calc_temp(),
        body->calc_temp());
}

int block_class::calc_temp() {
	int ret = 0;
	for (int i = body->first();
         body->more(i);
         i = body->next(i))
	{
		ret = std::max(ret, body->nth(i)->calc_temp());
	}
	return ret;
}

int let_class::calc_temp() {
	return std::max(init->calc_temp(),
                    body->calc_temp() + 1);
}

int plus_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp());
}

int sub_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp());
}

int mul_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp());
}

int divide_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp());
}

int neg_class::calc_temp() {
	return e1->calc_temp();
}

int lt_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp());
}

int eq_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp());
}

int leq_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp());
}

int comp_class::calc_temp() {
	return e1->calc_temp();
}

int int_const_class::calc_temp() {
	return 0;
}

int bool_const_class::calc_temp() {
	return 0;
}

int string_const_class::calc_temp() {
	return 0;
}

int new__class::calc_temp() {
	return 1;
}

int isvoid_class::calc_temp() {
	return e1->calc_temp();
}

int no_expr_class::calc_temp() {
	return 0;
}

int object_class::calc_temp() {
	return 0;
}

int branch_class::calc_temp(){
    return expr->calc_temp();
}


int typcase_class::calc_temp() {
	int cnt = expr->calc_temp();
	for (int i = cases->first();
         cases->more(i);
         i = cases->next(i))
	{
        cnt = std::max(cnt,cases->nth(i)->calc_temp() + 1);
	}
	return cnt;
}

//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

void attr_class::code(std::ostream& s, Symbol self, Environment var, Environment met)
{
    INFO_IN
	if (init->get_type())
	{
		init->code(s, self, var, met);
        auto pred = [=](EnvElement a){return a.name == name;};
		auto off = std::find_if(var->rbegin(), var->rend(), pred);
        int offset = 0;
        if (off != var->rend())
        {
            offset = off->offset;
        }
        s << "# attribute " << name << " was initailized as a " << init->get_type() << endl;
		emit_store(ACC, offset, SELF, s);
	}
    INFO_OUT
}

void method_class::code(ostream& s, Symbol self, Environment var, Environment met)
{
    INFO_IN

    auto curr_var = var;
    int size = FRAME_OFFSET + calc_temp() + formals->len();

    for(int i = formals->first(); formals->more(i) ; i = formals->next(i))
    {
        formal_class* f= dynamic_cast<formal_class*>(formals->nth(i));
        if (f)
        {
            EnvElement new_elem = EnvElement(self, f->name, size -i);
            var->push_back(new_elem);
            s << " # formal " << f->name->get_string() << endl;
        } else
        {
            s << " # unknown formal" << endl;
        }
    }
    expr->code(s, self, var, met);
    var = curr_var;

    INFO_OUT
}

void assign_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void static_dispatch_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void dispatch_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    Symbol type = expr->get_type();
    if (type == SELF_TYPE)
    {
//            type = global_node->get_name();
    }

    for (int i = actual->first(); actual->more( i); i = actual->next( i))
    {
            actual->nth(i)->code(s, self, var, met);
            emit_push(ACC, s);
    }
    expr->code(s, self, var, met);

    int jump_label = create_label();
    emit_bne(ACC, ZERO, jump_label, s);
//    emit_load_string( ACC, stringtable.lookup_string( global_node->filename->get_string()), s);
    emit_load_string(ACC, stringtable.lookup_string(""), s);
    emit_load_imm(T1, line_number, s);
    emit_jal("_dispatch_abort", s);

    CgenNodeP node = codegen_classtable->lookup(type);
//   int offset = ((int)(node->lookup_method_offset(name))) - DEFAULT_METHOD_OFFSET;

    emit_label_def(jump_label, s);
    emit_load(T1, DISPTABLE_OFFSET, ACC, s);
//    emit_load(T1, offset, T1, s);
    emit_jalr(T1, s);

    INFO_OUT_AS;
}

void cond_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void loop_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void typcase_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void block_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void let_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    auto curr_var = var;
    init->code(s, self, var, met);
    s << " # let code for str " << get_line_number() <<  "\n";
    int offset = calc_temp();
    emit_store(ACC, 0, FP, s);
    body->code(s, self, var, met);
    var = curr_var;
    INFO_OUT_AS;
}

void plus_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void sub_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void mul_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void divide_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void neg_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void lt_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void eq_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void leq_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void comp_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void int_const_class::code(ostream& s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    //
    // Need to be sure we have an IntEntry *, not an arbitrary Symbol
    //
    emit_load_int(ACC, inttable.lookup_string(token->get_string()), s);
    INFO_OUT_AS;
}

void string_const_class::code(ostream& s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    emit_load_string(ACC, stringtable.lookup_string(token->get_string()), s);
    INFO_OUT_AS;
}

void bool_const_class::code(ostream& s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    emit_load_bool(ACC, BoolConst(val), s);
    INFO_OUT_AS;
}

void new__class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void isvoid_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void no_expr_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}

void object_class::code(ostream &s, Symbol self, Environment var, Environment met) {
    INFO_IN_AS;
    INFO_OUT_AS;
}


