#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

void handler (int sig)
{
    pid_t pid;
    int stat_loc;
    FILE*fp;
    pid = waitpid(-1,&stat_loc,WNOHANG);
    fp = fopen("terminated.log","a");
    if(fp!=NULL){
    fprintf(fp,"child terminated\n");
    }
    fclose(fp);
}

int cd(char path[])
{

    return chdir(path);

}
int parse_line(char *command[],char input[])
{
    int i=0;
    gets(input);
    input[strlen(input)]= NULL;
    char *token=strtok(input," ");
    while( token != NULL)
    {
        command[i] = token;
        token=strtok(NULL," ");
        i++;

    }
    command[i]=NULL;
    return i;
}
int main()
{
    char *command[10]; // max length of one word
    char line[255];    // max length for the whole line
    pid_t child_pid;
    int stat_loc;
    int bg =0; //background flag
    signal(SIGCHLD,handler);
    while(1)
    {

        //read the input
        int size = parse_line(command,line);
        if(strcmp(command[0],"exit")==0) // if the first command is exit the process are terminated
        {
            exit(0);
            break;
        }
        if(strcmp(command[0],"cd")==0)
        {
            if(cd(command[1])<0)
            {
                perror(command[1]);
            }
            continue; // don't fork move to the next instruction
        }
        //create a child process and execute the command
        child_pid = fork();
        //check for bg process
        if(strcmp(command[size-1],"&") == 0) // if its a backgound process set the bg flag
        {
            bg=1;
        }

        if(child_pid < 0)
        {
            perror("fork failed");
            exit(1);
        }

        if(child_pid == 0)
        {
            //never returns if successful
            if(execvp(command[0], command) < 0)
            {
                perror(command[0]);
                exit(1);
            }
        }
        else
        {
            if(bg==1 )
            {

                continue;
            }

            waitpid(child_pid,&stat_loc,WUNTRACED);


        }

    }
    return 0;
}