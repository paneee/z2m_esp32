#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <z2m.h>

static const char *TAG = "ESP";

static void bdb_start_top_level_commissioning_cb(uint8_t mode_mask)
{
    ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(mode_mask));
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct)
{
    uint32_t *p_sg_p = signal_struct->p_app_signal;
    esp_err_t err_status = signal_struct->esp_err_status;
    esp_zb_app_signal_type_t sig_type = (esp_zb_app_signal_type_t)*p_sg_p;
    switch (sig_type)
    {
    case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Zigbee stack initialized");
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
        break;
    case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
        if (err_status == ESP_OK)
        {
            ESP_LOGI(TAG, "Start network steering");
            esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
        }
        else
        {
            /* commissioning failed */
            ESP_LOGI(TAG, "Failed to initialize Zigbee stack (status: %s)", esp_err_to_name(err_status));
        }
        break;
    case ESP_ZB_BDB_SIGNAL_STEERING:
        if (err_status == ESP_OK)
        {
            esp_zb_ieee_addr_t extended_pan_id;
            esp_zb_get_extended_pan_id(extended_pan_id);
            ESP_LOGI(TAG, "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx, Channel:%d, Short Address: 0x%04hx)",
                     extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4],
                     extended_pan_id[3], extended_pan_id[2], extended_pan_id[1], extended_pan_id[0],
                     esp_zb_get_pan_id(), esp_zb_get_current_channel(), esp_zb_get_short_address());
        }
        else
        {
            ESP_LOGI(TAG, "Network steering was not successful (status: %s)", esp_err_to_name(err_status));
            esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb, ESP_ZB_BDB_MODE_NETWORK_STEERING, 1000);
        }
        break;
    default:
        ESP_LOGI(TAG, "ZDO signal: %s (0x%x), status: %s", esp_zb_zdo_signal_to_string(sig_type), sig_type,
                 esp_err_to_name(err_status));
        break;
    }
}

esp_err_t zb_update_output(void *value, esp_zb_zcl_attr_type_t type, uint8_t endpoint, uint16_t cluster, uint16_t attr_id)
{
    esp_err_t state;
    switch (type)
    {
    case ESP_ZB_ZCL_ATTR_TYPE_BOOL:
        state = esp_zb_zcl_set_attribute_val(endpoint, cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, attr_id, (bool *)value, false);
        break;
    case ESP_ZB_ZCL_ATTR_TYPE_U8:
        state = esp_zb_zcl_set_attribute_val(endpoint, cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, attr_id, (uint8_t *)value, false);
        break;
    case ESP_ZB_ZCL_ATTR_TYPE_U16:
        state = esp_zb_zcl_set_attribute_val(endpoint, cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, attr_id, (uint16_t *)value, false);
        break;
    case ESP_ZB_ZCL_ATTR_TYPE_U32:
        state = esp_zb_zcl_set_attribute_val(endpoint, cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, attr_id, (uint32_t *)value, false);
        break;
    case ESP_ZB_ZCL_ATTR_TYPE_U64:
        state = esp_zb_zcl_set_attribute_val(endpoint, cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE, attr_id, (uint64_t *)value, false);
        break;
    default:
        ESP_LOGI(TAG, "Unsupported type\n");
        return ESP_FAIL;
    }

    esp_zb_zcl_report_attr_cmd_t ph_cmd_req = {0};
    ph_cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT;
    ph_cmd_req.attributeID = attr_id;
    ph_cmd_req.direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI;
    ph_cmd_req.clusterID = cluster;
    ph_cmd_req.zcl_basic_cmd.src_endpoint = endpoint;

    state = esp_zb_zcl_report_attr_cmd_req(&ph_cmd_req);

    if (state != ESP_ZB_ZCL_STATUS_SUCCESS)
    {
        ESP_LOGI(TAG, "Reporting ph attribute failed!");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t zb_attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message)
{
    esp_err_t ret = ESP_OK;
    uint16_t analog_temp = 0;
    uint16_t attr_id;
    bool digital_temp = false;

    if (!message)
    {
        ESP_LOGI(TAG, "Empty message");
    }
    if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS)
    {
        ESP_LOGI(TAG, "Received message: error status(%d)", message->info.status);
    }

    ESP_LOGI(TAG, "Received message: endpoint(%d), cluster(0x%x), attribute(0x%x), data size(%d)", message->info.dst_endpoint, message->info.cluster,
             message->attribute.id, message->attribute.data.size);

    if (message->info.dst_endpoint != ENDPOINT_BASIC && message->info.dst_endpoint != ENDPOINT_D_I &&
        message->info.dst_endpoint != ENDPOINT_D_O && message->info.dst_endpoint != ENDPOINT_A_I &&
        message->info.dst_endpoint != ENDPOINT_A_O)
    {

        return ESP_ERR_INVALID_ARG;
    }

    if (MODE_RANDOM_OR_REWRITE)
    {
        ESP_LOGI(TAG, "Working mode increment");
        return ret;
    }

    if ((message->info.dst_endpoint == ENDPOINT_D_O) && (message->info.cluster == CLUSTER_D_O) && (message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL))
    {
        digital_temp = message->attribute.data.value ? *(bool *)message->attribute.data.value : digital_temp;
        attr_id = (uint16_t)message->attribute.id;
        zb_update_output(&digital_temp, ESP_ZB_ZCL_ATTR_TYPE_BOOL, ENDPOINT_D_I, CLUSTER_D_I, attr_id);
        ESP_LOGI(TAG, "Digital output with attribute %x set to %s", attr_id, digital_temp ? "true" : "false");
    }

    if ((message->info.dst_endpoint == ENDPOINT_A_O) && (message->info.cluster == CLUSTER_A_O) && (message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_U16))
    {
        analog_temp = *(uint16_t *)message->attribute.data.value;
        attr_id = (uint16_t)message->attribute.id;
        zb_update_output(&analog_temp, ESP_ZB_ZCL_ATTR_TYPE_U16, ENDPOINT_A_I, CLUSTER_A_I, attr_id);
        ESP_LOGI(TAG, "Analog output attribute 0x%04x set level %d", attr_id, analog_temp);
    }
    return ret;
}

static esp_err_t zb_read_attribute_handler(esp_zb_zcl_cmd_read_attr_resp_message_t *message)
{
    ESP_LOGI(TAG, "Received message: endpoint(%d), cluster(0x%x), attribute(0x%x), data size(%d)", message->info.dst_endpoint, message->info.cluster,
             message->variables->attribute.id, message->variables->attribute.data.size);

    return ESP_OK;
}

static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message)
{
    esp_err_t ret = ESP_OK;
    switch (callback_id)
    {
    case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:
        ret = zb_attribute_handler((esp_zb_zcl_set_attr_value_message_t *)message);
        break;

    case ESP_ZB_CORE_CMD_READ_ATTR_RESP_CB_ID:
        ret = zb_read_attribute_handler((esp_zb_zcl_cmd_read_attr_resp_message_t *)message);
        break;

    default:
        ESP_LOGI(TAG, "Receive Zigbee action(0x%x) callback", callback_id);
        break;
    }
    return ret;
}

static void esp_zb_task(void *pvParameters)
{
    uint16_t analog_temp = 0;
    bool digital_temp = false;

    // Write config
    esp_zb_cfg_t zb_nwk_cfg = ESP_ZB_ZED_CONFIG();
    esp_zb_init(&zb_nwk_cfg);

    // Create end point list
    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();

    // --------------------------------- Endpoint 1 -- Basic Cluster -------------------------------------
    //
    // Create basic cluster list
    esp_zb_cluster_list_t *basic_cluster_list = esp_zb_zcl_cluster_list_create();
    esp_zb_basic_cluster_cfg_t basic_cluster_cfg = {.zcl_version = ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE, .power_source = POWER_SOURCE};
    esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(&basic_cluster_cfg);

    esp_zb_basic_cluster_add_attr(basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, MANUFACTURER);
    esp_zb_basic_cluster_add_attr(basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, MODELNAME);

    basic_cluster->cluster_id = ESP_ZB_ZCL_CLUSTER_ID_BASIC;
    esp_zb_cluster_list_add_basic_cluster(basic_cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    esp_zb_endpoint_config_t basic_endpoint_config = {ENDPOINT_BASIC, APP_PROFILE_ID, ESP_ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_DEVICE_ID, DEVICE_VER};
    esp_zb_ep_list_add_ep(ep_list, basic_cluster_list, basic_endpoint_config);

    // ------------------------------ Endpoint 10 -- Digital Input Cluster ------------------------------
    //
    // Create cluster list for digital input
    esp_zb_cluster_list_t *d_i_cluster_list = esp_zb_zcl_cluster_list_create();
    // Create attribute list for custom cluster digital input
    esp_zb_attribute_list_t *d_i_attribute_list = esp_zb_zcl_attr_list_create(CLUSTER_D_I);
    // Add custom attribute to attribute list
    esp_zb_cluster_add_attr(d_i_attribute_list, CLUSTER_D_I, 0x0000, ESP_ZB_ZCL_ATTR_TYPE_BOOL, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &digital_temp);
    esp_zb_cluster_add_attr(d_i_attribute_list, CLUSTER_D_I, 0x0001, ESP_ZB_ZCL_ATTR_TYPE_BOOL, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &digital_temp);
    esp_zb_cluster_add_attr(d_i_attribute_list, CLUSTER_D_I, 0x0002, ESP_ZB_ZCL_ATTR_TYPE_BOOL, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &digital_temp);
    // Add custom cluster to cluster list
    esp_zb_cluster_list_add_custom_cluster(d_i_cluster_list, d_i_attribute_list, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    // Add the endpoint to endpoint list
    esp_zb_endpoint_config_t d_i_ep_config = {ENDPOINT_D_I, APP_PROFILE_ID, ESP_ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_DEVICE_ID, DEVICE_VER};
    esp_zb_ep_list_add_ep(ep_list, d_i_cluster_list, d_i_ep_config);

    // ------------------------------ Endpoint 20 -- Digital Output Cluster ------------------------------
    //
    // Create cluster list for digital outpout
    esp_zb_cluster_list_t *d_o_cluster_list = esp_zb_zcl_cluster_list_create();
    // Create attribute list for custom cluster digital outpout
    esp_zb_attribute_list_t *d_o_attribute_list = esp_zb_zcl_attr_list_create(CLUSTER_D_O);
    // Add custom attribute to attribute list
    esp_zb_cluster_add_attr(d_o_attribute_list, CLUSTER_D_O, 0x0000, ESP_ZB_ZCL_ATTR_TYPE_BOOL, ESP_ZB_ZCL_ATTR_ACCESS_WRITE_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &digital_temp);
    esp_zb_cluster_add_attr(d_o_attribute_list, CLUSTER_D_O, 0x0001, ESP_ZB_ZCL_ATTR_TYPE_BOOL, ESP_ZB_ZCL_ATTR_ACCESS_WRITE_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &digital_temp);
    esp_zb_cluster_add_attr(d_o_attribute_list, CLUSTER_D_O, 0x0002, ESP_ZB_ZCL_ATTR_TYPE_BOOL, ESP_ZB_ZCL_ATTR_ACCESS_WRITE_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &digital_temp);
    // Add custom cluster to cluster list
    esp_zb_cluster_list_add_custom_cluster(d_o_cluster_list, d_o_attribute_list, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    // Add the endpoint to endpoint list
    esp_zb_endpoint_config_t d_o_ep_config = {ENDPOINT_D_O, APP_PROFILE_ID, ESP_ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_DEVICE_ID, DEVICE_VER};
    esp_zb_ep_list_add_ep(ep_list, d_o_cluster_list, d_o_ep_config);

    // ------------------------------ Endpoint 30 -- Analog Input Cluster ------------------------------
    //
    // Create cluster list for analog input
    esp_zb_cluster_list_t *a_i_cluster_list = esp_zb_zcl_cluster_list_create();
    // Create attribute list for custom cluster analog input
    esp_zb_attribute_list_t *a_i_attribute_list = esp_zb_zcl_attr_list_create(CLUSTER_A_I);
    // Add custom attribute to attribute list
    esp_zb_cluster_add_attr(a_i_attribute_list, CLUSTER_A_I, 0x0000, ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &analog_temp);
    esp_zb_cluster_add_attr(a_i_attribute_list, CLUSTER_A_I, 0x0001, ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &analog_temp);
    esp_zb_cluster_add_attr(a_i_attribute_list, CLUSTER_A_I, 0x0002, ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &analog_temp);
    // Add custom cluster to cluster list
    esp_zb_cluster_list_add_custom_cluster(a_i_cluster_list, a_i_attribute_list, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    // Add the endpoint to endpoint list
    esp_zb_endpoint_config_t a_i_ep_config = {ENDPOINT_A_I, APP_PROFILE_ID, ESP_ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_DEVICE_ID, DEVICE_VER};
    esp_zb_ep_list_add_ep(ep_list, a_i_cluster_list, a_i_ep_config);

    //------------------------------ Endpoint 40 -- Analog Output Cluster ------------------------------
    //
    // Create cluster list for analog output
    esp_zb_cluster_list_t *a_o_cluster_list = esp_zb_zcl_cluster_list_create();
    // Create attribute list for custom cluster analog output
    esp_zb_attribute_list_t *a_o_attribute_list = esp_zb_zcl_attr_list_create(CLUSTER_A_O);
    // Add custom attribute to attribute list
    esp_zb_cluster_add_attr(a_o_attribute_list, CLUSTER_A_O, ATTR_ID_0, ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_WRITE_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &analog_temp);
    esp_zb_cluster_add_attr(a_o_attribute_list, CLUSTER_A_O, ATTR_ID_1, ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_WRITE_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &analog_temp);
    esp_zb_cluster_add_attr(a_o_attribute_list, CLUSTER_A_O, ATTR_ID_2, ESP_ZB_ZCL_ATTR_TYPE_U16, ESP_ZB_ZCL_ATTR_ACCESS_WRITE_ONLY | ESP_ZB_ZCL_ATTR_ACCESS_REPORTING, &analog_temp);
    // Add custom cluster to cluster list
    esp_zb_cluster_list_add_custom_cluster(a_o_cluster_list, a_o_attribute_list, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    // Add the endpoint to endpoint list
    esp_zb_endpoint_config_t a_o_ep_config = {ENDPOINT_A_O, ESP_ZB_AF_HA_PROFILE_ID, ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID, DEVICE_VER};
    esp_zb_ep_list_add_ep(ep_list, a_o_cluster_list, a_o_ep_config);

    // Register end point list
    esp_zb_device_register(ep_list);

    // esp_zb_core_action_handler_register(common_action_handler);
    esp_zb_core_action_handler_register(zb_action_handler);

    // zigbee chanel
    esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);

    // Erase NVRAM before creating connection to new Coordinator
    // Comment out this line to erase NVRAM data if you are conneting to new Coordinator
    // esp_zb_nvram_erase_at_start(true);

    ESP_ERROR_CHECK(esp_zb_start(true));
    esp_zb_stack_main_loop();
    // esp_zb_main_loop_iteration();
}

void esp_zb_update_output_task(void *pvParameters)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        for (uint16_t i = 0; i <= 2; i++)
        {
            uint16_t random_analog = rand() % 256;
            bool random_digital = rand() % 2 == 0;
            zb_update_output(&random_analog, ESP_ZB_ZCL_ATTR_TYPE_U16, ENDPOINT_A_I, CLUSTER_A_I, i);
            zb_update_output(&random_digital, ESP_ZB_ZCL_ATTR_TYPE_BOOL, ENDPOINT_D_I, CLUSTER_D_I, i);
        }
    }
}

void app_main(void)
{
    srand(time(NULL));
    esp_zb_platform_config_t config =
        {
            .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
            .host_config = ESP_ZB_DEFAULT_HOST_CONFIG(),
        };

    ESP_ERROR_CHECK(esp_zb_platform_config(&config));
    xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);

    if (MODE_RANDOM_OR_REWRITE)
    {
        xTaskCreate(esp_zb_update_output_task, "Task_1s", 4096, NULL, 5, NULL);
    }
}
