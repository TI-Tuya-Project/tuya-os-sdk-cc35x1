#include <stdio.h>
#include <string.h>
#include "tkl_fs.h"

/* * TI CC35xx does not use FatFs for internal storage!
 * We use standard POSIX file I/O which TI routes to its secure flash.
 */

TUYA_FILE tkl_fopen(const char *path, const char *mode)
{
    if (!path || !mode) return NULL;

    // TI routes standard fopen directly into the SimpleLink File System
    FILE *fp = fopen(path, mode);
    return (TUYA_FILE)fp;
}

int tkl_fwrite(void *buf, int bytes, TUYA_FILE file)
{
    if (!file || !buf) return -1;

    // fwrite returns the number of items written. We request 'bytes' items of size 1.
    size_t written = fwrite(buf, 1, bytes, (FILE*)file);
    return (int)written;
}

int tkl_fread(void *buf, int bytes, TUYA_FILE file)
{
    if (!file || !buf) return -1;

    size_t read_bytes = fread(buf, 1, bytes, (FILE*)file);
    return (int)read_bytes;
}

int tkl_fclose(TUYA_FILE file)
{
    if (!file) return -1;

    // Returns 0 on success
    return fclose((FILE*)file);
}

int tkl_fs_remove(const char *path)
{
    if (!path) return -1;

    // Returns 0 on success
    return remove(path);
}
int32_t fatfs_getFatTime(void)
{
    return 0;
}
