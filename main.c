#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "train.h"
#include "user.h"
#include "order.h"
#include "admin.h"
void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void wait_for_enter() {
    printf("\n按回车键继续...");
    while (getchar() != '\n'); // 清除输入缓冲区
    getchar();
}

void print_train_info(Train *t) {
    if (!t) return;
    printf("| %-8s | %-8s -> %-8s | %s-%s | %dh%dm | 一等: %.1f(%d) | 二等: %.1f(%d) |\n",
           t->train_no, t->start_station, t->end_station,
           t->start_time, t->end_time,
           t->duration / 60, t->duration % 60,
           t->price_first, t->stock_first,
           t->price_second, t->stock_second);
}

void search_trains_ui() {
    char start[50], end[50], date[20];
    printf("出发站: "); scanf("%49s", start);
    printf("到达站: "); scanf("%49s", end);
    printf("日期(YYYY-MM-DD): "); scanf("%19s", date);
    
    printf("\n查询结果:\n");
    printf("------------------------------------------------------------------------------------------\n");
    printf("| 车次   | 路线               | 时间       | 历时   | 票价及余票                     |\n");
    printf("------------------------------------------------------------------------------------------\n");
    
    int found = 0;
    int i1=0;
    for (i1 = 0; i1 < get_train_count(); i1++) {
        Train *t = get_train_by_index(i1);
        if (t && strcmp(t->start_station, start) == 0 && strcmp(t->end_station, end) == 0) {
            print_train_info(t);
            found = 1;
        }
    }
    if (!found) printf("未找到车次。\n");
}

void buy_ticket_ui(const char *username) {
    char train_no[20], date[20];
    printf("车次号: "); scanf("%19s", train_no);
    printf("日期: "); scanf("%19s", date);
    
    int t_idx = find_train_index(train_no);
    if (t_idx == -1) { printf("车次不存在\n"); return; }
    
    Train *t = get_train_by_index(t_idx);
    if (!t) { printf("获取车次信息失败\n"); return; }
    
    printf("1.一等座(%.2f) 2.二等座(%.2f)\n选择: ", t->price_first, t->price_second);
    int choice; 
    if (scanf("%d", &choice) != 1) { // 检查输入
        printf("无效输入\n");
        return;
    }
    
    char seat_type[10];
    float price;
    if (choice == 1) { strcpy(seat_type, "FIRST"); price = t->price_first; }
    else if (choice == 2) { strcpy(seat_type, "SECOND"); price = t->price_second; }
    else { printf("无效选择\n"); return; }
    
    int oid = create_order(username, train_no, date, seat_type, price);
    if (oid > 0) {
        printf("订单生成: %d, 金额: %.2f. 是否支付? (1:是, 0:否): ", oid, price);
        int p; 
        scanf("%d", &p);
        if (p == 1) {
            if (pay_order(oid) == 0) {
                printf("支付成功\n");
            } else {
                printf("支付失败\n");
            }
        } else {
            printf("请后续在个人中心支付\n");
        }
    } else {
        printf("购票失败(错误码:%d)\n", oid);
    }
}

void view_orders_ui(const char *username) {
    Order my_orders[100];
    int count = 0;
    get_user_orders(username, my_orders, &count);
    
    printf("\n我的订单:\n");
    if (count == 0) {
        printf("暂无订单\n");
        return;
    }
    
    printf("%-10s %-8s %-10s %-8s %-10s %-8s\n", "订单号", "车次", "日期", "席别", "价格", "状态");
    int i2=0;
    for (i2 = 0; i2 < count; i2++) {
        const char *status_str = "未知";
        switch(my_orders[i2].status) {
            case STATUS_PENDING: status_str = "待支付"; break;
            case STATUS_PAID: status_str = "已支付"; break;
            case STATUS_REFUNDED: status_str = "已退票"; break;
            case STATUS_CHANGED: status_str = "已改签"; break;
        }
        printf("%-10d %-8s %-10s %-8s %-10.2f %-8s\n",
               my_orders[i2].order_id, my_orders[i2].train_no, my_orders[i2].date,
               my_orders[i2].seat_type, my_orders[i2].price, status_str);
    }
}

void refund_ui(const char *username) {
    view_orders_ui(username);
    int oid;
    printf("输入退票订单号(0取消): "); 
    scanf("%d", &oid);
    if (oid == 0) return;
    
    if (refund_order(oid) == 0) printf("退票成功\n");
    else printf("退票失败，请检查订单状态\n");
}

void change_ui(const char *username) {
    view_orders_ui(username);
    int oid;
    printf("输入改签订单号(0取消): "); 
    scanf("%d", &oid);
    if (oid == 0) return;
    
    Order *o = get_order_by_id(oid);
    if (!o || strcmp(o->username, username) != 0 || o->status != STATUS_PAID) {
        printf("无效订单(订单不存在、不属于您或状态不是已支付)\n"); 
        return;
    }
    
    // 修复：保存旧订单信息避免悬空指针
    float old_price = o->price;
    char old_seat[10];
    strcpy(old_seat, o->seat_type);
    
    printf("原车次: %s, 原价格: %.2f\n", o->train_no, old_price);
    printf("请输入新车次信息:\n");
    char new_train[20], new_date[20];
    printf("新车次: "); scanf("%19s", new_train);
    printf("新日期: "); scanf("%19s", new_date);
    
    int t_idx = find_train_index(new_train);
    if (t_idx == -1) { printf("新车次不存在\n"); return; }
    
    Train *t = get_train_by_index(t_idx);
    printf("1.一等座(%.2f) 2.二等座(%.2f)\n选择: ", t->price_first, t->price_second);
    int c; 
    scanf("%d", &c);
    char new_seat[10]; 
    float new_price;
    if (c == 1) { strcpy(new_seat, "FIRST"); new_price = t->price_first; }
    else if (c == 2) { strcpy(new_seat, "SECOND"); new_price = t->price_second; }
    else { printf("无效选择\n"); return; }
    
    int result = change_order(oid, new_train, new_date, new_seat, new_price);
    if (result == 0) {
        float diff = new_price - old_price; // 使用保存的旧价格
        printf("改签成功. 差价: %.2f\n", diff);
    } else {
        printf("改签失败(错误码:%d)\n", result);
    }
}

void user_menu(const char *username) {
    int choice;
    while(1) {
        clear_screen();
        printf("\n--- 用户菜单 (当前用户: %s) ---\n", username);
        printf("1.查票 2.购票 3.我的订单 4.退票 5.改签 6.修改资料 7.退出\n选择: ");
        if (scanf("%d", &choice) != 1) {
            printf("无效输入\n");
            while (getchar() != '\n'); // 清除错误输入
            continue;
        }
        
        if (choice == 7) break;
        switch(choice) {
            case 1: search_trains_ui(); break;
            case 2: buy_ticket_ui(username); break;
            case 3: view_orders_ui(username); break;
            case 4: refund_ui(username); break;
            case 5: change_ui(username); break;
            case 6: {
                char name[50] = "", phone[20] = "";
                while (getchar() != '\n'); // 清除缓冲区
                printf("新姓名(回车跳过): "); 
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                printf("新手机(回车跳过): "); 
                fgets(phone, sizeof(phone), stdin);
                phone[strcspn(phone, "\n")] = 0;
                int idx = find_user_index(username);
                if (idx >= 0) {
                    update_user_profile(idx, name, phone);
                    printf("资料更新成功\n");
                }
                break;
            }
            default: printf("无效选择\n");
        }
        wait_for_enter();
    }
}

void admin_menu() {
    int choice;
    while(1) {
        clear_screen();
        printf("\n--- 管理员菜单 ---\n");
        printf("1.加车次 2.删车次 3.改余票 4.看用户 5.看订单 6.退出\n选择: ");
        if (scanf("%d", &choice) != 1) {
            printf("无效输入\n");
            while (getchar() != '\n');
            continue;
        }
        
        if (choice == 6) break;
        switch(choice) {
            case 1: admin_add_train(); break;
            case 2: admin_delete_train(); break;
            case 3: admin_modify_stock(); break;
            case 4: admin_view_all_users(); break;
            case 5: admin_view_all_orders(); break;
            default: printf("无效选择\n");
        }
        wait_for_enter();
    }
}

int main() {
    system("chcp 65001 > nul");
    init_trains();
    init_users();
    init_orders();
    
    int choice;
    while(1) {
        clear_screen();
        printf("=== 铁路购票系统===\n");
        printf("1.注册 2.登录 3.管理员 4.退出\n选择: ");
        if (scanf("%d", &choice) != 1) {
            printf("无效输入\n");
            while (getchar() != '\n');
            continue;
        }
        
        if (choice == 4) {
            save_trains(); save_users(); save_orders();
            printf("感谢使用，再见！\n");
            break;
        }
        
        switch(choice) {
            case 1: {
                char u[50], p[50], n[50], id[20], ph[20];
                printf("用户名: "); scanf("%49s", u);
                printf("密码: "); scanf("%49s", p);
                printf("姓名: "); scanf("%49s", n);
                printf("身份证: "); scanf("%19s", id);
                printf("手机: "); scanf("%19s", ph);
                int res = register_user(u, p, n, id, ph);
                if (res == 0) printf("注册成功\n");
                else if (res == -2) printf("用户名已存在\n");
                else printf("注册失败\n");
                break;
            }
            case 2: {
                char u[50], p[50];
                printf("用户名: "); scanf("%49s", u);
                printf("密码: "); scanf("%49s", p);
                User curr;
                int idx = login_user(u, p, &curr);
                if (idx >= 0) {
                    printf("登录成功\n");
                    wait_for_enter();
                    user_menu(curr.username);
                } else {
                    printf("登录失败(用户名或密码错误)\n");
                }
                break;
            }
            case 3: {
                char u[50], p[50];
                printf("管理员用户名: "); scanf("%49s", u);
                printf("管理员密码: "); scanf("%49s", p);
                if (admin_login(u, p) == 0) {
                    printf("管理员登录成功\n");
                    wait_for_enter();
                    admin_menu();
                } else {
                    printf("管理员认证失败\n");
                }
                break;
            }
            default: 
                printf("无效选择\n");
        }
        wait_for_enter();
    }
    return 0;
}