#include <wiringPi.h>
#include <stdio.h>

// PWM 지원핀 1, 23, 24, 26
#define LEDPIN 1

// Clock/Range/Compare 레지스터 값을 다양하게 변경하여 테스트
int main(void)
{
    wiringPiSetup();

    // PWM 지원 핀을 PWM 출력 모드로 설정
    pinMode(LEDPIN, PWM_OUTPUT);

    // 주파수 설정을 위해 PWM_MODE_MS 모드 변경
    pwmSetMode(PWM_MODE_MS);

    // Clock Divider 레지스터를 19로 설정 (1010526Hz = 약 1MHz)
    pwmSetClock(19);

    // Range 레지스터를 100000으로 설정 -> 주기 약 1MHz
    pwmSetRange(100000);

    // Compare 레지스터 설정
    // 1초 1000000 Counter 중 500000 (약 절반)은 HIGH, 나머지 LOW
    pwmWrite(LEDPIN, 100000/2);

    return 0;
}
