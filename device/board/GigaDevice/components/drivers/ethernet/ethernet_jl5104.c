/*
 * Copyright (c) 2022 PandaBoard.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gd32f4xx.h"
#include "gd32f4xx_enet.h"
#include "ethernet_init.h"
#include <stdio.h>

/* 获取当前的PHY的连接状态以确认网络的硬件连接，决定是否重新初始化网络 */
eth_link_t get_phy_link_status(void)
{
    ErrStatus reval_state = ERROR;
    uint16_t phy_value = 0U;
    reval_state = enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
    if(phy_value & PHY_LINKED_STATUS) {
        return ETH_LINK_UP;
    }else {
        return ETH_LINK_DOWN;
    }
}
