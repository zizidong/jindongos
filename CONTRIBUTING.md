# jindongOS 기여 가이드

jindongOS 프로젝트에 기여해주셔서 감사합니다!

## 개발 환경 설정

1. 저장소 클론
```bash
git clone https://github.com/yourusername/myos.git
cd myos
```

2. 초기 설정
```bash
./configure
```

3. 개발 브랜치 생성
```bash
git checkout -b feature/your-feature-name
```

## 코드 작성 규칙

### C 코드 스타일

```c
// 함수명은 snake_case
void my_function(int param1, const char *param2) {
    // 4칸 들여쓰기
    int local_var = 0;
    
    // 한 줄에 80자 이내
    if (condition) {
        // 코드
    }
}

// 상수는 UPPER_CASE
#define MAX_BUFFER_SIZE 1024

// 구조체는 PascalCase_t
typedef struct {
    uint32_t field1;
    uint32_t field2;
} my_struct_t;
```

### 주석

```c
// 한 줄 주석은 //를 사용
int value; // 변수 설명

/*
 * 여러 줄 주석은 /* */를 사용
 * 각 줄은 * 로 시작
 */
```

## 빌드 및 테스트

```bash
# 빌드
make clean
make all

# 디스크 이미지 생성
make disk

# QEMU에서 테스트
make run
```

## 커밋 메시지 규칙

```
[타입] 간단한 설명

상세한 설명 (필요시)

- 변경 사항 1
- 변경 사항 2

Fixes #123
```

### 타입

- `feat`: 새로운 기능
- `fix`: 버그 수정
- `docs`: 문서 수정
- `style`: 코드 스타일 변경
- `refactor`: 코드 리팩토링
- `perf`: 성능 개선
- `test`: 테스트 추가

## Pull Request 프로세스

1. 코드 작성 및 테스트
2. 커밋 및 푸시
3. Pull Request 생성
4. 코드 리뷰 대기
5. 피드백 반영
6. 병합

## 문제 보고

버그를 발견했다면 GitHub Issues에서 보고해주세요.

### 이슈 템플릿

```markdown
## 버그 설명
간단한 설명

## 재현 방법
1. 단계 1
2. 단계 2
3. 단계 3

## 예상 동작
어떻게 되어야 하는가

## 실제 동작
실제로 어떻게 되는가

## 환경
- OS: Ubuntu 22.04
- GCC: 11.4.0
```

## 질문

질문이 있으시면 GitHub Discussions에서 물어봐주세요.
