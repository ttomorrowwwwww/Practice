#include "rtk_api.h"
#include "rtk_api_ext.h"
#include "rtk_error.h"
#include "rtl8309n_types.h"
#include "version.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtl8309n_asicdrv_ext.h"
#include "rtl8309n_asicdrv.h"

static void mtu_vlan_car1B1(void) {

  printf("allcarB-0.0.1\n");
  rtk_api_ret_t ret;

  rtl8309n_vlan_igrFilterEnable_set(ENABLED);

  rtk_fid_t fid = 0;
  rtk_portmask_t mbrmsk, untagmsk;
  mbrmsk.bits[0] = 0x03;   // 0b00000011,B0 B1（1,2）
  untagmsk.bits[0] = 0x02; // 0b00000010,B0为tag,B1为untag
  ret = rtk_vlan_set(100, mbrmsk, untagmsk, 0);
  if (ret != RT_ERR_OK) {
    printf("Failed to set VLAN %d.\n", 100);
  }
  // rtk_vlan_get(100, &mbrmsk, &untagmsk, &fid);

  // printf("100rtk_vlan_set mbrmsk = %#x untagmsk = %#x, fid= %#x.\r\n",
  //        mbrmsk.bits[0], untagmsk.bits[0], fid);

  mbrmsk.bits[0] = 0x1d;   // 0b00011101,B0 B2 B3 B4（1,3,4,5）
  untagmsk.bits[0] = 0x1c; // 0b00011100,B0为tag，B2 B3 B4为untag
  ret = rtk_vlan_set(200, mbrmsk, untagmsk, 0);
  if (ret != RT_ERR_OK) {
    printf("Failed to set VLAN %d.\n", 200);
  }
  // rtk_vlan_get(200, &mbrmsk, &untagmsk, &fid);

  // printf("200rtk_vlan_set mbrmsk = %#x untagmsk = %#x, fid= %#x.\r\n",
  //        mbrmsk.bits[0], untagmsk.bits[0], fid);
        
  // RTL8309N_VLAN_TAGINSERTREMOVE_DONTTOUCH假设其为3
  // A为trunk口，A7设置为出去的帧vlanid不处理
  uint32 txmoshi = 0;
  if(rtl8309n_vlan_tagInsertRemove_set(0, RTL8309N_VLAN_TAG_DONTTOUCH) == SUCCESS){
    printf("0tx remove set success\n");
  }else{
    printf("0tx remove set faild\n");
  }
  // rtl8309n_vlan_tagInsertRemove_get(0,&txmoshi);
  // printf("port0 txmode = %d\n",txmoshi);
  if(rtl8309n_vlan_tagInsertRemove_set(1, RTL8309N_VLAN_TAG_REMOVE) == SUCCESS){
    printf("1tx remove set success\n");
  }else{
    printf("1tx remove set faild\n");
  }
  // rtl8309n_vlan_tagInsertRemove_get(1,&txmoshi);
  // printf("port1 txmode = %d\n",txmoshi);
  if(rtl8309n_vlan_tagInsertRemove_set(2, RTL8309N_VLAN_TAG_REMOVE) == SUCCESS){
    printf("2tx remove set success\n");
  }else{
    printf("2tx remove set faild\n");
  }
  //   rtl8309n_vlan_tagInsertRemove_get(2,&txmoshi);
  // printf("port2 txmode = %d\n",txmoshi);
  if(rtl8309n_vlan_tagInsertRemove_set(3, RTL8309N_VLAN_TAG_REMOVE) == SUCCESS){
    printf("3tx remove set success\n");
  }else{
    printf("3tx remove set faild\n");
  }
  // rtl8309n_vlan_tagInsertRemove_get(3,&txmoshi);
  // printf("port3 txmode = %d\n",txmoshi);
  if(rtl8309n_vlan_tagInsertRemove_set(4, RTL8309N_VLAN_TAG_REMOVE) == SUCCESS){
    printf("4tx remove set success\n");
  }else{
    printf("4tx remove set faild\n");
  }
  // rtl8309n_vlan_tagInsertRemove_get(4,&txmoshi);
  // printf("port4 txmode = %d\n",txmoshi);
  // if(rtl8309n_vlan_tagInsertRemove_set(5, RTL8309N_VLAN_TAG_REMOVE) == SUCCESS){
  //   printf("5tx remove set success\n");
  // }else{
  //   printf("5tx remove set faild\n");
  // }
  // rtl8309n_vlan_tagInsertRemove_get(5,&txmoshi);
  // printf("port5 txmode = %d\n",txmoshi);
  // if(rtl8309n_vlan_tagInsertRemove_set(6, RTL8309N_VLAN_TAG_REMOVE) == SUCCESS){
  //   printf("6tx remove set success\n");
  // }else{
  //   printf("6tx remove set faild\n");
  // }
  // rtl8309n_vlan_tagInsertRemove_get(6,&txmoshi);
  // printf("port6 txmode = %d\n",txmoshi);
  // if(rtl8309n_vlan_tagInsertRemove_set(7, RTL8309N_VLAN_TAG_REMOVE) == SUCCESS){
  //   printf("7tx remove set success\n");
  // }else{
  //   printf("7tx remove set faild\n");
  // }
  // rtl8309n_vlan_tagInsertRemove_get(7,&txmoshi);
  // printf("port7 txmode = %d\n",txmoshi);

  uint32 reten;
  reten = rtl8309n_vlan_igrPortTagInsertEnable_set(0,1);
  if(reten != SUCCESS){
    printf("vlan_igrPortTagInsertEnable_set Failed\n");
  }
  reten = rtl8309n_vlan_igrPortTagInsertEnable_set(1,1);
  if(reten != SUCCESS){
    printf("vlan_igrPortTagInsertEnable_set Failed\n");
  }
  reten = rtl8309n_vlan_igrPortTagInsertEnable_set(2,1);
  if(reten != SUCCESS){
    printf("vlan_igrPortTagInsertEnable_set Failed\n");
  }
  reten = rtl8309n_vlan_igrPortTagInsertEnable_set(3,1);
  if(reten != SUCCESS){
    printf("vlan_igrPortTagInsertEnable_set Failed\n");
  }
  reten = rtl8309n_vlan_igrPortTagInsertEnable_set(4,1);
  if(reten != SUCCESS){
    printf("vlan_igrPortTagInsertEnable_set Failed\n");
  }
  // reten = rtl8309n_vlan_igrPortTagInsertEnable_set(5,1);
  // if(reten != SUCCESS){
  //   printf("vlan_igrPortTagInsertEnable_set Failed\n");
  // }
  // reten = rtl8309n_vlan_igrPortTagInsertEnable_set(6,1);
  // if(reten != SUCCESS){
  //   printf("vlan_igrPortTagInsertEnable_set Failed\n");
  // }
  // reten = rtl8309n_vlan_igrPortTagInsertEnable_set(7,1);
  // if(reten != SUCCESS){
  //   printf("vlan_igrPortTagInsertEnable_set Failed\n");
  // }

  reten = rtl8309n_vlan_igrPortTagInsert_set(0,0xFE);//11111110
  if(reten != SUCCESS){
    printf("rtl8309n_vlan_igrPortTagInsert_set Failed\n");
  }

  rtl8309n_acceptFrameType_t b = RTL8309N_ACCEPT_ALL;
  uint32 retacc = rtl8309n_vlan_portAcceptFrameType_set(0, b);
  if(retacc != SUCCESS){
    printf("vlan_portAcceptFrameType_set Failed\n");
  }

  ret = rtk_vlan_portPvid_set(1, 100, 0);
  if(ret != RT_ERR_OK){
    printf("rtk_vlan_portPvid_set Failed\n");
  }

  ret = rtk_vlan_portPvid_set(2, 200, 0);
  if(ret != RT_ERR_OK){
    printf("rtk_vlan_portPvid_set Failed\n");
  }

  ret = rtk_vlan_portPvid_set(3, 200, 0);
  if(ret != RT_ERR_OK){
    printf("rtk_vlan_portPvid_set Failed\n");
  }

  ret = rtk_vlan_portPvid_set(4, 200, 0);
  if(ret != RT_ERR_OK){
    printf("rtk_vlan_portPvid_set Failed\n");
  }
 
  printf("set end\n");
}

int main(int argc, char *argv[]) {
  rtk_api_ret_t ret;

  ret = rtk_switch_init();
  if (ret) {
    printf("RL SWITCH init fail\n");
    return -1;
  }

  /* init vlan */
  ret = rtk_vlan_init();
  if (ret) {
    printf("JL vlan init fail\n");
    return -1;
  }

  // mtu_vlan_test();
  mtu_vlan_car1B1();

  return 0;
}