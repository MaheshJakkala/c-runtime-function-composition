//a.c for higher order functions in c
#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<assert.h>
#include<string.h>
//#include<birchutils.h>
#define zero(p, size) memset((p), 0, (size))
#include <stdint.h>
#include <sys/mman.h> 

typedef uint8_t  int8;
typedef uint16_t int16;
typedef uint64_t int64;

//typedef unsigned int int8;
//typedef unsigned long long int int64;

#define $1 (int8*)
#define $i (int)
#define $c (char*)

struct s_tuple{
    void *addr;
    int16 size;
};
typedef struct s_tuple Tuple;

struct s_function{
    int8 machinecode[64];
    int (*call)(int);
};
typedef struct s_function function;

int8 pre[]="\x55\x89\xe5\x83\xec\x10";
int8 post[]="\x89\x45\xfc\x8b\x45\xfc\xc9\xc3";
//int8 ebp8[]="\x90\x90\x89\x45\x08\x90\x90";
int8 ebp8[] = "\x90\x90\x89\xc7\x90\x90"; // good: mov edi, eax + NOPs


#define prelen (sizeof(pre)-1)
#define postlen (sizeof(post)-1)
#define overhead (prelen+postlen)
#define ebp8len (sizeof(ebp8))

function compose(void*,void*);
//int8* compose(void*,void*);
void *findend(void*);
Tuple extract(void*);
int8* empty(int16);
int addone(int);
int x2(int);
int main(void);


function compose(void *f1, void *f2) {
    int8 *fun;
    Tuple f1_, f2_;
    function ret;

    f1_ = extract(f1);
    f2_ = extract(f2);

    // Remove RET from f1 if present
    if (((unsigned char*)f1_.addr)[f1_.size - 1] == 0xc3) {
        f1_.size -= 1;
    }

    // Allocate executable memory
    void *exec_mem = mmap(NULL, 64, PROT_READ | PROT_WRITE | PROT_EXEC,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (exec_mem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    fun = (int8 *)exec_mem;

    int8 *p = fun;
    memcpy(p, f1_.addr, f1_.size);
    p += f1_.size;

    memcpy(p, ebp8, ebp8len - 1);
    p += (ebp8len - 1);

    memcpy(p, f2_.addr, f2_.size);
    p += f2_.size;

    ret.call = (int (*)(int)) fun;
    memcpy(ret.machinecode, fun, 64);

    return ret;
}



/*function compose(void *f1, void *f2)
{
    int8 *fun, *p;
    Tuple f1_, f2_;
    function ret;

    fun = empty(64);
    f1_ = extract(f1);
    f2_ = extract(f2);

    // Remove the last byte (RET) from f1
    if (((unsigned char*)f1_.addr)[f1_.size - 1] == 0xc3) {
        //printf("\nhello\n");
        f1_.size -= 1;
    }

    p = fun ;
    memcpy($c p, $c f1_.addr, $i f1_.size);
    p += f1_.size;

    memcpy($c p, ebp8, (ebp8len - 1));
    p += (ebp8len - 1);

    memcpy($c p, $c f2_.addr, $i f2_.size);

    
    //memcpy($c &ret,$c fun,64);



    memcpy(ret.machinecode, fun, 64);
    ret.call = (int (*)(int)) ret.machinecode;


// void *exec_mem = mmap(NULL, 64, PROT_READ | PROT_WRITE | PROT_EXEC,
//                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
// if (exec_mem == MAP_FAILED) {
//     perror("mmap");
//     exit(1);
// }

// memcpy(exec_mem, fun, len); // Use correct len
// ret.call = (int (*)(int)) exec_mem;


    //int len = f1_.size + (ebp8len - 1) + f2_.size;
    // printf("Composed function:\n");
    // for (int i = 0; i < len; i++) {
    //     printf("%02x ", (unsigned char)fun[ i]);
    // }
    // printf("\n");

    return ret;
}*/
//


/*
int8* compose(void *f1,void *f2)
{
    int8*fun,*p;
    Tuple f1_,f2_;

    fun = empty(64);
    f1_=extract(f1);
    f2_ = extract(f2);
    p=fun+prelen;
    memcpy($c p,$c f1_.addr,$i f1_.size);

    printf("before composing: \n");
    unsigned char *orig = (unsigned char *) f1_.addr;
     for (int i = 0; i < f1_.size; i++) {
         printf("%02x ", orig[i]);
     }
     printf("\n");
     orig = (unsigned char *) f2_.addr;
 for (int i = 0; i < f2_.size; i++) {
     printf("%02x ", orig[i]);
 }
 printf("\n");

    p+=f1_.size;

    printf("upto f1:\n");
    for (int i = 0; i < f1_.size; i++) {
        printf("%02x ", (unsigned char)fun[i]);
    }
    printf("\n");

    memcpy($c p,ebp8,(ebp8len-1));
    p+=(ebp8len-1);

    // printf("upto ebp8:\n");
    // for (int i = 0; i < ((ebp8len-1)+f1_.size); i++) {
    //     printf("%02x ", (unsigned char)fun[i]);
    // }
    // printf("\n");

    memcpy($c p,$c f2_.addr,$i f2_.size);
   // p+=f2_.size;
   int len=f1_.size+f2_.size;
   printf("upto f2:\n");
   for (int i = 0; i < len; i++) {
       printf("%02x ", (unsigned char)fun[i]);
   }
   printf("\n");

   
    printf("Composing functions: f1 size=%d, f2 size=%d\n Composed content: \n", f1_.size, f2_.size);
    for (int i = 0; i <len ; i++) {
        printf("%02x ", (unsigned char)p[i]);
    }
    printf("\n");
    return fun;
}*/

Tuple extract(void*fun)
{
    void*start,*end;
    int16 size;
    int8* p;
    start=fun;
   // printf(" start: %p,x2 add: %p\n",start,fun);
    
    end=findend(fun)+1;
    size=end-start;
    printf("size : %d\n",$i size);
    p=(int8*)malloc($i size);
    assert(p);
    zero(p,size);
    memcpy($1 p,$1 start,$i size);

    Tuple ret={
        .addr=p,
        .size=size
    };
    // printf("Function extracted: Start=%p, End=%p, Size=%d\n", start, end, size);
    // for(int i=0;i<size;i++)
    // printf("%02x ",(unsigned int)p[i]);
    // printf("\n\n");
    return ret;
}       

// int8* empty(int16 size)
// {
//     int8*p;
//     assert(size>=overhead+1);
//     p=(int8*)malloc(size);
//     zero(p,size);

//     memset($c p,0x90,$i size);
//     memcpy(p,pre,prelen);
//     memcpy(p+size-postlen, post, postlen);

//     return p;
// }


int8* empty(int16 size)
{
    int8 *p;
    assert(size >= overhead + 1);

    // Allocate memory that is executable
    p = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Fill with NOPs
    memset(p, 0x90, size);
    memcpy(p, pre, prelen);
    memcpy(p + size - postlen, post, postlen);

    return p;
}


int addone(int x)
{
    int ret;
    ret=x+1;
    return ret;
}

int x2(int x){
    int ret;
    ret=x*2;
    return ret;
}

void*findend(void*fun)
{
    int8* p;
    void *end;
    int16 n;

    for(p=fun,n=0;n<1024; p++,n++)
    {
        if((*p==0xc3) || (*p==0xc9))
        {
            //end=p-1;
            end=p;
            break;
        }
    }
    
    return end;
}

int main()
{
    void*mem;
    function fun;
    //int8* fun;
   // int (*f)(int);
    int x;

    fun=compose(&addone,&x2);
    //mem=&fun;
    //mem=fun;
    //fun.call=mem;
    //fun.call = (int (*)(int)) fun.machinecode;

    x=fun.call(5);
    printf("x= %d\n",x);
    exit(0);

    return 0;
}

/*
int main()
{
    int (*f)(int);
    int8* fun;
    int8* p;
    void *mem;
    Tuple essence;
    int x=0;

    fun = empty(64);
    essence = extract(&x2);
    p = fun ;
    memcpy($1 p, $1 essence.addr, $i essence.size);
    mem =fun;
    f=mem;
    //f=x2;
printf("p:%p , f: %p\n", (void*)p,(void*)f);
printf("contents of p:\n ");
for (int i = 0; i < essence.size; i++) {
    printf("%02x ", (unsigned char)p[i]);
}
printf("\n");

printf("contents of extracted(essence):\n ");
unsigned char *orig = (unsigned char *) essence.addr;
for (int i = 0; i < essence.size; i++) {
    printf("%02x ", orig[i]);
}
printf("\n");

printf("essence.addr = %p\n", essence.addr);
printf("x2           = %p\n", (void*)x2);

printf("essence.size = %d\n", essence.size);



printf("\n");


    x=f(5);
    printf("x= %d\n",x);
    exit(0);

    return 0;
}*/