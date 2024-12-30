#include "w25q64.h"
#include "stm32f4xx.h"                  // Device header
#include "spi.h"
#include "selfprintf.h"
#include "BEEP.h"

//W25Q64��������ʼ��
void W25Q64_Init(void){
	//SPI��ʼ��
		spi1_controller_init();
	
	//����Ƭѡ�����ã�������ֲ
		//����PB14ΪƬѡ���ţ�ͨ�����ģʽ��
		RCC->AHB1ENR |= (1U<<1);//ʹ��GPIOA��GPIOBʱ��
		GPIOB->MODER &= ~(3U<<28);  // ���PB14ԭ����
		GPIOB->MODER |= (1U<<28);   // ����Ϊͨ�����ģʽ
		GPIOB->OTYPER &= ~(1U<<14); // �������
		GPIOB->OSPEEDR &= ~(3U<<28);// ����ٶ�����
		GPIOB->OSPEEDR |= (1U<<29); // ����Ϊ50MHz
		GPIOB->PUPDR &= ~(3U<<28);//��������
		// ȷ��CS���ų�ʼ��Ϊ��
		GPIOB->ODR |= (1U<<14);  // ����Ƭѡ��ͨ��ʱ�����͡�
	
	//������п�д����(��ֹ֮ǰ��д����),��״̬�Ĵ���ȫ������
		W25Q64_WriteStatus(0x00);
}


// дʹ��
void W25Q64_WriteEnable(void) {
		//Ƭѡ����
    W25Q64_CS_LOW;
    spi1_controller_TransmitReceive(W25Q64_WRITE_ENABLE); // ����дʹ��ָ��
		//Ƭѡ����
    W25Q64_CS_HIGH;
}


// ��ȡ״̬�Ĵ���
u8 W25Q64_ReadStatus(void) {
    u8 status;
    W25Q64_CS_LOW;
    spi1_controller_TransmitReceive(W25Q64_READ_STATUS); 
		//��㷢һ�����ݣ����ڴ����ӻ�������
    status = spi1_controller_TransmitReceive(0xFF);      
    W25Q64_CS_HIGH;
    return status;
}

//д״̬�Ĵ���
void W25Q64_WriteStatus(u8 data){
	W25Q64_WriteEnable();
	W25Q64_CS_LOW;
	//�ȷ���һ��ָ��
	spi1_controller_TransmitReceive(W25Q64_WRITE_STATUS);
	//��д״̬�Ĵ���
	//ֻ��дSRP,TB,BP2,BP1,BP0,����λ����
	spi1_controller_TransmitReceive(data);
	W25Q64_CS_HIGH;
	
	
	//��ָ��ִ�����BUSY��WEL�Զ���Ϊ0�����Զ�ȡBUSY״̬�ж��Ƿ�д��
	//��ͣ�Ķ�ȡ״̬
	while(W25Q64_ReadStatus() & (1U<<0));
	
} 


//��ָ����ַ��ȡ����(������)
//0x 7F F F FF ,127�ſ�  15������  15��ҳ 255���ֽڴ�
void W25Q64_ReadData(u32 inner_addr, u8* data, u32 length) {
    W25Q64_CS_LOW;
    spi1_controller_TransmitReceive(W25Q64_READ_DATA); // ������ָ��
		//����24λ��ַҲֻ��8λ8λ�ķ�
    spi1_controller_TransmitReceive((inner_addr >> 16) & 0xFF); // ��ַ��8λ
    spi1_controller_TransmitReceive((inner_addr >> 8) & 0xFF);  // ��ַ��8λ
    spi1_controller_TransmitReceive(inner_addr & 0xFF);         // ��ַ��8λ
		//�ڲ���ַ���Զ�����
    for (u32 i = 0; i < length; i++) {
				//ʹ�������±��﷨��
        data[i] = spi1_controller_TransmitReceive(0xFF); // ��ȡ����
    }
    W25Q64_CS_HIGH;
}


//ҳ���,����д256���ֽ�
//���ҳ�߽�Ҳ�ǹ̶��ģ����Ƕ�̬����ģ�
//inner_addr�ǲ���ҪУ�飿
void W25Q64_writePage(u32 inner_addr, u8* data, u16 length){
	W25Q64_WriteEnable();
	//��������,��һְ��ֻ����д�룬���������
	//W25Q64_SectorErase(inner_addr);
	W25Q64_CS_LOW;
	spi1_controller_TransmitReceive(W25Q64_PAGE_PROGRAM); 
	//����24λ��ַҲֻ��8λ8λ�ķ�
	spi1_controller_TransmitReceive((inner_addr >> 16) & 0xFF); // ��ַ��8λ
	spi1_controller_TransmitReceive((inner_addr >> 8) & 0xFF);  // ��ַ��8λ
	spi1_controller_TransmitReceive(inner_addr & 0xFF);         // ��ַ��8λ
	//printf("��ʼд������......\r\n");
	//�ڲ���ַ���Զ�����
	for (u16 i = 0; i < length; i++) {
			//ʹ�������±��﷨��
			spi1_controller_TransmitReceive(data[i]); // д������
	}
	W25Q64_CS_HIGH;
	// �ȴ�д�����
    while(W25Q64_ReadStatus() & (1U<<0));
	//printf("д��ɹ�......\r\n");
}



//����������������С������Χ�ˣ�д��֮ǰ��Ҫ�������ݣ�ȫ�����1
//0x 7F F F FF ,127�ſ�  15������  15��ҳ 255���ֽڴ�
void W25Q64_SectorErase(u32 inner_addr) {
		//��дʹ�ܣ�����д����������
    W25Q64_WriteEnable();
    W25Q64_CS_LOW;
		//��һ������ָ��
    spi1_controller_TransmitReceive(W25Q64_SECTOR_ERASE); 
		//����24λ��ַҲֻ��8λ8λ�ķ�
		spi1_controller_TransmitReceive((inner_addr >> 16) & 0xFF); // ��ַ��8λ
		spi1_controller_TransmitReceive((inner_addr >> 8) & 0xFF);  // ��ַ��8λ
		spi1_controller_TransmitReceive(inner_addr & 0xFF);         // ��ַ��8λ
    W25Q64_CS_HIGH;
		//��ͣ�Ķ�ȡ״̬
		while(W25Q64_ReadStatus() & (1U<<0));
		//printf("�����������\r\n");
}


//��������
void W25Q64_BlockErase(u32 inner_addr){
	//��дʹ�ܣ�����д����������
  W25Q64_WriteEnable();
	W25Q64_CS_LOW;
	//��һ������ָ��
	spi1_controller_TransmitReceive(W25Q64_BLOCK_ERASE); 
	//����24λ��ַҲֻ��8λ8λ�ķ�
	spi1_controller_TransmitReceive((inner_addr >> 16) & 0xFF); // ��ַ��8λ
	spi1_controller_TransmitReceive((inner_addr >> 8) & 0xFF);  // ��ַ��8λ
	spi1_controller_TransmitReceive(inner_addr & 0xFF);         // ��ַ��8λ
	W25Q64_CS_HIGH;
	//��ͣ�Ķ�ȡ״̬
	while(W25Q64_ReadStatus() & (1U<<0));
	//printf("�����������\r\n");
}


//оƬ���� 20s,����Ҫ�����ַ
void W25Q64_ChipErase(){
	//��дʹ�ܣ�����д����������
  W25Q64_WriteEnable();
	W25Q64_CS_LOW;
	//��һ������ָ��
	spi1_controller_TransmitReceive(W25Q64_CHIP_ERASE); 
	W25Q64_CS_HIGH;
	//��ͣ�Ķ�ȡ״̬
	while(W25Q64_ReadStatus() & (1U<<0));
	//printf("оƬ�������\r\n");
}





//W25Q64���Ժ���
void W25Q64_Test(void){
    u8 writeData[256] = {0}; // ��ʼ��Ϊ0
    u8 readData[256] = {0};  // ��ȡ���ݻ�����
    
    // ��ʼ��W25Q64������
    W25Q64_Init();
    printf("W25Q64 Test Start...\r\n");
    
    // ����������
    for (int i = 0; i < 256; i++) {
        writeData[i] = i; // ���0-255������
    }

    // ��ӡwriteData�����ǰ16��Ԫ��
    printf("WriteData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", writeData[i]);
    }
    printf("\r\n");
		//������������(ok)
    W25Q64_SectorErase(0x000000);
		//������������(ok)
    //W25Q64_BlockErase(0x000000);
		//����оƬ����(ok)
    //W25Q64_ChipErase();
    // д�����ݵ�0x000000��ַ д��һҳ����
    //W25Q64_writePage(0x000000, writeData, 256);
    printf("Write 256 Bytes Data\r\n");
    
    // ��0x000000��ַ��ȡ����
    W25Q64_ReadData(0x000000, readData, 256);
    printf("Read 256 Bytes Data\r\n");
    
    // ��ӡreadData�����ǰ16��Ԫ��
    printf("ReadData: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", readData[i]);
    }
    printf("\r\n");
    
    // �Ƚ�д��Ͷ�ȡ������
    int errorCount = 0;
    for (int i = 0; i < 256; i++) {
        if (writeData[i] != readData[i]) {
            errorCount++;
        }
    }
    
    if (errorCount == 0) {
        printf("Data Verify Success!\r\n");
    } else {
        printf("Data Verify Failed! Error Count: %d\r\n", errorCount);
    }
}