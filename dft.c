#include <math.h>
#include <stdio.h>
#include <stdlib.h> 

// MAX_WINDOW_SIZE는 DFT 계산에 사용되는 윈도우 크기를 정의하며 수식상 N에 해당
#define MAX_WINDOW_SIZE 2000
#define MAX_WINDOW_SIZE_1 500
#define MAX_WINDOW_SIZE_2 1000
#define MAX_WINDOW_SIZE_3 2000
#define MAX 5000 // 4993 samples
#define SAMPLING_RATE 8000 // 8kHz
#define NBITS 8
#define MONO 1
#define STEREO 2

// 파일에서 데이터를 읽어오는 함수
int read_file(char* filename, char* buf, int size) {
    FILE* fp;
    int read_len = 0;

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

    // window size만큼 DFT 계산 - 2중 루프를 통해 k와 n을 순회함함
    for (k = 0; k < window_size; k++) {
        Xr[k] = 0;
        Xi[k] = 0;
        Xk[k] = 0;
        for (n = 0; n < window_size; n++) {
            Xr[k] += xn[n] * cos(2 * pi * k * n / window_size);
            Xi[k] += xn[n] * sin(2 * pi * k * n / window_size * (-1));
        }
        // printf("k:[%d]\n", k);
        Xk[k] = sqrtf(
            Xr[k] * Xr[k] + Xi[k] * Xi[k]
        );  
    }
}

// 메인 함수
int main() {
    char filename[256] = "sample1.snd"; // 입력 파일명
    char result_filename[256] = "result.txt"; // 결과 저장용 파일명
    char xn[MAX] = { 0x00 }; // 8비트 퀀타이징 * 모노채널 => 8x1 = 8비트씩 해석 =>즉 1바이트씩 해석 => char 자료형 배열 사용
    float Xr[MAX] = { 0.0 }; // 실수부 
    float Xi[MAX] = { 0.0 }; // 허수부
    float Xk[MAX] = { 0.0 }; // 진폭 스펙트럼

    float max_Xk = Xk[0]; // 최대 진폭 스펙트럼 값
    int max_Xk_index = 0; // 최대 진폭 스펙트럼 인덱스

    // 파일에서 데이터를 읽어온 후 xn을 MAX_WINDOW_SIZE 크기로 DFT 계산 진행
    read_file(filename, xn, MAX);
    calculate_DFT(xn, MAX_WINDOW_SIZE, Xr, Xi, Xk);

    // DFT 계산 결과를 파일에 저장하기 위해 파일포인터 생성
    FILE* fp;
    if ((fp = fopen(result_filename, "w")) == NULL) {
        printf("파일 오픈 에러 '%s'\n", result_filename);
        return -1;
    }

    // DFT 결과를 파일에 저장
    for (int i = 0; i < MAX_WINDOW_SIZE; i++) {
        fprintf(fp, "xn[%3d]: [%10X]  Xr[%3d]: %12.6f  Xk[%3d]: %12.6f\n", i, xn[i], i, Xr[i], i, Xk[i]);

        if (fabsf(max_Xk) < fabsf(Xk[i])) {
            max_Xk = Xk[i];
            max_Xk_index = i;
        }
    }
    // 자원 반환
    fclose(fp);

    // 검증
    printf("max_Xk = %f, max_Xk_index = %d\n", max_Xk, max_Xk_index);

    // 최대 성분을 가지는 주파수 계산 => n(max_Xk_index)/N(MAX_WINDOW_SIZE) * fs(SAMPLING_RATE) = fk
    float max_frequency = (float)max_Xk_index * SAMPLING_RATE / MAX_WINDOW_SIZE; 
    printf("max_frequency = %f Hz\n", max_frequency);
    return 0;
}
