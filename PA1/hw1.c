#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>

bool compare(double a, double b){
    double base=fabs(a-b);
    double exactly=DBL_EPSILON*fmax(fabs(a), fabs(b))*100;
    return base<=exactly;
}

bool equal_point(double ax, double ay, double bx, double by){
    return (compare(ax, bx) && compare(ay, by));
}

bool collinear_point(double ax, double ay, double bx, double by, double cx, double cy) {
    if (compare(bx - ax, 0.0) && compare(cx - bx, 0.0)) {
        return 1;
    }
    else if(compare(bx - ax, 0.0) || compare(cx - bx, 0.0)){
        return 0;
    }

    double slope_AB=(by-ay)/(bx-ax);
    double slope_BC = (cy - by)/(cx - bx);
    return compare(slope_AB, slope_BC);
}


double side(double ax, double ay, double bx, double by){
    double result=(bx-ax)*(bx-ax)+(by-ay)*(by-ay);
    return result;
}

int figure(double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy){
    double AB, BC, CD, DA;
    double AC, BD;
    AB=side(ax, ay, bx, by);
    BC=side(bx, by, cx, cy);
    CD=side(cx,cy,dx,dy);
    DA=side(dx,dy,ax,ay);

    AC=side(ax,ay,cx,cy);
    BD=side(bx,by,dx,dy);

    int equality_side=(compare(AB, BC) && compare(BC, CD) && compare(CD, DA));
    int equality_diagonal=compare(AC,BD);

    if(equality_side && equality_diagonal){
        return 0;//ctverec
    }
    else if(equality_side && !equality_diagonal){
        return 1;//kosoctverec
    }
    else if (!equality_side && equality_diagonal){
        return 2;//obdelnik
    }
    else{
        return 3;//rovnobeznik
    }
}

int main(void) {
    double ax, ay, bx, by, cx, cy;
    char end_bkt;

    printf("Bod A:\n");
    if(scanf(" [ %lf , %lf %c", &ax, &ay, &end_bkt) != 3 || end_bkt!=']'){
        printf("Nespravny vstup.\n");
        return 1;
    }

    printf("Bod B:\n");
    if(scanf(" [ %lf , %lf %c",&bx, &by, &end_bkt) != 3 || end_bkt!=']'){
        printf("Nespravny vstup.\n");
        return 1;
    }

    printf("Bod C:\n");
    if(scanf(" [ %lf , %lf %c",&cx, &cy, &end_bkt) != 3 || end_bkt!=']'){
        printf("Nespravny vstup.\n");
        return 1;
    }

    if(equal_point(ax,ay,bx,by) || equal_point(ax, ay, cx, cy) || equal_point(bx,by, cx,cy)){
        printf("Rovnobezniky nelze sestrojit.\n");
        return 1;
    }

    if(collinear_point(ax,ay,bx,by,cx,cy)){
        printf("Rovnobezniky nelze sestrojit.\n");
        return 1;
    }

    double aax, aay, bbx, bby, ccx, ccy;

    aax=bx+cx-ax;
    aay=by+cy-ay;
    int a_is_figure=figure(ax, ay, bx, by, aax, aay, cx, cy);

    if(a_is_figure==0){
        printf("A': [%g,%g], ctverec\n", aax, aay);
    }
    else if(a_is_figure==1){
        printf("A': [%g,%g], kosoctverec\n", aax, aay);
    }
    else if(a_is_figure==2){
        printf("A': [%g,%g], obdelnik\n", aax, aay);
    }
    else{
        printf("A': [%g,%g], rovnobeznik\n", aax, aay);
    }

    bbx=ax+cx-bx;
    bby=ay+cy-by;
    int b_is_figure=figure(ax, ay, bx, by, cx, cy, bbx, bby);

    if(b_is_figure==0){
        printf("B': [%g,%g], ctverec\n", bbx, bby);
    }
    else if(b_is_figure==1){
        printf("B': [%g,%g], kosoctverec\n", bbx, bby);
    }
    else if(b_is_figure==2){
        printf("B': [%g,%g], obdelnik\n", bbx, bby);
    }
    else{
        printf("B': [%g,%g], rovnobeznik\n", bbx, bby);
    }

    ccx=ax+bx-cx;
    ccy=ay+by-cy;
    int c_is_figure=figure(ax, ay, ccx, ccy, bx, by, cx, cy);

    if(c_is_figure==0){
        printf("C': [%g,%g], ctverec\n", ccx, ccy);
    }
    else if(c_is_figure==1){
        printf("C': [%g,%g], kosoctverec\n", ccx, ccy);
    }
    else if(c_is_figure==2){
        printf("C': [%g,%g], obdelnik\n", ccx, ccy);
    }
    else{
        printf("C': [%g,%g], rovnobeznik\n", ccx, ccy);
    }

    return 0;
}