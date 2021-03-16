#include <stdio.h>
#include <stdlib.h>

#define debug 1
#define n 50


int main(){
    
    int * mem = (int*) malloc(n * sizeof(int));
    printf("This is a simple switch case func\n");

    gen(n,mem);
    for(int i = 0; i < n; i++)
        vm_call(mem[i]);
}

void vm_call(int m){
    switch (m)
    {
    case 0:
#ifdef debug
        printf("0\n");
#else

#endif
        break;
    
    case 1:
#ifdef debug
        printf("1\n");
#else

#endif
        break;
    
    case 2:
#ifdef debug
        printf("2\n");
#else

#endif
        break;
    
    case 3:
#ifdef debug
        printf("3\n");
#else

#endif
        break;
    
    case 4:
#ifdef debug
        printf("4\n");
#else

#endif
        break;
    
    case 5:
#ifdef debug
        printf("5\n");
#else

#endif
        break;
    
    case 6:
#ifdef debug
        printf("6\n");
#else

#endif
        break;
    
    case 7:
#ifdef debug
        printf("7\n");
#else

#endif
        break;
    
    case 8:
#ifdef debug
        printf("8\n");
#else

#endif
        break;
    default:
        break;
    }
}


void gen(int m,int* mem){
    
    
    for(int i = 0; i < m; i++){
        mem[i] = rand() % 8;    
    }
    
}