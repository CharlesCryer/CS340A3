#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int
du(char *path)
{
  char buf[512], *p;
  int fd, total, sub;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "du: cannot open %s\n", path);
    return -1;
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "du: cannot stat %s\n", path);
    close(fd);
    return -1;
  }
    if(st.type != T_DIR){
    close(fd);
    return st.size;
  }
    total = st.size;

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
    fprintf(2, "du: path too long\n");
    close(fd);
    return total;
  }
  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if(strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
      continue;
    sub = du(buf);
    if(sub > 0)
      total += sub;
  }
  close(fd);
    printf("%d\t%s\n", total, path);
  return total;
}


int
main(int argc, char *argv[])
{
  int fd, size;
  struct stat st;

  if(argc != 2){
    fprintf(2, "usage: du [PATH]\n");
    exit(1);
  }

  if((fd = open(argv[1], O_RDONLY)) < 0){
    fprintf(2, "du: cannot open %s\n", argv[1]);
    exit(1);
  }
  if(fstat(fd, &st) < 0){
    fprintf(2, "du: cannot stat %s\n", argv[1]);
    close(fd);
    exit(1);
  }
  close(fd);

  size = du(argv[1]);
  if(st.type != T_DIR && size >= 0)
    printf("%d\t%s\n", size, argv[1]);

  exit(0);
}