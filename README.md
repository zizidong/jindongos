# jindong - UEFI 기반 OS  

표준 Linux에 맞는 파이프라인을 사용하는 표준 Linux기반 OS입니다.

## 필수 도구

- GCC (x86_64)
- wsl
- GNU ld
- mtools
- QEMU
- Git

### Ubuntu/Debian 설치

```bash
sudo apt-get update
sudo apt-get install build-essential mtools qemu-system-x86
```

### Fedora/RHEL 설치

```bash
sudo dnf install gcc binutils mtools qemu-system-x86
```

## 빌드 방법

### 1. 초기 설정

```bash
chmod +x configure build.sh
./configure
```

### 2. Makefile을 사용한 빌드

```bash
# 커널만 빌드
make

# 디스크 이미지 생성
make disk

# 빌드 후 QEMU 실행
make run

# 빌드 정리
make clean
```

### 3. build.sh 스크립트 사용

```bash
# 커널만 빌드
./build.sh --build-only

# 빌드 후 QEMU 실행
./build.sh --run

# 빌드 정리
./build.sh --clean
```

### 4. CMake 사용

```bash
mkdir build
cd build
cmake ..
make
make run
```

## 프로젝트 구조

```
dos v2/
├── src/              # 커널 소스
├── gui/              # GUI 시스템
├── drivers/          # 드라이버
├── fs/               # 파일시스템
├── include/          # 헤더 파일
├── boot/             # 부트로더 설정
├── Makefile          # Make 빌드 시스템
├── CMakeLists.txt    # CMake 빌드 시스템
├── configure         # 초기 설정 스크립트
├── build.sh          # 빌드 스크립트
└── .github/          # GitHub Actions CI/CD
```

## 기능

- **UEFI 부팅**: Limine 부트로더 사용
- **GUI 시스템**: 윈도우, 버튼, 텍스트 렌더링
- **그래픽 효과**: 그라데이션, 블러, 그림자
- **더블 버퍼링**: 부드러운 렌더링
- **입력 시스템**: 키보드, 마우스 지원
- **파일시스템**: ext3 기반 구조

## 개발

### 코드 스타일

- C11 표준 준수
- 4칸 들여쓰기
- 함수명: snake_case
- 상수명: UPPER_CASE

### 빌드 옵션

```bash
# 디버그 정보 포함
CFLAGS="-g" make

# 최적화 활성화
CFLAGS="-O2" make

# 병렬 빌드
make -j$(nproc)
```
깃허브 특유의 대문자 기피에 맞는 이름 작성 -진동-

  사용된 ai
- claude
- gemini


## 라이선스

MIT License
