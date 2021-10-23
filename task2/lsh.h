#ifndef LSH_INCLUDED
#define LSH_INCLUDED

#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/stat.h>
#include <error.h>
#include <dirent.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define STAT_MAX 256

pid_t pid;
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int my_ls(char **args);
int my_mv(char **args);
int len_p(char **args);
void do_move(char *dist,char **files);
void Eliminate(char * str, char ch);
void INThandler(int sig);
void child_handler(int sig);
void lsh_loop(void);
int lsh_exit(char **args);
int lsh_num_builtins();
char **lsh_split_line(char *line);
char *lsh_read_line(void);
int lsh_execute(char **args);
int lsh_launch(char **args);
int (*builtin_func[5]) (char **);
char *builtin_str[5];

#endif