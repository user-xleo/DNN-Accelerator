# Introduction

This code utilizes [AirHDL](https://airhdl.com/) to automatically generate code for configuring neural network accelerators via the AXI protocol, specifically through Instruction Registers.

# Register Map

## Load Store Unit (LSU)

### Load from Memory
| Register Name            | Offset | Description                                |
|--------------------------|--------|--------------------------------------------|
| LSU_LD_SRC_ADDR_LOW      | 0      | Lower 32 bits of source memory address    |
| LSU_LD_SRC_ADDR_HIGH     | 1      | Higher 32 bits of source memory address   |
| LSU_LD_DST_ID            | 2      | Destination ID for loading data            |
| LSU_LD_LENGTH            | 3      | Length of data to be loaded                |
| LSU_LD_CONTROL           | 4      | Control register for load operation       |
| LSU_LD_STATUS            | 5      | Status register for load operation        |

### Store to Memory
| Register Name            | Offset | Description                                |
|--------------------------|--------|--------------------------------------------|
| LSU_ST_SRC_ID            | 0      | Source ID for storing data                 |
| LSU_ST_SRC_ADDR          | 1      | Source address for store operation         |
| LSU_ST_DST_ADDR_LOW      | 2      | Lower 32 bits of destination memory address|
| LSU_ST_DST_ADDR_HIGH     | 3      | Higher 32 bits of destination memory address |
| LSU_ST_LENGTH            | 4      | Length of data to be stored                |
| LSU_ST_CONTROL           | 5      | Control register for store operation       |
| LSU_ST_STATUS            | 6      | Status register for store operation        |

### Load from Cache
| Register Name            | Offset | Description                                |
|--------------------------|--------|--------------------------------------------|
| LSU_LD_CACHE_SRC_ID      | 0      | Source ID for cache load                   |
| LSU_LD_CACHE_SRC_ADDR    | 1      | Source address in cache                    |
| LSU_LD_CACHE_DST_ID      | 2      | Destination ID for cache load              |
| LSU_LD_CACHE_DST_ADDR_LOW| 3      | Lower 32 bits of destination address       |
| LSU_LD_CACHE_DST_ADDR_HIGH | 4    | Higher 32 bits of destination address      |
| LSU_LD_CACHE_LENGTH      | 5      | Length of data to be loaded from cache     |
| LSU_LD_CACHE_CONTROL     | 6      | Control register for cache load operation  |
| LSU_LD_CACHE_STATUS      | 7      | Status register for cache load operation   |

### Store to Cache
| Register Name            | Offset | Description                                |
|--------------------------|--------|--------------------------------------------|
| LSU_ST_CACHE_SRC_ADDR_LOW| 0      | Lower 32 bits of source address            |
| LSU_ST_CACHE_SRC_ADDR_HIGH | 1    | Higher 32 bits of source address           |
| LSU_ST_CACHE_DST_ID      | 2      | Destination ID for cache store             |
| LSU_ST_CACHE_DST_ADDR    | 3      | Destination address in cache               |
| LSU_ST_CACHE_LENGTH      | 4      | Length of data to be stored to cache       |
| LSU_ST_CACHE_CONTROL     | 5      | Control register for cache store operation |
| LSU_ST_CACHE_STATUS      | 6      | Status register for cache store operation  |

## Image-to-Column Conversion (IMG2COL)
| Register Name            | Offset | Description                                |
|--------------------------|--------|--------------------------------------------|
| IMG2COL_IN_HEIGHT        | 0      | Input image height                         |
| IMG2COL_IN_WIDTH         | 1      | Input image width                          |
| IMG2COL_IN_CHANNELS      | 2      | Number of input channels                   |
| IMG2COL_KERNEL_SIZE      | 3      | Convolution kernel size                    |
| IMG2COL_STRIDE           | 4      | Convolution stride                         |
| IMG2COL_PAD              | 5      | Padding size                               |
| IMG2COL_CONTROL          | 6      | Control register for img2col operation     |
| IMG2COL_STATUS           | 7      | Status register for img2col operation      |

## Systolic Array
| Register Name            | Offset | Description                                |
|--------------------------|--------|--------------------------------------------|
| SYSTOLIC_ARRAY_IN_HEIGHT | 0      | Input matrix height                        |
| SYSTOLIC_ARRAY_OUT_HEIGHT| 1      | Output matrix height                       |
| SYSTOLIC_ARRAY_IN_WIDTH  | 2      | Input matrix width                         |
| SYSTOLIC_ARRAY_OUT_WIDTH | 3      | Output matrix width                        |
| SYSTOLIC_ARRAY_IN_CHANNELS | 4    | Number of input channels                   |
| SYSTOLIC_ARRAY_OUT_CHANNELS | 5   | Number of output channels                  |
| SYSTOLIC_ARRAY_RELU_EN   | 6      | ReLU activation enable                     |
| SYSTOLIC_ARRAY_CONTROL   | 7      | Control register for systolic array        |
| SYSTOLIC_ARRAY_STATUS    | 8      | Status register for systolic array         |