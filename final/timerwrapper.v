// timerIntWrapper.v
// timerWrap.v
module timerWrapper(
					PCLK,
					PENABLE,
					PSEL,
					PRESETN,
					PWRITE,
					PREADY,
					PSLVERR,
					PADDR,
					PWDATA,
					PRDATA,
                    FABINT,
);


// APB Bus Interface
input PCLK,PENABLE, PSEL, PRESETN, PWRITE;
input  [31:0] PWDATA;
input  [7:0] PADDR;
output [31:0] PRDATA;
output PREADY, PSLVERR;
output FABINT;

 
assign BUS_WRITE_EN = (PENABLE && PWRITE && PSEL);
assign BUS_READ_EN = (!PWRITE && PSEL); //Data is ready during first cycle to make it availble on the bus when PENABLE is asserted

assign PREADY = 1'b1;
assign PSLVERR = 1'b0;

timer timer_0(	.pclk(PCLK),
			    .nreset(PRESETN), 
			    .bus_write_en(BUS_WRITE_EN),
			    .bus_read_en(BUS_READ_EN),
			    .bus_addr(PADDR),
			    .bus_write_data(PWDATA),
			    .bus_read_data(PRDATA),
                .fabint(FABINT)
			);


endmodule

module Button_Debouncer(
input clk,
input PB_in, // button input
output reg PB_out // debounced output
);
reg [15:0] PB_cnt; // 16-bit counter
reg [1:0] sync; // used as two flip-flops to synchronize
// button to the clk domain.
// First use two flipflops to synchronize the PB signal the "clk" clock domain
always @(posedge clk)
sync[1:0] <= {sync[0],~PB_in};
wire PB_idle = (PB_out==sync[1]); // See if we have a new input state for PB
wire PB_cnt_max = &PB_cnt; // true when all bits of PB_cnt are 1's
// using & in this way is a
// "reduction operation"
// When the push-button is pushed or released, we increment the counter.
// The counter has to be maxed out before we decide that the push-button
// state has changed
always @(posedge clk) begin
if(PB_idle)
PB_cnt<= 16'd0; // input and output are the same so clear counter
else begin
PB_cnt<= PB_cnt + 16'd1; // input different than output, count
if(PB_cnt_max)
PB_out<= ~PB_out; // if the counter is maxed out,
 // change PB output
end
end
endmodule
