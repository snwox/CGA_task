#include "lsh.h"

char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "ls",
  "mv",
};
int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &my_ls,
  &my_mv,
};
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}
int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}
int lsh_exit(char **args)
{
  return 0;
}
void INThandler(int sig){
  if(pid){
    kill(pid,SIGTERM);
    puts("\n");
    pid=0;
  }else{
    exit(EXIT_FAILURE);
  }
}

int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}
int lsh_launch(char **args)
{
  int status;
  pid = fork();
  if (pid == 0) {
      
    // Child process

    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  pid=0;
  return 1;
}
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

char *lsh_read_line(void)
{
#ifdef LSH_USE_STD_GETLINE
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We received an EOF
    } else  {
      perror("lsh: getline\n");
      exit(EXIT_FAILURE);
    }
  }
  return line;
#else
#define LSH_RL_BUFSIZE 1024
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
#endif
}
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;
  pid=0;
  struct sigaction act;
    act.sa_handler=INThandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    sigaction(SIGINT,&act,0);
  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);
    free(line);
    free(args);
  } while (status);
}
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
int len_p(char **args){
    int length=0;
    for(;args[length];length++);
    return length;
}
int my_ls(char **args){
    int opts_c=0,mod=0;
    DIR *dir;
    struct dirent *ent;
    dir=opendir("./");
    int args_len=len_p(args);
    if(args_len==1){
      while((ent = readdir(dir))!=NULL){
        if(ent->d_name[0]!='.')
          printf("%s  ",ent->d_name);
      }
    }
    else if(strcmp(args[args_len-1],"-l")==0||
            strcmp(args[args_len-1],"-la")==0||
            strcmp(args[args_len-1],"-al")==0){
      int flag=0,month_d;
      char *permission;
      char *file;
      char *month;
      char *user;
      time_t T;
      struct stat st;
      struct group *g;
      struct passwd *pw;
      struct tm tm;
      char *stats;
      int blocks=0;

      if(strncmp(args[args_len-1],"-la",3)==0)
        flag=1;
      while((ent = readdir(dir))!=NULL){
        file=ent->d_name;
        if(!flag&&file[0]=='.')
          continue;
        if(stat(file,&st)==0){
          blocks+=st.st_blocks/2;
        }
      }
      closedir(dir);
      printf("total %d\n",blocks);
      dir=opendir("./");
      while((ent = readdir(dir))!=NULL){
        permission=malloc(sizeof(char)*11+1);
        file=ent->d_name;
        if(!flag&&file[0]=='.')
          continue;
        if(stat(file,&st)==0){
          mode_t perm=st.st_mode;
          permission[0] = (perm & S_IFSOCK) ? 's' : permission[0];
          permission[0] = (perm & S_IFLNK) ? 'l' : permission[0];
          permission[0] = (perm & S_IFREG) ? '-' : permission[0];
          permission[0] = (perm & S_IFBLK) ? 'b' : permission[0];
          permission[0] = (perm & S_IFDIR) ? 'd' : permission[0];
          permission[0] = (perm & S_IFCHR) ? 'c' : permission[0];
          permission[0] = (perm & S_IFIFO) ? 'p' : permission[0];

          permission[1] = (perm & S_IRUSR) ? 'r' : '-';
          permission[2] = (perm & S_IWUSR) ? 'w' : '-';
          permission[3] = (perm & S_IXUSR) ? 'x' : '-';
          permission[4] = (perm & S_IRGRP) ? 'r' : '-';
          permission[5] = (perm & S_IWGRP) ? 'w' : '-';
          permission[6] = (perm & S_IXGRP) ? 'x' : '-';
          permission[7] = (perm & S_IROTH) ? 'r' : '-';
          permission[8] = (perm & S_IWOTH) ? 'w' : '-';
          permission[9] = (perm & S_IXOTH) ? 'x' : '-';
          permission[10] = '\0';
          month=ctime(&st.st_mtime);
          strtok(month," ");
          month=strtok(NULL," ");
          tm=*localtime(&st.st_mtime);
          month_d=tm.tm_mday;
          // month=strtok(month," ");
          pw=getpwuid(st.st_uid);

              int gid;

          g = getgrgid(st.st_gid);
          printf("%10s %ld %s %s %7lld %3s %2d %02d:%02d %s\n",
          permission,(long)st.st_nlink,pw->pw_name,g->gr_name,
          (long long)st.st_size,month,month_d,tm.tm_hour,tm.tm_min,file);
          free(permission);
        }
      }
    }
    else if(!strcmp(args[args_len-1],"--help")){
      puts("Usage : ls");
      puts("List information about the Files (only current directory");
      puts("Option : ");
      puts("  -l -> with file informations");
      puts("  -la -> with file informations ( include hidden files ");
      puts("  --help -> print help");
    }
    else{
      puts("no option");
    }
    puts("");
    closedir(dir);
    return 1;
}
void Eliminate(char * str, char ch)
{
    for (; *str != '\0'; str++)
    {
        if (*str == ch)
        {
            strcpy(str, str + 1);
            str--;
        }
    }
}
void do_move(char *dist,char **files){
  struct stat st;
  char temp[STAT_MAX],buf[4096];
  FILE *fp,*fp2;
  DIR *dir;
  char *file;
  struct dirent *ent;
  int permission;
  int n=0;
  char **next_files=malloc(STAT_MAX*(sizeof(char*)));
  int c=0;
  char *next_dist=malloc(STAT_MAX);
  for(int i=0;i<len_p(files);i++){
    if(!access(files[i],F_OK)){
      if(!stat(files[i],&st)){
        if(st.st_mode&S_IFDIR){
          dir=opendir(files[i]);
          if(stat(files[i],&st)==0){
            memset(next_dist,0,STAT_MAX);
            memset(temp,0,4096);
            sprintf(temp,"%s/%s",dist,files[i]);
            strcpy(next_dist,dist);
            mode_t perm=st.st_mode;
            mkdir(temp,(perm&S_IRWXU)|(perm&S_IRWXG)|(perm&S_IRWXO));
          }
          while((ent = readdir(dir))!=NULL){
            file=ent->d_name;
            if(strcmp(file,".")==0 || strcmp(file,"..")==0)
              continue;
            memset(temp,0,4096);
            sprintf(temp,"%s/%s",files[i],file);
            if(stat(temp,&st)==0){
              mode_t perm=st.st_mode;
              next_files[c]=malloc(STAT_MAX);
              strcpy(next_files[c++],temp);
            }
          }
          do_move(next_dist,next_files);
          rmdir(files[i]);
          closedir(dir);
        }else{
          memset(temp,0,STAT_MAX);
          memset(buf,0,4096);
          sprintf(temp,"%s/%s",dist,files[i]);
          fp=fopen(temp,"wb+");
          fp2=fopen(files[i],"rb");
          while((n=fread(buf,sizeof(char),4096,fp2)),n>0){
            fwrite(buf,sizeof(char),n,fp);
          }
          fclose(fp);
          fclose(fp2);
          remove(files[i]);
        }
      }
    }else{
      printf("%s not exist\n",files[i]);
    }
  }

  return;
}
int my_mv(char **args){
    struct stat st;
    mode_t mode;
    int i;
    char **files=malloc(STAT_MAX*sizeof(char*));
    char *dist;
    if(stat(args[len_p(args)-1],&st)==0){
      if(st.st_mode&S_IFDIR){
        for(i=1;i<len_p(args)-1;i++){
          files[i-1]=malloc(STAT_MAX);
          strcpy(files[i-1],args[i]);
        }
        files[i]=0;
        dist=args[len_p(args)-1];
        do_move(dist,files);
      }else{
        if(len_p(args)==3){
          if(access(args[1],F_OK)==0){
            rename(args[1],args[2]);
          }else{
            printf("No file exist\n");
          }
        }else{
          printf("too many files/dirs to rename\n");
        }
      }
    }else if(len_p(args)==3){
      if(access(args[1],F_OK)==0){
        rename(args[1],args[2]);
      }
    }else{
      printf("just rename or move\n");
    }
    // for(int j=0;j<i;j++){
    //   free(files[j]);
    // }
    // free(files);
    return 1;
}
