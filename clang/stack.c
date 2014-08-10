
#include <stdlib.h>
#include <stdio.h>


typedef struct {
    int len, capacity;
    int *entries;
} Stack;


void initStack(Stack *s) {
    s->len = 0;
    s->capacity = 4;
    s->entries = malloc( s->capacity * sizeof(int) );
}


void destroyStack(Stack *s) {
    s-> len = 0;
    s-> capacity = 0;
    free(s->entries);
}


void push(Stack *s, int k) {
    if (s->len == s->capacity) {
        s->entries = (int *) realloc(s->entries, 2 * s->capacity * sizeof(int));
        s->capacity = 2 * s->capacity;
    }

    s->entries[s->len++] = k;
}


int getEntry(Stack *s, int i) {
    return s->entries[i];
}


void setEntry(Stack *s, int k, int i) {
    s->entries[k] = i;
}


int pop(Stack *s) {
    int k = s->entries[--s->len];

    if (s->len < s->capacity / 4 && s-> capacity > 8) {
        s->entries = (int *) realloc(s->entries, s->capacity / 2 * sizeof(int));
    }

    return k;
}



int main(int argc, char **argv) {
    Stack s;
    initStack(&s);

    for (int n = 0; n < 10; n++) {
        push(&s, n * n + n + 41);
    }

    int p;
    while(s.len > 0) {
        p = pop(&s);
        printf("%d ", p);
    }
    printf("\n");


    destroyStack(&s);

    return 0;
}


