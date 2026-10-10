#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_EXIT    0x00
#define SYS_WRITE   0x01
#define SYS_READ    0x02
#define SYS_CLEAN   0x03

void syscall_init(void);

#endif