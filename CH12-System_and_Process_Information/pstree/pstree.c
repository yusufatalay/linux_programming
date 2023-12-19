#include "pstree.h"

process_node *process_ll_head;  // the latest node
process_node *process_ll_start; // the first node

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
    res->childs = NULL;
    res->next = NULL;

    if (process_ll_start == NULL) { // first call
        process_ll_start = res;
        process_ll_head =  process_ll_start;
    }

    // previous process' head will be newly created node
    process_ll_head->next = res;
    process_ll_head = res; // update the head

    return res;
}

process_node *find_node_in_ll(const pid_t pid) {
    for (process_node *i = process_ll_start; i != NULL; i = i->next) {
        if (i->pid == pid) {
            return i;
        }
    }
    return NULL;
}

void free_ll(void) {
    process_node *tmp;
    while (process_ll_start != NULL) {
        tmp = process_ll_start;
        process_ll_start = process_ll_start->next;
        free(tmp);
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
                    pid_t CmdPID = atoi(CmdPIDValue);
                    pid_t CmdPPID = atoi(CmdPPIDValue);
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
    for (process_node *i = process_ll_start; i->next != NULL; i = i->next) {
        printf("Process Name: %s\nProcesss ID: %d\nProcess Parent ID: %d\n", i->name, i->pid, i->ppid);
        printf("\t\t\t|\n\t\t\t|\n\t\t\t|\n\t\t\tv\n");
    }
    free_ll();

    return 0;
}
