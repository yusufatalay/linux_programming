#ifndef PSTREE_H
#define PSTREE_H

#include <dirent.h>
#include <err.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STATUS_FILE_LINE_LEN 100
#define PROC_PATH "/proc/"
#define STATUS_PATH "/status"

typedef struct process_node {
  char name[128];
  pid_t pid;
  pid_t ppid;
  struct process_node *parent;
  struct process_node *childs[512];
  uint child_array_index;
  struct process_node *next; // linked list feature will make it easy to search
                             // through the processes w/o dealing with tree
} process_node;

process_node *create_and_insert_ll_process_node(const char *name,pid_t pid,pid_t ppid);

process_node *find_node_in_ll(pid_t pid);

#endif
