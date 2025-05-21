#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int gcd(int m, int n){
   while(n!=0){
       int temp=n;
       n=m%n;
       m=temp;
   }
   return m;
}

struct Sides{
    int a, b,c;
};

int compare(const void *compare1, const void *compare2){
    struct Sides *side1=(struct Sides*)compare1;
    struct Sides *side2=(struct Sides*)compare2;

    if(side1->a!=side2->a){
        return side1->a-side2->a;
    }
    else if(side1->b!=side2->b){
        return side1->b-side2->b;
    }

    return side1->c-side2->c;
}


int main(void) {
    int lo, hi;
    char mark, beginBr, endBr, coma;

    printf("Problemy:\n");

    int count = 0, tmp;
    struct Sides sides[1000];

    while (1){
        tmp = scanf(" %c %c %d %c %d %c", &mark, &beginBr, &lo, &coma, &hi, &endBr);
        if(tmp == EOF){
            break;
        }
        if(tmp != 6){
            printf("Nespravny vstup.\n");
            return 1;
        }

        if((mark!='?' && mark!='#') || beginBr != '<' || endBr != '>' || coma != ';'){
            printf("Nespravny vstup.\n");
            return 1;
        }

        if(lo > hi || hi <= 0 || lo <= 0){
            printf("Nespravny vstup.\n");
            return 1;
        }

        if(mark=='?'){
            for(int m=2; m<= sqrt(hi); m++){
                for(int n=1; n<m;n++){
                    if(gcd(m,n)==1 && (m-n)%2==1){
                        int a, b, c;
                        a=m*m-n*n;
                        b=2*m*n;
                        c=m*m+n*n;

                        if(c>hi){
                            break;
                        }

                        for(int i=1; i*c<=hi; i++){
                            int aInc, bInc, cInc;
                            aInc=a*i<b*i?(a*i<c*i?a*i:c*i):(b*i<c*i?b*i:c*i);
                            bInc=a*i>b*i?(a*i<c*i?a*i:c*i):(b*i<c*i?b*i:c*i);
                            cInc=a*i>b*i?(a*i>c*i?a*i:c*i):(b*i>c*i?b*i:c*i);

                            if(aInc>=lo && aInc<=hi && bInc>=lo && bInc<=hi && cInc>=lo && cInc<=hi){
                                if (count < 1000) {
                                    sides[count].a = aInc;
                                    sides[count].b = bInc;
                                    sides[count].c = cInc;
                                    (count)++;
                                }
                            }
                        }
                    }
                }
            }
            qsort(sides,count, sizeof(struct Sides),compare);
            for(int i=0; i<count;i++){
                printf("* %d %d %d\n", sides[i].a, sides[i].b, sides[i].c);
            }
            printf("Celkem: %d\n", count);
            count = 0;
        }
        else if(mark=='#'){
            for(int m=2; m<= sqrt(hi); m++){
                for(int n=1; n<m;n++){
                    if(gcd(m,n)==1 && (m-n)%2==1){
                        int a, b, c;
                        a=m*m-n*n;
                        b=2*m*n;
                        c=m*m+n*n;

                        if(c>hi){
                            break;
                        }

                        for(int i=1; i*c<=hi; i++){
                            int aInc, bInc, cInc;
                            aInc=i*a;
                            bInc=i*b;
                            cInc=i*c;

                            if(aInc>=lo && aInc<=hi && bInc>=lo && bInc<=hi && cInc>=lo && cInc<=hi){
                                count++;
                            }
                        }
                    }
                }
            }
            printf("Celkem: %d\n", count);
            count = 0;
        }
    }
    return 0;
}
