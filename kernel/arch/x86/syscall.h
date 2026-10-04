#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_EXIT 0
#define SYS_WRITE 1
#define SYS_READ 2

void syscall_init(void);

#endif