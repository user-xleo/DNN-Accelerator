# Systolic Array Implementation
This folder contains a parameterized systolic array implementation in SystemVerilog, designed for efficient matrix multiplication. 

The design uses a grid of Processing Elements (PEs) that perform multiply-accumulate (MAC) operations using DSP48E1 blocks.

## Features
- Parameterized NxN array size
- Configurable data width
- Pipelined architecture for high throughput
- DSP48E1-based MAC operations
- Valid signal propagation for result verification

## Architecture
The systolic array consists of:
- An NxN grid of Processing Elements (PEs)
- Input data distribution network
- Output collection network
- Valid signal propagation chain

### Processing Element (PE)
Each PE performs the following operations:
- Receives input data from west (a_in) and north (b_in)
- Performs MAC operation: c_out = c_in + (a_in * b_in)
- Propagates data to east (a_out) and south (b_out)
- Propagates valid signal

## Parameters
- `ARRAY_SIZE`: Size of the NxN array (default: 8)
- `DATA_WIDTH`: Bit width of input data (default: 8)


## Usage
1. Configure the array size and data width parameters as needed
2. Provide input matrices A and B through `a_inputs` and `b_inputs`
3. Assert `valid_in` when input data is valid
4. Wait for `valid_out` to indicate valid output data
5. Read result matrix from `c_outputs`

## Timing
- Results appear after (2 * ARRAY_SIZE - 1) clock cycles
- New input can be started every clock cycle
- Output valid signal indicates when results are ready

## Example
For an 8x8 array with 8-bit data width:
- Matrix multiplication of 8x8 matrices
- Input data range: -128 to 127 (8-bit)
- Output data width: 19 bits (8*2 + 3 bits for accumulation)
- Latency: 15 clock cycles for first result