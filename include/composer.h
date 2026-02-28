#ifndef COMPOSER_H
#define COMPOSER_H

typedef struct {
    unsigned char machinecode[64];
    int (*call)(int);
} function;

function compose(void *f1, void *f2);

#endif