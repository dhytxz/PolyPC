
`timescale 1 ns / 1 ps

    module hapara_axis_barrier_v1_0 #
    (
        // Users to add parameters here
        parameter     integer NUM_SLAVES    = 2,
        parameter     integer DATA_WIDTH    = 32
    )
    (
        // Ports of Axi Master Bus Interface M00_AXIS
        input wire  m00_axis_aclk,
        input wire  m00_axis_aresetn,
        input wire  m00_axis_tvalid,
        input wire [DATA_WIDTH-1 : 0] m00_axis_tdata,
        output wire  m00_axis_tready,

        // Ports of Axi Master Bus Interface M01_AXIS
        input wire  m01_axis_aclk,
        input wire  m01_axis_aresetn,
        input wire  m01_axis_tvalid,
        input wire [DATA_WIDTH-1 : 0] m01_axis_tdata,
        output wire  m01_axis_tready,

        // Ports of Axi Master Bus Interface M02_AXIS
        input wire  m02_axis_aclk,
        input wire  m02_axis_aresetn,
        input wire  m02_axis_tvalid,
        input wire [DATA_WIDTH-1 : 0] m02_axis_tdata,
        output wire  m02_axis_tready,

        // Ports of Axi Master Bus Interface M03_AXIS
        input wire  m03_axis_aclk,
        input wire  m03_axis_aresetn,
        input wire  m03_axis_tvalid,
        input wire [DATA_WIDTH-1 : 0] m03_axis_tdata,
        output wire  m03_axis_tready,

        // Ports of Axi Master Bus Interface M04_AXIS
        input wire  m04_axis_aclk,
        input wire  m04_axis_aresetn,
        input wire  m04_axis_tvalid,
        input wire [DATA_WIDTH-1 : 0] m04_axis_tdata,
        output wire  m04_axis_tready,

        // Ports of Axi Master Bus Interface M05_AXIS
        input wire  m05_axis_aclk,
        input wire  m05_axis_aresetn,
        input wire  m05_axis_tvalid,
        input wire [DATA_WIDTH-1 : 0] m05_axis_tdata,
        output wire  m05_axis_tready,

        // Ports of Axi Master Bus Interface M06_AXIS
        input wire  m06_axis_aclk,
        input wire  m06_axis_aresetn,
        input wire  m06_axis_tvalid,
        input wire [DATA_WIDTH-1 : 0] m06_axis_tdata,
        output wire  m06_axis_tready,

        // Ports of Axi Master Bus Interface M07_AXIS
        input wire  m07_axis_aclk,
        input wire  m07_axis_aresetn,
        input wire  m07_axis_tvalid,
        input wire [DATA_WIDTH-1 : 0] m07_axis_tdata,
        output wire  m07_axis_tready,

        // Ports of Axi Master Bus Interface M08_AXIS
        input wire  m08_axis_aclk,
        input wire  m08_axis_aresetn,
        input wire  m08_axis_tvalid,
        input wire [DATA_WIDTH-1 : 0] m08_axis_tdata,
        output wire  m08_axis_tready
    );
    

    // Add user logic here
    // assumption NUM_SLAVES is alway greater than 0
    // clk and reset can use the first master port
    wire notRelease;
    wire syn;


    generate if (NUM_SLAVES == 1) 
    begin: NUM_SLAVES_1
        assign notRelease = m00_axis_tvalid;
        assign syn = m00_axis_tvalid;
        assign m00_axis_tready = curr_state == locking;
    end
    endgenerate

    generate if (NUM_SLAVES == 2)
    begin: NUM_SLAVES_2
        assign notRelease = m00_axis_tvalid | m01_axis_tvalid;
        assign syn = m00_axis_tvalid & m01_axis_tvalid;
        assign m00_axis_tready = curr_state == locking;
        assign m01_axis_tready = curr_state == locking;
    end
    endgenerate
   
    generate if (NUM_SLAVES == 3)
    begin: NUM_SLAVES_3
        assign notRelease = m00_axis_tvalid | m01_axis_tvalid | m02_axis_tvalid;
        assign syn = m00_axis_tvalid & m01_axis_tvalid & m02_axis_tvalid;
        assign m00_axis_tready = curr_state == locking;
        assign m01_axis_tready = curr_state == locking;
        assign m02_axis_tready = curr_state == locking;
    end
    endgenerate

    generate if (NUM_SLAVES == 4)
    begin: NUM_SLAVES_4
        assign notRelease = m00_axis_tvalid | m01_axis_tvalid | 
                            m02_axis_tvalid | m03_axis_tvalid;
        assign syn = m00_axis_tvalid & m01_axis_tvalid & m02_axis_tvalid &
                     m03_axis_tvalid;
        assign m00_axis_tready = curr_state == locking;
        assign m01_axis_tready = curr_state == locking;
        assign m02_axis_tready = curr_state == locking;
        assign m03_axis_tready = curr_state == locking;
    end
    endgenerate

    generate if (NUM_SLAVES == 5)
    begin: NUM_SLAVES_5
        assign notRelease = m00_axis_tvalid | m01_axis_tvalid |
                            m02_axis_tvalid | m03_axis_tvalid |
                            m04_axis_tvalid;
        assign syn = m00_axis_tvalid & m01_axis_tvalid & m02_axis_tvalid &
                     m03_axis_tvalid & m04_axis_tvalid;
        assign m00_axis_tready = curr_state == locking;
        assign m01_axis_tready = curr_state == locking;
        assign m02_axis_tready = curr_state == locking;
        assign m03_axis_tready = curr_state == locking;
        assign m04_axis_tready = curr_state == locking;
    end
    endgenerate

    generate if (NUM_SLAVES == 6)
    begin: NUM_SLAVES_6
        assign notRelease = m00_axis_tvalid | m01_axis_tvalid |
                            m02_axis_tvalid | m03_axis_tvalid |
                            m04_axis_tvalid | m05_axis_tvalid;
        assign syn = m00_axis_tvalid & m01_axis_tvalid & m02_axis_tvalid &
                     m03_axis_tvalid & m04_axis_tvalid & m05_axis_tvalid;
        assign m00_axis_tready = curr_state == locking;
        assign m01_axis_tready = curr_state == locking;
        assign m02_axis_tready = curr_state == locking;
        assign m03_axis_tready = curr_state == locking;
        assign m04_axis_tready = curr_state == locking;
        assign m05_axis_tready = curr_state == locking;
    end
    endgenerate

    generate if (NUM_SLAVES == 7) 
    begin: NUM_SLAVES_7
        assign notRelease = m00_axis_tvalid | m01_axis_tvalid |
                            m02_axis_tvalid | m03_axis_tvalid |
                            m04_axis_tvalid | m05_axis_tvalid |
                            m06_axis_tvalid;
        assign syn = m00_axis_tvalid & m01_axis_tvalid & m02_axis_tvalid &
                     m03_axis_tvalid & m04_axis_tvalid & m05_axis_tvalid &
                     m06_axis_tvalid;
        assign m00_axis_tready = curr_state == locking;
        assign m01_axis_tready = curr_state == locking;
        assign m02_axis_tready = curr_state == locking;
        assign m03_axis_tready = curr_state == locking;
        assign m04_axis_tready = curr_state == locking;
        assign m05_axis_tready = curr_state == locking;
        assign m06_axis_tready = curr_state == locking;
    end
    endgenerate

    generate if (NUM_SLAVES == 8)
    begin: NUM_SLAVES_8
        assign notRelease = m00_axis_tvalid | m01_axis_tvalid |
                            m02_axis_tvalid | m03_axis_tvalid |
                            m04_axis_tvalid | m05_axis_tvalid |
                            m06_axis_tvalid | m07_axis_tvalid;
        assign syn = m00_axis_tvalid & m01_axis_tvalid & m02_axis_tvalid &
                     m03_axis_tvalid & m04_axis_tvalid & m05_axis_tvalid &
                     m06_axis_tvalid & m07_axis_tvalid;
        assign m00_axis_tready = curr_state == locking;
        assign m01_axis_tready = curr_state == locking;
        assign m02_axis_tready = curr_state == locking;
        assign m03_axis_tready = curr_state == locking;
        assign m04_axis_tready = curr_state == locking;
        assign m05_axis_tready = curr_state == locking;
        assign m06_axis_tready = curr_state == locking;
        assign m07_axis_tready = curr_state == locking;
    end
    endgenerate

    generate if (NUM_SLAVES == 9)
    begin: NUM_SLAVES_9
        assign notRelease = m00_axis_tvalid | m01_axis_tvalid |
                            m02_axis_tvalid | m03_axis_tvalid |
                            m04_axis_tvalid | m05_axis_tvalid |
                            m06_axis_tvalid | m07_axis_tvalid |
                            m08_axis_tvalid;
        assign syn = m00_axis_tvalid & m01_axis_tvalid & m02_axis_tvalid &
                     m03_axis_tvalid & m04_axis_tvalid & m05_axis_tvalid &
                     m06_axis_tvalid & m07_axis_tvalid & m08_axis_tvalid;
        assign m00_axis_tready = curr_state == locking;
        assign m01_axis_tready = curr_state == locking;
        assign m02_axis_tready = curr_state == locking;
        assign m03_axis_tready = curr_state == locking;
        assign m04_axis_tready = curr_state == locking;
        assign m05_axis_tready = curr_state == locking;
        assign m06_axis_tready = curr_state == locking;
        assign m07_axis_tready = curr_state == locking;
        assign m08_axis_tready = curr_state == locking;
    end
    endgenerate

    localparam waiting = 2'b01;
    localparam locking = 2'b10;

    reg [1 : 0] curr_state;
    reg [1 : 0] next_state;

    always @(posedge m00_axis_aclk or negedge m00_axis_aresetn) begin
        if (!m00_axis_aresetn) begin
            // reset
            curr_state <= waiting;
        end
        else begin
            curr_state <= next_state;
        end
    end

    always @(curr_state or syn or notRelease) begin
        case(curr_state)
            waiting:
                if (syn) begin
                    next_state = locking;
                end 
                else begin
                    next_state = waiting;
                end
            locking:
                if (!notRelease) begin
                    next_state = waiting;
                end
                else begin
                    next_state = locking;
                end
            default:
                next_state = 2'bxx;    
        endcase
    end

    endmodule
