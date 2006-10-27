/**
 * Squid MySQL Authentication
 * 
 * @author      Jacques Marneweck <jacques@php.net>
 * @copyright   2003-2006 Jacques Marneweck
 * @version     $Id$
 * @license     The MIT License http://www.opensource.org/licenses/mit-license.php
 * @link        http://www.ataris.co.za/projects/squid_mysql_auth/
 * @see         http://dev.mysql.com/doc/mysql/en/C.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include <pwd.h>

#define	BUFSIZE	256

MYSQL mysql, *connection = NULL;
MYSQL_RES *result;
MYSQL_ROW row;

static FILE *fh = NULL;
static char host[BUFSIZE];
static char database[BUFSIZE];
static char db_user[BUFSIZE];
static char db_pass[BUFSIZE];

/**
 * currently doing jack with command line options.
 */
int main (int argc, char **argv) {
    struct passwd *pwd;
    char buf[BUFSIZE];
    char *s = {0};
    char *user = {0};
    char *pass = {0};
    char query[BUFSIZE];
    char my_user[BUFSIZE];
    char my_pass[BUFSIZE];

    fh = fopen("/usr/local/etc/squid_mysql_auth.conf", "r");
    if (!fh) {
        (void) printf ("Cannot read the configuration file.");
        exit(1);
    }

    fscanf(fh, "%s", &host);
    fscanf(fh, "%s", &database);
    fscanf(fh, "%s", &db_user);
    fscanf(fh, "%s", &db_pass);
    fclose(fh);

    setvbuf(stdout, NULL, _IOLBF, 0);

    while (1) {
        if (fgets(buf, BUFSIZE, stdin) == NULL)
            break;

        if ((s = strchr(buf, '\n')) == NULL)
            continue;

        *s = '\0';

        if ((s = strchr(buf, ' ')) == NULL) {
            (void) printf("ERR\n");
            continue;
        }

        *s = '\0';
        user = buf;
        pass = s + 1;

        /**
         * Initialise the mysql connection
         */
        if (!connection) {
            if (!mysql_init (&mysql)) {
                fprintf (stderr, "Cannot initialise mysql connection.\n");
                exit (-1);
            }

            /**
             * Connect to the database server
             */
            connection = mysql_real_connect (&mysql, host, db_user, db_pass, database,
                    0, NULL, 0);
            if (!connection) {
                fprintf (stderr, "No Connection.\n");
                exit (-100);
            }

            /**
             * check that the database exists
             */
            if (mysql_select_db (&mysql, database)) {
                fprintf (stderr, "Cannot select database %s.\n", database);
                exit (-100);
            }
        }
        
        (void) mysql_escape_string (my_user, user, strlen(user));
        (void) mysql_escape_string (my_pass, pass, strlen(pass));

        sprintf(query, "SELECT password FROM users WHERE md5(username)=md5('%s') LIMIT 1", my_user);
        
        if (mysql_query (connection, query)) {
            fprintf (stderr, mysql_error(&mysql));
            fprintf (stdout, "ERR\n");
            continue;
        }

        result = mysql_store_result(connection);
        row = mysql_fetch_row((MYSQL_RES *) result);

        if (row) {
            if (check_password(row[0], pass)) {
                fprintf(stdout, "OK\n");
            } else {
                fprintf(stdout, "ERR\n");
            }
        } else {
            fprintf(stdout, "ERR\n");
        }
        mysql_free_result ((MYSQL_RES *) result);
    } /* while (1) */
    mysql_close ((MYSQL *) connection);
    return 0;
}

int check_password (char *passwd, char *enc_passwd)
{
    return (!strcmp(passwd, enc_passwd));
}

/* vim: set noet ts=4 sw=4 ft=c: : */
