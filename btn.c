#include <wiringPi.h>
#include <stdio.h>
#define BTNPIN 3

// 버튼 눌림 인터럽트 발생시 호출되는 함수
void buttonPressed(void)
{
    printf("버튼을 눌렀습니다.\n");
}

int main(void)
{
    wiringPiSetup();

    // 버튼 입력 핀 모드 설정
    pinMode(BTNPIN, INPUT);

    // 3번 핀 LOW에서 버튼 누르면 HIGH -> RISING
    wiringPiISR(BTNPIN, INT_EDGE_RISING, buttonPressed);

    // KILL 될때까지 인터럽트 수행
    while(1)
    {
        delay(100);
    }

    return 0;
}
