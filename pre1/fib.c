#include "stdio.h"
#define x 0
#define y 1
int add(int a,int b){
    return a+b;
}
int main(){
    int a,b,i,t,n;
    a=x;
    b=y;
    const int j=1;
    i=j;
    //这是注释
    scanf("%d",&n);
    printf("%d\n",a);
    printf("%d\n",b);
    while(i<n){
        t=b;
        b=add(a,b);
        printf("%d",b);
        a=t;
        i=i+1;
    }
}
