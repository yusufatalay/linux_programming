#include <unistd.h>
int main(void){
    execve("./to_execute", NULL,NULL);
}
