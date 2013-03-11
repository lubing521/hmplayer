#ifndef NEXUSIO_MFTP_WD_H
#define NEXUSIO_MFTP_WD_H
/*****************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
/*****************************************************************************/
int nexusio_mftp_watch_dog_start_thread(void);
int nexusio_mftp_watch_dog_close_thread(void);
int nexusio_mftp_watch_dog_alloc(int sock);
int nexusio_mftp_watch_dog_enter(int n);
int nexusio_mftp_watch_dog_leave(int n);
int nexusio_mftp_watch_dog_clean(int n);
/*****************************************************************************/
#endif/*MFTP_WD_H*/
