
#include <stdio.h>
#include <string.h>
#include "admin.h"
#include "train.h"
#include "user.h"
#include "order.h"

int admin_login(const char *username, const char *password) {
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        return 0;
    }
    return -1;
}

void admin_add_train() {
    Train t;
    printf("输入车次号: "); scanf("%s", t.train_no);
    if (find_train_index(t.train_no) != -1) {
        printf("车次已存在\n"); return;
    }
    printf("出发站: "); scanf("%s", t.start_station);
    printf("到达站: "); scanf("%s", t.end_station);
    printf("出发时间(HH:MM): "); scanf("%s", t.start_time);
    printf("到达时间(HH:MM): "); scanf("%s", t.end_time);
    printf("历时(分钟): "); scanf("%d", &t.duration);
    printf("一等座票价: "); scanf("%f", &t.price_first);
    printf("一等座余票: "); scanf("%d", &t.stock_first);
    printf("二等座票价: "); scanf("%f", &t.price_second);
    printf("二等座余票: "); scanf("%d", &t.stock_second);
    
    add_train(&t);
    printf("添加成功\n");
}

void admin_delete_train() {
    char no[20];
    printf("输入要删除的车次号: "); scanf("%s", no);
    delete_train(no);
    printf("删除操作完成\n");
}

void admin_modify_stock() {
    char no[20];
    printf("输入车次号: "); scanf("%s", no);
    int idx = find_train_index(no);
    if (idx == -1) { printf("车次不存在\n"); return; }
    
    Train *t = get_train_by_index(idx);
    printf("当前一等座: %d, 二等座: %d\n", t->stock_first, t->stock_second);
    printf("新一等座余票: "); scanf("%d", &t->stock_first);
    printf("新二等座余票: "); scanf("%d", &t->stock_second);
    save_trains();
    printf("更新成功\n");
}

void admin_view_all_users() {
    printf("%-15s %-15s %-20s\n", "用户名", "姓名", "手机");
    int i = 0;
    for (i = 0; i < get_user_count(); i++) {
        User *u = get_user_by_index(i);
        printf("%-15s %-15s %-20s\n", u->username, u->real_name, u->phone);
    }
}

void admin_view_all_orders() {
    int count = get_order_count(); // 修复：使用新增的函数获取订单数量
    printf("%-10s %-10s %-8s %-12s %-8s %-8s %-10s\n", 
           "订单号", "用户", "车次", "日期", "席别", "价格", "状态");
    printf("--------------------------------------------------------------\n");
    int i = 0;
    for (i = 0; i < count; i++) {
        Order *o = get_order_by_index(i); // 修复：使用新增的函数获取订单
        if (o) {
            char *status_str = "未知";
            switch(o->status) {
                case STATUS_PENDING: status_str = "待支付"; break;
                case STATUS_PAID: status_str = "已支付"; break;
                case STATUS_REFUNDED: status_str = "已退票"; break;
                case STATUS_CHANGED: status_str = "已改签"; break;
            }
            printf("%-10d %-10s %-8s %-12s %-8s %-8.2f %-10s\n",
                   o->order_id, o->username, o->train_no, o->date,
                   o->seat_type, o->price, status_str);
        }
    }
    if (count == 0) {
        printf("暂无订单\n");
    }
}