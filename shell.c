#include <ctype.h>              
#include <stdio.h>             
#include <stdlib.h>             
#include <string.h>           
#include <sys/types.h>       
#include <sys/wait.h>           
#include <unistd.h>             
#include "smp1_tests.h"        

/* DEFINE SECTION */
#define SHELL_BUFFER_SIZE 256   
#define SHELL_MAX_ARGS 8        
#define SHELL_MAX_COMM 10
/* VARIABLE SECTION */
char comm[10][SHELL_BUFFER_SIZE];    // stores commands
int comm_count = 0;
enum { STATE_SPACE, STATE_NON_SPACE };

void update_history(char *h)
{
	int i = 0; // creating a counter starting with one
	if (comm_count == SHELL_MAX_COMM)
	{
		for (i = 0; i < SHELL_MAX_COMM - 1; i++)
		{
			memset(comm[i], 0, SHELL_BUFFER_SIZE);
			strcpy(comm[i], comm[i + 1]);
		}
		strcpy(comm[i], h);
		return;
	}
	else
	{
		strcpy(comm[comm_count], h);
		comm_count++; // increase counter
	}
}


int imthechild(const char *path_to_exec, char *const args[])
{
	// TO-DO P5.1
	return execvp(path_to_exec, args) ? -1 : 0; //return path
}

void imtheparent(pid_t child_pid, int run_in_background)
{
	int child_return_val, child_error_code;

	//fork returned a positive pid so we are the parent 

	fprintf(stderr,
		"  Parent says 'child process has been forked with pid=%d'\n",
		child_pid);
	if (run_in_background) {
		fprintf(stderr,
			"  Parent says 'run_in_background=1 ... so we're not waiting for the child'\n");
		return;
	}
	// TO-DO P5.4
	waitpid(child_pid, &child_return_val, run_in_background);// using waitpid needed extra parameters
	// Use the WEXITSTATUS to extract the status code from the return value 
	child_error_code = WEXITSTATUS(child_return_val);
	fprintf(stderr,
		"  Parent says 'wait() returned so the child with pid=%d is finished.'\n",
		child_pid);
	if (child_error_code != 0) {

		fprintf(stderr,
			"  Parent says 'Child process %d failed with code %d'\n",
			child_pid, child_error_code);
	}
}

int main(int argc, char **argv)
{
	pid_t shell_pid, pid_from_fork;
	int n_read, i, exec_argc, parser_state, run_in_background;
	char buffer[SHELL_BUFFER_SIZE];
	char *exec_argv[SHELL_MAX_ARGS + 1];
	// TO-DO new variables for P5.2, P5.3, P5.6
	int counter = 0;
	char History[9][SHELL_BUFFER_SIZE]; // taking values 1-9
	int History_Counter = 0; 
	int Not_Valid = 0;
	int sub = 1;


	if (argc > 1 && !strcmp(argv[1], "-test")) {
		return run_smp1_tests(argc - 1, argv + 1);
	}
	shell_pid = getpid();
	counter++; // count proc
	while (1) {
		// TO-DO P5.2
		//add %d and counter to print

		fprintf(stdout, "Shell(pid=%d)%d> ", shell_pid, counter); //add the correct printf format
		fflush(stdout);

		// reading line
		if (fgets(buffer, SHELL_BUFFER_SIZE, stdin) == NULL)
			return EXIT_SUCCESS;
		n_read = strlen(buffer);

		if (buffer[0] == '!')
		{
			if (strlen(buffer) > 2 && strlen(buffer) < 4)
			{
				char history_num = buffer[1];
				int index = 0;

				if (history_num > 48 && history_num < 58)
				{
					index = history_num - 48;

					if (index > comm_count)
					{
						fprintf(stderr, "Not valid\n");
						continue;
					}
					else
					{
						char temp[SHELL_BUFFER_SIZE] = "";
						strcpy(temp, comm[index - 1]);
						memset(buffer, 0, sizeof(buffer));
						strcpy(buffer, temp);
						n_read = strlen(buffer);
					}
				}
				else
				{
					fprintf(stderr, "Not valid\n");
					continue;
				}
			}
			else
			{
				fprintf(stderr, "Not valid\n");
				continue;
			}
		}
		else
		{
			update_history(buffer);
		}

		run_in_background = n_read > 2 && buffer[n_read - 2] == '&';
		buffer[n_read - run_in_background - 1] = '\n';

		// TO-DO P5.3

		/* Parse the arguments: the first argument is the file or command *
		 * we want to run.                                                */

		parser_state = STATE_SPACE;
		for (exec_argc = 0, i = 0;
			(buffer[i] != '\n') && (exec_argc < SHELL_MAX_ARGS); i++) {

			if (!isspace(buffer[i])) {
				if (parser_state == STATE_SPACE)
					exec_argv[exec_argc++] = &buffer[i];
				parser_state = STATE_NON_SPACE;
			}
			else {
				buffer[i] = '\0';
				parser_state = STATE_SPACE;
			}
		}

		/* run_in_background is 1 if the input line's last character *
		 * is an ampersand (indicating background execution).        */


		buffer[i] = '\0';	/* Terminate input, overwriting the '&' if it exists */

		/* If no command was given (empty line) the Shell just prints the prompt again */
		if (!exec_argc)
			continue;
		/* Terminate the list of exec parameters with NULL */
		exec_argv[exec_argc] = NULL;

		if (History_Counter < 9) // values no more than 9
		{

			strcpy(History[History_Counter], exec_argv[0]);

			History_Counter++;
		}
		else {
			for (int i = 0; i < 8; i++) {
				printf("%s =     %s =       \n\n", History[i], History[i + 1]);
				strcpy(History[i], History[i + 1]);
				printf("%s =     %s =       \n\n", History[i], History[i + 1]);
			}
			strcpy(History[History_Counter - 1], exec_argv[0]);
		}*/
			/* If Shell runs 'exit' it exits the program. */
			if (!strcmp(exec_argv[0], "exit")) {
				printf("Exiting process %d\n", shell_pid);
				return EXIT_SUCCESS;	/* End Shell program */

			}
			else if (!strcmp(exec_argv[0], "cd") && exec_argc > 1) {
				/* Running 'cd' changes the Shell's working directory. */
					/* Alternative: try chdir inside a forked child: if(fork() == 0) { */
				if (chdir(exec_argv[1]))
					/* Error: change directory failed */
					fprintf(stderr, "cd: failed to chdir %s\n", exec_argv[1]);
				/* End alternative: exit(EXIT_SUCCESS);} */


			}
			else {
				while (exec_argv[0][0] == '!') {
					int L = (int)(exec_argv[0][1]) - 48;
					int times = 0;
					if (L < 1 || L > History_Counter - 1) {
						fprintf(stderr, "Not valid\n");
						Not_Valid = 1;
						break;
					}
					else {

						strcpy(exec_argv[0], History[L - 1]);
						times++;
					}
				}
				if (Not_Valid) {
					Not_Valid = 0;
					continue;
				}

				/* Execute Commands */

				pid_from_fork = fork();

				if (pid_from_fork < 0) {
					// error for failed
					fprintf(stderr, "fork failed\n");
					continue;
				}
				if (pid_from_fork == 0) {

					// TO-DO P5.6
					if (!strcmp(exec_argv[0], "sub"))//  prevent subsells
					{
						if (sub + 1 <= 3) {
							strcpy(exec_argv[0], argv[0]);
							sub++;
							History_Counter = 0;
							counter = 0;
							shell_pid = getpid();
						}
						else {
							fprintf(stderr, "Too deep!\n");
							return imthechild(exec_argv[0], &exec_argv[0]);
						}
					}
					else
						return imthechild(exec_argv[0], &exec_argv[0]);
					/* Exit from main. */
				}
				else {
					counter++;
					imtheparent(pid_from_fork, run_in_background);
					/* Parent will continue around the loop. */
				}
			} /* end if */
	} /* end while loop */

	return EXIT_SUCCESS;
} /* end main() */
