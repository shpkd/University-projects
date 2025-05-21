#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

constexpr int PHONE_DIGITS = 10;

typedef struct TNode
{
    char                               * m_Name;
    struct TNode                       * m_Child[PHONE_DIGITS];
} TNODE;

typedef struct
{
    TNODE                              * m_Root;
    int                                  m_Size;
} TPHONEBOOK;

#endif /* __PROGTEST__ */

void  clear(TNODE * root){

    for(int i=0; i<PHONE_DIGITS; i++){
        if(root->m_Child[i] != NULL){
            clear(root->m_Child[i]);
        }
    }

    if(root->m_Name != NULL){
        free(root->m_Name);
    }

    free(root);
}

bool         addPhone  ( TPHONEBOOK * book,
                         const char * phone,
                         const char * name )
{
    if(book==NULL || phone==NULL || name==NULL){
        return false;
    }

    for(int i=0; phone[i]!='\0'; i++){
        if(!isdigit(phone[i])){
            return false;
        }
    }


    if(book->m_Root==NULL){
        book->m_Root=(TNODE *) malloc(sizeof (TNODE));
        if(book->m_Root==NULL){
            return false;
        }
        book->m_Root->m_Name=NULL;
        for(int i=0; i<PHONE_DIGITS; i++){
            book->m_Root->m_Child[i]=NULL;
        }
    }

    TNODE * root=book->m_Root;

    for(int i=0; phone[i]!='\0'; i++){
        int num=phone[i]-'0';

        if(root->m_Child[num]==NULL){
            root->m_Child[num]=(TNODE *) malloc(sizeof (TNODE));
            if(root->m_Child[num]==NULL){
                clear(book->m_Root);
                book->m_Root = NULL;
                return false;
            }
            root->m_Child[num]->m_Name=NULL;
            for(int j=0; j<PHONE_DIGITS; j++){
                root->m_Child[num]->m_Child[j]=NULL;
            }
        }

        root=root->m_Child[num];

    }

    if(root->m_Name==NULL){
        root->m_Name=(char *) malloc(strlen(name)+1);
        if(root->m_Name==NULL){
            clear(book->m_Root);
            book->m_Root = NULL;
            return false;
        }
        strcpy(root->m_Name,name);
        book->m_Size++;
    }
    else{
        free(root->m_Name);
        root->m_Name=(char *) malloc(strlen(name)+1);
        if(root->m_Name==NULL){
            clear(book->m_Root);
            book->m_Root = NULL;
            return false;
        }
        strcpy(root->m_Name,name);
    }

    return true;
}



void         delBook   ( TPHONEBOOK * book )
{
    if(book->m_Root!= NULL){
        clear(book->m_Root);
        book->m_Root=NULL;
    }
    book->m_Size=0;
}



bool recursionDel(TNODE * root, const char * phone, int d, int l){
    if(d==l){
        if(root->m_Name==NULL){
            return false;
        }

        free(root->m_Name);
        root->m_Name=NULL;

        for(int i=0; i<PHONE_DIGITS; i++){
            if(root->m_Child[i]!=NULL){
                return true;
            }
        }

        return false;
    }

    int level=phone[d]-'0';

    if(root->m_Child[level]==NULL){
        return false;
    }

    bool recursion=recursionDel(root->m_Child[level], phone, d+1, l);

    if(!recursion){
        free(root->m_Child[level]);
        root->m_Child[level]=NULL;
    }

    for(int i=0; i<PHONE_DIGITS; i++){
        if(root->m_Child[i]!=NULL){
            return true;
        }
    }

    if(root->m_Name!=NULL){
        return true;
    }
    else{
        return false;
    }
}

bool         delPhone  ( TPHONEBOOK * book,
                         const char * phone )
{
    if(book==NULL || phone==NULL || book->m_Root==NULL){
        return false;
    }

    for(int i=0; phone[i]!='\0'; i++){
        if(!isdigit(phone[i])){
            return false;
        }
    }


    TNODE  * root=book->m_Root;
    for(int i=0; phone[i]!='\0'; i++){
        int num=phone[i]-'0';
        if(num<0 || num>=PHONE_DIGITS || root->m_Child[num]==NULL){
            return false;
        }

        root=root->m_Child[num];
    }

    if(root->m_Name==NULL){
        return false;
    }

    bool result= recursionDel(book->m_Root, phone, 0, (int)strlen(phone));

    if(!result){
        free(book->m_Root);
        book->m_Root=NULL;
    }

    book->m_Size--;

    return true;
}

const char * findPhone ( TPHONEBOOK * book,
                         const char * phone )
{

    if(book==NULL || phone==NULL || book->m_Root==NULL){
        return NULL;
    }

    for(int i=0; phone[i]!='\0'; i++){
        if(!isdigit(phone[i])){
            return NULL;
        }
    }

    TNODE * root=book->m_Root;
    const char * resultName= NULL;

    for(int i=0; phone[i]!='\0';  i++){
        int num=phone[i]-'0';

        if(num<0 || num>=PHONE_DIGITS || root->m_Child[num]==NULL){
            return resultName;
        }

        root=root->m_Child[num];
        if(root->m_Name!=NULL){
            resultName=root->m_Name;
        }
    }

    return resultName;
}

#ifndef __PROGTEST__
int main ()
{
    TPHONEBOOK b = { nullptr, 0 };
    char tmpStr[100];
    const char * name;
    assert ( addPhone ( &b, "420", "Czech Republic" ) );
    assert ( addPhone ( &b, "42022435", "Czech Republic CVUT" ) );
    assert ( addPhone ( &b, "421", "Slovak Republic" ) );
    assert ( addPhone ( &b, "44", "Great Britain" ) );
    strncpy ( tmpStr, "USA", sizeof ( tmpStr ) - 1 );
    assert ( addPhone ( &b, "1", tmpStr ) );
    strncpy ( tmpStr, "Guam", sizeof ( tmpStr ) - 1 );
    assert ( addPhone ( &b, "1671", tmpStr ) );
    assert ( addPhone ( &b, "44", "United Kingdom" ) );
    assert ( b . m_Size == 6 );
    assert ( ! b . m_Root -> m_Name );
    assert ( ! b . m_Root -> m_Child[0] );
    assert ( ! strcmp ( b . m_Root -> m_Child[1] -> m_Name, "USA" ) );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Name );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Name );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[0] );
    assert ( ! strcmp ( b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Name, "Guam" ) );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[1] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[7] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Name );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Name, "Czech Republic" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[4] );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Name, "Czech Republic CVUT" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[9] );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Name, "Slovak Republic" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[3] );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[4] -> m_Name, "United Kingdom" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[9] );




    name = findPhone ( &b, "420800123456" );  assert ( !strcmp ( name, "Czech Republic" ) );
    name = findPhone ( &b, "420224351111" );  assert ( !strcmp ( name, "Czech Republic CVUT" ) );
    name = findPhone ( &b, "42022435" );  assert ( !strcmp ( name, "Czech Republic CVUT" ) );
    name = findPhone ( &b, "4202243" );  assert ( !strcmp ( name, "Czech Republic" ) );
    name = findPhone ( &b, "420224343258985224" );  assert ( !strcmp ( name, "Czech Republic" ) );
    name = findPhone ( &b, "42" );  assert ( !name );
    name = findPhone ( &b, "422" );  assert ( !name );
    name = findPhone ( &b, "4422" );  assert ( !strcmp ( name, "United Kingdom" ) );
    name = findPhone ( &b, "16713425245763" );  assert ( !strcmp ( name, "Guam" ) );
    name = findPhone ( &b, "123456789123456789" );  assert ( !strcmp ( name, "USA" ) );
    assert ( delPhone ( &b, "420" ) );
    assert ( delPhone ( &b, "421" ) );
    assert ( delPhone ( &b, "44" ) );
    assert ( delPhone ( &b, "1671" ) );
    assert ( !delPhone ( &b, "1672" ) );
    assert ( !delPhone ( &b, "1671" ) );
    name = findPhone ( &b, "16713425245763" );  assert ( !strcmp ( name, "USA" ) );
    name = findPhone ( &b, "4422" );  assert ( !name );
    name = findPhone ( &b, "420800123456" );  assert ( !name );
    name = findPhone ( &b, "420224351111" );  assert ( !strcmp ( name, "Czech Republic CVUT" ) );
    assert ( b . m_Size == 2 );
    assert ( ! b . m_Root -> m_Name );
    assert ( ! b . m_Root -> m_Child[0] );
    assert ( ! strcmp ( b . m_Root -> m_Child[1] -> m_Name, "USA" ) );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[1] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Name );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[4] );
    assert ( ! strcmp ( b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Name, "Czech Republic CVUT" ) );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[0] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[5] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[3] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[0] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[1] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[2] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[2] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[3] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[4] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[4] -> m_Child[9] );
    assert ( ! b . m_Root -> m_Child[5] );
    assert ( ! b . m_Root -> m_Child[6] );
    assert ( ! b . m_Root -> m_Child[7] );
    assert ( ! b . m_Root -> m_Child[8] );
    assert ( ! b . m_Root -> m_Child[9] );
    assert ( delPhone ( &b, "1" ) );
    assert ( delPhone ( &b, "42022435" ) );
    assert ( !addPhone ( &b, "12345XYZ", "test" ) );
    assert ( b . m_Size == 0 );
    assert ( ! b . m_Root );

    delBook ( &b );
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */