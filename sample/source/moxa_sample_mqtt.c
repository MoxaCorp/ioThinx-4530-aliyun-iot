// Copyright (C) 2019 Moxa Inc. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <iothinx/iothinxio.h>

#include "iot_import.h"
#include "iot_export.h"
#include "lite-utils.h"

#define PRODUCT_KEY             "a14qsGgMTtk"
#define DEVICE_NAME             "Example_Device"
#define DEVICE_SECRET           "CTA83oLWKq2n0YgvveFZusvVqeA0EVTq"

#define TOPIC_DATA              "/"PRODUCT_KEY"/"DEVICE_NAME"/data"
#define DEVICEID_KEY            "DeviceId"
#define DEVICEID_VAL            "ioThinx"
#define DI_VALUES_KEY           "DI_Values"
#define DO_VALUES_KEY           "DO_Values"
#define DO_SETVALUES_KEY        "DO_SetValues"
#define CLEAN_SESSION           0
#define REQUEST_TIMEOUT_MS      2000
#define KEEPALIVE_INTERVAL_MS   60000
#define YIELD_TIMEOUT_MS        2000
#define BUF_SIZE                1024

static void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type)
    {
    case IOTX_MQTT_EVENT_UNDEF:
        printf("undefined event occur.\r\n");
        break;
    case IOTX_MQTT_EVENT_DISCONNECT:
        printf("MQTT disconnect.\r\n");
        break;
    case IOTX_MQTT_EVENT_RECONNECT:
        printf("MQTT reconnect.\r\n");
        break;
    case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
        printf("subscribe success, packet-id=%u\r\n", (unsigned int)packet_id);
        break;
    case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
        printf("subscribe wait ack timeout, packet-id=%u\r\n", (unsigned int)packet_id);
        break;
    case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
        printf("subscribe nack, packet-id=%u\r\n", (unsigned int)packet_id);
        break;
    case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
        printf("unsubscribe success, packet-id=%u\r\n", (unsigned int)packet_id);
        break;
    case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
        printf("unsubscribe timeout, packet-id=%u\r\n", (unsigned int)packet_id);
        break;
    case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
        printf("unsubscribe nack, packet-id=%u\r\n", (unsigned int)packet_id);
        break;
    case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
        printf("publish success, packet-id=%u\r\n", (unsigned int)packet_id);
        break;
    case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
        printf("publish timeout, packet-id=%u\r\n", (unsigned int)packet_id);
        break;
    case IOTX_MQTT_EVENT_PUBLISH_NACK:
        printf("publish nack, packet-id=%u\r\n", (unsigned int)packet_id);
        break;
    case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
        printf("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s\r\n",
               topic_info->topic_len,
               topic_info->ptopic,
               topic_info->payload_len,
               topic_info->payload);
        break;
    case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
        printf("buffer overflow\r\n");
        break;
    default:
        printf("Should NOT arrive here.\r\n");
        break;
    }
}

static void subscribe_callback(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    int rc = 0;
    char *val = NULL;
    uint32_t slot = 1;
    iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

    printf("Recv Topic\t: %.*s\r\n"
           "Recv Payload\t: %.*s\r\n",
           ptopic_info->topic_len, ptopic_info->ptopic,
           ptopic_info->payload_len, ptopic_info->payload);

    val = LITE_json_value_of(DEVICEID_KEY, (char *)ptopic_info->payload);
    if (val == NULL)
    {
        return;
    }

    if (strcmp(DEVICEID_VAL, LITE_json_value_of(DEVICEID_KEY, (char *)ptopic_info->payload)) != 0)
    {
        return;
    }

    val = LITE_json_value_of(DO_SETVALUES_KEY, (char *)ptopic_info->payload);
    if (val == NULL)
    {
        return;
    }

    rc = ioThinx_DO_SetValues(slot, strtoul(val, NULL, 0));
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_DO_SetValues() = %d\r\n", rc);
        return;
    }

    printf("%s SUCCESS !!\r\n", DO_SETVALUES_KEY);

    return;
}

static int mqtt_client(void)
{
    int rc = 0;
    uint32_t slot = 1;
    uint32_t di_values = 0;
    uint32_t do_values = 0;
    uint32_t DI_Values = 0;
    uint32_t DO_Values = 0;
    void *pclient = NULL;
    iotx_conn_info_pt pconn_info = NULL;
    iotx_mqtt_param_t mqtt_params = {0};
    iotx_mqtt_topic_info_t topic_msg = {0};
    char msg_pub[BUF_SIZE] = {0};

    rc = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&pconn_info);
    if (rc != 0)
    {
        printf("IOT_SetupConnInfo() = %d\r\n", rc);
        return -1;
    }

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.clean_session = CLEAN_SESSION;
    mqtt_params.request_timeout_ms = REQUEST_TIMEOUT_MS;
    mqtt_params.keepalive_interval_ms = KEEPALIVE_INTERVAL_MS;
    mqtt_params.handle_event.h_fp = event_handle;
    mqtt_params.handle_event.pcontext = NULL;

    mqtt_params.read_buf_size = BUF_SIZE;
    mqtt_params.write_buf_size = BUF_SIZE;

    mqtt_params.pread_buf = calloc(mqtt_params.read_buf_size, sizeof(char));
    if (mqtt_params.pread_buf == NULL)
    {
        printf("calloc() = %s\r\n", "NULL");
        return -1;
    }

    mqtt_params.pwrite_buf = calloc(mqtt_params.write_buf_size, sizeof(char));
    if (mqtt_params.pwrite_buf == NULL)
    {
        printf("calloc() = %s\r\n", "NULL");
        free(mqtt_params.pread_buf);
        return -1;
    }

    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (pclient == NULL)
    {
        printf("IOT_MQTT_Construct() = %s\r\n", "NULL");
        free(mqtt_params.pwrite_buf);
        free(mqtt_params.pread_buf);
        return -1;
    }

    rc = IOT_MQTT_Subscribe(pclient, TOPIC_DATA, IOTX_MQTT_QOS1, subscribe_callback, NULL);
    if (rc < 0)
    {
        printf("IOT_MQTT_Subscribe() = %d\r\n", rc);
        IOT_MQTT_Destroy(&pclient);
        free(mqtt_params.pwrite_buf);
        free(mqtt_params.pread_buf);
        return -1;
    }

    rc = ioThinx_DI_GetValues(slot, &di_values);
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_DI_GetValues() = %d\r\n", rc);
        IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        free(mqtt_params.pwrite_buf);
        free(mqtt_params.pread_buf);
        return -1;
    }

    rc = ioThinx_DO_GetValues(slot, &do_values);
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_DO_GetValues() = %d\r\n", rc);
        IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        free(mqtt_params.pwrite_buf);
        free(mqtt_params.pread_buf);
        return -1;
    }

    DI_Values = di_values;
    DO_Values = do_values;

    snprintf(msg_pub,
             sizeof(msg_pub) - 1,
             "{\"DeviceId\":\"ioThinx\",\"DI_Values\":%u,\"DO_Values\":%u}",
             DI_Values,
             DO_Values);

    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.payload = msg_pub;
    topic_msg.payload_len = strlen(msg_pub);

    rc = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
    if (rc < 0)
    {
        printf("IOT_MQTT_Publish() = %d\r\n", rc);
        IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        free(mqtt_params.pwrite_buf);
        free(mqtt_params.pread_buf);
        return -1;
    }

    printf("Send Topic\t: %s\r\n", TOPIC_DATA);
    printf("Send Payload\t: %s\r\n", msg_pub);

    rc = IOT_MQTT_Yield(pclient, YIELD_TIMEOUT_MS);
    if (rc != 0)
    {
        printf("IOT_MQTT_Yield() = %d\r\n", rc);
        IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
        IOT_MQTT_Destroy(&pclient);
        free(mqtt_params.pwrite_buf);
        free(mqtt_params.pread_buf);
        return -1;
    }

    while (true)
    {
        sleep(1);

        rc = ioThinx_DI_GetValues(slot, &di_values);
        if (rc != IOTHINX_ERR_OK)
        {
            printf("ioThinx_DI_GetValues() = %d\r\n", rc);
            IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
            IOT_MQTT_Destroy(&pclient);
            free(mqtt_params.pwrite_buf);
            free(mqtt_params.pread_buf);
            return -1;
        }

        rc = ioThinx_DO_GetValues(slot, &do_values);
        if (rc != IOTHINX_ERR_OK)
        {
            printf("ioThinx_DO_GetValues() = %d\r\n", rc);
            IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
            IOT_MQTT_Destroy(&pclient);
            free(mqtt_params.pwrite_buf);
            free(mqtt_params.pread_buf);
            return -1;
        }

        if (DI_Values == di_values && DO_Values == do_values)
        {
            continue;
        }

        DI_Values = di_values;
        DO_Values = do_values;

        snprintf(msg_pub,
                 sizeof(msg_pub) - 1,
                 "{\"DeviceId\":\"ioThinx\",\"DI_Values\":%u,\"DO_Values\":%u}",
                 DI_Values,
                 DO_Values);

        topic_msg.qos = IOTX_MQTT_QOS1;
        topic_msg.payload = msg_pub;
        topic_msg.payload_len = strlen(msg_pub);

        rc = IOT_MQTT_Publish(pclient, TOPIC_DATA, &topic_msg);
        if (rc < 0)
        {
            printf("IOT_MQTT_Publish() = %d\r\n", rc);
            IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
            IOT_MQTT_Destroy(&pclient);
            free(mqtt_params.pwrite_buf);
            free(mqtt_params.pread_buf);
            return -1;
        }

        printf("Send Topic\t: %s\r\n", TOPIC_DATA);
        printf("Send Payload\t: %s\r\n", msg_pub);

        rc = IOT_MQTT_Yield(pclient, YIELD_TIMEOUT_MS);
        if (rc != 0)
        {
            printf("IOT_MQTT_Yield() = %d\r\n", rc);
            IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
            IOT_MQTT_Destroy(&pclient);
            free(mqtt_params.pwrite_buf);
            free(mqtt_params.pread_buf);
            return -1;
        }
    }

    IOT_MQTT_Unsubscribe(pclient, TOPIC_DATA);
    IOT_MQTT_Destroy(&pclient);
    free(mqtt_params.pwrite_buf);
    free(mqtt_params.pread_buf);

    return 0;
}

int main(int argc, char const *argv[])
{
    int rc = 0;

    IOT_OpenLog("mqtt");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    rc = ioThinx_IO_Client_Init();
    if (rc != IOTHINX_ERR_OK)
    {
        printf("ioThinx_IO_Client_Init() = %d\r\n", rc);
        return -1;
    }

    rc = mqtt_client();
    if (rc != 0)
    {
        return -1;
    }

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    return 0;
}
