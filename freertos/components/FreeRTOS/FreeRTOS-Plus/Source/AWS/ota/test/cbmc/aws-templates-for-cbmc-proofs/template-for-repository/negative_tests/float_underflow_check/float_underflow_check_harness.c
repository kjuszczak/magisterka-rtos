/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT-0.
 */

/**
 * A negative test for --float-overflow-check flag
 */
void float_underflow_check_harness() {
    float underflow, increment;
    underflow -= increment;
}
