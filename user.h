#ifndef USER_H
#define USER_H

// ===== 宏定义必须在最前面 =====
#define MAX_NAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_ID_LEN 20
#define MAX_PHONE_LEN 20


typedef struct {
    char username[MAX_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char real_name[MAX_NAME_LEN];
    char id_card[MAX_ID_LEN];
    char phone[MAX_PHONE_LEN];
} User;


void init_users();
int load_users();
void save_users();
int register_user(const char *username, const char *password, const char *real_name, const char *id_card, const char *phone);
int login_user(const char *username, const char *password, User *out_user);
int find_user_index(const char *username);
User* get_user_by_index(int index);
int get_user_count();
void update_user_profile(int index, const char *real_name, const char *phone);

#endif