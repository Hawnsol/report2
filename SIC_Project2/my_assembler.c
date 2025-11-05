#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINES 5000
#define MAX_OPERAND 30
#define MAX_INST 256

// ---------------------------
// ① 소스코드 한 줄 단위 구조체
// ---------------------------
typedef struct token_unit {
    char label[20];       // 명령어의 Label
    char operator[20];    // 명령어 (Opcode)
    char operand[MAX_OPERAND]; // 피연산자
    char comment[100];    // 주석
} token;

// ---------------------------
// ② 명령어 테이블 구조체 (OPTAB)
// ---------------------------
typedef struct inst_unit {
    char mnemonic[10];   // 명령어 이름 (예: LDA)
    unsigned char opcode; // 기계어 코드 (예: 0x00)
    int format;           // 명령어 형식 (예: 3)
    int operand_count;    // 피연산자 개수 (예: 1)
} inst;

inst inst_table[MAX_INST];
int inst_index = 0;

// ---------------------------
// ③ inst.data 로드 함수
// ---------------------------
void load_inst_table() {
    FILE *fp = fopen("inst.data", "r");
    if (!fp) {
        printf("❌ inst.data 파일을 열 수 없습니다.\n");
        exit(1);
    }

    while (fscanf(fp, "%s %x %d %d",
                  inst_table[inst_index].mnemonic,
                  &inst_table[inst_index].opcode,
                  &inst_table[inst_index].format,
                  &inst_table[inst_index].operand_count) != EOF) {
        inst_index++;
    }

    fclose(fp);
}

// ---------------------------
// ④ 명령어 검색 함수 (OPTAB 탐색)
// ---------------------------
int search_opcode(char *mnemonic) {
    for (int i = 0; i < inst_index; i++) {
        if (strcmp(inst_table[i].mnemonic, mnemonic) == 0)
            return inst_table[i].opcode;
    }
    return -1; // 못 찾은 경우
}

// ---------------------------
// ⑤ main 함수 (전체 실행 흐름)
// ---------------------------
int main() {
    FILE *fp = fopen("input.txt", "r");
    if (!fp) {
        printf("❌ input.txt 파일을 열 수 없습니다.\n");
        return 1;
    }

    load_inst_table();  // 명령어 테이블 로드
    printf("=========== SIC 파싱 결과 (Parsing Result) ===========\n\n");

    char line[100];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '.') continue; // 주석 제외

        token t;
        strcpy(t.label, "");
        strcpy(t.operator, "");
        strcpy(t.operand, "");
        strcpy(t.comment, "");

        int count = sscanf(line, "%s %s %s", t.label, t.operator, t.operand);

        // LABEL 없는 경우 처리 (예: LDA ALPHA)
        if (count == 2) {
            strcpy(t.operand, t.operator);
            strcpy(t.operator, t.label);
            strcpy(t.label, "");
        }

        // OPTAB 검색
        int code = search_opcode(t.operator);

        // 출력
        if (code != -1)
            printf("%-10s %-10s %-10s %02X\n", t.label, t.operator, t.operand, code);
        else
            printf("%-10s %-10s %-10s\n", t.label, t.operator, t.operand);
    }

    fclose(fp);
    return 0;
}
