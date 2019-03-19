# Copyright (C) 2019 Moxa Inc. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

DEPENDS                 := src/platform
LDFLAGS                 := -liot_sdk
LDFLAGS                 += -liot_platform
LDFLAGS                 += -Bstatic -liot_tls
CFLAGS                  := $(filter-out -ansi,$(CFLAGS))

ifneq (,$(filter -DMQTT_COMM_ENABLED,$(CFLAGS)))
TARGET                  += moxa_sample_mqtt
SRCS_moxa_sample_mqtt   := moxa_sample_mqtt.c

    ifneq (,$(filter -DMQTT_ID2_AUTH,$(CFLAGS)))
    ifneq (,$(filter -DMQTT_ID2_ENV=daily,$(CFLAGS)))
    LDFLAGS             += -ltfs
    else
    LDFLAGS             += -ltfs_online
    endif
    LDFLAGS             += -liot_tls
    endif

endif
