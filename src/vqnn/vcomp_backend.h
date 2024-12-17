/*
 * Copyright 2024 Qualcomm Innovation Center, Inc. All Rights Reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VCOMP_BACKEND_H
#define VCOMP_BACKEND_H

#include "vcomp_common.h"

struct vcomp_context;

struct vcomp_backend
{
    struct vcomp_object base;
};

VCOMP_DEFINE_OBJECT_CAST(backend, Qnn_BackendHandle_t)
void vcomp_context_init_backend_dispatch(struct vcomp_context *vctx);

#endif /* VCOMP_BACKEND_H */
