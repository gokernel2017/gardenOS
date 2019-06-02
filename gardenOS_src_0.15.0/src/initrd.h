//-------------------------------------------------------------------
//
// FILE:
//   initrd.h
//
//-------------------------------------------------------------------
//
#ifndef _INITRD_H_
#define _INITRD_H_

// filesystem:
//
//###############################################

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08 // Is the file an active mountpoint?

struct fs_node;

// These typedefs define the type of callbacks - called when read/write/open/close
// are called.
typedef unsigned int (*read_type_t)  (struct fs_node*, unsigned int, unsigned int, unsigned char*);
typedef unsigned int (*write_type_t) (struct fs_node*,unsigned int,unsigned int,unsigned char *);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent * (*readdir_type_t)(struct fs_node*,unsigned int);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*,char *name);

typedef struct fs_node {
    char name[128];     // The filename.
    unsigned int mask;        // The permissions mask.
    unsigned int uid;         // The owning user.
    unsigned int gid;         // The owning group.
    unsigned int flags;       // Includes the node type. See #defines above.
    unsigned int inode;       // This is device-specific - provides a way for a filesystem to identify files.
    unsigned int length;      // Size of the file, in bytes.
    unsigned int impl;        // An implementation-defined number.
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct fs_node *ptr; // Used by mountpoints and symlinks.
} fs_node_t;

struct dirent {
    char name[128]; // Filename.
    unsigned int ino;     // Inode number. Required by POSIX.
};


extern fs_node_t *fs_root; // The root of the filesystem.

// Standard read/write/open/close functions. Note that these are all suffixed with
// _fs to distinguish them from the read/write/open/close which deal with file descriptors
// , not file nodes.
extern unsigned int read_fs(fs_node_t *node, unsigned int offset, unsigned int size, unsigned char *buffer);
extern unsigned int write_fs(fs_node_t *node, unsigned int offset, unsigned int size, unsigned char *buffer);
extern void open_fs(fs_node_t *node, unsigned char read, unsigned char write);
extern void close_fs(fs_node_t *node);
extern struct dirent *readdir_fs(fs_node_t *node, unsigned int index);
extern fs_node_t *finddir_fs(fs_node_t *node, char *name);

// initrd_headee:
//
//###############################################


typedef struct {
    unsigned int nfiles; // The number of files in the ramdisk.
} initrd_header_t;

typedef struct
{
    unsigned char magic;     // Magic number, for error checking.
    char name[64];  // Filename.
    unsigned int offset;   // Offset in the initrd that the file starts.
    unsigned int length;   // Length of the file.
} initrd_file_header_t;

// Initialises the initial ramdisk. It gets passed the address of the multiboot module,
// and returns a completed filesystem node.
//fs_node_t *initialise_initrd(u32int location);

#endif // ! _INITRD_H_

