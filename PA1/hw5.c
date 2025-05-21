#include <stdio.h>
#include <stdlib.h>

/**
 * Function prints out the result of profit and loss
 */
void print_output(const int profit, const int fromProfit, const int toProfit,
                  const int loss, const int fromLoss, const int toLoss) {
    if (profit > 0) {
        printf("Nejvyssi zisk: %d (%d - %d)\n", profit, fromProfit, toProfit);
    } else {
        printf("Nejvyssi zisk: N/A\n");
    }
    if (loss > 0) {
        printf("Nejvyssi ztrata: %d (%d - %d)\n", loss, fromLoss, toLoss);
    } else {
        printf("Nejvyssi ztrata: N/A\n");
    }
}

/**
 * Calculates the highest profit and loss over a range of prices and prints the results
 * @param prices pointer to an array of integers representing the prices on different time
 * @param from beginning of the interval
 * @param to end of the interval
 */
void calculate_operations(const int *prices, const int from, const int to) {
    int min = prices[from], max = prices[from];
    int minDayP = from, minDayL = from;
    int profit = 0, fromProfit = 0, toProfit = 0;
    int loss = 0, fromLoss = 0, toLoss = 0;

    for (int i = from; i <= to; i++) {
        // Update the minimum price and the day it occurred
        if (min > prices[i]) {
            min = prices[i];
            minDayP = i;
        }

        // Calculate the current profit and update the highest profit and days
        int currProfit = prices[i] - min;
        if (currProfit > profit) {
            profit = currProfit;
            fromProfit = minDayP;
            toProfit = i;
        }

        // Update the maximum price and the day it occurred
        if (max < prices[i]) {
            max = prices[i];
            minDayL = i;
        }

        // Calculate the current loss and update the highest loss and days
        int currLoss = max - prices[i];
        if (currLoss > loss) {
            loss = currLoss;
            fromLoss = minDayL;
            toLoss = i;
        }
    }
    print_output(profit, fromProfit, toProfit, loss, fromLoss, toLoss);
}

/**
 * Reallocates memory for the prices array when its capacity is exceeded
 */
void reallocate_memory(int **prices, int *capacity){
    *capacity *= 2;
    int *tmp = (int *) realloc(*prices, *capacity * sizeof(int));
    if (tmp == NULL) {
        free(*prices);
        exit(1);
    }
    *prices = tmp;
}

/**
 * Function prints an invalid input message and frees allocated memory
 */
void invalid_input(int **prices){
    printf("Nespravny vstup.\n");
    free(*prices);
}

int main(void) {
    int pricesCapacity = 10;
    int pricesCount = 0;
    int *prices = (int *) malloc(pricesCapacity * sizeof(int));
    if(prices == NULL){
        return 1;
    }

    char sign;
    int price, from, to;
    printf("Ceny, hledani:\n");
    while (1) {
        int eof_check = scanf(" %c", &sign);
        if (eof_check == EOF) {
            break;
        }
        if (sign == '+') {
            if (scanf("%d", &price) != 1 || price <= 0) {
                invalid_input(&prices);
                return 1;
            }

            //If the prices array has reached its current capacity, prices array will be reallocated
            if (pricesCapacity == pricesCount) {
                reallocate_memory(&prices, &pricesCapacity);
            }
            prices[pricesCount] = price;
            pricesCount++;

        } else if (sign == '?') {
            if (scanf("%d %d", &from, &to) != 2 || from < 0 || from > to || to >= pricesCount) {
                invalid_input(&prices);
                return 1;
            }
            calculate_operations(prices, from, to);
        } else {
            invalid_input(&prices);
            return 1;
        }
    }
    free(prices);
    return 0;
}