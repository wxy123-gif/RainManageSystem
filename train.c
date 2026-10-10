#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "train.h"

#define MAX_TRAINS 100
#define FILE_TRAIN "trains.txt"  // 改为 .txt

static Train trains[MAX_TRAINS];
static int train_count = 0;

void init_trains() {
    train_count = load_trains();
    if (train_count == 0) {
        Train t;
        memset(&t, 0, sizeof(Train));
        strcpy(t.train_no, "G101");
        strcpy(t.start_station, "北京南");
        strcpy(t.end_station, "上海虹桥");
        strcpy(t.start_time, "09:00");
        strcpy(t.end_time, "13:30");
        t.duration = 270;
        t.price_first = 933.0;
        t.stock_first = 50;
        t.price_second = 553.0;
        t.stock_second = 100;
        add_train(&t);
    }
}

int load_trains() {
    FILE *fp = fopen(FILE_TRAIN, "w");  // 改为文本读模式
    if (!fp) return 0;
    
    fscanf(fp, "%d\n", &train_count);  // 读取数量
    if (train_count > MAX_TRAINS) train_count = MAX_TRAINS;
    
    int i;
    for (i = 0; i < train_count; i++) {
        fscanf(fp, "%[^\n]\n", trains[i].train_no);
        fscanf(fp, "%[^\n]\n", trains[i].start_station);
        fscanf(fp, "%[^\n]\n", trains[i].end_station);
        fscanf(fp, "%[^\n]\n", trains[i].start_time);
        fscanf(fp, "%[^\n]\n", trains[i].end_time);
        fscanf(fp, "%d\n", &trains[i].duration);
        fscanf(fp, "%f\n", &trains[i].price_first);
        fscanf(fp, "%d\n", &trains[i].stock_first);
        fscanf(fp, "%f\n", &trains[i].price_second);
        fscanf(fp, "%d\n", &trains[i].stock_second);
    }
    
    fclose(fp);
    return train_count;
}

void save_trains() {
    FILE *fp = fopen(FILE_TRAIN, "w");  // 改为文本写模式
    if (!fp) return;
    
    fprintf(fp, "%d\n", train_count);  // 写入数量
    
    int i;
    for (i = 0; i < train_count; i++) {
        fprintf(fp, "%s\n", trains[i].train_no);
        fprintf(fp, "%s\n", trains[i].start_station);
        fprintf(fp, "%s\n", trains[i].end_station);
        fprintf(fp, "%s\n", trains[i].start_time);
        fprintf(fp, "%s\n", trains[i].end_time);
        fprintf(fp, "%d\n", trains[i].duration);
        fprintf(fp, "%.2f\n", trains[i].price_first);
        fprintf(fp, "%d\n", trains[i].stock_first);
        fprintf(fp, "%.2f\n", trains[i].price_second);
        fprintf(fp, "%d\n", trains[i].stock_second);
    }
    
    fclose(fp);
}

int find_train_index(const char *train_no) {
    int i;
    for (i = 0; i < train_count; i++) {
        if (strcmp(trains[i].train_no, train_no) == 0) return i;
    }
    return -1;
}

void add_train(Train *t) {
    if (train_count >= MAX_TRAINS) return;
    trains[train_count++] = *t;
    save_trains();
}

void delete_train(const char *train_no) {
    int idx = find_train_index(train_no);
    int i;
    if (idx == -1) return;
    for (i = idx; i < train_count - 1; i++) {
        trains[i] = trains[i+1];
    }
    train_count--;
    save_trains();
}

void update_train_stock(const char *train_no, int seat_type, int count) {
    int idx = find_train_index(train_no);
    if (idx == -1) return;
    if (seat_type == 1) {
        trains[idx].stock_first += count;
    } else {
        trains[idx].stock_second += count;
    }
    save_trains();
}

Train* get_train_by_index(int index) {
    if (index < 0 || index >= train_count) return NULL;
    return &trains[index];
}

int get_train_count() {
    return train_count;
}