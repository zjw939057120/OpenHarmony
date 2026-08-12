#ifndef _MAIN_H_
#define _MAIN_H_

#include "third_party/RTT/SEGGER_RTT.h"

#define USE_DHCP       /* enable DHCP, if disabled static address is used */

/* MAC address: BOARD_MAC_ADDR0:BOARD_MAC_ADDR1:BOARD_MAC_ADDR2:BOARD_MAC_ADDR3:BOARD_MAC_ADDR4:BOARD_MAC_ADDR5 */
#define BOARD_MAC_ADDR0   0x20
#define BOARD_MAC_ADDR1   0x40
#define BOARD_MAC_ADDR2   0x60
#define BOARD_MAC_ADDR3   0x80
#define BOARD_MAC_ADDR4   0xA0
#define BOARD_MAC_ADDR5   0xC0
 
/* static IP address: BOARD_IP_ADDR0.BOARD_IP_ADDR1.BOARD_IP_ADDR2.BOARD_IP_ADDR3 */
#define BOARD_IP_ADDR0   10
#define BOARD_IP_ADDR1   50
#define BOARD_IP_ADDR2   3
#define BOARD_IP_ADDR3   39

/* remote IP address: IP_S_ADDR0.IP_S_ADDR1.IP_S_ADDR2.IP_S_ADDR3 */
#define IP_S_ADDR0   10
#define IP_S_ADDR1   50
#define IP_S_ADDR2   3
#define IP_S_ADDR3   79
 
/* net mask */
#define BOARD_NETMASK_ADDR0   255
#define BOARD_NETMASK_ADDR1   255
#define BOARD_NETMASK_ADDR2   255
#define BOARD_NETMASK_ADDR3   0

/* gateway address */
#define BOARD_GW_ADDR0   10
#define BOARD_GW_ADDR1   50
#define BOARD_GW_ADDR2   3
#define BOARD_GW_ADDR3   1

/* MII and RMII mode selection */
#define RMII_MODE  // user have to provide the 50 MHz clock by soldering a 50 MHz oscillator
//#define MII_MODE

/* clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef  MII_MODE
#define PHY_CLOCK_MCO
#endif

#endif
