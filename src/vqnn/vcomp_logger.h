/*
 * Copyright 2024 Qualcomm Innovation Center, Inc. All Rights Reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VCOMP_LOGGER_H
#define VCOMP_LOGGER_H

#include "vcomp_common.h"

struct vcomp_context;

struct vcomp_logger
{
    struct vcomp_object base;
};

VCOMP_DEFINE_OBJECT_CAST(logger, Qnn_LogHandle_t)
void vcomp_context_init_logger_dispatch(struct vcomp_context *vctx);


#endif /* VCOMP_LOGGER_H */
