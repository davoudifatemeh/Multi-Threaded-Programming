#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <pthread.h>

#define NUMBER_OF_THREADS 5

using namespace std;

string csv_names[NUMBER_OF_THREADS] = {"dataset_0.csv", "dataset_1.csv", "dataset_2.csv", "dataset_3.csv", "dataset_4.csv"};
int done1[NUMBER_OF_THREADS] = {0, 0, 0, 0, 0};
int done2[NUMBER_OF_THREADS] = {0, 0, 0, 0, 0};
int correct_detected[NUMBER_OF_THREADS] = {0, 0, 0, 0, 0};
int flag = 0;
int total = 0;
vector<vector<int>> datum[NUMBER_OF_THREADS];
float means[NUMBER_OF_THREADS][2][8] = {0};
vector<int> prediction[NUMBER_OF_THREADS];
float final_means[2][8] = {0};
float final_stds[2][8] = {0};

struct thread_data {
   int thread_id;
   string path;
   int price_threshold;
};
struct thread_data thread_data_array[NUMBER_OF_THREADS];

void read_dataset(string csv_path, vector <vector<int>> &data)
{
    ifstream csvfile(csv_path);
    string str, feature;
    getline(csvfile, str);
    while (getline(csvfile, str))
    {
        vector<int> row;
        stringstream s(str);
        while (getline(s, feature, ','))
            row.push_back(stoi(feature));
        data.push_back(row);
    }
    csvfile.close();
    return;
}

void labelization(int price_threshold, vector <vector<int>> &data)
{
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i][data[i].size() - 1] < price_threshold)
            data[i][data[i].size() - 1] = 0;
        else
            data[i][data[i].size() - 1] = 1;
    }
    return;
}

void calc_mean(int tid, vector <vector<int>> data)
{
    for (int i = 0; i < data[0].size() - 1; i++)
    {
        long int sum_inexpensive = 0;
        long int sum_expensive = 0;
        int inexpensive_cnt = 0;
        int expensive_cnt = 0;
        for (int j = 0; j < data.size(); j++)
        {
            if(data[j][data[j].size() - 1] == 0)
            {
                sum_inexpensive += data[j][i];
                inexpensive_cnt++;
            }
            else
            {
                sum_expensive += data[j][i];
                expensive_cnt++;
            }
        }
        means[tid][0][i] = static_cast<long double>(sum_inexpensive) / static_cast<double>(inexpensive_cnt);
        means[tid][1][i] = static_cast<long double>(sum_expensive) / static_cast<double>(expensive_cnt);
    }
    return;
}

void predict(int tid)
{
    for (int i = 0; i < datum[tid].size(); i++)
    {
        if ((datum[tid][i][5] >= (final_means[1][5] - final_stds[1][5])) && (datum[tid][i][5] <= (final_means[1][5] + final_stds[1][5])))
            prediction[tid].push_back(1);
        else
            prediction[tid].push_back(0);
    }
    return;
}

void calc_correct_det(int tid)
{
    int correct_det = 0;
    total += datum[tid].size();
    for (int i = 0; i < datum[tid].size(); i++)
    {
        if (datum[tid][i][8] == prediction[tid][i])
            correct_det++;
    }
    correct_detected[tid] = correct_det;
    return;
}

void* do_functions(void* data)
{
    struct thread_data* my_data = (struct thread_data*) data;
    read_dataset(my_data->path, datum[my_data->thread_id]);
    labelization(my_data->price_threshold, datum[my_data->thread_id]);
    calc_mean(my_data->thread_id, datum[my_data->thread_id]);
    done1[my_data->thread_id] = 1;
    while (!flag)
    {
    }
    predict(my_data->thread_id);
    calc_correct_det(my_data->thread_id);
    done2[my_data->thread_id] = 1;
    pthread_exit(NULL);
}

void calc_final_means()
{
    for (int j = 0; j < 2; j++)
    {
        for (int k = 0; k < 8; k++)
        {
            float sum = 0;
            for (int i = 0; i < NUMBER_OF_THREADS; i++)
                sum += means[i][j][k];

            final_means[j][k] = sum/NUMBER_OF_THREADS;
        }
    }
    return;
}

void calc_std()
{
    for (int i = 0; i < 8; i++)
    {
        long double inexpensive = 0;
        long double expensive = 0;
        int inexpensive_cnt = 0;
        int expensive_cnt = 0;
        for (int j = 0; j < NUMBER_OF_THREADS; j++)
        {
            for (int k = 0; k < datum[j].size(); k++)
            {
                if (datum[j][k][8] == 0)
                {
                    inexpensive += ((static_cast<double>(datum[j][k][i]) - final_means[0][i])*(static_cast<double>(datum[j][k][i]) - final_means[0][i]));
                    inexpensive_cnt++;
                }
                else
                {
                    expensive += ((static_cast<double>(datum[j][k][i]) - final_means[1][i])*(static_cast<double>(datum[j][k][i]) - final_means[1][i]));
                    expensive_cnt++;
                }
            }
        }
        final_stds[0][i] = sqrt(inexpensive / static_cast<double>(inexpensive_cnt));
        final_stds[1][i] = sqrt(expensive / static_cast<double>(expensive_cnt));
    }
    return;
}

double calc_accuracy()
{
    int total_correct = 0;
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
        total_correct += correct_detected[i];
    double accuracy = static_cast<double>(total_correct) / static_cast<double>(total);
    return accuracy;
}

int main(int argc, char *argv[])
{
    string file_path(argv[1]);
    pthread_t threads[NUMBER_OF_THREADS];
    for (int tid = 0; tid < NUMBER_OF_THREADS; tid++) {
        thread_data_array[tid].thread_id = tid;
        thread_data_array[tid].path = file_path + csv_names[tid];
        thread_data_array[tid].price_threshold = atoi(argv[2]);
        pthread_create(&threads[tid], NULL, do_functions, (void*)&thread_data_array[tid]);
    }
    while (!done1[0] || !done1[1] || !done1[2] || !done1[3])
    {
    }
    calc_final_means();
    calc_std();
    flag = 1;
    while (!done2[0] || !done2[1] || !done2[2] || !done2[3])
    {
    }
    double accuracy = calc_accuracy();
    cout << setprecision(2) << fixed;
    cout << "Accuracy: " << accuracy*100 << "%" << endl;
    pthread_exit(NULL);
}