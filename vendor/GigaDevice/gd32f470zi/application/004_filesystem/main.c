#include <stdio.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/stat.h> 
#include <ohos_init.h>


#define BUFF_LEN    11
#define BUFF_STR    "1234567890"
#define FILE_NAME   "/data/test.txt"

static void filesystem_test(void)   
{   
	int fd;   
    char r_buffer[BUFF_LEN] = {0};

    fd = open(FILE_NAME, O_WRONLY | O_CREAT, S_IRUSR);  
    if (-1 == fd) 
    {      
        printf("open %s fail!\n", FILE_NAME);
        return ;   
    }     

    printf("write %s, write data is: %s\n", FILE_NAME, BUFF_STR);  
    write(fd, BUFF_STR,  BUFF_LEN);
    close(fd); 

    fd = open(FILE_NAME, O_RDONLY, S_IRUSR);
    if (-1 == fd) 
    {      
        printf("open %s fail!\n", FILE_NAME);
        return ;   
    }   

    read(fd, r_buffer, BUFF_LEN);
    printf("read %s, read data is: %s\n", FILE_NAME, r_buffer);    
    close(fd); 

    // 删除文件
    if (unlink(FILE_NAME) == 0) {
        printf("Successfully deleted %s\n\n", FILE_NAME);
    } else {
        printf("Failed to delete %s.\n\n", FILE_NAME);
    } 
}   

APP_FEATURE_INIT(filesystem_test);
