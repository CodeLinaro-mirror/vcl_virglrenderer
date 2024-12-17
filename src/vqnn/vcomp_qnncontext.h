/*
 * Copyright 2024 Qualcomm Innovation Center, Inc. All Rights Reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VCOMP_QNN_CONTEXT_H
#define VCOMP_QNN_CONTEXT_H

#include "vcomp_common.h"

struct vcomp_context;

// extern uint8_t *data;
// extern uint64_t offset;

struct vcomp_qnn_context
{
    struct vcomp_object base;
};

VCOMP_DEFINE_OBJECT_CAST(qnn_context, Qnn_ContextHandle_t)
void vcomp_context_init_context_dispatch(struct vcomp_context *vctx);

#endif /* VCOMP_QNN_CONTEXT_H */
