# Final Project Sistem Operasi IT

## Peraturan
1. Waktu pengerjaan dimulai hari Kamis (19 Juni 2025) setelah soal dibagikan hingga hari Rabu (25 Juni 2025) pukul 23.59 WIB.
2. Praktikan diharapkan membuat laporan penjelasan dan penyelesaian soal dalam bentuk Readme(github).
3. Format nama repository github “Sisop-FP-2025-IT-[Kelas][Kelompok]” (contoh:Sisop-FP-2025-IT-A01).
4. Setelah pengerjaan selesai, seluruh source code dan semua script bash, awk, dan file yang berisi cron job ditaruh di github masing - masing kelompok, dan link github dikumpulkan pada form yang disediakan. Pastikan github di setting ke publik.
5. Commit terakhir maksimal 10 menit setelah waktu pengerjaan berakhir. Jika melewati maka akan dinilai berdasarkan commit terakhir.
6. Jika tidak ada pengumuman perubahan soal oleh asisten, maka soal dianggap dapat diselesaikan.
7. Jika ditemukan soal yang tidak dapat diselesaikan, harap menuliskannya pada Readme beserta permasalahan yang ditemukan.
8. Praktikan tidak diperbolehkan menanyakan jawaban dari soal yang diberikan kepada asisten maupun praktikan dari kelompok lainnya.
9. Jika ditemukan indikasi kecurangan dalam bentuk apapun di pengerjaan soal final project, maka nilai dianggap 0.
10. Pengerjaan soal final project sesuai dengan modul yang telah diajarkan.

## Kelompok 21

Nama | NRP
--- | ---
Mochkamad Mualana Syafaat | 5027241021
Alnico Virendra Kitaro Diaz | 5027241081
Yasykur Khalis Jati Maulana Yuwono | 5027241112
Reza Aziz Simatupang | 5027241051

## Deskripsi Soal

21. FUSE - Reverse text in a file
Buatlah sebuah program FUSE yang dapat mount sebuah directory. Saat sebuah file text di directory tersebut dibuka, maka isi dari file tersebut akan dibalik per baris.

### Catatan

Struktur repository:
```
├── reverse-fs/                
│
├── reverse_fs.c              
│
├── source/                    
│   └── test.txt              
│
├── mount/                   
│   
│
├── README.md                  # Laporan
└── 
```

## Pengerjaan

> Buatlah sebuah program FUSE yang dapat mount sebuah directory. Saat sebuah file text di directory tersebut dibuka, maka isi dari file tersebut akan dibalik per baris.

# 🔄 FUSE Reverse Filesystem

Filesystem virtual berbasis FUSE yang menampilkan isi file dari folder sumber dengan transformasi **setiap baris dibalik urutan karakternya**. Cocok untuk eksperimen sistem berkas custom dan edukasi FUSE.

---

## 📚 Library

```c
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
```

## 🔧 Fungsi reverse_lines
```c
char *reverse_lines(const char *input) {
    char *copy = strdup(input);                      // Duplikasi input string
    if (!copy) return NULL;

    char *result = malloc(strlen(input) * 2 + 1);     // Alokasikan buffer untuk hasil
    if (!result) {
        free(copy);
        return NULL;
    }
    result[0] = '\0';

    char *line = strtok(copy, "\n");                 // Ambil baris per baris
    while (line != NULL) {
        size_t len = strlen(line);
        for (int i = len - 1; i >= 0; --i) {
            strncat(result, &line[i], 1);            // Balik karakter
        }
        strcat(result, "\n");                        // Tambahkan newline
        line = strtok(NULL, "\n");
    }

    free(copy);
    return result;
}
```
- Fungsi ini digunakan untuk membalik urutan karakter pada setiap baris dari string input
  
Misal
```
hello\nworld
```
Menjadi
```
olleh\ndlrow
```

## 📂 Fungsi ```fullpath```

```c
void fullpath(char fpath[PATH_MAX], const char *path) {
    snprintf(fpath, PATH_MAX, "%s%s", source_dir, path);
}
```
-  Fungsi ini menggabungkan path dari direktori sumber (```source_dir```)dengan path yang diminta oleh FUSE. Hasilnya adalah path absolut file asli.
- Misalnya

jika
```
path = "/notes.txt"
```
maka hasilnya adalah
```
./source/notes.txt
```

## 📌 Fungsi ```reverse_getattr()```
```c
static int reverse_getattr(const char *path, struct stat *stbuf)
```
- Mendapatkan atribut file (ukuran, waktu modifikasi, dll).
- Menggunakan ```lstat()```untuk membaca atribut file sebenarnya di ```source_dir```.


## 📁 Fungsi ```reverse_readdir```
```c
int reverse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    char fpath[PATH_MAX];

    fullpath(fpath, path);
    dp = opendir(fpath);
    if (!dp) return -errno;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    while ((de = readdir(dp)) != NULL) {
        filler(buf, de->d_name, NULL, 0);
    }

    closedir(dp);
    return 0;
}
```
-  Fungsi ini membaca isi direktori dan mengisi hasilnya ke dalam struktur yang ditampilkan oleh FUSE di folder mount point.
-  Menggunakan ```opendir()``` dan ```readdir()``` untuk membaca direktori asli (```source_dir```).
- Kemudian mengisi buffer dengan nama file dan folder menggunakan ```filler()```.


## 📖 Fungsi ```reverse_open```
```c
int reverse_open(const char *path, struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    fullpath(fpath, path);
    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;
    close(fd);
    return 0;
}
```
- Fungsi ini mengecek apakah file bisa dibuka (read-only). Tidak menyimpan descriptor karena ```read()``` membukanya lagi.

## 📄 Fungsi ```reverse_read```
```c
int reverse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[PATH_MAX];
    fullpath(fpath, path);

    FILE *fp = fopen(fpath, "r");
    if (!fp) return -errno;

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
    if (!reversed) return -ENOMEM;

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
```
- Fungsi ini membaca seluruh isi file dari path asli, membalik setiap baris dengan ```reverse_lines()```, lalu menyalin bagian tertentu (sesuai ```offset``` dan ```size```) ke buffer yang akan dibaca oleh aplikasi pengguna.

## 🚀 Struktur FUSE dan ```main()```
```c
static struct fuse_operations reverse_oper = {
    .getattr = reverse_getattr,
    .readdir = reverse_readdir,
    .open    = reverse_open,
    .read    = reverse_read,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &reverse_oper, NULL);
}
```
-  Ini adalah struktur utama yang mendefinisikan fungsi-fungsi yang akan dijalankan oleh FUSE. Fungsi ```main()``` memulai sistem filesystem FUSE dengan operasi yang telah didefinisikan di atas.


## Pengerjaan

> Insert poin soal...

**Teori**

...

**Solusi**

...

Saat file teks dibuka, baca isi file dan balik urutan setiap baris.

**Teori**

Given two strings X and Y over an alphabet Σ where
|Σ|  2, the edit distance is the total number of operations needed to transform X into Y . A common model
of string editing is based on insert, delete, and substitute
operations at character level. There are other models
(see for example [3,6,12,5,13]). In general, multiple operations in the same position in X are not allowed. This
is necessary to obtain a metric that can be efficiently
computed.
We denote by Si the ith symbol of string S, and by
Si..j the substring of S starting at position i, and ending
at position j . We use ←−
S to denote the reverse of string
S, i.e. ←−
S = S|S| ...S2S1.
In this paper, we consider the edit distance model
introduced and studied first by Muthukrishnan and Sahinalp [9,10]. We say that substring X(i−k+1)..i of length
k is reversible if X(i−k+1)..i = ←−−−−−−−− Y(i−k+1)..i. In this model,
a character can be substituted for a character in X, and
a substring of X can be reversed in a single step (if it is
reversible). The assumption is that the string lengths are
the same: |X|=|Y| = n, and multiple edit operations
involving the same position in X are not allowed.
Let Ri be the lengths of reversible substrings ending
at position i. More formally,
Ri =
k

 X(i−k+1)..i = ←−−−−−−−− Y(i−k+1)..i,
1  i  n, 1  k  i

.
If Ri contains k > 0 then by a single reversal,
Y(i−k+1)..i can be obtained from X(i−k+1)..i. We consider all possible reversals ending at position i for
every i.
Let Di denote the edit distance between X1..i and
Y1..i in the model we study. We can compute Di for all i,
1  i  n, as follows:
Di = min
Di−1 + s(Xi, Yi), min
k∈Ri
Di−k + f (k)
, (1)
where D0 = 0, s(Xi, Yi) is the cost of replacing Xi
by Yi, and f (k) is the cost of a reversal of length k.

**Solusi**

```c
char *reverse_lines(const char *input) {
    char *copy = strdup(input);                      // Duplikasi input string
    if (!copy) return NULL;

    char *result = malloc(strlen(input) * 2 + 1);     // Alokasikan buffer untuk hasil
    if (!result) {
        free(copy);
        return NULL;
    }
    result[0] = '\0';

    char *line = strtok(copy, "\n");                 // Ambil baris per baris
    while (line != NULL) {
        size_t len = strlen(line);
        for (int i = len - 1; i >= 0; --i) {
            strncat(result, &line[i], 1);            // Balik karakter
        }
        strcat(result, "\n");                        // Tambahkan newline
        line = strtok(NULL, "\n");
    }

    free(copy);
    return result;
}
```
**Video Menjalankan Program**

https://github.com/user-attachments/assets/3980bede-9325-4264-a70c-09a55450ee99

## Daftar Pustaka

1. Arslan, A. N. (2008). An algorithm with linear expected running time for string editing with substitutions and substring reversals. *Information Processing Letters, 106*(5), 213–218. https://doi.org/10.1016/j.ipl.2007.12.001

Sitasi 2
Sitasi 3
