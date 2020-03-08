`timescale 1ns / 1ps

// Parameterized Systolic Array for matrix multiplication
// Uses DSP48E1 for MAC operations
module systolic_array #(
    parameter ARRAY_SIZE = 8,    // NxN array size
    parameter DATA_WIDTH = 8     // Input data width
)(
    input  logic                                 clk,
    input  logic                                 rst_n,
    input  logic                                 valid_in,
    input  logic [DATA_WIDTH-1:0]                a_inputs [ARRAY_SIZE-1:0], // Input matrix A
    input  logic [DATA_WIDTH-1:0]                b_inputs [ARRAY_SIZE-1:0], // Input matrix B
    output logic [DATA_WIDTH*2+ARRAY_SIZE-1:0]   c_outputs [ARRAY_SIZE-1:0], // Output matrix C
    output logic                                 valid_out
);

    // Internal signals for PE connections
    logic [DATA_WIDTH-1:0] a_wires [ARRAY_SIZE:0][ARRAY_SIZE:0];
    logic [DATA_WIDTH-1:0] b_wires [ARRAY_SIZE:0][ARRAY_SIZE:0];
    logic [DATA_WIDTH*2+ARRAY_SIZE-1:0] c_wires [ARRAY_SIZE:0][ARRAY_SIZE:0];
    
    // Valid signal propagation
    logic valid_regs [ARRAY_SIZE:0][ARRAY_SIZE:0];
    
    // Input assignment
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (int i = 0; i < ARRAY_SIZE; i++) begin
                a_wires[0][i] <= '0;
                b_wires[i][0] <= '0;
            end
        end else begin
            for (int i = 0; i < ARRAY_SIZE; i++) begin
                a_wires[0][i] <= a_inputs[i];
                b_wires[i][0] <= b_inputs[i];
            end
        end
    end

    // Generate the PE array
    genvar i, j;
    generate
        for (i = 0; i < ARRAY_SIZE; i++) begin : ROW
            for (j = 0; j < ARRAY_SIZE; j++) begin : COL
                processing_element #(
                    .DATA_WIDTH(DATA_WIDTH)
                ) pe (
                    .clk(clk),
                    .rst_n(rst_n),
                    .valid_in(valid_regs[i][j]),
                    .a_in(a_wires[i][j]),
                    .b_in(b_wires[i][j]),
                    .c_in(c_wires[i][j]),
                    .a_out(a_wires[i+1][j]),
                    .b_out(b_wires[i][j+1]),
                    .c_out(c_wires[i+1][j+1]),
                    .valid_out(valid_regs[i+1][j+1])
                );
            end
        end
    endgenerate

    // Output assignment
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (int i = 0; i < ARRAY_SIZE; i++) begin
                c_outputs[i] <= '0;
            end
            valid_out <= 1'b0;
        end else begin
            for (int i = 0; i < ARRAY_SIZE; i++) begin
                c_outputs[i] <= c_wires[ARRAY_SIZE][i+1];
            end
            valid_out <= valid_regs[ARRAY_SIZE][ARRAY_SIZE];
        end
    end

endmodule

// Processing Element (PE) module
module processing_element #(
    parameter DATA_WIDTH = 8
)(
    input  logic                                 clk,
    input  logic                                 rst_n,
    input  logic                                 valid_in,
    input  logic [DATA_WIDTH-1:0]               a_in,
    input  logic [DATA_WIDTH-1:0]               b_in,
    input  logic [DATA_WIDTH*2+ARRAY_SIZE-1:0]  c_in,
    output logic [DATA_WIDTH-1:0]               a_out,
    output logic [DATA_WIDTH-1:0]               b_out,
    output logic [DATA_WIDTH*2+ARRAY_SIZE-1:0]  c_out,
    output logic                                valid_out
);

    // Registered outputs
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            a_out <= '0;
            b_out <= '0;
            c_out <= '0;
            valid_out <= 1'b0;
        end else begin
            a_out <= a_in;
            b_out <= b_in;
            // MAC operation using DSP48E1
            c_out <= c_in + (a_in * b_in);
            valid_out <= valid_in;
        end
    end

endmodule
