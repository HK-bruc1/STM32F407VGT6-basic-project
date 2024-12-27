#include "functions.h"
#include "adc.h"
#include "GeneralTim.h"
#include "wang_time.h"
#include "selfprintf.h"
#include "nvic.h"
#include "key.h"
#include "RNG.h"
#include "event_handler.h"
#include "at24c02.h"
#include "string.h"


//��Դ�ļ����ڶ����ۺ��Ժ����������������ĳ�ʼ�����ã�����ʱ�������úõ�


volatile uint8_t randomColorFlag = 0;




// ��¼����ӡ���������ĺ���
// ���ܣ�ͨ��AT24C02 EEPROM��¼��׷��ϵͳ��������
void PrintBootCount(void) {
    // �������壬���ں���ά��
    const u8 BOOT_FLAG = 0xAA;    //�״ο�����д��ı�־
    const u8 FLAG_ADDR = 255;     // ��־λ��ַ��EEPROM���һ���ֽڣ�
    const u8 COUNT_ADDR = 0;      // ����ֵ��ַ��EEPROM��һ���ֽڣ�
    
    u8 bootCount = 0;      
    u8 firstBootFlag = 0;
    
    // ��ȡ�״ο�����־�����ж϶�ȡ����û����������ж��Ƿ�ΪBOOT_FLAG
	//firstBootFlag�ڶ�ȡ��ᱻ����
    if (at24c02_read_byte(FLAG_ADDR, &firstBootFlag) == 0 && firstBootFlag != BOOT_FLAG) {
        // �״ο�������ʼ��
        bootCount = 1;
        at24c02_write_byte(FLAG_ADDR, BOOT_FLAG);    // д�뿪����־
        at24c02_write_byte(COUNT_ADDR, bootCount);   // д���ʼ��������
    } else {
        // ���״ο�������ȡ���ۼӿ�������
        if (at24c02_read_byte(COUNT_ADDR, &bootCount) == 0) {
			//��ȡ�ɹ���bootCount�ᱻ����
            bootCount++;  // ֱ���ۼ�
            at24c02_write_byte(COUNT_ADDR, bootCount);  // д���µĿ�������
        } else {
            // ��ȡʧ�ܣ�����Ϊ1
            bootCount = 1;
            at24c02_write_byte(COUNT_ADDR, bootCount);
        }
    }
    
    printf("��������: %d\n", bootCount);
}



//AT24C02����ṹ��
void WriteBookToEEPROM(Book *book, u16 baseAddr) {
    // ʹ�ÿ�ҳд�뺯��ֱ��д�������ṹ��
    at24c02_skip_page_write(baseAddr, (char*)book, sizeof(Book));
}


// ��AT24C02��ȡBook�ṹ��
void ReadBookFromEEPROM(Book *book, uint16_t baseAddr) {
    // ʹ��������ȡ����ֱ�Ӷ�ȡ�����ṹ��
    at24c02_page_read(baseAddr, (char*)book, sizeof(Book));
}

// ��ӡBook�ṹ������
void PrintBook(const char* prefix, const Book* book) {
    printf("\n%s�鼮��Ϣ:\n", prefix);
    printf("---------------------------\n");
    printf("����: %s\n", book->name);
    printf("����: %s\n", book->writer);
    printf("���: %s\n", book->number);
    printf("�����: %lu\n", (unsigned long)book->hot);
    printf("�ղ���: %lu\n", (unsigned long)book->sc);
    printf("�۸�: %.2f\n", book->price);
    printf("---------------------------\n");
}


// ���Ժ���
void TestBookStorage(void) {
    // 1. ��������ӡԭʼ����
    Book BK1 = {"���μ�","��ж�","W201955",0,30,55.5};
    PrintBook("ԭʼ", &BK1);
    
    // 2. д��EEPROM
    WriteBookToEEPROM(&BK1, 0);
    printf("������д��EEPROM\n");
    
    // 3. ��EEPROM��ȡ
    Book readBook = {0};  // �����ʼ��
    ReadBookFromEEPROM(&readBook, 0);
    
    // 4. ��ӡ��ȡ������
    PrintBook("��ȡ", &readBook);
    
    // 5. ��֤�����Ƿ�һ��
    if (memcmp(&BK1, &readBook, sizeof(Book)) == 0) {
        printf("������֤�ɹ�: ��ȡ��������ԭʼ������ȫһ��!\n");
    } else {
        printf("������֤ʧ��: ��ȡ��������ԭʼ���ݲ�һ��!\n");
        
        // ��ϸ�ȶԸ��ֶ�
        if (strcmp((char*)BK1.name, (char*)readBook.name) != 0) 
            printf("������һ��\n");
        if (strcmp((char*)BK1.writer, (char*)readBook.writer) != 0) 
            printf("���߲�һ��\n");
        if (strcmp((char*)BK1.number, (char*)readBook.number) != 0) 
            printf("��Ų�һ��\n");
        if (BK1.hot != readBook.hot) 
            printf("�������һ��\n");
        if (BK1.sc != readBook.sc) 
            printf("�ղ�����һ��\n");
        if (BK1.price != readBook.price) 
            printf("�۸�һ��\n");
    }
}




//��������RGB��ɫ,�ȳ�ʼ��tim5_RGBInit����keyInit();
void tim5_Key_RGB(void){
	u8 keyVal = 0;
	keyVal = key_scan();
	switch(keyVal){
		//��ɫ
		case 1:
			TIM5->CCR2 = 0;
			TIM5->CCR3 = 0;
			TIM5->CCR4 = 999;
			break;
		//��ɫ
		case 2:
			TIM5->CCR2 = 0;
			TIM5->CCR3 = 999;
			TIM5->CCR4 = 0;
			break;
		//��ɫ
		case 3:
			TIM5->CCR2 = 999;
			TIM5->CCR3 = 0;
			TIM5->CCR4 = 0;
			break;
		case 4:
			//���������ɫģʽ
			randomColorFlag = !randomColorFlag;
			if(!randomColorFlag) {
				TIM5->CCR2 = 0; // ֹͣ�����ɫʱ������ѡ��һ��Ĭ����ɫ��ر�LED
				TIM5->CCR3 = 0;
				TIM5->CCR4 = 0;
      }
			break;
		default:
			break;
	}
}

//RGB�����ɫ����
void changeColorRandomly(void) {
    u16 red = getRandomNumber() % 1000;  // ȷ��ֵ��0��999֮��
    u16 green = getRandomNumber() % 1000;
    u16 blue = getRandomNumber() % 1000;
    
    TIM5->CCR2 = red;
    TIM5->CCR3 = green;
    TIM5->CCR4 = blue;
}



//��������ʼ��
void SR04_Init(void){
	
	//����ECHO����Ķ�ʱ��4�ĳ�ʼ��
	tim4_SRECHO_CaptureInit();
	
	//���ڷ���ߵ�ƽ��PB8�ĳ�ʼ��
	GPIOB->MODER &= ~(3U<<16);
	GPIOB->MODER |= (1U<<16);
	
	GPIOB->OTYPER |= (1U<<8);
	
	GPIOB->OSPEEDR &= ~(3U<<16);
	GPIOB->OSPEEDR |= (1U<<17);
	
	GPIOB->PUPDR &= ~(3U<<16);
	//��Ĭ�ϵ͵�ƽ
	GPIOB->ODR &= ~(1U<<8);
	//printf("��������ʼ�����");
}

//��������ʼ����
//ͨ��PB8��������ߵ�ƽ10us,���Գ�������trig
void SR04_Start(void){
	GPIOB->ODR |= (1U<<8);
	
	baseTim6Delay_Us(10);
	
	GPIOB->ODR &= ~(1U<<8);
	//printf("����һ�γ�����");
}


//�����������ĳ�ʼ��
void LightSensor(void){
	//��ʼ��ADC1���������
	adc1_PC2_ch12_init();
}

//�������ĳ�ʼ��
void gasDetection(void){
	//��ʼ��ADC2���������
	adc2_PC0_ch10_init();
}

//�����Ͷ���ͬADC��ʼ��
void light_gas_detection_init(void){
	//��ʼ��ADC1���������
	adc1_PC2PC0_ch12ch10ch16_init();
}

//��ͨ��ʹ���жϷ�ʽ��ȡ����
void light_gas_temp_interrupt_init(void){
	adc1_interrupt_init();
}