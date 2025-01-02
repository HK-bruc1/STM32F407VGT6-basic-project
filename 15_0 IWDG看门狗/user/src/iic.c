#include "iic.h"
#include "stm32f4xx.h"                  // Device header
#include "wang_time.h"



//iicͨ������֡�ĳ�ʼ��,SCL PB6 SDA PB7
void iic_io_init(void){
	//�˿ڶ�Ӧʱ��ʹ��
	RCC->AHB1ENR |= (1U<<1);
	//��Ϊ��©���ģʽ,������
	GPIOB->MODER &= ~((3U<<12) | (3U<<14));
	GPIOB->MODER |= ((1U<<12) | (1u<<14));
	
	GPIOB->OTYPER |= ((1U<<6) | (1U<<7));
	
	GPIOB->OSPEEDR &= ~((3U<<12) | (3U<<14));
	
	GPIOB->PUPDR &= ~((3U<<12) | (3U<<14));
	
	//��ʼ״̬���߶�Ϊ�ߵ�ƽ,��©ģʽ�£����ָ���̬
	GPIOB->ODR |= ((1U<<6) | (1U<<7));
}



//iic����ʼ�ź�,����ǰ��Ҫ����SCL��ֹ�����ظ���ʶ��Ϊ��ʼ������ֹͣ����
void iic_start(void){
	//SCL�ڸߵ�ƽ�ڼ�SDA�����½���
	
	//SCL���ͣ���ȫ���ã����SCL��ǰΪ�ߣ�SDAΪ�ͣ�SDA���߻���������ؼ�ֹͣ�ź�
	IIC_SCL_L;
	baseTim6Delay_Us(5);//Ϊ�˱��������ȶ�
	
	
	//SDA����
	IIC_SDA_OUT_H;
	baseTim6Delay_Us(5);
	//SCL����
	IIC_SCL_H;
	baseTim6Delay_Us(5);
	//SDA����
	IIC_SDA_OUT_L;
	baseTim6Delay_Us(5);
	
	
	//SCL�����ͣ���ȫ���ã���ֹ��������SDA���������SDA���ߣ��������������
	IIC_SCL_L;
}


//ֹͣ�ź�
void iic_stop(void){
	//SCL�ڸߵ�ƽ�ڼ�SDA����������
	
	//SCL����
	IIC_SCL_L;
	baseTim6Delay_Us(5);
	
	//SDA����
	IIC_SDA_OUT_L;
	baseTim6Delay_Us(5);
	//SCL����
	IIC_SCL_H;
	baseTim6Delay_Us(5);
	//SDA����
	IIC_SDA_OUT_H;
	baseTim6Delay_Us(5);
	
	
	//SCL���ͣ�����Ҫ�ˣ���Ϊ���ص��˳�ʼ״̬,�Ѿ�����ֹͣ�źţ����Შ��
	//IIC_SCL_L;
}


//�Ƿ���Ӧ�𲿷�
void iic_send_ack(u8 ack){
	//ʱ��ͼ���ѣ�ֻҪ��SCL��SDA�ܶ�Ӧ���ɴﵽЧ����
	//�ڵھŸ����ݴ������ڵĵ͵�ƽ�ڼ䣬SDA����ΪӦ������Ϊ��Ӧ��
	//Ҳ����˵�ڵھŸ����ݴ������ڵĸߵ�ƽ�ڼ䣬�ӻ� ���ȡ��SDA�Ѿ������˵͵�ƽ���߱��ָߵ�ƽ
	//����SDA�����ͣ���SCL���ߡ���Ϊ�ȶ���SDA����֮ǰ��Ҫ��SCL����
	
	//��SCL����
	IIC_SCL_L;
	baseTim6Delay_Us(3);
	//SDA���ͻ�����
	ack ? (IIC_SDA_OUT_H) : (IIC_SDA_OUT_L);
	baseTim6Delay_Us(2);
	
	
	IIC_SCL_H;//�����ӻ����ߣ������ȡ
	baseTim6Delay_Us(5);
	
	//��ȫ����
	IIC_SCL_L;
}


//����Ƿ�ΪӦ�𲿷�
u8 iic_get_ack(void){
	u8 ack;
	
	//�Ȱ��������л�Ϊ����״̬������������1��Ϊ����̬
	IIC_SCL_L;//��ȫ����
	IIC_SDA_OUT_H;
	
	
	
	//��ȥӦ���ź�
	//�ӻ���Ҫ����������SCL���ͣ��Ա�ӻ����Է���ACK��NACK�ź�
	IIC_SCL_L;
	baseTim6Delay_Us(5);
	// ����SCL׼����ȡӦ���ź�
	IIC_SCL_H;
	baseTim6Delay_Us(5);
	
	//��IO�ڵ��������ݼĴ����ж�ֵ
	if(IIC_SDA_IN){
		//��һ���ߵ�ƽ��NACK
		ack = 1;
	}else {
		//��һ���͵�ƽ��ACK
		ack = 0;
	}
	
	//��ȫ����
	IIC_SCL_L;
	return ack;
}


//����һ���ֽ�����
void iic_send_byte(u8 data){
	u8 count = 8;
	// �����λ(MSB)��ʼ����
	while(count > 0){
		// SCL�ȱ��ֵ͵�ƽ��׼�����ݱ仯
		IIC_SCL_L;
		baseTim6Delay_Us(3);
		
		
		// ��������λ
		if(data & (1U << (count-1))){
				IIC_SDA_OUT_H;  // ����1
		}else{
				IIC_SDA_OUT_L;  // ����0
		}
		
		baseTim6Delay_Us(2);
		// SCL���ߣ��ӻ���������
		IIC_SCL_H;
		baseTim6Delay_Us(5);
		
		
		count--;
	}
    
	//��ȫ����
	IIC_SCL_L;
}


//����һ���ֽ�����
u8 iic_rec_byte(void){
	u8 data;
	u8 i;
	
	//�������л�Ϊ����״̬
	IIC_SCL_L;
	IIC_SDA_OUT_H;
	
	
	//��ȡ8λ����
	for(i = 0;i<8;i++){
		//���������ӻ�����ʱ���ߣ��ӻ��Զ�����һλ����,�����Ͳ���ʡ�ԣ�Ҫ����8��
		IIC_SCL_L;
		baseTim6Delay_Us(5);
		//ʱ�������ߣ��Ա�������ȡ����
		IIC_SCL_H;
		baseTim6Delay_Us(5);
		//��ȡ�����ߵĵ�ƽ�ж���0����1
		data = data<<1;
		if(IIC_SDA_IN){
			data = data | 0x01;
		}
	}
	
	//��ȫ����
	IIC_SCL_L;
	return data;
}