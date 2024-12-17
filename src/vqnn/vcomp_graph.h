/*
 * Copyright 2024 Qualcomm Innovation Center, Inc. All Rights Reserved.
 * SPDX-License-Identifier: MIT
 */

#ifndef VCOMP_GRAPH_H
#define VCOMP_GRAPH_H

#include "vcomp_common.h"

struct vcomp_context;

struct vcomp_graph
{
    struct vcomp_object base;
};

VCOMP_DEFINE_OBJECT_CAST(graph, Qnn_GraphHandle_t)
void vcomp_context_init_graph_dispatch(struct vcomp_context *vctx);

#endif /* VCOMP_GRAPH_H */