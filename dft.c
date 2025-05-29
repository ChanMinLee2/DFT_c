#include <math.h>
#include <stdio.h>
#include <stdlib.h> 

#define MAX_WINDOW_SIZE 500
#define MAX 4993 // 4993 samples
#define NBITS 8
#define MONO 1
#define STEREO 2

int write_file(FILE* fp, char* filename, float* buf, int size) {
    // FILE* fp;
    int write_len = 0;
    // if ((fp = fopen(filename, "wb")) == NULL) {
    //     printf("파일 오픈 에러 '%s'\n", filename);
    //     return -1;
    // }
    if ((write_len = fwrite(buf, sizeof(float), size, fp)) < size) {
        // fclose(fp);
        printf("쓰기 에러 '%s' '%d(%d)'\n", filename, size, write_len);
    }
    // fclose(fp);
    return 0;
}

int read_file(char* filename, char* buf, int size) {
    FILE* fp;
    int read_len = 0;

    // 24바이트 헤더 건너뛰기
    // printf("%d\n",1);  // 파일의 시작(SEEK_SET)에서 24바이트 이동
    // fseek(fp, 24, SEEK_SET);

    if ((fp = fopen(filename, "rb")) == NULL) {
        printf("파일 오픈 에러 '%s'\n", filename);
        return -1;
    }
    if ((read_len = fread(buf, sizeof(char), size, fp)) < size) {
        fclose(fp);
        printf("읽기 에러 '%s' '%d(%d)'\n", filename, size, read_len);
    }
    fclose(fp);
    return 0;
}

// DFT 계산 함수
void calculate_DFT(char* xn, int window_size, float* Xr, float* Xi) {
    float pi = 3.14;
    int a, k, n;

    for (a = 0; a < 4993/window_size; a++) {
        for (k = 0; k < window_size; k++) {
            Xr[a*window_size + k] = 0;
            Xi[a*window_size + k] = 0;
            for (n = 0; n < NBITS; n++) {
                Xr[a*window_size + k] += xn[n] * cos(2 * pi * k * n / NBITS);
                Xi[a*window_size + k] += xn[n] * sin(2 * pi * k * n / NBITS * (-1));
                // if (k == 0 && n == 1) 
                //     printf("xn[%d] = %d, Xr[%d] = %f k = %d, n = %d\n", n, xn[n], n, Xr[n], k, n);
            }
        // printf("(%f) + j(%f)\n", Xr[k], Xi[k]);
        }
        // printf("a = %d\n", a);
    }
    
}

// Driver Code
int main() {
    char filename[256] = "sample1.snd";
    char xn[MAX] = { 0x00 };
    float Xr[MAX] = { 0.0 }; // 실수부 
    float Xi[MAX] = { 0.0 }; // 허수부, 안쓰긴 함함

    float max_Xr = Xr[0];
    int max_Xr_index = 0;

    read_file(filename, xn, MAX);
    // for (int i = 0; i < 20; i++)
    //     printf("0x%X ", xn[i]);

    calculate_DFT(xn, MAX_WINDOW_SIZE, Xr, Xi);

    FILE* fp;
    if ((fp = fopen("Xr.txt", "w")) == NULL) {
        printf("파일 오픈 에러 '%s'\n", "Xr.bin");
        return -1;
    }

    for (int i = 0; i < 4993; i++) {
        // printf("Xr[%d] = %f\n", i, Xr[i]);
         fprintf(fp, "Xr[%d] %f \n", i, Xr[i]);
        // write_file(fp,"Xr.txt", (float*)&Xr[i], sizeof(float));
        if (abs(max_Xr) < abs(Xr[i])) {
            max_Xr = Xr[i];
            max_Xr_index = i;
        }
    }
    fclose(fp);
    printf("max_Xr = %f, max_Xr_index = %d\n", max_Xr, max_Xr_index);
    return 0;
}
