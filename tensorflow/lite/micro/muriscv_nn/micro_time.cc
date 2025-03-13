/*
 * Copyright (C) 2023 Chair of Electronic Design Automation, TUM
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tensorflow/lite/micro/micro_time.h"

#include "muriscv_nn_util.h"

namespace tflite {

// Use the time implementation provided by muRISCV-NN. This way, we can measure
// performance on RISC-V machines. See muriscv_nn_util.h for more infos.

uint32_t ticks_per_second() { return MURISCV_NN_RDTIME_PER_SECOND; }

uint32_t GetCurrentTimeTicks() { return rdtime(); }

}  // namespace tflite
