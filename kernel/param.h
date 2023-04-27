#define INF         1000000000
#define NPROC        64  // maximum number of processes
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       2000  // size of file system in blocks
#define MAXPATH      128   // maximum file path name
#define DMBSIZE      2048 // diagnotic messages buffer size
#define SLTABLE_SIZE 32  // maximum number of the process loks

// Diagnostic messages definitions:
#define DIAG_MODE_ON        1 // diagnostics messages are turned on
#define DIAG_MODE_OFF       2 // diagnostics messages are turned off
#define DIAG_MODE_SECONDS   3 // diagnostics messages are turned on for the given amount of time