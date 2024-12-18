
#include "esp_zigbee_core.h"
#include "esp_zigbee_endpoint.h"
#include "esp_zigbee_cluster.h"
#include "ha/esp_zigbee_ha_standard.h" 
#include <stdio.h>
#include "esp_err.h"   // Dodaj ten nagłówek
#include "esp_system.h"
#include "esp_log.h"

#define INSTALLCODE_POLICY_ENABLE       false                                   /* enable the install code policy for security          */
#define ED_AGING_TIMEOUT                ESP_ZB_ED_AGING_TIMEOUT_64MIN           /* Timeout                                              */
#define ED_KEEP_ALIVE                   3000                                    /* 3000 millisecond                                     */
#define MANUFACTURER                    "\x05""panee"                           /* necessary for z2m                                    */
#define MODELNAME                       "\x08""ESP32_H2"                        /* necessary for z2m                                    */
#define APP_PROFILE_ID                  0x0104                                  /* 0x0104   ==  Home Automation (HA)                    */
#define POWER_SOURCE                    1                                       /* 0x01     ==  External power supply                   */
#define ESP_ZB_PRIMARY_CHANNEL_MASK     ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK    /* Zigbee primary channel mask use in the example       */
#define HOST_CONNECTION_MODE_NONE       0x0                                     /* Disable host connection                              */
#define RADIO_MODE_NATIVE               0x0                                     /* Use the native 15.4 radio                            */
#define DEVICE_ID                       0x0100                                  /* Application Device ID (e.g. On/Off Switch)           */
#define DEVICE_VER                      1                                       /* Device version (max. 4 bity)                         */
#define ENDPOINT_BASIC                  1                                       /* Basic endpoint                                       */
#define ENDPOINT_D_I                    10                                      /* Digital input endpoint                               */
#define ENDPOINT_D_O                    20                                      /* Digital output endpoint                              */
#define ENDPOINT_A_I                    30                                      /* Analog  input endpoint                               */
#define ENDPOINT_A_O                    40                                      /* Analog  output  endpoint                             */
#define CLUSTER_D_I                     0xfd10                                  /* Digital input cluster                                */
#define CLUSTER_D_O                     0xfd11                                  /* Digital output cluster                               */
#define CLUSTER_A_I                     0xfd12                                  /* Analog  input cluster                                */
#define CLUSTER_A_O                     0xfd13                                  /* Analog  output  cluster                              */
#define ATTR_ID_0                       0x00                                    /* Attribute Id 0                                       */
#define ATTR_ID_1                       0x01                                    /* Attribute Id 1                                       */ 
#define ATTR_ID_2                       0x02                                    /* Attribute Id 2                                       */
#define MODE_RANDOM_OR_REWRITE          false                                   /* For true value outputs are incmented by 1 every 1 s  */
                                                                                /* For false value outputs are copied from inputs       */                             
/* Default End Device config */
#define ESP_ZB_ZED_CONFIG()                                     \
    {                                                           \
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,                   \
        .install_code_policy = INSTALLCODE_POLICY_ENABLE,       \
        .nwk_cfg = {                                            \
            .zed_cfg = {                                        \
                .ed_timeout = ED_AGING_TIMEOUT,                 \
                .keep_alive = ED_KEEP_ALIVE,                    \
            },                                                  \
        },                                                      \
    }

#define ESP_ZB_DEFAULT_RADIO_CONFIG()                           \
    {                                                           \
        .radio_mode = RADIO_MODE_NATIVE,                        \
    }

#define ESP_ZB_DEFAULT_HOST_CONFIG()                            \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,      \
    }


