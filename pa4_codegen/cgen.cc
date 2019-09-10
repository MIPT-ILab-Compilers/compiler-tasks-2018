
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
//#define DEBUG_AS_NAMES
//#define DEBUG_TEMPS

#ifdef DEBUG_AS_NAMES
std::string function_name = "";
#endif

#ifdef DEBUG_AS
#define INFO_IN_AS { std::string tmp_in; for(long i_in =0; i_in < __offset; ++i_in) tmp_in += " ";\
    s << "# " << tmp_in << " " << __PRETTY_FUNCTION__ << " in\n"; }
#define INFO_OUT_AS { std::string tmp_out; for(long i_out =0; i_out < __offset; ++i_out) tmp_out += " ";\
    s << "# " << tmp_out << " " << __PRETTY_FUNCTION__ << " out\n"; }
#else
#define INFO_IN_AS
#define INFO_OUT_AS
#endif

#ifdef DEBUG_AS_EMIT
#define INFO_IN_AS_EMIT { std::string tmp_in; for(long i_in =0; i_in < __offset; ++i_in) tmp_in += " ";\
    s << "\n # " << tmp_in << " " << __PRETTY_FUNCTION__ << " in\n"; }
#define INFO_OUT_AS_EMIT { std::string tmp_out; for(long i_out =0; i_out < __offset; ++i_out) tmp_out += " ";\
    s << "\n # " << tmp_out << " " << __PRETTY_FUNCTION__ << " out\n"; }
#else
#define INFO_IN_AS_EMIT
#define INFO_OUT_AS_EMIT
#endif

#ifdef DEBUG_PRINT
#define INFO_IN { std::string tmp_in; for(long i_in =0; i_in < __offset; ++i_in) tmp_in += " ";\
    fprintf(stderr, "%s %s: in\n", tmp_in.c_str(), __PRETTY_FUNCTION__); ++__offset; }
#define INFO_OUT { --__offset; std::string tmp_out; for(long i_out =0; i_out < __offset; ++i_out) tmp_out += " ";\
    fprintf(stderr, "%s %s: out\n", tmp_out.c_str(), __PRETTY_FUNCTION__);}
#else
#define INFO_IN
#define INFO_OUT
#endif

#ifdef DEBUG_TEMPS
bool debug_temps = false;
long temps__offset = 0;
#define TEMPS_IN { std::string tmp_in; for(long i_in =0; i_in < temps__offset; ++i_in) tmp_in += " ";\
    if (debug_temps) {fprintf(stderr, "%s in %s \n", tmp_in.c_str(), __PRETTY_FUNCTION__); ++temps__offset;}}
#define TEMPS_OUT { std::string tmp_in; for(long i_in =0; i_in < temps__offset; ++i_in) tmp_in += " ";\
    if (debug_temps) {fprintf(stderr, "%s out %s: %d\n", tmp_in.c_str(), __PRETTY_FUNCTION__, ret); --temps__offset;}}
#else
#define TEMPS_OUT
#define TEMPS_IN
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
bool expr_is_const = false;

void program_class::cgen(ostream &s) {
    // spim wants comments to start with '#'
    s << "# start of generated code\n";
    INFO_IN_AS;

    initialize_constants();
    codegen_classtable = new CgenClassTable(classes, s);
    codegen_classtable->code();

    s << "\n# end of generated code\n";
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

void attr_class::cgen(std::ostream& s, Symbol self_class, Environment var, Environment met) {
    INFO_IN

    INFO_OUT
}

void method_class::cgen(ostream& s, Symbol self_class, Environment var, Environment met) {
    INFO_IN
    init_alloc_temp();
    auto curr_var = new std::vector<EnvElement>(*var);
    int size = calc_temp() + formals->len();

    int cnt = FRAME_OFFSET + size;
    for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
        formal_class* f = dynamic_cast<formal_class*> (formals->nth(i));
        if (f) {
            EnvElement new_elem = EnvElement(self, f->name, --cnt, Type::METHOD);
            curr_var->push_back(new_elem);
            s << " # formal " << f->name->get_string() << endl;
        } else {
            s << " # unknown formal" << endl;
        }
    }
    expr->cgen(s, self_class, curr_var, met);
    INFO_OUT
}

void assign_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    expr->cgen(s, self_class, var, met);

    auto pred = [ = ](EnvElement a){return a.name == name;};
    auto off_var = std::find_if(var->rbegin(), var->rend(), pred);
    if (off_var->type == Type::OBJECT) {
        s << "#> for " << name << " self \n";
        emit_store(ACC, off_var->offset, SELF, s);
    } else {
        s << "#> for " << name << " method \n";
        emit_store(ACC, off_var->offset, FP, s);
    }
    expr_is_const = true;
    INFO_OUT_AS;
}

void static_dispatch_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    Symbol type = type_name;
    if (type == SELF_TYPE) {
        type = curr_node->get_name();
    }

    for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
        actual->nth(i)->cgen(s, self_class, var, met);
        emit_push(ACC, s);
    }
    expr->cgen(s, self_class, var, met);

    int good_label = create_label();
    emit_abort(good_label, line_number, DISPATHABORT, s);

    CgenNodeP node = codegen_classtable->lookup(type);
    auto table = node->get_method_table();
    auto pred = [ = ](EnvElement a){return a.name == name;};
    
    auto off = std::find_if(table->begin(), table->end(), pred);
    int offset = 1;
    if(off != table->end())
    {
        offset = off->offset;
    }
    else
    {
        assert(0);
    }

    emit_label_def(good_label, s);
    emit_partial_load_address(T3, s);
    emit_disptable_ref(type, s);
    s << endl;
    emit_load(T3, offset, T3, s);
    emit_jalr(T3, s);

    expr_is_const = true;
    INFO_OUT_AS;
}

void dispatch_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    auto tmp_met = new std::vector<EnvElement>(*met);
    Symbol type = expr->get_type();
    if (type == SELF_TYPE) {
        type = curr_node->get_name();
    }

    for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
        actual->nth(i)->cgen(s, self_class, var, tmp_met);
        emit_push(ACC, s);
    }
    expr->cgen(s, self_class, var, tmp_met);

    int jump_label = create_label();
    emit_bne(ACC, ZERO, jump_label, s);
    emit_load_string(ACC, stringtable.lookup_string(
            curr_node->filename->get_string()), s);
    emit_load_imm(T1, line_number, s);
    emit_jal("_dispatch_abort", s);

    CgenNodeP node = codegen_classtable->lookup(type);
    tmp_met = node->get_method_table();
    auto pred = [ = ](EnvElement a){return a.name == name;};
    auto off = std::find_if(tmp_met->rbegin(), tmp_met->rend(), pred);

    int offset = 1;
    if(off != tmp_met->rend())
    {
        offset = off->offset;
    }
    else
    {
        assert(0);
    }
    
    emit_label_def(jump_label, s);
    emit_load(T1, DISPTABLE_OFFSET, ACC, s);
    emit_load(T1, offset, T1, s);
    emit_jalr(T1, s);
    expr_is_const = true;
    INFO_OUT_AS;
}

void cond_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void loop_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    int cond_label = create_label();
    int end_label = create_label();
    emit_label_def(cond_label, s);
    pred->cgen(s, self_class, var, met);
    emit_load_bool(T3, falsebool, s);
    emit_beq(ACC, T3, end_label, s);
    body->cgen(s, self_class, var, met);
    emit_branch(cond_label, s);
    emit_label_def(end_label, s);

    emit_move(ACC, ZERO, s);
    INFO_OUT_AS;
}

static std::vector<std::pair<std::pair<int, int>, int>> case_list;

void typcase_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    expr->cgen(s, self_class, var, met);
    int last_label = create_label();
    emit_abort(last_label, line_number, CASEABORT2, s);
    emit_label_def(last_label, s);
    emit_load(T3, TAG_OFFSET, ACC, s);
    last_label = create_label();

    int temp = alloc_temp() + FRAME_OFFSET;
    emit_store(ACC, temp, FP, s);

    case_list.clear();
    for (int i(cases->first()); cases->more(i); i = cases->next(i)) {
        Symbol type = cases->nth(i)->get_type_decl();
        CgenNodeP class_node = codegen_classtable->lookup(type);
        case_list.push_back(std::make_pair(std::make_pair(class_node->get_id(),
                class_node->get_max_id()), i));
    }

    auto lmbd = [](auto a, auto b) {
        return a.first.first >= b.first.first && a.first.second <= b.first.second;
    };
    std::sort(case_list.begin(), case_list.end(), lmbd);

    int id = 0, max_id = 0, node_idx = 0;
    int cur_label = 0, next_label = create_label();
    for (auto it = case_list.begin(); it != case_list.end();) {
        id=  it->first.first;
        max_id = it->first.second;
        node_idx = it->second;
        it++;

        cur_label = next_label;
        next_label = create_label();

        emit_label_def(cur_label, s);
        emit_blti(T3, id, next_label, s);
        emit_bgti(T3, max_id, next_label, s);

        Case br = cases->nth(node_idx);
        auto tmp_var = new std::vector<EnvElement>(*var);
        tmp_var->push_back(EnvElement(self_class, br->get_name(), temp, Type::METHOD));
        br->get_expr()->cgen(s, self_class, tmp_var, met);
        emit_branch(last_label, s);
    }

    emit_label_def(next_label, s);
    emit_jal(CASEABORT, s);

    emit_label_def(last_label, s);

    expr_is_const = true;
    INFO_OUT_AS;
}

void block_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void let_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    s << " # let code for name " << identifier << "\n";
    auto curr_var = new std::vector<EnvElement>(*var);
    int offset = alloc_temp() + FRAME_OFFSET;
    bool filled = false;
    if (!init->get_type()) {
        if (type_decl == Int) {
            emit_load_int(ACC, inttable.lookup_string("0"), s);
            filled = true;
        }
        if (type_decl == Bool) {
            emit_load_bool(ACC, falsebool, s);
            filled = true;
        }
        if (type_decl == Str) {
            emit_load_string(ACC, stringtable.lookup_string(""), s);
            filled = true;
        }
    }
    if (!filled) {
        init->cgen(s, self_class, curr_var, met);
    }
    EnvElement new_elem = EnvElement(curr_node->get_name(), identifier, offset, Type::METHOD);
    curr_var->push_back(new_elem);
    emit_store(ACC, offset, FP, s);
    body->cgen(s, self_class, curr_var, met);
    INFO_OUT_AS;
}


void plus_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void sub_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void mul_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void divide_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void neg_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    e1->cgen(s, self_class, var, met);
    if (expr_is_const) {
        emit_push(ACC, s);
        emit_new(Int, s);
        emit_pop(T3, s);
        emit_fetch_int(T3, T3, s);

        expr_is_const = 0;
    } else {
        emit_fetch_int(T3, ACC, s);
    }
    emit_neg(T3, T3, s);
    emit_store_int( T3, ACC, s);
    INFO_OUT_AS;
}

void lt_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void eq_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void leq_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    e1->cgen(s, self, var, met);
    emit_push(T4, s);
    emit_fetch_int(T4, ACC, s);
    e2->cgen(s, self, var, met);
    emit_fetch_int(T3, ACC, s);

    int end_label = create_label();
    emit_load_bool(ACC, falsebool, s);
    emit_blt(T3, T4, end_label, s);
    emit_load_bool(ACC, truebool, s);
    emit_label_def(end_label, s);
    emit_pop(T4, s);
    INFO_OUT_AS;
}

void comp_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;

    INFO_OUT_AS;
}

void int_const_class::cgen(ostream& s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    //
    // Need to be sure we have an IntEntry *, not an arbitrary Symbol
    //
    emit_load_int(ACC, inttable.lookup_string(token->get_string()), s);
    expr_is_const = true;
    INFO_OUT_AS;
}

void string_const_class::cgen(ostream& s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    emit_load_string(ACC, stringtable.lookup_string(token->get_string()), s);
    INFO_OUT_AS;
}

void bool_const_class::cgen(ostream& s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    emit_load_bool(ACC, BoolConst(val), s);
    INFO_OUT_AS;
}

void new__class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    if (type_name == SELF_TYPE) {
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
    } else {
        if (type_name != Bool) {
            emit_new(type_name, s);
            s << JAL;
            emit_init_ref(type_name, s);
            s << endl;
            expr_is_const = true;
        } else {
            emit_load_bool(ACC, falsebool, s);
        }
    }
    INFO_OUT_AS;
}

void isvoid_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    e1->cgen(s, self_class, var, met);

    int end_label = create_label();
    emit_load_bool(T3, falsebool, s);
    emit_bne(ACC, ZERO, end_label, s);
    emit_load_bool(T3, truebool, s);
    emit_label_def(end_label, s);
    emit_move(ACC, T3, s);

    INFO_OUT_AS;
}

void no_expr_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    emit_load_imm(ACC, 0, s);
    INFO_OUT_AS;
}

void object_class::cgen(ostream &s, Symbol self_class, Environment var, Environment met) {
    INFO_IN_AS;
    if (name != self && name != self_class) {
        auto pred = [ = ](EnvElement a){return a.name == name;};
        auto off_var = std::find_if(var->rbegin(), var->rend(), pred);

#ifdef DEBUG_AS
        for(auto it = var->begin(); it != var->end(); ++it)
        {
            s << "#  ==> " << it->name->get_string() << " offset : "
                    << it->offset << " " << it->self->get_string() << " type:" <<
                    (it->type == Type::OBJECT ? "O" : "M") << " \n";
        }
        s << "#  -> curr off " << (off_var->offset)*WORD_SIZE << "\n";
#endif
        if (off_var->type == Type::OBJECT) {
#ifdef DEBUG_AS
            s << "# for " << name << " self \n";
#endif
            emit_load(ACC, off_var->offset, SELF, s);
        } else {
#ifdef DEBUG_AS
            s << "# for " << name << " method \n";
#endif
            emit_load(ACC, off_var->offset, FP, s);
        }
        expr_is_const = true;
    } else {
#ifdef DEBUG_AS
        s << "# for " << name << " SELF \n";
#endif
        emit_move(ACC, SELF, s);
    }
    INFO_OUT_AS;
}

