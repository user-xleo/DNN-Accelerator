`timescale 1 ns / 1 ps

	module axi_dma_write_ctrl #
	(
    parameter  C_M_START_DATA_VALUE	= 32'h0000_0000,
    parameter  C_M_TARGET_SLAVE_BASE_ADDR	= 32'h0000_0000,
    parameter integer C_M_AXI_ADDR_WIDTH	= 32,
    parameter integer C_M_AXI_DATA_WIDTH	= 32,
    parameter integer C_M_TRANSACTIONS_NUM	= 4
)
(
    // User Define
    input wire [31:0] DMA_DA_CONFIG,
    input wire [25:0] DMA_LENGTH_CONFIG,
    input wire  DMA_WRITE_IRQ, 
    input wire  DMA_WRITE_VALID,
    output wire DMA_WRITE_IDLE,
    // AXI Lite Defination
    input wire  M_AXI_ACLK,
    input wire  M_AXI_ARESETN,
    output wire [C_M_AXI_ADDR_WIDTH-1 : 0] M_AXI_AWADDR,
    output wire [2 : 0] M_AXI_AWPROT,
    output wire  M_AXI_AWVALID,
    input wire  M_AXI_AWREADY,
    output wire [C_M_AXI_DATA_WIDTH-1 : 0] M_AXI_WDATA,
    output wire [C_M_AXI_DATA_WIDTH/8-1 : 0] M_AXI_WSTRB,
    output wire  M_AXI_WVALID,
    input wire  M_AXI_WREADY,
    input wire [1 : 0] M_AXI_BRESP,
    input wire  M_AXI_BVALID,
    output wire  M_AXI_BREADY,
    output wire [C_M_AXI_ADDR_WIDTH-1 : 0] M_AXI_ARADDR,
    output wire [2 : 0] M_AXI_ARPROT,
    output wire  M_AXI_ARVALID,
    input wire  M_AXI_ARREADY,
    input wire [C_M_AXI_DATA_WIDTH-1 : 0] M_AXI_RDATA,
    input wire [1 : 0] M_AXI_RRESP,
    input wire  M_AXI_RVALID,
    output wire  M_AXI_RREADY
);
	// AXI4LITE signals
	//write address valid
	reg  	axi_awvalid;
	//write data valid
	reg  	axi_wvalid;
	//read address valid
	reg  	axi_arvalid;
	//read data acceptance
	reg  	axi_rready;
	//write response acceptance
	reg  	axi_bready;
	//write address
	reg [C_M_AXI_ADDR_WIDTH-1 : 0] 	axi_awaddr;
	//write data
	reg [C_M_AXI_DATA_WIDTH-1 : 0] 	axi_wdata;
	//read addresss
	reg [C_M_AXI_ADDR_WIDTH-1 : 0] 	axi_araddr;
	//A pulse to initiate a write transaction
	reg  	start_single_write;
	//A pulse to initiate a read transaction
	reg  	start_single_read;
    // User Logic
    // Internal signals
    logic [3:0]              state;                // Main FSM state
    logic [3:0]              write_state;         // Write sequence state  
    logic                    read_complete;        // Read transaction complete flag
    logic                    dma_write_idle;      // DMA idle status

    // State machine parameters
    typedef enum logic [3:0] {
        ST_IDLE         = 4'b0000,    // Wait for DMA write valid
        ST_CHECK_IDLE   = 4'b0001,    // Check if DMA is idle
        ST_CTRL_DMA     = 4'b0010,    // Configure DMA registers
        ST_WAIT_IRQ     = 4'b0100,    // Wait for interrupt
        ST_CLEAR_IRQ    = 4'b1000     // Clear interrupt flag
    } state_t;

    // Write sequence states
    typedef enum logic [3:0] {
        WR_IRQ          = 4'b0000,    // Write IRQ configuration
        WR_DA           = 4'b0001,    // Write destination address
        WR_LENGTH       = 4'b0010,    // Write transfer length
        WR_COMPLETE     = 4'b0011     // Write sequence complete
    } write_state_t;

    // Register addresses
    localparam ADDR_IDLE    = 32'h0000_0034;     // Idle status register
    localparam ADDR_IRQ     = 32'h0000_0030;     // IRQ control register
    localparam ADDR_DA      = 32'h0000_0048;     // Destination address register
    localparam ADDR_LENGTH  = 32'h0000_0058;     // Transfer length register
    localparam ADDR_CLEAR   = 32'h0000_0034;     // IRQ clear register

    // Register values
    localparam VAL_IRQ      = 32'h00011003;      // IRQ enable configuration
    localparam VAL_CLEAR    = 32'h00011001;      // IRQ clear value

    // Configuration signals
    logic [31:0]            dma_da_config;       // Destination address config
    logic [25:0]            dma_length_config;   // Transfer length config

    // Register updates on valid
    assign dma_da_config = DMA_WRITE_VALID ? DMA_DA_CONFIG : dma_da_config;
    assign dma_length_config = DMA_WRITE_VALID ? DMA_LENGTH_CONFIG : dma_length_config;

	//Adding the offset address to the base addr of the slave
	assign M_AXI_AWADDR	= C_M_TARGET_SLAVE_BASE_ADDR + axi_awaddr;
	//AXI 4 write data
	assign M_AXI_WDATA	= axi_wdata;
	assign M_AXI_AWPROT	= 3'b000;
	assign M_AXI_AWVALID	= axi_awvalid;
	//Write Data(W)
	assign M_AXI_WVALID	= axi_wvalid;
	//Set all byte strobes in this example
	assign M_AXI_WSTRB	= 4'b1111;
	//Write Response (B)
	assign M_AXI_BREADY	= axi_bready;
	//Read Address (AR)
	assign M_AXI_ARADDR	= C_M_TARGET_SLAVE_BASE_ADDR + axi_araddr;
	assign M_AXI_ARVALID	= axi_arvalid;
	assign M_AXI_ARPROT	= 3'b001;
	//Read and Read Response (R)
	assign M_AXI_RREADY	= axi_rready;
    // User Logic
    wire  [31:0]            dma_da_config        ;
    wire  [25:0]            dma_length_config    ;
    assign dma_da_config = DMA_WRITE_VALID == 1 ? DMA_DA_CONFIG : dma_da_config;
    assign dma_length_config = DMA_WRITE_VALID == 1 ? DMA_LENGTH_CONFIG : dma_length_config;
	//Write Address Channel
    always @(posedge M_AXI_ACLK) begin                                                                         
        if (M_AXI_ARESETN == 0) begin                                                                    
            axi_awvalid <= 1'b0;                                                   
        end                                                                           
        else begin                                                                    
            if (start_single_write) begin                                                                
                axi_awvalid <= 1'b1;                                               
            end                                                                  
            else if (M_AXI_AWREADY && axi_awvalid) begin                                                                
                axi_awvalid <= 1'b0;                                               
            end                                                                  
        end                                                                      
    end                                                                          
	//Write Data Channel
    always @(posedge M_AXI_ACLK) begin                                                                         
        if (M_AXI_ARESETN == 0 ) begin                                                                     
            axi_wvalid <= 1'b0;                                                     
        end                                                                           
        else if (start_single_write) begin                                                                     
            axi_wvalid <= 1'b1;                                                     
        end                                                                            
        else if (M_AXI_WREADY && axi_wvalid) begin                                                                     
            axi_wvalid <= 1'b0;                                                      
        end                                                                       
    end                                                                           
	//Write Response (B) Channel
    always @(posedge M_AXI_ACLK) begin                                                                
        if (M_AXI_ARESETN == 0) begin                                                            
            axi_bready <= 1'b0;                                            
        end                                                                           
        else if (M_AXI_BVALID && ~axi_bready) begin                                                            
            axi_bready <= 1'b1;                                            
        end                                                                                            
        else if (axi_bready) begin                                                            
            axi_bready <= 1'b0;                                            
        end                                                                                              
        else begin                                                        
            axi_bready <= axi_bready;
        end                                      
    end                                                                              
    // Write Response (B) Valid
    always @(posedge M_AXI_ACLK) begin                                                                            
        if (M_AXI_ARESETN == 0) begin                                                                        
            axi_arvalid <= 1'b0;                                                       
        end       
        else if (start_single_read) begin                                                                        
            axi_arvalid <= 1'b1;                                                       
        end                                                                          
        else if (M_AXI_ARREADY && axi_arvalid) begin                                                                        
            axi_arvalid <= 1'b0;                                                       
        end                                                                                                                          
    end                                                                              
	//Read Data (and Response) Channel
    always @(posedge M_AXI_ACLK) begin                                                                 
        if (M_AXI_ARESETN == 0 ) begin                                                             
            axi_rready <= 1'b0;                                             
        end                                                                                      
        else if (M_AXI_RVALID && ~axi_rready) begin                                                             
            axi_rready <= 1'b1;                                             
        end                                                                                                 
        else if (axi_rready) begin
            axi_rready <= 1'b0;                
        end                           
    end
	//--------------------------------
	//User Logic
	//--------------------------------
    // DMA Read_Example By CNILeo    
    // 顶层状态机
    always @(posedge M_AXI_ACLK) begin
        if(M_AXI_ARESETN == 0) begin
            state <= ST_IDLE;
        end
        else begin
            case(state)
                ST_IDLE: begin
                    if(DMA_WRITE_VALID) begin
                        state <= ST_CHECK_IDLE;
                    end
                end
                ST_CHECK_IDLE: begin
                    if(M_AXI_RDATA[0] == 1 || M_AXI_RDATA[1] == 1) begin
                        state <= ST_CTRL_DMA;
                    end
                end
                ST_CTRL_DMA: begin
                    if(write_state == WR_COMPLETE) begin
                        state <= ST_WAIT_IRQ;
                    end
                end
                ST_WAIT_IRQ: begin
                    if(DMA_WRITE_IRQ) begin
                        state <= ST_CLEAR_IRQ;
                    end
                end
                ST_CLEAR_IRQ: begin
                    if(~DMA_WRITE_IRQ) begin
                        state <= ST_IDLE;
                    end
                end
                default:
                    state <= ST_IDLE;
            endcase
        end
    end
    //Control DMA_IDLE
    always @(posedge M_AXI_ACLK) begin
        if (M_AXI_ARESETN == 0) begin                                                 
            dma_write_idle <= 1'b0;
        end
        else if(state == ST_IDLE) begin
            dma_write_idle <= 1'b1;
        end
        else begin
            dma_write_idle <= 1'b0;
        end       
    end
    
    assign DMA_WRITE_IDLE = dma_write_idle & ~DMA_WRITE_VALID;
    //Read Addresses                                              
    always @(posedge M_AXI_ACLK) begin                                                     
        if (M_AXI_ARESETN == 0) begin                                                 
            axi_araddr <= 0;                                    
        end                                                                        
        else if (start_single_read) begin                                                 
            axi_araddr <= ADDR_IDLE;
        end
        else begin
            axi_araddr <= 0;
        end                     
    end
    //Read Complete
    always @(posedge M_AXI_ACLK) begin
        if(M_AXI_ARESETN == 0) begin
            read_complete <= 1'b1;
        end
        else begin
            if(axi_rready == 1'b1)
                read_complete <= 1'b1;
            else if(start_single_read == 1'b1)
                read_complete <= 1'b0;
            else
                read_complete <= read_complete;
        end
    end
    //ENA Check_IDLE
    always @(posedge M_AXI_ACLK)begin
        if(M_AXI_ARESETN == 0) begin
            start_single_read <= 1'b0;
        end
        else begin
            if(state == ST_CHECK_IDLE) begin
                if(~axi_arvalid && ~M_AXI_RVALID && ~start_single_read  && read_complete) begin
                    start_single_read <= 1'b1;
                end
                else begin
                    start_single_read <= 1'b0; //Negate to generate a pulse
                end
            end
            else begin
                start_single_read <= 1'b0;
            end
        end
    end
    // 计数器 - 控制写地址 
    // 4'b0001 - IRQ_Set - 32'h0000_0000 - 
    // 4'b0010 - Source Address Set - 32'h0000_0018
    // 4'b0011 - Length_Set - 32'h0000_0028
    // 4'b0100 - Clear_IRQ - 32'h0000_0004
    always @(posedge M_AXI_ACLK)begin
        if(M_AXI_ARESETN == 0) begin
            write_state <= WR_IRQ;
        end
        else begin
            if (state == ST_CTRL_DMA) begin
                if(M_AXI_AWREADY && axi_awvalid) begin
                    write_state <= write_state + 1'b1;
                end
                else begin
                    write_state <= write_state;
                end
            end
            else begin
                write_state <= WR_IRQ;
            end
        end
    end
    // Write Addresses
    always @(posedge M_AXI_ACLK) begin                                                     
        if (M_AXI_ARESETN == 0) begin                                                 
            axi_awaddr <= 32'h0000_0000;                               
        end                                                            
        else if(state == ST_CTRL_DMA) begin
            case(write_state)
                WR_IRQ: axi_awaddr <= ADDR_IRQ;
                WR_DA: axi_awaddr <= ADDR_DA;
                WR_LENGTH: axi_awaddr <= ADDR_LENGTH;
                default: axi_awaddr <= 32'h0000_0000;
            endcase
        end
        else if(state == ST_CLEAR_IRQ) begin
            axi_awaddr <= ADDR_CLEAR;
        end
        else begin
                axi_awaddr <= 0; 
        end           
    end
    //start single write
    always @(posedge M_AXI_ACLK) begin
        if(M_AXI_ARESETN == 0) begin
            start_single_write <= 0;
        end
        else begin
            if (state == ST_CTRL_DMA || state == ST_CLEAR_IRQ) begin
                if (~axi_awvalid && ~axi_wvalid && ~M_AXI_BVALID && ~start_single_write) begin                                                           
                   start_single_write <= 1'b1;                                                                      
                end           
                else begin
                    start_single_write <= 0;
                end
            end
            else begin
                start_single_write <= 0;
            end
        end
    end
    // Write Data Control            
    always @(posedge M_AXI_ACLK) begin                                                     
        if (M_AXI_ARESETN == 0) begin                                                 
            axi_wdata <= C_M_START_DATA_VALUE;                  
        end                                                                     
        else if (state == ST_CTRL_DMA) begin       
            case(write_state)
                WR_IRQ: axi_wdata <= C_M_START_DATA_VALUE + VAL_IRQ;
                WR_DA: axi_wdata <= C_M_START_DATA_VALUE + dma_da_config;
                WR_LENGTH: axi_wdata <= C_M_START_DATA_VALUE + dma_length_config;
                default: axi_wdata <= C_M_START_DATA_VALUE;
            endcase                                                 
        end
        else if(state == ST_CLEAR_IRQ)begin
            axi_wdata <= VAL_CLEAR;
        end     
        else begin
            axi_wdata <= C_M_START_DATA_VALUE;                                          
        end  
    end
endmodule
