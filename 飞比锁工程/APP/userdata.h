#ifndef __USERDATA_H
#define __USERDATA_H
#include "stm8l15x.h"

#define LOW_BATTERY_LEVEL  30


/*** ���û����ݵĸ�����ַ *******/

#define lock_user_No_addr (0x1000)  //�û���ŵ�ַ 
#define lock_user_No_len (2)  //�û���ų���
#define lock_username_addr (0x1002)  //�û������ʱ����ַ
#define lock_username_len (8)   //�û������ʱ��볤��
#define lock_user_attribute_addr (0x100a) //�û����Ե�ַ
#define lock_user_attribute_len (1) //�û����Գ���
#define lock_start_validtime_addr  (0x100b) //�û���Ч��ʼʱ��
#define lock_start_validtime_len  (3) //�û���Ч��ʼʱ��
#define lock_end_validtime_addr  (0x100e) //�û���Ч����ʱ��
#define lock_end_validtime_len     (3) //�û���Ч����ʱ��
#define lock_everyday_validtime_addr  (0x1011) //�û�ÿ����Ч��ʼʱ��
#define lock_everyday_validtime_len      (12) //�û�ÿ����Ч��ʼʱ��
#define lock_everyweek_validtime_addr  (0x1023) //�û�ÿ����Ч����ʱ��
#define lock_everyweek_validtime_len      (1) //�û�ÿ����Ч����ʱ��
#define lock_allowed_counts_addr       (0x1024) //�û�����Ŀ��Ŵ�����ַ
#define lock_allowed_counts_len           (2) //�û�����Ŀ��Ŵ����ĳ��� 
#define lock_type_addr                 (0x1026) //����Կ�׵�����
#define lock_type_len                    (1) //����Կ�׵�����
#define lock_psw_addr                 (0x1027) //Կ�����ݵĵ�ַ
#define lock_psw_len                     (7)//Կ�����ݵĳ���
#define auto_hand_closelock_addr            (0x102e)//�Զ��ֶ�������ַ
#define auto_hand_closelock_len               (1)//�Զ��ֶ���������
#define openlock_holdtime_addr            (0x102f) //��������ʱ��ĵ�ַ
#define openlock_holdtime_len               (1) //��������ʱ��ĵ�ַ
#define sound_volume_addr                (0x1030) //������ַ
#define sound_volume_len                    (1) //������ַ
#define chinese_english_language_addr     (0x1031) //����Ӣ���л���ַ 
#define chinese_english_language_len         (1) //����Ӣ���л����� 
#define wireless_open_close_addr       (0x1032) //���߿���
#define wireless_open_close_len            (1) //���߿���
#define lock_authority_addr            (0x1033) //����Ȩ�޵�ַ
#define lock_authority_len               (1) //����Ȩ�޵�ַ
#define lock_clock_addr            (0x1034) //����ʱ���ַ
#define lock_clock_len                  (7) //����ʱ�䳤��
#define lock_battery_level_addr        (0x103b) //���1�����ٷֱ�
#define lock_battery_len                  (1) //���1�����ٷֱ�
#define lock_battery_level_addr        (0x103c) //���2�����ٷֱ�
#define lock_battery_len                  (1) //���2�����ٷֱ�
#define lock_switch_state_addr      (0x103d) //����״̬�ĵ�ַ                                                                                                                    
#define lock_switch_state_len            (2) //����״̬�ĵ�ַ   

/*** zigbee�û����ݵĸ�����ַ *******/
#define zigbee_clock_addr (0x1080)
#define zigbee_clock_len (6)
#define zigbee_user_attribute_addr (0x1086) //�û����Ե�ַ
#define zigbee_user_attribute_len (1) //�û����Ե�ַ
#define zigbee_user_No_addr (0x1087)  //�û���ŵ�ַ 
#define zigbee_user_No_len  (1) //�û���ų���
#define zigbee_psw_len_addr     (0x1088)  //���볤�ȵĵ�ַ
#define zigbee_psw_len_len     (0x01)  //���볤�ȵĳ���
#define zigbee_psw_addr  (0x1089) //�����ַ
#define zigbee_psw_len    (10) /*********��Ҫ��falsh�ж�ȡ ***********/
#define zigbee_psw_start_validtime_addr  (0x1095) //������Ч��ʼʱ��
#define zigbee_psw_start_validtime_len  (6) //������Ч��ʼʱ��
#define zigbee_psw_end_validtime_addr   (0x109b)  //������Ч����ʱ��
#define zigbee_psw_end_validtime_len   (6)  //������Ч����ʱ��









/*****����������****************/
enum {
 lock_user_No =0 ,
 lock_username =1,
 lock_user_attribute =2,



};

/*****Zigbee��������****************/

enum {
 zigbee_user_No =0 ,
 zigbee_username =1,
 zigbee_user_attribute =2,
 zigbee_clock =3,
 

};








/******�⺯��**********/
void write_userdata2eeprom(u16 addr, u8 *ptr, u8 len);
void read_userdata4eeprom(u16 addr, u8 *ptr, u8 len);




#endif 