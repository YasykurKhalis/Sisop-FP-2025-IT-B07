#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

static const char *source_dir = "./source";  // direktori asli

// Fungsi membalik string per baris
char *reverse_lines(const char *input) {
    char *copy = strdup(input);
    if (!copy) return NULL;

    // Alokasikan buffer cukup besar
    char *result = malloc(strlen(input) * 2 + 1);  // lebih longgar untuk jaga-jaga
    if (!result) {
        free(copy);
        return NULL;
    }
    result[0] = '\0';

    char *line = strtok(copy, "\n");
    while (line != NULL) {
        size_t len = strlen(line);
        for (int i = len - 1; i >= 0; --i) {
            strncat(result, &line[i], 1);
        }
        strcat(result, "\n");
        line = strtok(NULL, "\n");
    }

    free(copy);
    return result;
}

// Mendapatkan path sebenarnya
static void fullpath(char fpath[PATH_MAX], const char *path) {
    snprintf(fpath, PATH_MAX, "%s%s", source_dir, path);
}

// getattr
static int reverse_getattr(const char *path, struct stat *stbuf) {
    char fpath[PATH_MAX];
    fullpath(fpath, path);
    if (lstat(fpath, stbuf) == -1)
        return -errno;
    return 0;
}

// readdir
static int reverse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                           off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    char fpath[PATH_MAX];

    fullpath(fpath, path);
    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    while ((de = readdir(dp)) != NULL) {
        filler(buf, de->d_name, NULL, 0);
    }

    closedir(dp);
    return 0;
}

// open
static int reverse_open(const char *path, struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    fullpath(fpath, path);

    int fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    close(fd);
    return 0;
}

// read
static int reverse_read(const char *path, char *buf, size_t size, off_t offset,
                        struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    fullpath(fpath, path);

    FILE *fp = fopen(fpath, "r");
    if (fp == NULL)
        return -errno;

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    char *content = malloc(fsize + 1);
    if (!content) {
        fclose(fp);
        return -ENOMEM;
    }

    fread(content, 1, fsize, fp);
    content[fsize] = '\0';
    fclose(fp);

    char *reversed = reverse_lines(content);
    free(content);
    if (!reversed)
        return -ENOMEM;

    size_t len = strlen(reversed);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, reversed + offset, size);
    } else {
        size = 0;
    }

    free(reversed);
    return size;
}

static struct fuse_operations reverse_oper = {
    .getattr = reverse_getattr,
    .readdir = reverse_readdir,
    .open    = reverse_open,
    .read    = reverse_read,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &reverse_oper, NULL);
}
