#include "global.h"
#include "global_ir.h"
#ifndef _SEMANTIC_
#define _SEMANTIC_

int semanticAnalyze(int);                             //done
int ProgramAnalyze(int);                              //done
int ExtDefListAnalyze(int);                           //done
int ExtDefAnalyze(int);                               //done
int SpecifierAnalyze(int, Type *);                    //done
int ExtDecListAnalyze(int, Type *);                   //done
int FunDecAnalyze(int index, Type *type, TypeKind);   //done
int CompStAnalyze(int, Symbol *);                     //done
int VarDecAnalyze(int, Type *, Field *, SymbolKind);  //done
int StructAnalyze(int, Type *);                       //done
char *OptTagAnalyze(int);                             //done
int DefListAnalyze(int, Field *, SymbolKind);         //done
int DefAnalyze(int, Field *, SymbolKind);             //done
char *TagAnalyze(int);                                //done
Field *VarListAnalyze(int, TypeKind);                 //done
Field *ParamDecAnalyze(int, TypeKind);                //done
int UndefFuncParamAnalyze(int, Type*, Field*);
int StmtListAnalyze(int, Symbol *);                   //done
Type *ExpAnalyze(int);                                //done
int StmtAnalyze(int, Symbol *);                       //done
int DecListAnalyze(int, Type *, Field *, SymbolKind); //done
int DecAnalyze(int, Type *, Field *, SymbolKind);     //done
Field *ArgsAnalyze(int);                              //done
int IntAnalyze(int);

#endif