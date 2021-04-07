#include "semantic.h"

int programAnalyze(int last_node){
    ASTNode node = nodes[last_node];
    ASTNode child = nodes[node.child];
    do{
        // printf("%s %s %s\n", child.name, child.value, node.name);
        ExtDefListAnalyze(node.child);
    }while(child.brother!=-1);
    return 0;
}


int ExtDefListAnalyze(int last_node){
    ASTNode ExtDefList = nodes[last_node];
    //type = 1 represent ExtDefList -> ExtDef ExtDefList
    if(ExtDefList.type==ExtDefList_ExtDefExtDefList){
        int ExtDef = ExtDefList.child;
        int ExtDefList = nodes[ExtDef].brother;
        ExtDefAnalyze(ExtDef);
        ExtDefListAnalyze(ExtDefList);
    }
    else{

    }
    return 0;
}

int ExtDefAnalyze(int ExtDef){
    ASTNode ExtDefNode = nodes[ExtDef];
    int child = ExtDefNode.child;
    ASTNode childNode = nodes[child];
    switch (ExtDefNode.type)
    {
    case ExtDef_SpecifierExtDecListSEMI:
        break;
    case ExtDef_SpecifierSEMI:
        break;
    case ExtDef_SpecifierFunDecSEMI:
        SpecifierAnalyze(child);
        FunDecAnalyze(childNode.brother);
        // childNode = nodes[childNode.brother];
        break;
    case ExtDef_SpecifierFunDecCompSt:
        break;
    default:
        break;
    }
    printf("%s %d\n", ExtDefNode.name, ExtDefNode.type);
}

int SpecifierAnalyze(int Specifier){
    ASTNode spe = nodes[Specifier];
    printf("%s %d\n", spe.name, spe.type);
}

int FunDecAnalyze(int funDec){
    ASTNode fun = nodes[funDec];
    printf("%s %d\n", fun.name, fun.type);
}

int semanticAnalyze(int last_node){
    return programAnalyze(last_node);
}