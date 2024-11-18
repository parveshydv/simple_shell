#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

// Struct to track background jobs
typedef struct bg_process {
    pid_t pid;
    char command[512];//considering command size <=512 
    struct bg_process *next;
} bg_process;

// Head of the background process list
bg_process *bg_list_head = NULL;

void add_bg_process(pid_t process_id, const char *cmd) {
    bg_process *bg_proc = (bg_process *)malloc(sizeof(bg_process)); 
    bg_proc->pid = process_id; 
    strncpy(bg_proc->command, cmd, sizeof(bg_proc->command) - 1); 
    bg_proc->command[sizeof(bg_proc->command) - 1] = '\0'; 
    bg_proc->next = bg_list_head; 
    bg_list_head = bg_proc; 
}


//  removing  process from  background list
void remove_bg_process(pid_t pid) {
    bg_process *prev = NULL, *cur = bg_list_head;

    while (cur != NULL && cur->pid != pid) {
        prev = cur;
        cur = cur->next;
    }

    if (cur != NULL) {  
        if (prev == NULL) {
            bg_list_head = cur->next;
        } else {
            prev->next = cur->next;
        }
        free(cur);
    }
}

//func for bacground list displaying
void list_bg_processes() {
    bg_process *cur = bg_list_head;
    int count = 0;
    while (cur != NULL) {
        printf("%d: %s\n", cur->pid, cur->command);
        cur = cur->next;
        count=count+1;
    }
    printf("Total Background jobs: %d\n", count);
}

// Check for terminated bg  processes
void check_bg_processes() {
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Process %d has terminated.\n", pid);
        remove_bg_process(pid);
    }
}




void change_directory(char **args) {
    const char *dir = args[1];

    if (dir == NULL || strcmp(dir, "~") == 0) {
        dir = getenv("HOME"); 
    }

    if (chdir(dir) != 0) {
        perror("cd");  // Display error if chdir fails
    }
}



void execute_command(char **args, int bg_flag) {
    pid_t pid = fork();

    switch (pid) {
        case -1:  // Error case
            perror("fork");
            break;
        
        case 0:  // Child process
            if (execvp(args[0], args) == -1) {
                perror("exec");
                exit(EXIT_FAILURE);
            }
            break;
        
        default:  // Parent process
            if (bg_flag) {
                add_bg_process(pid, args[0]);
                printf("[%d] %s started in background\n", pid, args[0]);
            } else {
                waitpid(pid, NULL, 0);  // Foreground process
            }
            break;
    }
}


int main() {
    char cwd[1024], hostname[256], command_line[1024];
    char *username, *token;
    char *args[100];
    int bg_flag;

    // Retrieve username and hostname
    username = getlogin();
    gethostname(hostname, sizeof(hostname));

    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s@%s: %s > ", username, hostname, cwd);
        } else {
            perror("getcwd error");
        }
        if (fgets(command_line, sizeof(command_line), stdin) == NULL) {
            continue; 
        }
        command_line[strcspn(command_line, "\n")] = '\0';

        // Ignore empty input
        if (command_line[0] == '\0') {
            continue;
        }

        // Tokenize the command line
        int i = 0;
        token = strtok(command_line, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Check for background flag ('bg' command)
        bg_flag = 0;
        if (strcmp(args[0], "bg") == 0) {
            bg_flag = 1;
            for (int j = 0; args[j + 1] != NULL; j++) {
                args[j] = args[j + 1];  
            }
            args[i - 1] = NULL;  
        }
       
        // Handle built-in commands
        if (strcmp(args[0], "cd") == 0) {
            change_directory(args);
        } else if (strcmp(args[0], "bglist") == 0) {
            list_bg_processes();
        } else if (strcmp(args[0], "exit") == 0) {
            break;  
        } else {
            execute_command(args, bg_flag);
        }

        check_bg_processes();
    }
    return 0;
}

