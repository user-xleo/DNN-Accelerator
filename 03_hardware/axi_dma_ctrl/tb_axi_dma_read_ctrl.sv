`timescale 1ns / 1ps

module axi_dma_read_ctrl_tb();

    // Clock period parameters
    localparam CLK_PERIOD = 20; // 50MHz clock
    localparam SA_INCREMENT = 32'h0000_0020;
    localparam MAX_SA = 32'h0000_0800;
    
    // Testbench signals
    logic           clk;
    logic           rst_n;  // Active low reset
    
    // DUT interface signals
    logic [25:0]    dma_length;
    logic [31:0]    dma_start_addr;
    logic           dma_read_valid;
    logic           dma_idle;
    logic           dma_irq;

    // DUT instantiation
    dma_read_ctrl dut (
        .CLK            (clk),
        .RST           (rst_n),
        .dma_read_valid(dma_read_valid),
        .dma_length_config(dma_length),
        .dma_sa_config (dma_start_addr),
        .DMA_IRQ       (dma_irq),
        .dma_idle      (dma_idle)
    );

    // Clock generation
    always begin
        clk = 1'b0;
        #(CLK_PERIOD/2);
        clk = 1'b1;
        #(CLK_PERIOD/2);
    end

    // DMA read valid control
    always_ff @(posedge clk) begin
        if (!rst_n) begin
            dma_read_valid <= 1'b0;
        end else begin
            dma_read_valid <= dma_idle;
        end
    end

    // Start address increment on each transaction
    always_ff @(negedge dma_read_valid) begin
        if (!rst_n) begin
            dma_start_addr <= '0;
        end else begin
            dma_start_addr <= dma_start_addr + SA_INCREMENT;
        end
    end

    // Test stimulus
    initial begin
        // Initialize signals
        rst_n = 1'b1;
        dma_length = 26'h0000040;
        
        // Apply reset
        #(CLK_PERIOD);
        rst_n = 1'b0;
        #(2*CLK_PERIOD);
        rst_n = 1'b1;

        // Wait for completion or timeout
        wait(dma_start_addr > MAX_SA);
        
        // Add some delay for final transaction
        #(10*CLK_PERIOD);
        $finish;
    end

    // Optional: Add assertions
    // pragma translate_off
    assert property (@(posedge clk) disable iff(!rst_n)
        dma_read_valid |-> !$isunknown(dma_start_addr))
    else $error("Start address unknown when read_valid is high");
    // pragma translate_on

endmodule 