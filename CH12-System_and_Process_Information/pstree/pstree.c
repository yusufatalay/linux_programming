#include "pstree.h"

process_node *process_ll_head;  // the latest node
process_node *process_ll_start; // the first node
process_node *ghost_process; // top of the process tree (ghost because such process does not exist)

process_node *create_and_insert_process_node_ll(const char *name,
                                                pid_t pid, pid_t ppid) {
    process_node *res;
    res = (process_node *) malloc(sizeof(process_node));
    if (res == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(res->name, name);
    res->pid = pid;
    res->ppid = ppid;
    res->next = NULL;
    res->child_array_index = 0;

    if (process_ll_start == NULL) { // first call
        process_ll_start = res;
        process_ll_head =  process_ll_start;
    }

    // previous process' head will be newly created node
    process_ll_head->next = res;
    process_ll_head = res; // update the head

    return res;
}

process_node *find_node_in_ll(pid_t pid) {
    for (process_node *i = process_ll_start; i != NULL; i = i->next) {
        if (i->pid == pid) {
            return i;
        }
    }
    return NULL;
}

void free_everything(void) {
    process_node *tmp;
    while (process_ll_start != NULL) {
        tmp = process_ll_start;
        process_ll_start = process_ll_start->next;
        free(tmp);
    }

    free(ghost_process);
}

void create_tree(void){
   // initite ghost process
   // we are doing this because in my system two processes have ppid of 0 (systemd and kthreadd)


  ghost_process = (process_node *) malloc(sizeof(process_node));
    if (ghost_process == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(ghost_process->name, "GHOST PROCESS");
    ghost_process->pid = 0;
    ghost_process->ppid = 0;
    ghost_process->parent = NULL;
    ghost_process->next = NULL;
    ghost_process->child_array_index = 0;

    process_node  *parent;

    // the process ids' are already sorted increasing.
    for (process_node *i = process_ll_start; i != NULL; i = i->next) {
      if (i->ppid == 0)  {
          i->parent = ghost_process;
          ghost_process->childs[ghost_process->child_array_index++] = i;
      }else{
         parent=  find_node_in_ll(i->ppid);;
         if(parent == NULL){
             printf("find_node_in_ll\n");
             exit(EXIT_FAILURE);
         }

         parent->childs[parent->child_array_index++] = i;
         i->parent = parent;
      }
    }
}

void print_tree_recursive(process_node *node, int depth, int isLast) {
    // Print dashes for the current depth
    for (int i = 1; i < depth; i++) {
        printf("%s", isLast ? "    " : "|   "); // Adjust spacing based on depth
    }

    // Print vertical dash if not the root
    if (depth > 0) {
        printf("%s", isLast ? "`-- " : "|-- ");
    }

    // Print the current node
    printf("%s(%d)\n", node->name, node->pid);

    // Recursively print child nodes
    for (uint i = 0; i < node->child_array_index; i++) {
        print_tree_recursive(node->childs[i], depth + 1, i == node->child_array_index - 1);
    }
}

void print_tree(void) {
    printf("GHOST PROCESS(0)\n");

    // Start printing from the ghost process
    for (uint i = 0; i < ghost_process->child_array_index; i++) {
        print_tree_recursive(ghost_process->childs[i], 1, i == ghost_process->child_array_index - 1);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        printf("Usage: %s \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    DIR *procDIR;
    FILE *statusFiled;
    struct dirent *procDirContent;

    char *processStatusFilePath;

    char *endptr = "";

    char statusFileLine[STATUS_FILE_LINE_LEN];

    procDIR = opendir(PROC_PATH);
    if (procDIR == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((procDirContent = readdir(procDIR)) != NULL) {
        if ((strcmp(procDirContent->d_name, ".") == 0) ||
            (strcmp(procDirContent->d_name, "..") == 0)) {
            continue;
        }
        if (procDirContent->d_type == DT_DIR) {
            // make sure the filename is numeric (all processes have numeric names)
            strtol(procDirContent->d_name, &endptr, 10);
            if (*endptr == '\0') {
                // We've found process directory
                // construct path to that directory  /proc/<name>/status
                processStatusFilePath =
                        malloc(strlen(PROC_PATH) + strlen(procDirContent->d_name) +
                               strlen(STATUS_PATH) + 1);
                if (processStatusFilePath == NULL) {
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }
                processStatusFilePath[strlen(PROC_PATH) +
                                      strlen(procDirContent->d_name)] = '\0';
                processStatusFilePath = strcpy(processStatusFilePath, PROC_PATH);
                processStatusFilePath =
                        strcat(processStatusFilePath, procDirContent->d_name);
                processStatusFilePath = strcat(processStatusFilePath, STATUS_PATH);

                statusFiled = fopen(processStatusFilePath, "r");
                if (statusFiled == NULL) {
                    // we've detected the file but cannot open it, probably because
                    // the corresponding process no longer exists
                    fprintf(stderr,
                            "file %s detected but cannot opened, probably because the "
                            "corresponding process does not exists\n",
                            processStatusFilePath);

                } else {
                    // these three values are defined for each file
                    char *CmdNameValue;
                    char *CmdPIDValue;
                    char *CmdPPIDValue;
                    while (fgets(statusFileLine, STATUS_FILE_LINE_LEN, statusFiled) !=
                           NULL) {
                        // Here we are proccessing each file, we should tokenize them by \t
                        // character
                        char *currentToken;
                        currentToken = strtok(statusFileLine, "\t");
                        if (strcmp(currentToken, "Name:") == 0) {
                            currentToken = strtok(NULL, "\t");
                            CmdNameValue = malloc(strlen(currentToken) + 1);
                            if (CmdNameValue == NULL) {
                                perror("malloc");
                                exit(EXIT_FAILURE);
                            }
                            CmdNameValue = strcpy(CmdNameValue, currentToken);
                            CmdNameValue[strlen(CmdNameValue) - 1] = '\0';
                            continue;
                        } else if (strcmp(currentToken, "Pid:") == 0) {
                            currentToken = strtok(NULL, "\t");
                            CmdPIDValue = malloc(strlen(currentToken) + 1);
                            if (CmdPIDValue == NULL) {
                                perror("malloc");
                                exit(EXIT_FAILURE);
                            }
                            CmdPIDValue = strcpy(CmdPIDValue, currentToken);
                            CmdPIDValue[strlen(CmdPIDValue) - 1] = '\0';
                            continue;
                        } else if (strcmp(currentToken, "PPid:") == 0) {
                            currentToken = strtok(NULL, "\t");
                            CmdPPIDValue = malloc(strlen(currentToken) + 1);
                            if (CmdPPIDValue == NULL) {
                                perror("malloc");
                                exit(EXIT_FAILURE);
                            }
                            CmdPPIDValue = strcpy(CmdPPIDValue, currentToken);
                            CmdPPIDValue[strlen(CmdPPIDValue) - 1] = '\0';
                            continue;
                        } else {
                            continue;
                        }
                    }
                    // we got all the values we need;
                    pid_t CmdPID = (pid_t)strtol(CmdPIDValue, &endptr, 10);
                    pid_t CmdPPID = (pid_t)strtol(CmdPPIDValue, &endptr , 10);
                    create_and_insert_process_node_ll(CmdNameValue, CmdPID, CmdPPID);

                    free(CmdNameValue);
                    free(CmdPIDValue);
                    free(CmdPPIDValue);
                }

                fclose(statusFiled);
                free(processStatusFilePath);
            }
        }
    }

    closedir(procDIR);

    create_tree();

    print_tree();

    free_everything();

    return 0;
}
