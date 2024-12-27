#include "at24c02.h"
#include "stm32f4xx.h"                  // Device header
#include "iic.h"
#include "wang_time.h"

//���������������ݵ�
char at24c02_rec_str[] = {0};

//I2C�����ʼ������ʵ����ͨ�Ŷ�ӦGPIO�ڵ���س�ʼ��
void at24c02_init(){
	iic_io_init();
}



//��at24c02��ָ����ַд��һ���ֽ�����
//һ���ڲ���ַ��һ��Ҫд�����ݣ�������ַ�Ѿ������̶���
u8 at24c02_write_byte(u8 inner_addr,u8 data){
	//��ʼ����
	iic_start();
	//����������ַ
	iic_send_byte(AT24C02_ADDR_W);
	//���ӻ��Ƿ��յ�������ַ
	if(iic_get_ack()){
		//��Ӧ��,ֹͣ�ź�
		iic_stop();
		return AT24C02_ERR1;
	}
	
	
	//�����ڲ��ռ��ַ
	iic_send_byte(inner_addr);
	//�����û���յ��ڲ���ַ
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR2;
	}
	
	//��������
	iic_send_byte(data);
	//�����û���յ�����
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR3;
	}
	
	//�����ź�
	iic_stop();
	//д���ڣ�ʵ�ʴ���eeprom����Ҫʱ��
	baseTim6Delay_Ms(5);
	
	return AT24C02_NO_ERR;
}



//��ȡָ���ڲ��ռ��ַ�е�����
//���ر�״̬ռ�ã����ô��������ַ�Ѷ�ȡ��ֵ����
u8 at24c02_read_byte(u8 inner_addr,u8 *data){
	//��ʼ����
	iic_start();
	
	//����������ַ
	iic_send_byte(AT24C02_ADDR_W);
	//���ӻ��Ƿ��յ�������ַ
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//�����ڲ��ռ��ַ
	iic_send_byte(inner_addr);
	//�����û���յ��ڲ���ַ
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR2;
	}
	
	//��ʼ����
	iic_start();
	
	//����������ַ
	iic_send_byte(AT24C02_ADDR_R);
	//���ӻ��Ƿ��յ�������ַ
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//��ȡһ������
	*data = iic_rec_byte();
	//����NACK:���ߴ���������Ҫ�ٷ���
	iic_send_ack(1);
	
	//ֹͣ����
	iic_stop();
	//û��д�����ݣ�����д���ڣ���ȡҲ����Ҫ
	return AT24C02_NO_ERR;
}



//��ҳд�����һ����д�˸��ֽڣ�����һ���׵�ַ��ָ��,һ��д�볤��
//ҳ�߽���Ҫ�Լ����գ��������дҳ�׵�ַ
u8 at24c02_page_write(u8 inner_addr,const char *address,u8 length){
	//�ȼ���Ƿ��ҳ
	if(inner_addr/8!=(inner_addr+length-1)/8){
		//ֱ�ӷ���
		return AT24C02_OVER;
	}
	
	//��һ����ʱ����ͳ���ֽ�����ֹҳ�߽����
	u8 count = 0;
	
	//��ʼ����
	iic_start();
	
	//����������ַ
	iic_send_byte(AT24C02_ADDR_W);
	//���ӻ��Ƿ��յ�������ַ
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//�����ڲ��ռ��ַ
	iic_send_byte(inner_addr);
	//�����û���յ��ڲ���ַ
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR2;
	}
	
	//�����ݲ�һ��һ��д��,��β��Ҳ����һ���ֽ�
	while(count < length){
		//д������
		iic_send_byte(*address);
		//�����û���յ�����
		if(iic_get_ack()){
			//��Ӧ��
			iic_stop();
			return AT24C02_ERR3;
		}
		//ָ������ƶ�
		address++;
		//�����ֽڼ���
		count++;
	}
	
	//�����ź�
	iic_stop();
	//д���ڣ�ʵ�ʴ���eeprom����Ҫʱ�䣬�������Ĵ�СΪһҳ��8���ֽ�
	baseTim6Delay_Ms(5);
	
	return AT24C02_NO_ERR;
}



//ͨ�����������˳���ȡ
//��ȡ����ʼ��ַ����ȡ����
//����û��ҳ�߽�ĸ������������ȡ������NACK���洢��β�ͻ�ص��洢���׼�����
u8 at24c02_page_read(u8 inner_addr,char *data,u8 length){
	//Ϊ�������±����ַ��Ӧ
	data+=inner_addr;
	
	//��һ����ʱ����ͳ���ֽ�����ֹҳ�߽����
	u8 count = 0;
	
	
	//��ʼ����
	iic_start();
	
	//����������ַ
	iic_send_byte(AT24C02_ADDR_W);
	//���ӻ��Ƿ��յ�������ַ
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//�����ڲ��ռ��ַ
	iic_send_byte(inner_addr);
	//�����û���յ��ڲ���ַ
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR2;
	}
	
	//��ʼ����
	iic_start();
	
	//����������ַ
	iic_send_byte(AT24C02_ADDR_R);
	//���ӻ��Ƿ��յ�������ַ
	if(iic_get_ack()){
		//��Ӧ��
		iic_stop();
		return AT24C02_ERR1;
	}
	
	//��ȡǰ��length-1�Σ����һ�ε���
	while(count < (length-1)){
		//��ȡһ������
		*data = iic_rec_byte();
		//���ƫ��һ����ַ
		data++;
		//�ֽ���+1
		count++;
		//����ACK,���ߴӻ���Ҫ��ȡ
		iic_send_ack(0);
	
	}
	
	//��ȡһ������
	*data = iic_rec_byte();
	//����ACK,���ߴӻ���Ҫ��ȡ
	iic_send_ack(1);
	
	//ֹͣ����
	iic_stop();
	//û��д�����ݣ�����д���ڣ���ȡҲ����Ҫ
	return AT24C02_NO_ERR;
}


//��ҳд���ֽ�
void at24c02_skip_page_write(u8 inner_addr,const char *address,u8 length){
	u8 less_num;
	while(1){
		//���㱾ҳ���ж����ֽڿռ����дless_num
		less_num = 8 - inner_addr % 8;
		
		//�ж�Ҫд���ֽ����ݲ���Ҫ��ҳ
		if(less_num >= length){
			//����ҳд������д�뼴��  inner_addr,address,length
			at24c02_page_write(inner_addr,address,length);
			//����Ҫ��ҳ��ֱ������
			break;
		}
		//Ҫд����ֽ�������Ҫ��ҳ��
		else {
			//����ҳд�����Ȱѱ�ҳʣ��Ŀռ�д��
			at24c02_page_write(inner_addr,address,less_num);
			//���㻹�ж����ֽ�Ҫд
			length = length - less_num;
			//����ʣ�����ݵĴ����ַ
			address = address + less_num;
			//������һҳ����ʼ��ַ
			inner_addr = inner_addr + less_num;
		}
	}
}
