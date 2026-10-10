#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "user.h"

#define MAX_USERS 100
#define FILE_USER "users.txt"  // 改为 .txt

static User users[MAX_USERS];
static int user_count = 0;

void init_users() {
    user_count = load_users();
}

int load_users() {
    FILE *fp = fopen(FILE_USER, "w");  // 改为文本读模式
    if (!fp) return 0;
    
    fscanf(fp, "%d\n", &user_count);  // 读取数量
    if (user_count > MAX_USERS) user_count = MAX_USERS;
    
    int i;
    for (i = 0; i < user_count; i++) {
        fscanf(fp, "%[^\n]\n", users[i].username);
        fscanf(fp, "%[^\n]\n", users[i].password);
        fscanf(fp, "%[^\n]\n", users[i].real_name);
        fscanf(fp, "%[^\n]\n", users[i].id_card);
        fscanf(fp, "%[^\n]\n", users[i].phone);
    }
    
    fclose(fp);
    return user_count;
}

void save_users() {
    FILE *fp = fopen(FILE_USER, "w");  // 改为文本写模式
    if (!fp) return;
    
    fprintf(fp, "%d\n", user_count);  // 写入数量
    
    int i;
    for (i = 0; i < user_count; i++) {
        fprintf(fp, "%s\n", users[i].username);
        fprintf(fp, "%s\n", users[i].password);
        fprintf(fp, "%s\n", users[i].real_name);
        fprintf(fp, "%s\n", users[i].id_card);
        fprintf(fp, "%s\n", users[i].phone);
    }
    
    fclose(fp);
}

int find_user_index(const char *username) {
    int i;
    for (i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) return i;
    }
    return -1;
}

int register_user(const char *username, const char *password, const char *real_name, const char *id_card, const char *phone) {
    User u;
    if (user_count >= MAX_USERS) return -1;
    if (find_user_index(username) != -1) return -2;
    
    memset(&u, 0, sizeof(User));
    strncpy(u.username, username, MAX_NAME_LEN-1);
    u.username[MAX_NAME_LEN-1] = '\0';
    strncpy(u.password, password, MAX_PASSWORD_LEN-1);
    u.password[MAX_PASSWORD_LEN-1] = '\0';
    strncpy(u.real_name, real_name, MAX_NAME_LEN-1);
    u.real_name[MAX_NAME_LEN-1] = '\0';
    strncpy(u.id_card, id_card, MAX_ID_LEN-1);
    u.id_card[MAX_ID_LEN-1] = '\0';
    strncpy(u.phone, phone, MAX_PHONE_LEN-1);
    u.phone[MAX_PHONE_LEN-1] = '\0';
    
    users[user_count++] = u;
    save_users();
    return 0;
}

int login_user(const char *username, const char *password, User *out_user) {
    int idx = find_user_index(username);
    if (idx == -1) return -1;
    if (strcmp(users[idx].password, password) != 0) return -2;
    
    *out_user = users[idx];
    return idx;
}

User* get_user_by_index(int index) {
    if (index < 0 || index >= user_count) return NULL;
    return &users[index];
}

int get_user_count() {
    return user_count;
}

void update_user_profile(int index, const char *real_name, const char *phone) {
    if (index < 0 || index >= user_count) return;
    if (real_name && strlen(real_name) > 0) {
        strncpy(users[index].real_name, real_name, MAX_NAME_LEN-1);
        users[index].real_name[MAX_NAME_LEN-1] = '\0';
    }
    if (phone && strlen(phone) > 0) {
        strncpy(users[index].phone, phone, MAX_PHONE_LEN-1);
        users[index].phone[MAX_PHONE_LEN-1] = '\0';
    }
    save_users();
}