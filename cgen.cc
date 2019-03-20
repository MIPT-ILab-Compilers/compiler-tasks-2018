
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
#define DEBUG_AS
//#define DEBUG_AS_EMIT

#ifdef DEBUG_AS
#  define INFO_IN_AS { std::string tmp_in; for(long i_in =0; i_in < __offset; ++i_in) tmp_in += " ";\
    s << "# " << tmp_in << " " << __PRETTY_FUNCTION__ << " in\n"; }
#  define INFO_OUT_AS { std::string tmp_out; for(long i_out =0; i_out < __offset; ++i_out) tmp_out += " ";\
    s << "# " << tmp_out << " " << __PRETTY_FUNCTION__ << " out\n"; }
#else
#  define INFO_IN_AS
#  define INFO_OUT_AS
#endif

#ifdef DEBUG_AS_EMIT
#  define INFO_IN_AS_EMIT { std::string tmp_in; for(long i_in =0; i_in < __offset; ++i_in) tmp_in += " ";\
    s << "\n # " << tmp_in << " " << __PRETTY_FUNCTION__ << " in\n"; }
#  define INFO_OUT_AS_EMIT { std::string tmp_out; for(long i_out =0; i_out < __offset; ++i_out) tmp_out += " ";\
    s << "\n # " << tmp_out << " " << __PRETTY_FUNCTION__ << " out\n"; }
#else
#  define INFO_IN_AS_EMIT
#  define INFO_OUT_AS_EMIT
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
    for (List<StringEntry> *l = tbl; l; l = l->tl())
    {
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


// CgenClassTable && CgenNode classes defines

#include "cgen_class_table.cxx"

//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

void attr_class::code(std::ostream& s, Symbol self_class, Environment var, Environment met)
{
    INFO_IN
	if (init->get_type())
	{
		init->code(s, self_class, var, met);
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

void method_class::code(ostream& s, Symbol self_class, Environment var, Environment met)
{
    INFO_IN
    init_alloc_temp();
    auto curr_var = new std::vector<EnvElement>(*var);
    int size = FRAME_OFFSET + calc_temp() + formals->len();

    for(int i = formals->first(); formals->more(i) ; i = formals->next(i))
    {
        formal_class* f= dynamic_cast<formal_class*>(formals->nth(i));
        if (f)
        {
            EnvElement new_elem = EnvElement(self, f->name, size -i, Type::METHOD);
            var->push_back(new_elem);
            s << " # formal " << f->name->get_string() << endl;
        } else
        {
            s << " # unknown formal" << endl;
        }
    }
    expr->code(s, self_class, var, met);
    var = curr_var;

    INFO_OUT
}

void assign_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    expr->code(s, self_class, var, met);

    auto pred = [=](EnvElement a){return a.name == name;};
    auto off_var = std::find_if(var->rbegin(), var->rend(), pred);
    if (off_var->type == Type::OBJECT)
    {
        s << "#> for " << name << " self \n";
        emit_store(ACC, off_var->offset, SELF, s);
    }
    else
    {
        s << "#> for " << name << " method \n";
        emit_store(ACC, off_var->offset, FP, s);
    }

    INFO_OUT_AS;
}

void static_dispatch_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	Symbol type = type_name;
	if (type == SELF_TYPE)
	{
		type = curr_node->get_name();
	}

	for ( int i = actual->first(); actual->more( i); i = actual->next( i))
	{
		actual->nth( i)->code(s, self_class, var, met);
		emit_push(ACC, s);
	}
	expr->code(s, self_class, var, met);

	int good_label = create_label();
	emit_abort(good_label, line_number, DISPATHABORT, s);

	CgenNodeP node = codegen_classtable->lookup( type);
    auto table = node->get_method_table();
    auto pred = [=](EnvElement a){return a.name == name;};
	int offset = std::find_if(table->begin(), table->end(), 
                    pred)->offset - FRAME_OFFSET;

	emit_label_def( good_label, s);
	emit_partial_load_address( T3, s); emit_disptable_ref( type, s); s << endl;
	emit_load( T3, offset, T3, s);
	emit_jalr( T3, s);

    INFO_OUT_AS;
}

void dispatch_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    auto tmp_met = new std::vector<EnvElement>(*met);
    Symbol type = expr->get_type();
    if (type == SELF_TYPE)
    {
            type = curr_node->get_name();
    }

    for (int i = actual->first(); actual->more( i); i = actual->next( i))
    {
            actual->nth(i)->code(s, self_class, var, met);
            emit_push(ACC, s);
    }
    expr->code(s, self_class, var, met);

    int jump_label = create_label();
    emit_bne(ACC, ZERO, jump_label, s);
    emit_load_string(ACC, stringtable.lookup_string(
                curr_node->filename->get_string()), s);
    emit_load_imm(T1, line_number, s);
    emit_jal("_dispatch_abort", s);

    CgenNodeP node = codegen_classtable->lookup(type);
    met = node->get_method_table();
    auto pred = [=](EnvElement a){return a.name == name;};
    auto off = std::find_if(met->rbegin(), met->rend(), pred);
    int offset = (off->offset);

    emit_label_def(jump_label, s);
    emit_load(T1, DISPTABLE_OFFSET, ACC, s);
    emit_load(T1, offset, T1, s);
    emit_jalr(T1, s);
    met = tmp_met;
    INFO_OUT_AS;
}

void cond_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	int else_label = create_label();
	int end_label = create_label();
	pred->code(s, self_class, var, met);
	emit_load_bool(T3, falsebool, s);
	emit_beq(ACC, T3, else_label, s);
	then_exp->code(s, self_class, var, met);
	emit_branch(end_label, s);
	emit_label_def(else_label, s);
	else_exp->code(s, self_class, var, met);
	emit_label_def(end_label, s);
    INFO_OUT_AS;
}

void loop_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	int cond_label = create_label();
	int end_label = create_label();
	emit_label_def(cond_label, s);
	pred->code(s, self_class, var, met);
	emit_load_bool(T3, falsebool, s);
	emit_beq(ACC, T3, end_label, s);
	body->code(s, self_class, var, met);
	emit_branch(cond_label, s);
	emit_label_def(end_label, s);

	emit_move(ACC, ZERO, s);
    INFO_OUT_AS;
}

static std::vector<std::pair<std::pair<int, int>, int>> vec;

void push_vec( int x, int y, int c)
{
	vec.push_back( std::make_pair( std::make_pair( x, y), c));
}

void typcase_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	expr->code(s, self_class, var, met);
	int last_label = create_label();
	emit_abort( last_label, line_number, CASEABORT2, s);
	emit_label_def( last_label, s);
	emit_load( T3, TAG_OFFSET, ACC, s);
	last_label = create_label();

	int temp = alloc_temp() + FRAME_OFFSET;
	emit_store( ACC, temp, FP, s);

	vec.clear();
    for ( int i( cases->first()); cases->more( i); i = cases->next( i))
	{
		Symbol type = cases->nth( i)->get_type_decl();
		CgenNodeP class_node = codegen_classtable->lookup( type);
        int max_class_tag = 0;
        for ( List<CgenNode> *leg = class_node->get_children(); leg; leg = leg->tl())
	    {
            max_class_tag = std::max (max_class_tag, leg->hd()->get_id());
	    }        
		push_vec(class_node->get_id(), max_class_tag, i);
	}

    auto lmbd = [](auto a,auto b) {
        return a.first.first >= b.first.first && a.first.second <= b.first.second;};
	std::sort(vec.begin(), vec.end(), lmbd);

    int x, y, c, cur_label, next_label = create_label();
	for (auto vec_leg = vec.begin(); vec_leg != vec.end(); )
	{
    	x = vec_leg->first.first;
	    y = vec_leg->first.second;
	    c = vec_leg->second;
	    vec_leg++;
		
        cur_label = next_label;
		next_label = create_label();
		
        emit_label_def(cur_label, s);
		emit_blti(T3, x, next_label, s);
		emit_bgti(T3, y, next_label, s);

		Case br = cases->nth(c);
		auto tmp_var = new std::vector<EnvElement>(*var);
		var->push_back(EnvElement(self_class, br->get_name(), temp, Type::METHOD));
		br->get_expr()->code(s, self_class, var, met);
        var = tmp_var;
		emit_branch(last_label, s);
	}

	emit_label_def(next_label, s);
	emit_jal(CASEABORT, s);

	emit_label_def(last_label, s);
    
    INFO_OUT_AS;
}

void block_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    for (int i = body->first(); body->more(i); i = body->next(i))
    {
        body->nth(i)->code(s, self_class, var, met);
    }
    INFO_OUT_AS;
}

void let_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    s << " # let code for name " << identifier <<  "\n";
    auto curr_var = new std::vector<EnvElement>(*var);
    int offset = alloc_temp() + FRAME_OFFSET;
    bool filled = false;
	if (!init->get_type())
    {
        if (type_decl == Int)
        {
            emit_load_int(ACC, inttable.lookup_string("0"), s);
            filled=true;
        }
        if (type_decl == Bool)
        {
            emit_load_bool(ACC, falsebool, s);
            filled=true;
        }
        if (type_decl == Str)
        {
            emit_load_string(ACC, stringtable.lookup_string(""),s);
            filled=true;
        }
    }
    if (!filled)
    {
        init->code(s, self_class, var, met);
    }
    EnvElement new_elem = EnvElement(curr_node->get_name(), identifier, offset, Type::METHOD);
    var->push_back(new_elem);
    emit_store(ACC, offset, FP, s);
    body->code(s, self_class, var, met);
    var = curr_var;
    INFO_OUT_AS;
}

#define ARITH_CODE( cmd, s)\
{\
	e1->code( s, self, var, met);\
	emit_push( ACC, s);\
	e2->code( s, self, var, met);\
	emit_move( T3, ACC, s);\
            emit_push( T3, s);\
            emit_new( Int, s);\
            emit_pop( T3, s);\
            emit_pop( T1, s);\
	emit_fetch_int( T1, T1, s);\
	emit_fetch_int( T3, T3, s);\
	emit_##cmd( T3, T1, T3, s);\
	emit_store_int( T3, ACC, s);\
}

void plus_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    ARITH_CODE( add, s);
    INFO_OUT_AS;
}

void sub_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    ARITH_CODE( sub, s);
    INFO_OUT_AS;
}

void mul_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    ARITH_CODE( mul, s);
    INFO_OUT_AS;
}

void divide_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    ARITH_CODE( div, s);
    INFO_OUT_AS;
}

void neg_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    e1->code( s, self, var, met);
    emit_push( ACC, s);
    emit_new( Int, s);
    emit_pop( T2, s);
    emit_fetch_int( T2, T2, s);

	emit_neg( T2, T2, s);
	emit_store_int( T2, ACC, s);
    INFO_OUT_AS;
}

void lt_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	e1->code( s, self, var, met);
	emit_push( T4, s);
	emit_fetch_int( T4, ACC, s);
	e2->code( s, self, var, met);
	emit_fetch_int( T3, ACC, s);

	int end_label = create_label();
	emit_load_bool( ACC, truebool, s);
	emit_blt( T4, T3, end_label, s);
	emit_load_bool( ACC, falsebool, s);
	emit_label_def( end_label, s);
	emit_pop( T4, s);

    INFO_OUT_AS;
}

void eq_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	e1->code( s, self_class, var, met);
	emit_push( ACC, s);
	e2->code( s, self_class, var, met);
	emit_pop( T2, s);

	emit_move( T1, ACC, s);
	emit_load_bool( A1, falsebool, s);
	emit_load_bool( ACC, truebool, s);

	int good_label = create_label();
	emit_xor( T3, T1, T2, s);
	emit_beqz( T3, good_label, s);
	emit_jal( EQUALITY_TEST, s);
	emit_label_def( good_label, s);
    INFO_OUT_AS;
}

void leq_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	e1->code( s, self, var, met);
	emit_push( T4, s);
	emit_fetch_int( T4, ACC, s);
	e2->code( s, self, var, met);
	emit_fetch_int( T3, ACC, s);

	int end_label = create_label();
	emit_load_bool( ACC, falsebool, s);
	emit_blt( T3, T4, end_label, s);
	emit_load_bool( ACC, truebool, s);
	emit_label_def( end_label, s);
	emit_pop( T4, s);
    INFO_OUT_AS;
}

void comp_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	e1->code( s, self, var, met);
	emit_load_bool( T3, falsebool, s);
	emit_xor( ACC, T3, ACC, s);
	emit_load_bool( T3, truebool, s);
	emit_xor( ACC, T3, ACC, s);
    INFO_OUT_AS;
}

void int_const_class::code(ostream& s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    //
    // Need to be sure we have an IntEntry *, not an arbitrary Symbol
    //
    emit_load_int(ACC, inttable.lookup_string(token->get_string()), s);
    INFO_OUT_AS;
}

void string_const_class::code(ostream& s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    emit_load_string(ACC, stringtable.lookup_string(token->get_string()), s);
    INFO_OUT_AS;
}

void bool_const_class::code(ostream& s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    emit_load_bool(ACC, BoolConst(val), s);
    INFO_OUT_AS;
}

void new__class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	if ( type_name == SELF_TYPE)
	{
		// Calculate address
		emit_load(T3, TAG_OFFSET, SELF, s);
		emit_sll(T3, T3, 3, s);

		emit_push(T4, s);
		emit_load_address(T4, CLASSOBJTAB, s);
		emit_addu(T4, T3, T4, s);

		// Call copying
		emit_load(ACC, 0, T4, s);
		s << JAL;
        emit_method_ref(Object, ::copy, s);
        s << endl;

		// Run init.
		emit_load(T3, 1, T4, s);
		emit_jalr(T3, s);
		emit_pop(T4, s);
	}
	else
	{
		if (type_name != Bool)
		{
			emit_new(type_name, s);
			s << JAL;
            emit_init_ref(type_name, s);
            s << endl;
		}
		else
		{
			emit_load_bool(ACC, falsebool, s);
		}
	}
    INFO_OUT_AS;
}

void isvoid_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	e1->code(s, self_class, var, met);

	int end_label = create_label();
	emit_load_bool( T3, falsebool, s);
	emit_bne( ACC, ZERO, end_label, s);
	emit_load_bool( T3, truebool, s);
	emit_label_def( end_label, s);
	emit_move( ACC, T3, s);

    INFO_OUT_AS;
}

void no_expr_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
	emit_load_imm( ACC, 0, s);
    INFO_OUT_AS;
}

void object_class::code(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    if (name != self && name != self_class)
    {
        auto pred = [=](EnvElement a){return a.name == name;};
        auto off_var = std::find_if(var->rbegin(), var->rend(), pred);
        if (off_var->type == Type::OBJECT)
        {
            s << "# for " << name << " self \n";
            emit_load(ACC, off_var->offset, SELF, s);
        }
        else
        {
            s << "# for " << name << " method \n";
            emit_load(ACC, off_var->offset, FP, s);
        }
    }
    else
    {
        s << "# for " << name << " SELF \n";
        emit_move(ACC, SELF, s);
    }
    INFO_OUT_AS;
}


int method_class::calc_temp() {
    INFO_IN;
    //std::cout << expr->calc_temp() << " : for method\n";
    int res = expr->calc_temp();
    INFO_OUT;
	return res;
}

int attr_class::calc_temp() {
    INFO_IN;
    int res = init->calc_temp();
    INFO_OUT;
	return res;
}

int assign_class::calc_temp() {
    INFO_IN;
    int res =  expr->calc_temp();
    INFO_OUT;
	return res;
}

int static_dispatch_class::calc_temp() {
    INFO_IN;
    int ret = expr->calc_temp();
    for (int i = actual->first();
            actual->more(i); 
            i = actual->next(i))
    {
        ret = std::max(ret, actual->nth(i)->calc_temp());
    }
    INFO_OUT;
    return ret;
}

int dispatch_class::calc_temp() {
    INFO_IN;
	int ret = expr->calc_temp();
	for (int i = actual->first();
         actual->more(i);
         i = actual->next(i))
	{
		ret = std::max(ret, actual->nth(i)->calc_temp());
	}
    INFO_OUT;
	return ret;
}

int cond_class::calc_temp() {
    INFO_IN;
    int ret = std::max(
        then_exp->calc_temp(),
        else_exp->calc_temp());
    ret = std::max(ret, pred->calc_temp());
    INFO_OUT;
	return ret;
}

int loop_class::calc_temp() {
    INFO_IN;
    int res = std::max(
        pred->calc_temp(),
        body->calc_temp());
    INFO_OUT;
	return res;
}

int block_class::calc_temp() {
    INFO_IN
	int ret = 0;
	for (int i = body->first();
         body->more(i);
         i = body->next(i))
	{
		ret = std::max(ret, body->nth(i)->calc_temp());
	}
    INFO_OUT
	return ret;
}

int let_class::calc_temp() {
	return std::max(init->calc_temp(),
                    body->calc_temp() + 1);
}

int plus_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp() + 1);
}

int sub_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp() + 1);
}

int mul_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp() + 1);
}

int divide_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp() + 1);
}

int neg_class::calc_temp() {
	return e1->calc_temp();
}

int lt_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp() + 1);
}

int eq_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp() + 1);
}

int leq_class::calc_temp() {
	return std::max(e1->calc_temp(),e2->calc_temp() + 1);
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

