#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "sqlite3.h"
void create_faculty_table(sqlite3 *db) {
    const char *create_faculty_table_sql =
        "create table faculty("
        "instructor_name TEXT,"
        "department_name TEXT,"
        "office TEXT"
        ");";
    sqlite3_exec(db, create_faculty_table_sql, NULL, NULL, NULL);
}

void create_course_table(sqlite3 *db) {
    const char *create_course_table_sql =
        "create table course("
        "code TEXT,"
        "section INTEGER,"
        "semester INTEGER,"
        "year INTEGER,"
        "title TEXT,"
        "classroom TEXT,"
        "instructor_name TEXT"
        ");";
    sqlite3_exec(db, create_course_table_sql, NULL, NULL, NULL);
}

static int callback_function(void *notUsed, int argc, char **argv, char **azColName) {
    int i = 0;
    for (i = 0; i < argc; i++) {
        if (argv[i] != NULL)
            printf("%s = %s\n", azColName[i], argv[i]);
        else
            printf("%s = %p\n", azColName[i], NULL);
    }
    return 0;
}

int main() {
    char *name, *dept_name, *office, *p;
    int i, fd, rc;
    char line[1024];
    char *pLine;
    char sql_buf[1024];
    sqlite3 *faculty_db, *course_db;

    rc = sqlite3_open("./faculty.db", &faculty_db);
    create_faculty_table(faculty_db);
    create_course_table(faculty_db);

    const char *insert_faculty_sql =
        "insert into faculty(instructor_name,department_name,office) "
        "values('%s','%s','%s')";
    FILE *in = fopen("faculty.txt", "r");
    while (!feof(in)) {
        pLine = fgets(line, 1024, in);
        if (pLine == NULL)
            break;
        name = strtok(pLine, " ");
        dept_name = strtok(NULL, " ");
        office = strtok(NULL, " ");
        office[strlen(office) - 2] = '\0';
        sprintf(sql_buf, insert_faculty_sql, name, dept_name, office);
        sqlite3_exec(faculty_db, sql_buf, NULL, NULL, NULL);
    }
    fclose(in);

    const char *insert_course_sql =
        "insert into course(code,section,semester,year,title,classroom,instructor_name) "
        "values('%s',%s,%s,%s,'%s','%s','%s')";
    char *code, *section, *semester, *year, *title, *classroom, *instructor_name;
    in = fopen("course.txt", "r");
    while (!feof(in)) {
        pLine = fgets(line, 1024, in);
        if (pLine == NULL)
            break;
        code = strtok(pLine, " ");
        section = strtok(NULL, " ");
        semester = strtok(NULL, " ");
        year = strtok(NULL, " ");
        title = strtok(NULL, " ");
        classroom = strtok(NULL, " ");
        instructor_name = strtok(NULL, " ");
        instructor_name[strlen(instructor_name) - 2] = '\0';
        sprintf(sql_buf, insert_course_sql, code, section, semester, year, title, classroom, instructor_name);
        sqlite3_exec(faculty_db, sql_buf, NULL, NULL, NULL);
    }

    const int last_problem = 6;
    const char *problem2_sql =
        "SELECT DISTINCT title "
        "FROM course "
        "WHERE instructor_name='Wookhee_Kim';";
    const char *problem3_sql =
        "SELECT DISTINCT title "
        "FROM course "
        "WHERE code NOT LIKE 'BBAB%';";
    const char *problem4_sql =
        "SELECT * "
        "FROM course "
        "NATURAL JOIN faculty;";
    const char *problem5_sql =
        "SELECT code,section,semester,year,title,classroom,instructor_name "
        "FROM course "
        "NATURAL JOIN faculty "
        "WHERE office LIKE 'Engineering_Building_A%'";
    const char *problem6_sql =
        "SELECT code,section,semester,year,title,classroom,instructor_name "
        "FROM course "
        "NATURAL JOIN faculty "
        "WHERE department_name!='CSE';";
    const char *problem6_sqlv2 =
        "SELECT code,section,semester,year,title,classroom,instructor_name "
        "FROM course "
        "WHERE instructor_name NOT IN ("
        "SELECT instructor_name "
        "from faculty);";
    const char *sql_list[] = {problem2_sql, problem3_sql, problem4_sql, problem5_sql, problem6_sqlv2};
    for (int i = 2; i <= last_problem; i++) {
        printf("===========PROBLEM %d===========\n", i);
        sqlite3_exec(faculty_db, sql_list[i - 2], callback_function, NULL, NULL);
    }

    sqlite3_close(faculty_db);
    return 0;
}
