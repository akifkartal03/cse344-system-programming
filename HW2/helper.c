#include "helper.h"

void checkArgument(int argc){
    if (argc < 2)
    {
        showUsageAndExit();
    }
}
void my_printf(const char *str);
void my_stderr(const char *str);
void showUsageAndExit(){
    my_stderr("Usage: ./processM pathToFile\n");
    exit(EXIT_FAILURE);
}
int safeRead(int fd, void* buf, size_t size);
int safeWrite(int fd, void* buf, size_t size);
int safeOpen(const char* file, int oflag);
int safeLseek(int fd, __off_t off, int flag);
void lockFile(int fd, struct flock *fl, int read);
void unlockFile(int fd, struct flock *fl);