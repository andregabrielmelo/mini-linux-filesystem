// user.h
#ifndef USER_H
#define USER_H

#define MAX_NAME 50

typedef struct {
    char name[MAX_NAME];
    int32_t uid;
} user;

extern user current_user;

#endif