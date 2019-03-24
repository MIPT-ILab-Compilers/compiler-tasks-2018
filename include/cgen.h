#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

#define FRAME_OFFSET 3

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;

CgenNodeP curr_node = nullptr;

class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
   List<CgenNode> *nds;
   ostream& s;
   int stringclasstag = -1;
   int intclasstag = -1;
   int boolclasstag = -1;
   int currclasstag = 0;

// The following methods emit code for
// constants and global declarations.

   void code_global_data();
   void code_global_text();
   void code_bools(int);
   void code_select_gc();
   void code_constants();

   void code_classNameTab();
   void code_classObjTab();
   void code_protObjs();
   void code_dispTabs();
   void code_methods();
   void code_initCode();

// The following creates an inheritance graph from
// a list of classes.  The graph is implemented as
// a tree of `CgenNode', and class names are placed
// in the base class symbol table.

   void install_basic_classes();
   void install_class(CgenNodeP nd);
   void install_classes(Classes cs);
   void build_inheritance_tree();
   void set_relations(CgenNodeP nd);
public:
   CgenClassTable(Classes, ostream& str);
   void code();
   CgenNodeP root();
};


class CgenNode : public class__class {
private: 
   CgenNodeP parentnd;                        // Parent of class
   List<CgenNode> *children;                  // Children of class
   Basicness basic_status;                    // `Basic' if class is basic
                                              // `NotBasic' otherwise   
   int id;                                    // Current class tag
   int max_inherits_id = 0;
   Environment attrTable;
   Environment methodTable;

public:
   CgenNode(Class_ c,
            Basicness bstatus,
            CgenClassTableP class_table);

   void add_child(CgenNodeP child);
   List<CgenNode> *get_children() { return children; }
   void set_parentnd(CgenNodeP p);
   CgenNodeP get_parentnd() { return parentnd; }
   int basic() {
       return (basic_status == Basic);
   }

   void set_id(int _id) { id = _id;}
   //   methods after full initialization
   void code_ref(ostream&);
   void code_disp(ostream&);
   void code_prot(ostream&);
   void code_attr_prot(ostream&);
   int calc_temp();
   void emit_init(ostream&, int);
   int  get_attr_num();
   int  get_id() {return id;}
   int  get_max_id() {return max_inherits_id;}
   Environment get_attr_table() {return attrTable;}
   Environment get_method_table() {return methodTable;}
   void fill_table();
};

//	max_class_tag = class_tag;
//	for ( List<CgenNode> *leg = children; leg; leg = leg->tl())
//	{
//		leg->hd()->walk_down();
//		max_class_tag = max( max_class_tag, leg->hd()->max_class_tag);
//	}      


class BoolConst 
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};

