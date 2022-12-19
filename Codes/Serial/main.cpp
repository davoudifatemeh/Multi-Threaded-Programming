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

using namespace std;

void read_dataset(string csv_path, vector <vector<int>> &datum)
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
        datum.push_back(row);
    }
    csvfile.close();
    return;
}

void labelization(int price_threshold, vector <vector<int>> &datum)
{
    for (int i = 0; i < datum.size(); i++)
    {
        if (datum[i][datum[i].size() - 1] < price_threshold)
            datum[i][datum[i].size() - 1] = 0;
        else
            datum[i][datum[i].size() - 1] = 1;
    }
    return;
}

void calc_mean(vector <vector <float>> &means, vector <vector<int>> datum)
{
    vector <float> temp1;
    vector <float> temp2;
    for (int i = 0; i < datum[0].size() - 1; i++)
    {
        long int sum_inexpensive = 0;
        long int sum_expensive = 0;
        int inexpensive_cnt = 0;
        int expensive_cnt = 0;
        for (int j = 0; j < datum.size(); j++)
        {
            if(datum[j][datum[j].size() - 1] == 0)
            {
                sum_inexpensive += datum[j][i];
                inexpensive_cnt++;
            }
            else
            {
                sum_expensive += datum[j][i];
                expensive_cnt++;
            }
        }
        temp1.push_back(static_cast<long double>(sum_inexpensive) / static_cast<double>(inexpensive_cnt));
        temp2.push_back(static_cast<long double>(sum_expensive) / static_cast<double>(expensive_cnt));
    }
    means.push_back(temp1);
    means.push_back(temp2);
    return;
}

void calc_std(vector <vector <float>> means, vector <vector <float>> &stds, vector <vector<int>> datum)
{
    vector <float> temp1;
    vector <float> temp2;
    for (int i = 0; i < datum[0].size() - 1; i++)
    {
        long double inexpensive = 0;
        long double expensive = 0;
        int inexpensive_cnt = 0;
        int expensive_cnt = 0;
        for (int j = 0; j < datum.size(); j++)
        {
            if(datum[j][datum[j].size() - 1] == 0)
            {
                inexpensive += ((static_cast<double>(datum[j][i]) - means[0][i])*(static_cast<double>(datum[j][i]) - means[0][i]));
                inexpensive_cnt++;
            }
            else
            {
                expensive += ((static_cast<double>(datum[j][i]) - means[1][i])*(static_cast<double>(datum[j][i]) - means[1][i]));
                expensive_cnt++;
            }
        }
        temp1.push_back(sqrt(inexpensive / static_cast<double>(inexpensive_cnt)));
        temp2.push_back(sqrt(expensive / static_cast<double>(expensive_cnt)));
    }
    stds.push_back(temp1);
    stds.push_back(temp2);
    return;
    return;
}

void predict(vector <vector <float>> means, vector <vector <float>> stds, vector <vector<int>> datum, vector<int> &prediction)
{
    for (int i = 0; i < datum.size(); i++)
    {
        if ((datum[i][5] >= (means[1][5] - stds[1][5])) && (datum[i][5] <= (means[1][5] + stds[1][5])))
            prediction.push_back(1);
        else
            prediction.push_back(0);
    }
    return;
}

double calc_accuracy(vector <int> prediction, vector <vector<int>> datum)
{
    double accuracy = 0;
    int correct_det = 0;
    for (int i = 0; i < datum.size(); i++)
    {
        if (datum[i][datum[0].size() - 1] == prediction[i])
            correct_det++;
    }
    accuracy = static_cast<double>(correct_det) / static_cast<double>(datum.size());
    return accuracy;
}

int main(int argc, char *argv[])
{
    string file_name(argv[1]);
    int price_threshold = atoi(argv[2]);
    string csv_path = file_name + "dataset.csv";
    vector <vector<int>> datum;
    read_dataset(csv_path, datum);
    labelization(price_threshold, datum);
    vector <vector <float>> means;
    vector <vector <float>> stds;
    calc_mean(means, datum);
    calc_std(means, stds, datum);
    vector<int> prediction;
    predict(means, stds, datum, prediction);
    for (int j = 0; j < 2; j++)
    {
        for (int k = 0; k < 8; k++)
        {
            cout << means[j][k] << "\t";
        }
        cout << "\n";
    }
    double accuracy = calc_accuracy(prediction, datum);
    cout << setprecision(2) << fixed;
    cout << "Accuracy: " << accuracy*100 << "%" << endl;
    return 0;
}