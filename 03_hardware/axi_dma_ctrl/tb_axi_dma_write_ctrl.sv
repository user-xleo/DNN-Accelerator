`timescale 1ns / 1ps

module axi_dma_write_ctrl_tb();

    // Clock period parameters
    localparam CLK_PERIOD = 20; // 50MHz clock
    localparam DA_INCREMENT = 32'h0000_0020;
    localparam MAX_DA = 32'h0000_0800;
    
    // Testbench signals
    logic           clk;
    logic           rst_n;  // Active low reset
    
    // DUT interface signals
    logic [25:0]    dma_length;
    logic [31:0]    dma_dest_addr;
    logic           dma_write_valid;
    logic           dma_idle;
    logic           dma_irq;

    // DUT instantiation
    axi_dma_write_ctrl dut (
        .CLK            (clk),
        .RST           (rst_n),
        .dma_write_valid(dma_write_valid),
        .dma_length_config(dma_length),
        .dma_da_config (dma_dest_addr),
        .DMA_Write_INT (dma_irq),
        .dma_write_idle(dma_idle)
    );

    // Clock generation
    always begin
        clk = 1'b0;
        #(CLK_PERIOD/2);
        clk = 1'b1;
        #(CLK_PERIOD/2);
    end

    // DMA write valid control
    always_ff @(posedge clk) begin
        if (!rst_n) begin
            dma_write_valid <= 1'b0;
        end else begin
            dma_write_valid <= dma_idle;
        end
    end

    // Destination address increment on each transaction
    always_ff @(negedge dma_write_valid) begin
        if (!rst_n) begin
            dma_dest_addr <= '0;
        end else begin
            dma_dest_addr <= dma_dest_addr + DA_INCREMENT;
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
        wait(dma_dest_addr > MAX_DA);
        
        // Add some delay for final transaction
        #(10*CLK_PERIOD);
        $finish;
    end

    // Optional: Add assertions
    // pragma translate_off
    assert property (@(posedge clk) disable iff(!rst_n)
        dma_write_valid |-> !$isunknown(dma_dest_addr))
    else $error("Destination address unknown when write_valid is high");
    // pragma translate_on

endmodule 