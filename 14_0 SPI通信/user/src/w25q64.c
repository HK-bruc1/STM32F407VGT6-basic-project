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
//���ҳ�߽�Ҳ�ǹ̶���
//inner_addrҪУ��
void W25Q64_writePage(u32 inner_addr, u8* data, u16 length){
		if (length == 0) {
			printf("д�볤��Ϊ0����Ч����\r\n");
			return;
		}

	// ����Ƿ��ҳ
    u32 start_page = inner_addr / 256;
	//��ʼ��ַҲʹ������Ҫ��ȥ1
    u32 end_addr = inner_addr + length - 1;
    u32 end_page = end_addr / 256;
  //C�����������������Զ���ȥС������,������Ըպÿ�����������ҳ��
	//������һ�������ĵ�ַ����256��һ���ҳ:
	//0~255 ������256�õ�0(��0ҳ)
	//256~511 ������256�õ�1(��1ҳ)
    if(start_page != end_page){
        printf("��ַ��Ԫ�س��ȳ�ͻ�����ܿ�ҳ����\r\n");
        return;
    }
		
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


//������ҳд����
void W25Q64_skip_page_write(u32 inner_addr, u8* data, u32 length){
    if (length == 0) {
        printf("д�볤��Ϊ0����Ч����\r\n");
        return;
    }

	  //�Ѿ�д����ֽ���
    u32 written = 0;
	 	//��0��ʼʹ��<����
    while (written < length) {
			// ���㵱ǰҳ��д����ֽ���
			u16 page_remaining = 256 - (inner_addr % 256);
			//ȷ������д��ľ����ֽ���
			//ʣ��Ԫ��<��ҳʣ��ռ�  ����  д����ҳ
			u16 to_write = (length - written < page_remaining) ? length - written : page_remaining;

			// д�뵱ǰҳ
			W25Q64_writePage(inner_addr, data + written, to_write);

			// ������д���ֽ����͵�ַ
			written += to_write;
			inner_addr += to_write;
     }
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


//�����������
void W25Q64_BlocksErase(u32 inner_addr,u8 blocks){
	while(blocks){
		W25Q64_BlockErase(inner_addr);
		//һ��Ĵ�СΪ65536���ֽ�
		inner_addr+=65536;
		blocks--;
	}
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
