// user.h
#ifndef USER_H
#define USER_H

#define USERNAME_MAX 32

typedef struct {
    char username[USERNAME_MAX];
    int id;
    int group_id;
} User;

#endif
