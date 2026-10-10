#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "order.h"
#include "train.h"

#define MAX_ORDERS 1000
#define FILE_ORDER "orders.txt"  // 改为 .txt

static Order orders[MAX_ORDERS];
static int order_count = 0;
static int next_order_id = 10001;

void init_orders() {
    order_count = load_orders();
    if (order_count > 0) {
        next_order_id = orders[order_count - 1].order_id + 1;
    }
}

int load_orders() {
    FILE *fp = fopen(FILE_ORDER, "w");  // 改为文本读模式
    if (!fp) return 0;
    
    fscanf(fp, "%d\n", &order_count);
    if (order_count > MAX_ORDERS) order_count = MAX_ORDERS;
    
    int i;
    for (i = 0; i < order_count; i++) {
        fscanf(fp, "%d\n", &orders[i].order_id);
        fscanf(fp, "%[^\n]\n", orders[i].username);
        fscanf(fp, "%[^\n]\n", orders[i].train_no);
        fscanf(fp, "%[^\n]\n", orders[i].start_station);
        fscanf(fp, "%[^\n]\n", orders[i].end_station);
        fscanf(fp, "%[^\n]\n", orders[i].date);
        fscanf(fp, "%[^\n]\n", orders[i].seat_type);
        fscanf(fp, "%f\n", &orders[i].price);
        fscanf(fp, "%d\n", (int*)&orders[i].status);
        fscanf(fp, "%ld\n", &orders[i].create_time);
        fscanf(fp, "%ld\n", &orders[i].pay_time);
    }
    
    fclose(fp);
    return order_count;
}

void save_orders() {
    FILE *fp = fopen(FILE_ORDER, "w");  // 改为文本写模式
    if (!fp) return;
    
    fprintf(fp, "%d\n", order_count);
    
    int i;
    for (i = 0; i < order_count; i++) {
        fprintf(fp, "%d\n", orders[i].order_id);
        fprintf(fp, "%s\n", orders[i].username);
        fprintf(fp, "%s\n", orders[i].train_no);
        fprintf(fp, "%s\n", orders[i].start_station);
        fprintf(fp, "%s\n", orders[i].end_station);
        fprintf(fp, "%s\n", orders[i].date);
        fprintf(fp, "%s\n", orders[i].seat_type);
        fprintf(fp, "%.2f\n", orders[i].price);
        fprintf(fp, "%d\n", orders[i].status);
        fprintf(fp, "%ld\n", orders[i].create_time);
        fprintf(fp, "%ld\n", orders[i].pay_time);
    }
    
    fclose(fp);
}

int get_next_order_id() {
    return next_order_id++;
}

int get_order_count() {
    return order_count;
}

Order* get_order_by_index(int index) {
    if (index < 0 || index >= order_count) return NULL;
    return &orders[index];
}

int create_order(const char *username, const char *train_no, const char *date, const char *seat_type, float price) {
    int t_idx;
    Train *t;
    int *stock;
    Order o;
    
    if (order_count >= MAX_ORDERS) return -1;
    
    t_idx = find_train_index(train_no);
    if (t_idx == -1) return -2;
    
    t = get_train_by_index(t_idx);
    stock = (strcmp(seat_type, "FIRST") == 0) ? &t->stock_first : &t->stock_second;
    
    if (*stock <= 0) return -3;
    
    (*stock)--;
    save_trains();
    
    memset(&o, 0, sizeof(Order));
    o.order_id = get_next_order_id();
    strncpy(o.username, username, MAX_NAME_LEN-1);
    o.username[MAX_NAME_LEN-1] = '\0';
    strncpy(o.train_no, train_no, MAX_TRAIN_NO_LEN-1);
    o.train_no[MAX_TRAIN_NO_LEN-1] = '\0';
    strncpy(o.date, date, MAX_DATE_LEN-1);
    o.date[MAX_DATE_LEN-1] = '\0';
    strncpy(o.seat_type, seat_type, MAX_SEAT_TYPE_LEN-1);
    o.seat_type[MAX_SEAT_TYPE_LEN-1] = '\0';
    o.price = price;
    o.status = STATUS_PENDING;
    o.create_time = time(NULL);
    o.pay_time = 0;
    
    strncpy(o.start_station, t->start_station, MAX_STATION_LEN-1);
    o.start_station[MAX_STATION_LEN-1] = '\0';
    strncpy(o.end_station, t->end_station, MAX_STATION_LEN-1);
    o.end_station[MAX_STATION_LEN-1] = '\0';
    
    orders[order_count++] = o;
    save_orders();
    return o.order_id;
}

int pay_order(int order_id) {
    int i;
    for (i = 0; i < order_count; i++) {
        if (orders[i].order_id == order_id && orders[i].status == STATUS_PENDING) {
            orders[i].status = STATUS_PAID;
            orders[i].pay_time = time(NULL);
            save_orders();
            return 0;
        }
    }
    return -1;
}

int refund_order(int order_id) {
    int i;
    for (i = 0; i < order_count; i++) {
        if (orders[i].order_id == order_id && orders[i].status == STATUS_PAID) {
            int seat_type = (strcmp(orders[i].seat_type, "FIRST") == 0) ? 1 : 2;
            update_train_stock(orders[i].train_no, seat_type, 1);
            orders[i].status = STATUS_REFUNDED;
            save_orders();
            return 0;
        }
    }
    return -1;
}

int change_order(int order_id, const char *new_train_no, const char *new_date, const char *new_seat_type, float new_price) {
    int i;
    for (i = 0; i < order_count; i++) {
        if (orders[i].order_id == order_id && orders[i].status == STATUS_PAID) {
            char old_train_no[MAX_TRAIN_NO_LEN];
            int old_seat_type;
            int t_idx;
            Train *t;
            int new_seat;
            int *new_stock;
            
            strncpy(old_train_no, orders[i].train_no, MAX_TRAIN_NO_LEN-1);
            old_train_no[MAX_TRAIN_NO_LEN-1] = '\0';
            old_seat_type = (strcmp(orders[i].seat_type, "FIRST") == 0) ? 1 : 2;
            
            update_train_stock(old_train_no, old_seat_type, 1);
            
            t_idx = find_train_index(new_train_no);
            if (t_idx == -1) {
                update_train_stock(old_train_no, old_seat_type, -1);
                return -2;
            }
            
            t = get_train_by_index(t_idx);
            new_seat = (strcmp(new_seat_type, "FIRST") == 0) ? 1 : 2;
            new_stock = (new_seat == 1) ? &t->stock_first : &t->stock_second;
            
            if (*new_stock <= 0) {
                update_train_stock(old_train_no, old_seat_type, -1);
                return -3;
            }
            (*new_stock)--;
            save_trains();
            
            strncpy(orders[i].train_no, new_train_no, MAX_TRAIN_NO_LEN-1);
            orders[i].train_no[MAX_TRAIN_NO_LEN-1] = '\0';
            strncpy(orders[i].date, new_date, MAX_DATE_LEN-1);
            orders[i].date[MAX_DATE_LEN-1] = '\0';
            strncpy(orders[i].seat_type, new_seat_type, MAX_SEAT_TYPE_LEN-1);
            orders[i].seat_type[MAX_SEAT_TYPE_LEN-1] = '\0';
            orders[i].price = new_price;
            strncpy(orders[i].start_station, t->start_station, MAX_STATION_LEN-1);
            orders[i].start_station[MAX_STATION_LEN-1] = '\0';
            strncpy(orders[i].end_station, t->end_station, MAX_STATION_LEN-1);
            orders[i].end_station[MAX_STATION_LEN-1] = '\0';
            orders[i].status = STATUS_CHANGED;
            
            save_orders();
            return 0;
        }
    }
    return -1;
}

Order* get_order_by_id(int order_id) {
    int i;
    for (i = 0; i < order_count; i++) {
        if (orders[i].order_id == order_id) return &orders[i];
    }
    return NULL;
}

void get_user_orders(const char *username, Order *user_orders, int *count) {
    int i;
    *count = 0;
    for (i = 0; i < order_count; i++) {
        if (strcmp(orders[i].username, username) == 0) {
            user_orders[*count] = orders[i];
            (*count)++;
        }
    }
}