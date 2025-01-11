#include "config.h"
#include "uthash.h"
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

typedef struct KeyValue {
  char key[64];
  char value[256];
  pid_t creator_pid;
  UT_hash_handle hh; // makes this struct hashable
} KeyValue;
KeyValue *hash_table = NULL;

// Insert or update key-value
void upsert_value(const char *key, const char *value, int creator_id) {
  KeyValue *kv = NULL;
  HASH_FIND_STR(hash_table, key, kv);
  if (kv == NULL) {
    // Create new entry
    kv = malloc(sizeof(KeyValue));
    if (kv == NULL) {
      perror("malloc");
    }
    strncpy(kv->key, key, sizeof(kv->key));
    strncpy(kv->value, value, sizeof(kv->value));
    kv->creator_pid = creator_id;
    HASH_ADD_STR(hash_table, key, kv);
  } else {
    // Key already exists, verify the ownership
    if (kv->creator_pid == creator_id) {
      strncpy(kv->value, value, sizeof(kv->value));
    } else {
      fprintf(stderr, "Error: client %d not authorized to update key %s\n",
              creator_id, key);
    }
  }
}

// Get value by key
char *get_value(const char *key) {
  KeyValue *kv = NULL;
  HASH_FIND_STR(hash_table, key, kv);
  if (kv) {
    return kv->value;
  }
  return NULL;
}

// Delete key-value
void delete_key_value(const char *key, int creator_id) {
  KeyValue *kv = NULL;
  HASH_FIND_STR(hash_table, key, kv);
  if (kv) {
    if (kv->creator_pid == creator_id) {
      HASH_DEL(hash_table, kv);
      free(kv);
    } else {
      fprintf(stderr, "Error: client %d not authorized to delete key %s\n",
              creator_id, key);
    }
  }
}

// Cleanup
void cleanup_hash_table() {
  KeyValue *current, *tmp;
  HASH_ITER(hh, hash_table, current, tmp) {
    HASH_DEL(hash_table, current);
    free(current);
  }
}

int main(int argc, char *argv[]) {
  struct sockaddr_in6 svaddr, claddr;
  int sfd, j;
  ssize_t numBytes;
  socklen_t len;
  char buf[BUF_SIZE];
  char claddrStr[INET6_ADDRSTRLEN];

  // info gathered from client
  char action; // action can be 'c' for create  'r' for read 'u' for update and
               // 'd' for delete
  char key[64];
  char value[256];
  pid_t creator_pid;

  sfd = socket(AF_INET6, SOCK_DGRAM, 0);
  if (sfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&svaddr, 0, sizeof(struct sockaddr_in6));
  svaddr.sin6_family = AF_INET6;
  svaddr.sin6_addr = in6addr_any; /*Wildcard address */
  svaddr.sin6_port = htons(SERVER_PORT_NUM);

  if (bind(sfd, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_in6)) ==
      -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  /*Receive messages*/
  for (;;) {
    memset(buf, 0, BUF_SIZE);
    len = sizeof(struct sockaddr_in6);
    numBytes =
        recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *)&claddr, &len);
    if (numBytes == -1) {
      perror("recvfrom");
      exit(EXIT_FAILURE);
    }

    // Initialize them so we can detect if nothing was read
    action = '\0';
    creator_pid = 0;
    key[0] = '\0';
    value[0] = '\0';

    // We expect four parts: command, pid, key, value
    // %c    for command (single char)
    // %d    for pid (integer)
    // %[^:] for a string up to the next colon
    // %[^:] for a string up to the next colon
    fprintf(stdout, "DEBUG: received %s\n", buf);
    int n = sscanf(buf, "%c:%d:%[^:]:%[^:]", &action, &creator_pid, key, value);

    if (n < 3) {
      // If we didn't get at least command, pid, and key, it's invalid
      fprintf(stderr, "Invalid format or not enough parts\n");
      continue;
    }

    if (n == 4) {
      // We got a value
      switch (action) {
      case 'c':
      case 'u':
        upsert_value(key, value, creator_pid);
        break;
      default:
        fprintf(stderr, "unknown action");
        break;
      }
    } else {
      // No value found (e.g. if the command was 'r' or 'd')
      switch (action) {
      case 'r':
        char *val = get_value(key);
        if (val == NULL) {
          // Key not found
          const char *notFound = "Key not found";
          if (sendto(sfd, notFound, strlen(notFound), 0,
                     (struct sockaddr *)&claddr,
                     len) != (ssize_t)strlen(notFound)) {
            perror("sendto");
            exit(EXIT_FAILURE);
          }
        } else {
          if (sendto(sfd, val, strlen(val), 0, (struct sockaddr *)&claddr,
                     len) != (ssize_t)strlen(val)) {
            perror("sendto");
            exit(EXIT_FAILURE);
          }
        }
        break;
      case 'd':
        delete_key_value(key, creator_pid);
        break;
      default:
        fprintf(stderr, "unknown action");
        break;
      }
    }
  }

  cleanup_hash_table();
}
