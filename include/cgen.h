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


class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
   List<CgenNode> *nds;
   ostream& str;
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
    // FIXME - check it
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
   Environment get_attr_table() {return attrTable;}
   Environment get_method_table() {return methodTable;}
   void fill_table() {
      int attr_offset = FRAME_OFFSET;
      int meth_offset = 0;
      std::cout << get_name() << "\n";
      if (parentnd)
      {
        //std::cerr << "call for " << get_name() << "\n";
        parentnd->fill_table();
        attrTable = new std::vector<EnvElement>(
            *parentnd->get_attr_table());
        attr_offset += attrTable->size();
        methodTable = new std::vector<EnvElement>(
            *parentnd->get_method_table());
        meth_offset += methodTable->size();
      }
      else
      {
          attrTable = new std::vector<EnvElement>();
          methodTable = new std::vector<EnvElement>();
      }

      for(int i = features->first();
              features->more(i);
              i = features->next(i))
      {
          Feature_class * f = features->nth(i);
          method_class * m = dynamic_cast<method_class*>(f);
          attr_class * a = dynamic_cast<attr_class*>(f);
          if (m)
          {
              auto pred = [=](EnvElement a){return a.name == m->get_name();};
              auto prev = std::find_if(
                                 methodTable->begin(),
                                 methodTable->end(),
                                 pred);
              EnvElement new_elem = EnvElement(get_name(), m->get_name(), meth_offset);
              if (prev == methodTable->end())
              {
                  std::cout << " for " << m->get_name() << "\n";
                  methodTable->push_back(new_elem);
                  ++meth_offset;
              }
              else
              {
                  std::replace_if(methodTable->begin(), methodTable->end(),
                            pred, new_elem);
              }
          }
          if (a)
          {
              attrTable->push_back(EnvElement(get_name(), a->get_name(), attr_offset));
              ++attr_offset;

          }
      }

   }
};

class BoolConst 
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};

