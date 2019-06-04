//-------------------------------------------------------------------
//
// Make The Initial RamDrive:
//
//-------------------------------------------------------------------
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MY_INITRD_MAGIC_ID    77

typedef struct DATA DATA;

struct DATA {
    char *pname;
    char *pdata;
    unsigned int name; // name start
    unsigned int data; // data start
    unsigned int size; // data size
    DATA *next;
};

typedef union {
    unsigned int  i;
    unsigned char buf[4];
}VALUE;

VALUE v;

unsigned int total_size;

DATA *iten_first = NULL;
DATA *iten_last = NULL;


DATA * new_data (char *pname, int name, int data, int size) {
    DATA *d;
    if ((d = (DATA*)malloc(sizeof(DATA))) != NULL) {
        d->pname = strdup (pname);
        d->pdata = NULL;
        d->name = name;
        d->data = data;
        d->size = size;
        d->next = NULL;
    }
    return d;
}

void write_int (FILE *fp) {
    fputc (v.buf[0], fp);
    fputc (v.buf[1], fp);
    fputc (v.buf[2], fp);
    fputc (v.buf[3], fp);
}

int main (int argc, char **argv) {
    FILE *fp;
    int i;
    if (argc >= 2 && (fp = fopen("initrd.img", "w")) != NULL) {
        // 0:
        fputc (MY_INITRD_MAGIC_ID, fp);
        // 1:
        v.i = argc-1; write_int (fp); // unsigned int | 4 bytes
        //
        for (i = 1; i < argc; i++) {
            FILE *a;
            int file_size;
            int name, data, size;
            DATA *d;
            if ((a = fopen(argv[i], "r")) != NULL) {
                fseek (a, 0, SEEK_END);
                file_size = ftell(a);
                fseek (a, 0, SEEK_SET);

                name = total_size; total_size += strlen (argv[i]);
                data = total_size; total_size += file_size;
                size = file_size-1;

                if ((d = new_data(argv[i], name, data, size)) != NULL) {
                    d->pdata = (char *) malloc (size + 5);
                    if (!d->pdata) {
                        fclose (a);
                        fclose (fp);
                        return 0;
                    }
                    int i = fread (d->pdata, 1, file_size, a);
                    d->pdata[i] = 0;

                    // ADICIONE O PRIMEIRO ITEN NO INICIO
                    if (iten_first == NULL) {
                        iten_first = d;
                        iten_last = d;
                    }
                    // ADICIONE O ITEN NO FINAL
                    else {
                        iten_last->next = d;
                        iten_last = iten_last->next;
                    }
                }
//                printf ("file(%s)\n", argv[i]);
                fclose (a);
            }
        }

        DATA *o = iten_first;
        // urite file info index:
        while (o) {
            v.i = o->name; write_int(fp);
            v.i = o->data; write_int(fp);
            v.i = o->size; write_int(fp);
            printf ("DATA NAME(%s): %d | %d | %d\n", o->pname, o->name, o->data, o->size);
            o = o->next;
        }
        o = iten_first;
        // urite file info index:
        while (o) {
            char *sname = o->pname;
            char *sdata = o->pdata;
            while (*sname) {
                fputc (*sname, fp);
                sname++;
            }
            while (*sdata) {
                fputc (*sdata, fp);
                sdata++;
            }

            //printf ("DATA NAME(%s): %d | %d | %d\n", o->pname, o->name, o->data, o->size);
            o = o->next;
        }

        fclose (fp);

    } else {
        printf ("USAGE: %s file1 file2 file3 ...\n", argv[0]);
    }


    printf ("Exiting With Sucess\n");

    return 0;
}

