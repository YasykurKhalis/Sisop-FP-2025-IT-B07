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

```




## Pengerjaan

program FUSE yang dapat mount sebuah directory

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

1. Arslan, A. N. (2008). An algorithm with linear expected running time for string editing with substitutions and substring reversals. *Information Processing Letters, 106*(5), 213–218. https://doi.org/10.1016/j.ipl.2007.11.017

Sitasi 2
Sitasi 3
