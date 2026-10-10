
#ifndef ADMIN_H
#define ADMIN_H

#include "train.h"
#include "user.h"
#include "order.h"

// 管理员功能声明
int admin_login(const char *username, const char *password);
void admin_add_train();
void admin_delete_train();
void admin_modify_stock();
void admin_view_all_users();
void admin_view_all_orders();

#endif
