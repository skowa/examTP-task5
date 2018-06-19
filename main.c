//
//  main.c
//  example5
//
//  Created by Alexandra Gushchenskova on 6/1/18.
//  Copyright © 2018 Alexandra Gushchenskova. All rights reserved.
//

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#define bool char
#define true 1
#define false 0
#define MAXLEN 50

struct student
{
    int id;
    char firstname[MAXLEN];
    char lastname[MAXLEN];
    char patronymic[MAXLEN];
    int birthYear;
    char gender[MAXLEN];
    int marks[10];
};

void showMenu()
{
    printf("Input:\n");
    printf("1 to insert student\n");
    printf("2 to insert mark\n");
    printf("3 to exit program\n");
}

bool INSERTst(sqlite3 *db);
bool INSERTmark(sqlite3 *db);

bool flag = false;

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    if (argc != 0)
    {
        flag = true;
    }
    return 0;
}

int main() {
    sqlite3 *db;
    
    int rc = sqlite3_open("/Users/skowa/Documents/study.db", &db);
    
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    int userChoice;
    bool flag = true;
    
    
    while (flag)
    {
        showMenu();
        scanf("%d", &userChoice);
        
        switch (userChoice)
        {
            case 1:
                INSERTst(db);
                break;
            case 2:
                INSERTmark(db);
                break;
            case 3:
                flag = false;
                break;
            default:
                break;
        }
    }
    sqlite3_close(db);
    return 0;
}

bool INSERTst(sqlite3 *db) //also demonstrates autocommit inserting
{
    struct student st;
    char* sql = malloc(700);
    char* err_msg = 0;
    getchar();
    printf("Insert firstname:");
    scanf("%s", st.firstname);
    
    printf("Insert patronymic:");
    scanf("%s",st.patronymic);
    
    printf("Insert lastname:");
    scanf("%s", st.lastname);
    
    printf("Year of birth: ");
    scanf("%i", &st.birthYear);
    
    printf("Gender: ");
    scanf("%s", st.gender);
    
    getchar();
    
    sprintf(sql, "insert into students(firstname, patronymic, lastname, birthYear, gender)\
            values ('%s', '%s', '%s', '%i', (select id from gender where gender='%s'));",
            st.firstname, st.patronymic, st.lastname, st.birthYear, st.gender);
    
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        free(sql);
        
        return 0;
    }
    free(sql);
    
    return 1;
}

bool INSERTmark(sqlite3 *db) //also demonstrates transaction inserting
{
    struct student st;
    char* sql = malloc(700);
    char* err_msg = 0;
    char subject[MAXLEN];
    int mark;
    
    getchar();
    printf("Insert firstname:");
    scanf("%s", st.firstname);
    
    printf("Insert lastname:");
    scanf("%s", st.lastname);
    
    sprintf(sql, "select lastname, firstname\
            from students where lastname='%s' and firstname='%s'", st.lastname, st.firstname);
    
    int  rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    if (!flag)
    {
        printf("There is no such student!");
        return 0;
    }
    
    printf("Insert subject:");
    scanf("%s", subject);
    
    printf("Insert mark:");
    scanf("%i", &mark);
    
    getchar();
    
    sprintf(sql, "begin transaction; \
            insert into marks(student, subject, mark)\
            values ((select id from students where firstname='%s' and lastname='%s'),\
            (select id from subjects where subject='%s'), '%i');\
            commit;", st.firstname, st.lastname, subject, mark);
    
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        free(sql);
        
        return 0;
    }
    free(sql);
    
    return 1;
}
