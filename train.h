#ifndef TRAIN_H
#define TRAIN_H


#define MAX_STATION_LEN 50
#define MAX_TRAIN_NO_LEN 20


typedef struct {
    char train_no[MAX_TRAIN_NO_LEN];
    char start_station[MAX_STATION_LEN];
    char end_station[MAX_STATION_LEN];
    char start_time[20];
    char end_time[20];
    int duration;
    float price_first;
    int stock_first;
    float price_second;
    int stock_second;
} Train;


void init_trains();
int load_trains();
void save_trains();
int find_train_index(const char *train_no);
void add_train(Train *t);
void delete_train(const char *train_no);
void update_train_stock(const char *train_no, int seat_type, int count);
Train* get_train_by_index(int index);
int get_train_count();

#endif