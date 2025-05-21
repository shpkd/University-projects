#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

constexpr unsigned DOW_MON      = 0b0000'0001;
constexpr unsigned DOW_TUE      = 0b0000'0010;
constexpr unsigned DOW_WED      = 0b0000'0100;
constexpr unsigned DOW_THU      = 0b0000'1000;
constexpr unsigned DOW_FRI      = 0b0001'0000;
constexpr unsigned DOW_SAT      = 0b0010'0000;
constexpr unsigned DOW_SUN      = 0b0100'0000;
constexpr unsigned DOW_WORKDAYS = DOW_MON | DOW_TUE | DOW_WED | DOW_THU | DOW_FRI;
constexpr unsigned DOW_WEEKEND  = DOW_SAT | DOW_SUN;
constexpr unsigned DOW_ALL      = DOW_WORKDAYS | DOW_WEEKEND;

typedef struct TDate
{
    unsigned m_Year;
    unsigned m_Month;
    unsigned m_Day;
} TDATE;

TDATE makeDate ( unsigned y,
                 unsigned m,
                 unsigned d )
{
    TDATE res = { y, m, d };
    return res;
}
#endif /* __PROGTEST__ */

unsigned dayOfWeek(unsigned day, unsigned month, unsigned year){
    //Zeller's Algorithm
    if(month==1){
        month=13;
        year--;
    }
    else if(month==2){
        month=14;
        year--;
    }


    unsigned result=(day + 13 * (month + 1) / 5 + year + year / 4 + year/400 - year/4000 - year /100) % 7;


    switch(result){
        case 2: return 1 << 0;
        case 3: return 1 << 1;
        case 4: return 1 << 2;
        case 5: return 1 << 3;
        case 6: return 1 << 4;
        case 0: return 1 << 5;
        case 1: return 1 << 6;
        default: return 0;
    }
}

int leapYear(unsigned year){
    if((year%4==0 && year%100!=0) || ((year%400==0) && year%4000!=0)){
        return 1;
    }
    else{
        return 0;
    }
}

unsigned dayInMonth(unsigned year, unsigned month){
    switch(month){
        case 1: return 31;
        case 2:
            if(leapYear(year)==1){
                return 29;
            }
            else{
                return 28;
            }
        case 3: return 31;
        case 4: return 30;
        case 5: return 31;
        case 6: return 30;
        case 7: return 31;
        case 8: return 31;
        case 9: return 30;
        case 10: return 31;
        case 11: return 30;
        case 12: return 31;
        default: return 0;
    }
}

TDATE decrement(unsigned day, unsigned month, unsigned year){
    if(day>1){
        day--;
    }
    else{
        if(month>1){
            month--;
        }
        else{
            month=12;
            year--;
        }
        day= dayInMonth(year,month);
    }

    TDATE result;
    result.m_Day=day;
    result.m_Month=month;
    result.m_Year=year;

    return result;
}

long long countConnections ( TDATE     from,
                             TDATE     to,
                             unsigned  perWorkDay,
                             unsigned  dowMask )
{
    long long connections=0;

    if(from.m_Year>to.m_Year || (from.m_Year==to.m_Year && from.m_Month>to.m_Month) || (from.m_Year==to.m_Year && from.m_Month==to.m_Month && from.m_Day>to.m_Day)){
        return -1;
    }

    if(from.m_Day> dayInMonth(from.m_Year, from.m_Month) || to.m_Day> dayInMonth(to.m_Year, to.m_Month) || from.m_Year<1 || from.m_Month<1 || from.m_Month>12 || from.m_Day<1 || to.m_Year<1 || to.m_Month<1 || to.m_Month>12 || to.m_Day<1){
        return -1;
    }

    if(perWorkDay==0 || dowMask==0){
        return 0;
    }

    for(TDATE current=from; current.m_Year<to.m_Year || (current.m_Year==to.m_Year && current.m_Month<to.m_Month) || (current.m_Year==to.m_Year && current.m_Month==to.m_Month && current.m_Day<=to.m_Day); ){
        unsigned currentMask=dayOfWeek(current.m_Day, current.m_Month, current.m_Year);

        if(dowMask & currentMask){
            if(currentMask & DOW_WORKDAYS){
                connections+=perWorkDay;
            }
            else if(currentMask & DOW_SAT){
                connections+=(perWorkDay+1)/2;
            }
            else if(currentMask & DOW_SUN){
                connections+=(perWorkDay+2)/3;
            }
        }
        current.m_Day++;

        if(current.m_Day>dayInMonth(current.m_Year, current.m_Month)){
            current.m_Day=1;
            current.m_Month++;
        }

        if(current.m_Month>12){
            current.m_Month=1;
            current.m_Year++;
        }
    }

    return connections;
}


TDATE     endDate          ( TDATE     from,
                             long long connections,
                             unsigned  perWorkDay,
                             unsigned  dowMask )
{
    TDATE invalid;
    invalid.m_Year=0;
    invalid.m_Month=0;
    invalid.m_Day=0;

    if(from.m_Year<1 || from.m_Month<1 || from.m_Month>12 || from.m_Day<1 || from.m_Day> dayInMonth(from.m_Year, from.m_Month)
       || connections<0 || perWorkDay<=0 || dowMask==0){

        return invalid;
    }

    TDATE current=from;
    TDATE previousDate;
    long long conect=0;

    while(connections>=0){
        unsigned currentMask= dayOfWeek(current.m_Day, current.m_Month, current.m_Year);

        if(dowMask & currentMask){
            if(currentMask & DOW_WORKDAYS){
                conect=perWorkDay;
            }
            else if(currentMask & DOW_SAT){
                conect=(perWorkDay+1)/2;
            }
            else if(currentMask & DOW_SUN){
                conect=(perWorkDay+2)/3;
            }

            connections-=conect;


            if(connections<0){
                previousDate=decrement(current.m_Day, current.m_Month, current.m_Year);

                if(from.m_Year>previousDate.m_Year || (from.m_Year==previousDate.m_Year && from.m_Month>previousDate.m_Month) || (from.m_Year==previousDate.m_Year && from.m_Month==previousDate.m_Month && from.m_Day>previousDate.m_Day)){
                    return invalid;
                }
                return previousDate;
            }

        }

        current.m_Day++;

        if(current.m_Day>dayInMonth(current.m_Year, current.m_Month)){
            current.m_Day=1;
            current.m_Month++;
        }

        if(current.m_Month>12){
            current.m_Month=1;
            current.m_Year++;
        }

    }
    return invalid;
}

#ifndef __PROGTEST__
int main ()
{
    TDATE d;

    assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 31 ), 1, DOW_ALL ) == 31 );
    assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 31 ), 10, DOW_ALL ) == 266 );
    assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 31 ), 1, DOW_WED ) == 5 );
    assert ( countConnections ( makeDate ( 2024, 10, 2 ), makeDate ( 2024, 10, 30 ), 1, DOW_WED ) == 5 );
    assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 1 ), 10, DOW_TUE ) == 10 );
    assert ( countConnections ( makeDate ( 2024, 10, 1 ), makeDate ( 2024, 10, 1 ), 10, DOW_WED ) == 0 );
    assert ( countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 5, DOW_MON | DOW_FRI | DOW_SAT ) == 7462 );
    assert ( countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 0, DOW_MON | DOW_FRI | DOW_SAT ) == 0 );
    assert ( countConnections ( makeDate ( 2024, 1, 1 ), makeDate ( 2034, 12, 31 ), 100, 0 ) == 0 );
    assert ( countConnections ( makeDate ( 2024, 10, 10 ), makeDate ( 2024, 10, 9 ), 1, DOW_MON ) == -1 );
    assert ( countConnections ( makeDate ( 2024, 2, 29 ), makeDate ( 2024, 2, 29 ), 1, DOW_ALL ) == 1 );
    assert ( countConnections ( makeDate ( 2023, 2, 29 ), makeDate ( 2023, 2, 29 ), 1, DOW_ALL ) == -1 );
    assert ( countConnections ( makeDate ( 2100, 2, 29 ), makeDate ( 2100, 2, 29 ), 1, DOW_ALL ) == -1 );
    assert ( countConnections ( makeDate ( 2400, 2, 29 ), makeDate ( 2400, 2, 29 ), 1, DOW_ALL ) == 1 );
    assert ( countConnections ( makeDate ( 4000, 2, 29 ), makeDate ( 4000, 2, 29 ), 1, DOW_ALL ) == -1 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_ALL );
    assert ( d . m_Year == 2025 && d . m_Month == 1 && d . m_Day == 8 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 6, DOW_ALL );
    assert ( d . m_Year == 2024 && d . m_Month == 10 && d . m_Day == 20 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_WORKDAYS );
    assert ( d . m_Year == 2025 && d . m_Month == 2 && d . m_Day == 17 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 4, DOW_WORKDAYS );
    assert ( d . m_Year == 2024 && d . m_Month == 11 && d . m_Day == 4 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, DOW_THU );
    assert ( d . m_Year == 2026 && d . m_Month == 9 && d . m_Day == 2 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 2, DOW_THU );
    assert ( d . m_Year == 2025 && d . m_Month == 9 && d . m_Day == 17 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 0, DOW_THU );
    assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );
    d = endDate ( makeDate ( 2024, 10, 1 ), 100, 1, 0 );
    assert ( d . m_Year == 0 && d . m_Month == 0 && d . m_Day == 0 );


    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */