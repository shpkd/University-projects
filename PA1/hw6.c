#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct{
    char **grid;
    int rows, columns;
} Osmismerka;

typedef struct{
    int x, y;
} Direction;


int counter(char **grid, int columns, int rows, const char* word, int length){
    int count=0;
    Direction direction[]={{-1,-1},{1,1},{1,-1},{-1,1}, {0,1}, {1,0}, {-1,0}, {0,-1}};

    for(int r=0; r<rows; r++){
        for(int c=0; c<columns; c++){
            for(int d=0; d<8; d++){
                int x=r;
                int y=c;

                int i;
                for(i=0; i<length; i++){
                    if(x<0 || y<0 || x>=rows || y>=columns || grid[x][y]!=word[i]){
                       break;
                    }
                    x+=direction[d].x;
                    y+=direction[d].y;
                }

                if(i==length){
                    count++;
                }
            }
        }
    }
    return count;
}

void replace(char **grid,char **modified, int columns, int rows, const char* word, int length){

    Direction direction[]={{-1,-1},{1,1},{1,-1},{-1,1}, {0,1}, {1,0}, {-1,0}, {0,-1}};

    for(int r=0; r<rows; r++){
        for(int c=0; c<columns; c++){
            for(int d=0; d<8; d++){
                int x=r;
                int y=c;

                int i;
                for(i=0; i<length; i++){
                    if(x<0 || y<0 || x>=rows || y>=columns || grid[x][y]!=word[i]){
                        break;
                    }
                    x+=direction[d].x;
                    y+=direction[d].y;
                }

                if(i==length){
                    x=r;
                    y=c;
                    for(i=0; i<length; i++){
                        modified[x][y]='.';
                        x+=direction[d].x;
                        y+=direction[d].y;
                    }
                }
            }
        }
    }
}



int main(void) {
    //initialize struct osmismerka
    Osmismerka basic;
    basic.rows=0;
    basic.columns=0;

    int capacity=10;
    basic.grid=(char **)malloc(capacity * sizeof(char *));

    if(!basic.grid){
        return 1;
    }


    //read and validate text
    printf("Osmismerka:\n");

    char *text=NULL;
    size_t textSize=0;


    int empty=0;

    while(getline(&text, &textSize, stdin) != -1){
        text[strcspn(text, "\n")]=0;

        if(strlen(text) == 0){
            empty=1;
            break;
        }

        for(int i=0; text[i] != '\0'; i++){
            if(!islower(text[i]) && text[i] != '.'){
                printf("Nespravny vstup.\n");

                free(text);

                for(int j=0; j <basic.rows; j++){
                    free(basic.grid[j]);
                }
                free(basic.grid);

                return 1;
            }
        }

        if(basic.rows == 0){
            basic.columns= (int)strlen(text);
        }
        else if(basic.columns != (int)strlen(text)){
            printf("Nespravny vstup.\n");

            free(text);
            for(int i=0; i < basic.rows; i++){
                free(basic.grid[i]);
            }
            free(basic.grid);

            return 1;
        }

        if(basic.rows >= capacity){
            capacity*=2;
            char **tmp=(char **)realloc(basic.grid, capacity * sizeof(char *));

            if(!tmp){
                free(text);
                for(int i=0; i < basic.rows; i++){
                    free(basic.grid[i]);
                }

                return 1;
            }
            basic.grid=tmp;
        }

        basic.grid[basic.rows]=strdup(text);
        if(!basic.grid[basic.rows]){
            free(text);
            for(int i=0; i < basic.rows; i++){
                free(basic.grid[i]);
            }
            free(basic.grid);

            return 1;
        }

        basic.rows++;
    }

    free(text);//----------------------------------------------free text


    if(empty==0 || basic.rows < 1 || basic.columns < 1){
        printf("Nespravny vstup.\n");

        for(int i=0; i < basic.rows; i++){
            free(basic.grid[i]);
        }
        free(basic.grid);

        return 1;
    }


    Osmismerka modified;
    modified.rows=basic.rows;
    modified.columns=basic.columns;

    modified.grid=(char **)malloc(modified.rows * sizeof(char *));
    if(!modified.grid){
        //free(modified.grid);

        for(int i=0; i < basic.rows; i++){
            free(basic.grid[i]);
        }
        free(basic.grid);

        return 1;
    }

    for(int i=0; i<modified.rows; i++){
        modified.grid[i]=(char *)malloc((modified.columns+1)*sizeof(char));
        if(!modified.grid[i]){
            for(int j=0; j<i; j++) {
                free(modified.grid[j]);
            }

            free(modified.grid[i]);

            for(int j=0; j<basic.rows; j++) {
                free(basic.grid[j]);
            }
            free(basic.grid);

            return 1;
        }

        strcpy(modified.grid[i], basic.grid[i]);
    }


    char *command=NULL;
    size_t commandSize=0;

    while(getline(&command, &commandSize, stdin)!=-1){
        command[strcspn(command,"\n")]=0;

//        if(strlen(command)==0){
//            printf("Nespravny vstup.\n");
//            free(command);
//
//            for(int j=0; j < modified.rows; j++){
//                free(modified.grid[j]);
//            }
//            free(modified.grid);
//
//            for(int i=0; i < basic.rows; i++){
//                free(basic.grid[i]);
//            }
//            free(basic.grid);
//
//            return 1;
//        }

        if(command[0]=='-' || command[0]=='#'){
            char *word=command+1;


            while(*word==' ' || *word=='\t' || *word=='\n'){
                word++;
            }


            if(strlen(word)<2){
                printf("Nespravny vstup.\n");

                free(command);

                for(int j=0; j < modified.rows; j++){
                    free(modified.grid[j]);
                }
                free(modified.grid);

                for(int i=0; i < basic.rows; i++){
                    free(basic.grid[i]);
                }
                free(basic.grid);

                return 1;
            }

            for(int i=0; word[i] != '\0'; i++){
                if(!islower(word[i])){
                    printf("Nespravny vstup.\n");

                    free(command);

                    for(int j=0; j < modified.rows; j++){
                        free(modified.grid[j]);
                    }
                    free(modified.grid);

                    for(int j=0; j < basic.rows; j++){
                        free(basic.grid[j]);
                    }
                    free(basic.grid);

                    return 1;
                }
            }

            int count=0;

            if(command[0]=='-'){
                count=counter(basic.grid, basic.columns, basic.rows, word, strlen(word));
                replace(basic.grid,modified.grid, modified.columns, modified.rows, word, strlen(word));
            }
            else if(command[0]=='#'){
                count=counter(modified.grid, modified.columns, modified.rows, word, strlen(word));
            }

            printf("%s: %dx\n",word,count);
        }
        else if(command[0]=='?'){
            if(command[1]!='\0'){
                printf("Nespravny vstup.\n");

                free(command);

                for(int j=0; j < modified.rows; j++){
                    free(modified.grid[j]);
                }
                free(modified.grid);

                for(int j=0; j < basic.rows; j++){
                    free(basic.grid[j]);
                }
                free(basic.grid);

                return 1;
            }

            char *secret=(char *)malloc((modified.rows*modified.columns+1)*sizeof(char));
            if(!secret){
                //free(secret);
                free(command);

                for(int j=0; j < modified.rows; j++){
                    free(modified.grid[j]);
                }
                free(modified.grid);

                for(int j=0; j < basic.rows; j++){
                    free(basic.grid[j]);
                }
                free(basic.grid);

                return 1;
            }

            int i=0;
            for(int r=0; r<modified.rows; r++){
                for(int c=0; c<modified.columns; c++){
                    if(modified.grid[r][c]!='.'){
                        secret[i]=modified.grid[r][c];
                        i++;
                    }
                }
            }

            secret[i]='\0';

            if(i==0){
                printf("Tajenka:\n");
            }
            else{
                printf("Tajenka:\n");
                for(int j=0; j<i; j+=60){
                    printf("%.60s\n", secret + j);
                }
            }
            free(secret);
        }
        else{
            printf("Nespravny vstup.\n");

            free(command);

            for(int i=0; i < modified.rows; i++){
                free(modified.grid[i]);
            }
            free(modified.grid);

            for(int i=0; i < basic.rows; i++){
                free(basic.grid[i]);
            }
            free(basic.grid);

            return 1;
        }
    }





    free(command);


    for(int i=0; i < modified.rows; i++){
        free(modified.grid[i]);
    }
    free(modified.grid);

    for(int i=0; i < basic.rows; i++){
        free(basic.grid[i]);
    }
    free(basic.grid);

    return 0;
}
