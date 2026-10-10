#ifndef ORDER_H
#define ORDER_H

#include "train.h"
#include "user.h"
#include <time.h>

#define MAX_DATE_LEN 20
#define MAX_SEAT_TYPE_LEN 10

typedef enum {
    STATUS_PENDING = 0,
    STATUS_PAID = 1,
    STATUS_REFUNDED = 2,
    STATUS_CHANGED = 3
} OrderStatus;

typedef struct {
    int order_id;
    char username[MAX_NAME_LEN];
    char train_no[MAX_TRAIN_NO_LEN];
    char start_station[MAX_STATION_LEN];
    char end_station[MAX_STATION_LEN];
    char date[MAX_DATE_LEN];
    char seat_type[MAX_SEAT_TYPE_LEN]; // "FIRST" or "SECOND"
    float price;
    OrderStatus status;
    time_t create_time;
    time_t pay_time;
} Order;

// 订单管理函数声明
void init_orders();
int load_orders();
void save_orders();
int create_order(const char *username, const char *train_no, const char *date, const char *seat_type, float price);
int pay_order(int order_id);
int refund_order(int order_id);
int change_order(int order_id, const char *new_train_no, const char *new_date, const char *new_seat_type, float new_price);
Order* get_order_by_id(int order_id);
void get_user_orders(const char *username, Order *orders, int *count);
int get_next_order_id();
int get_order_count(); // 新增：获取订单总数
Order* get_order_by_index(int index); // 新增：通过索引获取订单

#endif