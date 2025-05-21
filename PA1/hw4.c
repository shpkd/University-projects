#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int from;
    int to;
    int quantity;
    int cost;
} Transport;



int cmpTransport(const void *a, const void *b){
    int fromA=((Transport *)a)->from;
    int fromB=((Transport *)b)->from;

    return fromA-fromB;
}


int main(void) {
    int maxTransport=100000;
    int transportCount=0;
    Transport * transports=(Transport *)malloc(maxTransport * sizeof(Transport));

    printf("Moznosti dopravy:\n");
    bool validInput=true;
    int from, to, capacity, cost;
    char beginW, beginS, mark, endS, endW, coma1, coma2;

    if(scanf(" %c", &beginW)!=1 || beginW!='{'){
        printf("Nespravny vstup.\n");
        validInput=false;
    }

    while(validInput){
        if (scanf(" %c", &beginS) != 1 || beginS != '[') {
            printf("Nespravny vstup.\n");
            validInput = false;
            break;
        }

        int resultInput= scanf(" %d %c %d %c %d %c %d %c", &from, &mark, &to, &coma1, &capacity, &coma2, &cost, &endS);

        if(resultInput!=8 || from<0 || to<0 || from>to || capacity<=0 || cost<=0 || endS!=']' || mark!='-' || coma1!=',' || coma2!=','){
            printf("Nespravny vstup.\n");
            validInput=false;
            break;
        }

        if(transportCount>=maxTransport){
            printf("Nespravny vstup.\n");
            validInput=false;
            break;
        }

        transports[transportCount].from=from;
        transports[transportCount].to=to;
        transports[transportCount].quantity=capacity;
        transports[transportCount].cost=cost;
        transportCount++;

        if (scanf(" %c", &endW) != 1 || (endW != ',' && endW != '}')) {
            printf("Nespravny vstup.\n");
            validInput = false;
            break;
        }

        if (endW == '}') {
            break;
        }
    }


    if (!validInput){
        free(transports);
        return 1;
    }

    qsort(transports, transportCount, sizeof (Transport), cmpTransport);

    printf("Naklad:\n");

    int start, amount;

    while(1){
        int eof_check=scanf(" %d %d", &start, &amount);

        if(eof_check==EOF){
            break;
        }

        if(eof_check!=2 || start<0 || amount<=0){
            printf("Nespravny vstup.\n");
            return 1;
        }

        int amountLeft=amount;
        int last=-1;
        int totalCost=0;
        bool impossible=true;

        int maxDay=0;
        for(int i=0; i<transportCount; i++){
            if(transports[i].to>maxDay){
                maxDay=transports[i].to;
            }
        }

        int day=start;

        while(amountLeft>0){

            if(day>maxDay){
                last=-1;
                break;
            }

            int dayMax=0;
            int dayCost=0;

            for(int i=0; i<transportCount; i++){
                if(day>=transports[i].from && day<=transports[i].to){
                    dayMax+=transports[i].quantity;
                    dayCost+=transports[i].cost;
                }
            }

            if(dayMax==0){
                day++;
                continue;
            }

            impossible=false;

            amountLeft-=dayMax;
            last=day;
            totalCost+=dayCost;

            day++;
        }

        if(amountLeft>0 || impossible){
            printf("Prilis velky naklad, nelze odvezt.\n");
        }
        else{
            printf("Konec: %d, cena: %d\n", last, totalCost);
        }
    }
    
    free(transports);
    return 0;
}
