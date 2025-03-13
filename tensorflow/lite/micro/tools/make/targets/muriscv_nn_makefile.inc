# Copyright (C) 2023 Chair of Electronic Design Automation, TUM
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

MAKEFILE_PATH := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

TARGET_ARCH := riscv32
GCC_TOOLCHAIN_PREFIX := riscv32-unknown-elf

RISCV_ARCH := rv32imc
RISCV_ABI := ilp32
RISCV_CODE_MODEL := medany

BUILD_TYPE := micro
LDFLAGS += --specs=nano.specs
LDFLAGS += -mno-relax


# Path to GCC toolchain
GCC_TOOLCHAIN_ROOT := $(MAKEFILE_PATH)../../../../../../../../Toolchain/$(TARGET_ARCH)

ifneq (,$(MURISCV_NN_LIBS))
  ifeq (,$(MURISCV_NN_PATH))
    $(error MURISCV_NN_LIBS provided but not MURISCV_NN_PATH)
  endif
endif

# Path to the muriscv_nn library
MURISCV_NN_PATH := $(MAKEFILE_PATH)../../../../../../../../../muriscv_nn

ifeq (,$(MURISCV_NN_LIBS))
  THIRD_PARTY_CC_SRCS += $(call recursive_find,$(MURISCV_NN_PATH)/Source,*.c)
else
  MICROLITE_LIBS += $(MURISCV_NN_LIBS)
endif
THIRD_PARTY_CC_HDRS += $(call recursive_find,$(MURISCV_NN_PATH)/Include,*.h)
INCLUDES += \
  -I$(MURISCV_NN_PATH)/Include \
  -I$(MURISCV_NN_PATH)/Include/CMSIS/NN

# Flags requied in order to compile with RISC-V toolchains

# Enable vector extension if requested
ifeq ($(USE_VEXT), ON)
  ifeq ($(USE_PORTABLE), ON)
    PLATFORM_FLAGS += \
      -DUSE_PORTABLE_VEXT
  else
    PLATFORM_FLAGS += \
      -DUSE_VEXT
  endif
endif
# Enable packed extension if requested
ifeq ($(USE_PEXT), ON)
  PLATFORM_FLAGS += \
    -DUSE_PEXT
endif

ifeq ($(TOOLCHAIN), gcc)
  ifneq ($(TARGET_ARCH), native)
    TARGET_TOOLCHAIN_ROOT := $(GCC_TOOLCHAIN_ROOT)/bin/
    TARGET_TOOLCHAIN_PREFIX := $(GCC_TOOLCHAIN_PREFIX)-

    PLATFORM_FLAGS += \
  -march=$(RISCV_ARCH) \
  -mabi=$(RISCV_ABI) \
  -mcmodel=$(RISCV_CODE_MODEL) \
  -mexplicit-relocs \
  -fno-builtin-printf \
  -DTF_LITE_MCU_DEBUG_LOG \
  -DTF_LITE_USE_GLOBAL_CMATH_FUNCTIONS \
  -funsigned-char \
  -fno-delete-null-pointer-checks \
  -fomit-frame-pointer
  endif


else ifeq ($(TOOLCHAIN), llvm)

  CXX_TOOL := $(LLVM_TOOLCHAIN_ROOT)/clang++
  CC_TOOL := $(LLVM_TOOLCHAIN_ROOT)/clang
  AR_TOOL := $(LLVM_TOOLCHAIN_ROOT)/llvm-ar
  CP_TOOL := $(LLVM_TOOLCHAIN_ROOT)/llvm-objcopy

  PLATFORM_FLAGS += \
  -march=$(TARGET_ARCH) \
  -mabi=ilp32d \
  --target=riscv32 \
  --gcc-toolchain=$(GCC_TOOLCHAIN_ROOT) \
  --sysroot=$(GCC_TOOLCHAIN_ROOT)/$(GCC_TOOLCHAIN_PREFIX)

  LDFLAGS += -fuse-ld=lld

else
  $(error "TOOLCHAIN=$(TOOLCHAIN) is not supported.")
endif

# No idea why the C flag variable is called CCFLAGS instead of CFLAGS
CCFLAGS += $(PLATFORM_FLAGS)
CXXFLAGS += $(PLATFORM_FLAGS) \
  -fpermissive \
  -fno-use-cxa-atexit \
  -DTF_LITE_USE_GLOBAL_MIN \
  -DTF_LITE_USE_GLOBAL_MAX

include $(MAKEFILE_DIR)/ext_libs/eyalroz_printf.inc
