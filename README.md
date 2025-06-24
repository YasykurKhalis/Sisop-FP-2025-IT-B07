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
├── reverse-fs/                # Hasil compile code
│
├── reverse_fs.c               # Source code
│
├── source/                    # Direktori sumber file asli (input file)
│   └── test.txt               # Contoh file teks
│
├── mount/                     # Mount point FUSE
│   (kosong, dibuat saat runtime)
│
├── README.md                  # Laporan
└── .gitignore                 # File/folder yang tidak perlu di-push
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

**Video Menjalankan Program**

https://github.com/user-attachments/assets/3980bede-9325-4264-a70c-09a55450ee99

## Daftar Pustaka

Sitasi 1
Sitasi 2
Sitasi 3
