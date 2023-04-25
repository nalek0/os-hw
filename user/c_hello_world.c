#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    char * output_message = "Hello world from C!\n";
    write(1, output_message, strlen(output_message));
    
    exit(0);
}
