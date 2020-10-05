/*

  Name: Henry Isobe
  ID: 1000915977

*/

#define _GNU_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports five arguments

int main()
{

  // initializing variables
  // this includes the history of commands array and history of pids array
  int i, indexp = 0, indexh =0;
  char *history[15]; 
  int history_count1 = 0; 
  int history_count2 = 0;
  int pid_history[15];
  int pid_count1 = 0;
  int pid_count2 = 0;

  //allocates the memeory for the command string that will be given by user
  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                               
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // saves the history of commands here
    // will also reset history_count1 to 0 if needed
    // history_count2 is used to keep track of command count below 15
    if(strcmp(cmd_str, "\n") != 0)
    {
      history[history_count1++] = strndup(cmd_str, MAX_COMMAND_SIZE);
      history_count2++;
      if(history_count1 >14)
      {
        history_count1 = 0;
      }
    }

    // this is where we will do the ! command replacement
    // also checks to see if the !nth command even exists 
    if(cmd_str[0] == '!')
    {
      int int_value = atoi(&cmd_str[1]);
      if(int_value > history_count2)
      {
        printf("Command not in history.\n");
      }
      else
      {
        working_str = strdup(history[int_value]);
      }
    }

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // will keep on printing msh> on new line if no input is given
    // this is done by comparing the first command to NULL
    // continue will finish this iteration of the loop and go to the next one
    if(token[0] == NULL)
    {
      continue; 
    }

    // exit or quit command
    // compares the first token with the word "exit" or "quit"
    // shell will exit if command is "exit" or "quit"
    int compare = 1;
    compare = strcmp(token[0],"quit");
    if(compare != 0)
    {
      compare = strcmp(token[0],"exit");
    }
    else if(compare == 0)
    {
      exit(0);
    }

    // history command to show the last 15 commands
    // prints out list of history from array
    // separated for when there are less than or greater than 15 commands made
    if(strcmp(token[0], "history") == 0)
    {
      if(history_count2 <= 15)
      {
        for(i = 0; i < history_count2; i++)
        {
          printf("%d: %s", i, history[i]);
        }
      }
      indexh = history_count1;
      if(history_count2 > 15)
      {
        for(i = 0; i < 15; i++)
        {
          printf("%d: %s", i, history[indexh++]);
          if(indexh > 14)
          {
            indexh = 0;
          }
        }
      }
      continue;
    }

    // shows at least 15 processes spawned by shell
    // shows the process ids from the pid array
    // separated for when there are less than or greater than 15 processes
    if(strcmp(token[0], "showpids") == 0)
    {
      if(pid_count2 <= 15)
      {
        for(i = 0; i < pid_count2; i++)
        {
          printf("%d: %d\n", i, pid_history[i]);
        }
      }
      indexp = pid_count1;
      if(pid_count2 > 15)
      {
        for(i = 0; i < 15; i++)
        {
          printf("%d: %d\n", i, pid_history[indexp++]);
          if(indexp > 14)
          {
            indexp = 0;
          }
        }
      }
      continue;
    }

    // if command is cd, will use chdir to change directories
    // it will use token[1] since token[0] is the cd command
    if(strcmp(token[0], "cd") == 0)
    {
      chdir(token[1]); 
      continue;
    }

    // forks the process to create a child process
    pid_t pid = fork();
    // child pprocess
    if(pid == 0)
    {
      // turns child process into an individual one with execvp()
      // checks to see if execvp failed or not.
      int ret = execvp(token[0], &token[0]);
      if(ret == -1)
      {
        printf("%s: Command not found\n\n", token[0]);
      }
      exit(0);
    }

    // parent process
    // also adds the parent pid to the pid history array
    // then waits for child process to end
    else
    {
      int status;
      wait(&status);
      
      pid_history[pid_count1++] = pid;
      pid_count2++;
      if(pid_count1 > 14)
      { 
        pid_count1 = 0;
      }
    }

    // frees initial memory allocated for the command input
    free( working_root );
  }
  return 0;
}

