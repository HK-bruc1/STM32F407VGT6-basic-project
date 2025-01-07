#include "iwdg.h"
#include "stm32f4xx.h"                  // Device header
#include "selfprintf.h"

void iwdg_init(void)
{
    //时钟源配置-开启低速时钟
    RCC->CSR |= (1U<<0);
    //等待时钟稳定
    while(!(RCC->CSR & (1U<<1)));
    
    //解除写保护
    IWDG->KR = 0x5555;
    //设置分频为32分频
    IWDG->PR = 3;     //直接赋值更清晰
    //重装载值设置为3000 (超时时间约3秒)
    IWDG->RLR = 3000;
    //重载到计数器
    IWDG->KR = 0xAAAA;
    //启动看门狗
    IWDG->KR = 0xCCCC;
    
    printf("看门狗已经启动\r\n");
}


//独立喂狗程序
void iwdg_feed(void)
{
    IWDG->KR = 0xAAAA;
}