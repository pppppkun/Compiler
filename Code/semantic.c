#include "semantic.h"

int programAnalyze(int last_node){
    ASTNode node = nodes[last_node];
    ASTNode child = nodes[node.child];
    do{
        // printf("%s %s %s\n", child.name, child.value, node.name);
        ExtDefListAnalyze(node.child);
    }while(child.brother!=-1);
}


int ExtDefListAnalyze(int last_node){
    ASTNode ExtDefList = nodes[last_node];
    //type = 1 represent ExtDefList -> ExtDef ExtDefList
    if(ExtDefList.type==1){
        
        ExtDefListAnalyze
    }
}

int semanticAnalyze(int last_node){
    programAnalyze(last_node);
}