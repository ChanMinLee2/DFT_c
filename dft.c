#include <math.h>
#include <stdio.h>
#include <stdlib.h> 

#define MAX_WINDOW_SIZE 4000
#define MAX 4993 // 4993 samples
#define SAMPLE_RATE 8000 // 8kHz
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
void calculate_DFT(char* xn, int window_size, float* Xr, float* Xi, float* Xk) {
    float pi = 3.1415;
    int a, k, n;

    // *1 : 500개만 뽑아보는 케이스
    a = 0;
     for (k = 0; k < window_size; k++) {
        for (k = 0; k < window_size; k++) {
            Xr[a*window_size + k] = 0;
            Xi[a*window_size + k] = 0;
            Xk[a*window_size + k] = 0;
            for (n = 0; n < window_size; n++) {
                Xr[a*window_size + k] += xn[n] * cos(2 * pi * k * n / window_size);
                Xi[a*window_size + k] += xn[n] * sin(2 * pi * k * n / window_size * (-1));
            }
            Xk[a*window_size + k] = sqrtf(
                Xr[a*window_size + k] * Xr[a*window_size + k] 
                + Xi[a*window_size + k] * Xi[a*window_size + k]
            );  
        }
    }


    // *2 : 다 뽑아보는 케이스
    // for (a = 0; a < 4993/window_size; a++) {
    //     for (k = 0; k < window_size; k++) {
    //         Xr[a*window_size + k] = 0;
    //         Xi[a*window_size + k] = 0;
    //         Xk[a*window_size + k] = 0;
    //         for (n = 0; n < NBITS; n++) {
    //             // printf("xn[%d] = %d,a = %d, k = %d, n = %d\n", n, xn[n], k, n);
    //             Xr[a*window_size + k] += xn[n] * cos(2 * pi * k * n / window_size);
    //             Xi[a*window_size + k] += xn[n] * sin(2 * pi * k * n / window_size * (-1));
    //             // printf("Xr[%d] = %d, k = %d, n = %d\n", n, Xr[a*window_size + k], k, n);
    //             // if (k == 0 && n == 1)
    //             //     printf("xn[%d] = %d, Xr[%d] = %f k = %d, n = %d\n", n, xn[n], n, Xr[n], k, n);
    //         }
    //         Xk[a*window_size + k] = sqrtf(
    //             Xr[a*window_size + k] * Xr[a*window_size + k] 
    //             + Xi[a*window_size + k] * Xi[a*window_size + k]
    //         );  
    //         // printf("(%f) + j(%f)\n", Xr[k], Xi[k]);
    //     }
    //     // printf("a = %d\n", a);
    // }
    
}

// Driver Code
int main() {
    char filename[256] = "sample1.snd";
    char result_filename[256] = "result.txt";
    char xn[MAX] = { 0x00 };
    float Xr[MAX] = { 0.0 }; // 실수부 
    float Xi[MAX] = { 0.0 }; // 허수부, 안쓰긴 함
    float Xk[MAX] = { 0.0 }; // 진폭 스펙트럼

    float max_Xk = Xk[0];
    int max_Xk_index = 0;

    read_file(filename, xn, MAX);
    calculate_DFT(xn, MAX_WINDOW_SIZE, Xr, Xi, Xk);

    FILE* fp;
    if ((fp = fopen(result_filename, "w")) == NULL) {
        printf("파일 오픈 에러 '%s'\n", result_filename);
        return -1;
    }

    // for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
    for (int i = 0; i < MAX; i++) {
        // printf("Xr[%d] = %f\n", i, Xr[i]);
        fprintf(fp, "xn[%3d]: [%10X]  Xr[%3d]: %12.6f  Xk[%3d]: %12.6f\n", i, xn[i], i, Xr[i], i, Xk[i]);

        if (fabsf(max_Xk) < fabsf(Xk[i])) {
            max_Xk = Xk[i];
            max_Xk_index = i;
        }
    }
    fclose(fp);

    printf("max_Xk = %f, max_Xk_index = %d\n", max_Xk, max_Xk_index);
    return 0;
}
