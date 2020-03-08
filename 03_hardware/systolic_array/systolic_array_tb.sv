`timescale 1ns / 1ps

module systolic_array_tb;
    // Parameters
    localparam ARRAY_SIZE = 8;
    localparam DATA_WIDTH = 8;
    localparam CLK_PERIOD = 10;
    
    // Signals
    logic clk;
    logic rst_n;
    logic valid_in;
    logic [DATA_WIDTH-1:0] a_inputs [ARRAY_SIZE-1:0];
    logic [DATA_WIDTH-1:0] b_inputs [ARRAY_SIZE-1:0];
    logic [DATA_WIDTH*2+ARRAY_SIZE-1:0] c_outputs [ARRAY_SIZE-1:0];
    logic valid_out;
    
    // DUT instantiation
    systolic_array #(
        .ARRAY_SIZE(ARRAY_SIZE),
        .DATA_WIDTH(DATA_WIDTH)
    ) dut (.*);
    
    // Clock generation
    initial begin
        clk = 0;
        forever #(CLK_PERIOD/2) clk = ~clk;
    end
    
    // Test stimulus
    initial begin
        // Initialize test matrices
        logic [DATA_WIDTH-1:0] matrix_a [ARRAY_SIZE][ARRAY_SIZE];
        logic [DATA_WIDTH-1:0] matrix_b [ARRAY_SIZE][ARRAY_SIZE];
        
        // Reset
        rst_n = 0;
        valid_in = 0;
        foreach(a_inputs[i]) a_inputs[i] = '0;
        foreach(b_inputs[i]) b_inputs[i] = '0;
        repeat(3) @(posedge clk);
        rst_n = 1;
        
        // Initialize test matrices with simple values
        for (int i = 0; i < ARRAY_SIZE; i++) begin
            for (int j = 0; j < ARRAY_SIZE; j++) begin
                matrix_a[i][j] = i + 1;  // 1-8 in each row
                matrix_b[i][j] = j + 1;  // 1-8 in each column
            end
        end
        
        // Feed the matrices
        valid_in = 1;
        for (int cycle = 0; cycle < ARRAY_SIZE*2; cycle++) begin
            for (int i = 0; i < ARRAY_SIZE; i++) begin
                // Diagonal feeding pattern
                if (cycle - i >= 0 && cycle - i < ARRAY_SIZE) begin
                    a_inputs[i] = matrix_a[i][cycle-i];
                    b_inputs[i] = matrix_b[cycle-i][i];
                end else begin
                    a_inputs[i] = '0;
                    b_inputs[i] = '0;
                end
            end
            @(posedge clk);
        end
        
        valid_in = 0;
        
        // Wait for computation to complete
        // Need to wait ARRAY_SIZE more cycles for the result to propagate
        repeat(ARRAY_SIZE + 5) @(posedge clk);
        
        // Print results
        $display("Matrix multiplication results:");
        for (int i = 0; i < ARRAY_SIZE; i++) begin
            $write("Row %0d: ", i);
            for (int j = 0; j < ARRAY_SIZE; j++) begin
                $write("%d ", c_outputs[i]);
            end
            $display("");
        end
        
        // End simulation
        #100;
        $finish;
    end
    
    // Optional: Waveform dumping
    initial begin
        $dumpfile("systolic_array_tb.vcd");
        $dumpvars(0, systolic_array_tb);
    end

endmodule 