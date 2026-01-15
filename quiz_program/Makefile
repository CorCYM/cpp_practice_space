# 컴파일러
CC = gcc

# 컴파일 옵션
CFLAGS = -Wall -Wextra -O2

# 실행 파일 이름
TARGET = playGame

# 소스 파일 목록
SRCS = main.c
OBJS = $(SRCS:.c=.o)

# 기본 타겟: 그냥 make만 치면 여기로 온다
all: $(TARGET)

# 링크 단계
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 개별 .c -> .o 컴파일 규칙
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# make run 하면 실행까지
run: $(TARGET)
	./$(TARGET)

# 빌드 산출물 정리
clean:
	rm -f $(TARGET) $(OBJS)
