# Multi Threaded Programming
The goal of this project is multi-threaded design of a problem. In this project, we analyze the data collected from the specifications and selling prices of houses.

## Project Description
At first, the program reads and extracts the provided data set and stores them in its memory. After extracting the data and their characteristics, the program starts labeling the price column of the data, obtaining the average and standard deviation of the data, and finally determines the price category of the houses.

## Feature Extraction

In the prepared dataset, the SalePrice column, target column and other columns represent the characteristics of each sample. At first, we will extract the characteristics of each sample and store them in the program memory.

## labeling

The price column is expressed in dollars. In this project, we want to obtain the price category of houses, which includes two categories, cheap and expensive. These two categories are relative. For this purpose, we change the SalePrice column of the houses in such a way that if it is lower than the price threshold (this threshold The price is given as an input to the program) becomes 0 (category 0, which is a cheap representative) and if it is greater than or equal to the set price threshold, it becomes a number 1 (category 1, which is an expensive representative). We save the result in the program memory in the SalePrice column.

## Serial Implementation

In this part of the project, I have implemented the requested program in a serial way.

## Multi threaded Implementation

In this part of the exercise, we deal with the parallelization of the operations performed in the functions. Parsing files and storing them in memory is a time-consuming process in many programs. To parallelize this part of the dataset, I have divided it into smaller files and performed the relevant operations by several threads.

## Project Run

Run example:
```
./HousePricePrediction.out datasets/ 330000
```

Output format:
```
Accuracy: <accuracy_percentage>%
```
