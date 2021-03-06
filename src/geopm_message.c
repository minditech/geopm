/*
 * Copyright (c) 2015, 2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY LOG OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "geopm_message.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

const struct geopm_policy_message_s GEOPM_POLICY_UNKNOWN = {-1, -1, -1, -1.0};
const struct geopm_sample_message_s GEOPM_SAMPLE_INVALID = {0, {0}};

int geopm_is_policy_equal(const struct geopm_policy_message_s *a, const struct geopm_policy_message_s *b)
{
    int result = 1;
    if (a->mode != b->mode ||
        a->flags != b->flags ||
        a->num_sample != b->num_sample ||
        a->power_budget != b->power_budget) {
        result = 0;
    }
    return result;
}

int geopm_is_sample_equal(const struct geopm_sample_message_s *a, const struct geopm_sample_message_s *b)
{

    int i;
    int result = 1;
    for (i = 0; result && (i < GEOPM_NUM_SAMPLE_TYPE); ++i) {
        if (a->signal[i] != b->signal[i]) {
            result = 0;
        }
    }
    if (a->region_id != b->region_id) {
        result = 0;
    }
    return result;
}
