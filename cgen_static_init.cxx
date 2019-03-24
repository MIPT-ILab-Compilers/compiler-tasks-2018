//
// Initializing the predefined symbols.
//

static void initialize_constants(void) {
    INFO_IN;
    arg = idtable.add_string("arg");
    arg2 = idtable.add_string("arg2");
    Bool = idtable.add_string("Bool");
    concat = idtable.add_string("concat");
    cool_abort = idtable.add_string("abort");
    copy = idtable.add_string("copy");
    Int = idtable.add_string("Int");
    in_int = idtable.add_string("in_int");
    in_string = idtable.add_string("in_string");
    IO = idtable.add_string("IO");
    length = idtable.add_string("length");
    Main = idtable.add_string("Main");
    main_meth = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class = idtable.add_string("_no_class");
    No_type = idtable.add_string("_no_type");
    Object = idtable.add_string("Object");
    out_int = idtable.add_string("out_int");
    out_string = idtable.add_string("out_string");
    prim_slot = idtable.add_string("_prim_slot");
    self = idtable.add_string("self");
    SELF_TYPE = idtable.add_string("SELF_TYPE");
    Str = idtable.add_string("String");
    str_field = idtable.add_string("_str_field");
    substr = idtable.add_string("substr");
    type_name = idtable.add_string("type_name");
    val = idtable.add_string("_val");
    INFO_OUT;
}



//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_load(const char *dest_reg, const int offset, const char *source_reg, ostream& s) {
    INFO_IN_AS_EMIT;
    s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")";
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_store(const char *source_reg, const int offset, const char *dest_reg, ostream& s) {
    INFO_IN_AS_EMIT;
    s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")";
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_load_imm(const char *dest_reg, const int val, ostream& s) {
    INFO_IN_AS_EMIT;
    s << LI << dest_reg << " " << val ;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_load_address(const char *dest_reg, const char *address, ostream& s) {
    INFO_IN_AS_EMIT;
    s << LA << dest_reg << " " << address ;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_partial_load_address(const char *dest_reg, ostream& s) {
    INFO_IN_AS_EMIT;
    s << LA << dest_reg << " ";
    INFO_OUT_AS_EMIT;
}

static void emit_load_bool(const char *dest, const BoolConst& b, ostream& s) {
    INFO_IN_AS_EMIT;
    emit_partial_load_address(dest, s);
    b.code_ref(s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_load_string(const char *dest, StringEntry *str, ostream& s) {
    INFO_IN_AS_EMIT;
    emit_partial_load_address(dest, s);
    str->code_ref(s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_load_int(char *dest, IntEntry *i, ostream& s) {
    INFO_IN_AS_EMIT;
    emit_partial_load_address(dest, s);
    i->code_ref(s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_move(char *dest_reg, char *source_reg, ostream& s) {
    INFO_IN_AS_EMIT;
    s << MOVE << dest_reg << " " << source_reg ;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_neg(char *dest, char *src1, ostream& s) {
    INFO_IN_AS_EMIT;
    s << NEG << dest << " " << src1;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_add(char *dest, char *src1, char *src2, ostream& s) {
    INFO_IN_AS_EMIT;
    s << ADD << dest << " " << src1 << " " << src2;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_addu(char *dest, char *src1, char *src2, ostream& s) {
    INFO_IN_AS_EMIT;
    s << ADDU << dest << " " << src1 << " " << src2;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_addiu(char *dest, char *src1, int imm, ostream& s) {
    INFO_IN_AS_EMIT;
    s << ADDIU << dest << " " << src1 << " " << imm;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_div(char *dest, char *src1, char *src2, ostream& s) {
    INFO_IN_AS_EMIT;
    s << DIV << dest << " " << src1 << " " << src2;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_mul(char *dest, char *src1, char *src2, ostream& s) {
    INFO_IN_AS_EMIT;
    s << MUL << dest << " " << src1 << " " << src2;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_xor(char *dest_reg, char *src0, char *src1, ostream &s)
{
    s << XOR << dest_reg << " " << src0 << " " << src1;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
}

static void emit_sub(char *dest, char *src1, char *src2, ostream& s) {
    INFO_IN_AS_EMIT;
    s << SUB << dest << " " << src1 << " " << src2;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_sll(char *dest, char *src1, int num, ostream& s) {
    INFO_IN_AS_EMIT;
    s << SLL << dest << " " << src1 << " " << num;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_jalr(char *dest, ostream& s) {
    INFO_IN_AS_EMIT;
    s << JALR << dest;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_jal(const char *address, ostream &s) {
    INFO_IN_AS_EMIT;
    s << JAL << address;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_return(ostream& s) {
    INFO_IN_AS_EMIT;
    s << RET;
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_gc_assign(ostream& s) {
    INFO_IN_AS_EMIT;
    s << JAL << "_GenGC_Assign";
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_disptable_ref(Symbol sym, ostream& s) {
    INFO_IN_AS_EMIT;
    s << sym << DISPTAB_SUFFIX;
    INFO_OUT_AS_EMIT;
}

static void emit_init_ref(Symbol sym, ostream& s) {
    INFO_IN_AS_EMIT;
    s << sym << CLASSINIT_SUFFIX;
    INFO_OUT_AS_EMIT;
}

static void emit_label_ref(int l, ostream &s) {
    INFO_IN_AS_EMIT;
    s << "label" << l;
    INFO_OUT_AS_EMIT;
}

static void emit_protobj_ref(Symbol sym, ostream& s) {
    INFO_IN_AS_EMIT;
    s << sym << PROTOBJ_SUFFIX;
    INFO_OUT_AS_EMIT;
}

static void emit_method_ref(Symbol classname, Symbol methodname, ostream& s) {
    INFO_IN_AS_EMIT;
    s << classname << METHOD_SEP << methodname;
    INFO_OUT_AS_EMIT;
}

static void emit_label_def(int l, ostream &s) {
    INFO_IN_AS_EMIT;
    emit_label_ref(l, s);
    s << ":";
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_beqz(char *source, int label, ostream &s) {
    INFO_IN_AS_EMIT;
    s << BEQZ << source << " ";
    emit_label_ref(label, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s) {
    INFO_IN_AS_EMIT;
    s << BEQ << src1 << " " << src2 << " ";
    emit_label_ref(label, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_bne(char *src1, char *src2, int label, ostream &s) {
    INFO_IN_AS_EMIT;
    s << BNE << src1 << " " << src2 << " ";
    emit_label_ref(label, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s) {
    INFO_IN_AS_EMIT;
    s << BLEQ << src1 << " " << src2 << " ";
    emit_label_ref(label, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_blt(char *src1, char *src2, int label, ostream &s) {
    INFO_IN_AS_EMIT;
    s << BLT << src1 << " " << src2 << " ";
    emit_label_ref(label, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_blti(char *src1, int imm, int label, ostream &s) {
    INFO_IN_AS_EMIT;
    s << BLT << src1 << " " << imm << " ";
    emit_label_ref(label, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_bgti(char *src1, int imm, int label, ostream &s) {
    INFO_IN_AS_EMIT;
    s << BGT << src1 << " " << imm << " ";
    emit_label_ref(label, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static void emit_branch(int l, ostream& s) {
    INFO_IN_AS_EMIT;
    s << BRANCH;
    emit_label_ref(l, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//

static void emit_push(char *reg, ostream& s) {
    INFO_IN_AS_EMIT;
    emit_store(reg, 0, SP, s);
    emit_addiu(SP, SP, -4, s);
    INFO_OUT_AS_EMIT;
}

static void emit_pop( char *reg, ostream& s)
{
    INFO_IN_AS_EMIT;
    emit_addiu(SP, SP, 4, s);
    emit_load(reg, 0, SP, s);
    INFO_OUT_AS_EMIT;
}

static void emit_new( Symbol name, ostream &s)
{
    INFO_IN_AS_EMIT;
    emit_partial_load_address( ACC, s);
    emit_protobj_ref( name, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << "\n";
    s << JAL;
    emit_method_ref( Object, copy, s);
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << "\n";
    INFO_IN_AS_EMIT;
}

static void emit_abort( int label, int lineno, char *dest_addr, ostream &s)
{
    emit_bne(ACC, ZERO, label, s);
    emit_load_string(ACC,
         stringtable.lookup_string(
                curr_node->filename->get_string()), s);
    emit_load_imm(T1, lineno, s);
    emit_jal(dest_addr, s);
}

// Push entry function header
//Object_init:
//        addiu   $sp $sp -12 # push 1
//        sw      $fp 12($sp) # push fp to stack
//        sw      $s0 8($sp)  # push SELF to stack
//        sw      $ra 4($sp)  # push $ra to stack (return address!)
//        addiu   $fp $sp 4   # increese $fp
//        move    $s0 $a0     # save previous return value to s0
static void emit_push_header(ostream& s, int size)
{
    emit_addiu(SP, SP, -(FRAME_OFFSET+size)* WORD_SIZE, s);
    // Save frame pointer
    emit_store(FP, 3, SP, s);
    // save self pointer
    emit_store(SELF, 2, SP, s);
    // save return address
    emit_store(RA, 1, SP, s);
    // save register $s1
    emit_addiu(FP, SP, WORD_SIZE, s);
    // save SELF in acc
    emit_move( SELF, ACC, s);
}

//        move    $a0 $s0     # pop previous return value to a0
//        lw      $fp 12($sp) # pop previous frame pointer
//        lw      $s0 8($sp)  # pop previous SELF
//        lw      $ra 4($sp)  # pop previous return address
//        addiu   $sp $sp 12  # decrease sp

//        jr      $ra
static void emit_pop_header(ostream& s, int size)
{
    // restore previous fp
    emit_load(FP, 3, SP, s);
    // restore previous SELF
    emit_load(SELF, 2, SP, s);
    // restore previous return address
    emit_load(RA, 1, SP, s);
    // decreese sp
    emit_addiu(SP, SP, (FRAME_OFFSET + size) * WORD_SIZE, s);
}

//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//

static void emit_fetch_int(char *dest, char *source, ostream& s) {
    INFO_IN_AS_EMIT;
    emit_load(dest, DEFAULT_OBJFIELDS, source, s);
    INFO_OUT_AS_EMIT;
}

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//

static void emit_store_int(char *source, char *dest, ostream& s) {
    INFO_IN_AS_EMIT;
    emit_store(source, DEFAULT_OBJFIELDS, dest, s);
    INFO_OUT_AS_EMIT;
}

/// Begin here

static void emit_test_collector(ostream &s) {
    INFO_IN_AS_EMIT;
    emit_push(ACC, s);
    emit_move(ACC, SP, s); // stack end
    emit_move(A1, ZERO, s); // allocate nothing
    s << JAL << gc_collect_names[cgen_Memmgr];
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    emit_addiu(SP, SP, 4, s);
    emit_load(ACC, 0, SP, s);
    INFO_OUT_AS_EMIT;
}

static void emit_gc_check(char *source, ostream &s) {
    INFO_IN_AS_EMIT;
    if (source != (char*) A1) emit_move(A1, source, s);
    s << JAL << "_gc_check";
#ifdef DEBUG_AS_NAMES
    s << " # " << function_name;
#endif
    s << endl;
    INFO_OUT_AS_EMIT;
}

static int create_label()
{
    static int label = 0;
    return label++;
}

static int temp_offset = 0;
static void init_alloc_temp()
{
	temp_offset = 0;
}
static int alloc_temp()
{
	return temp_offset++;
}


