#include "VFinka.h"
#include "VFinka_Finka.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include "../../../../VexRiscv.pinned/src/test/cpp/common/framework.h"
#include "../../../../VexRiscv.pinned/src/test/cpp/common/jtag.h"
#include "../../../../VexRiscv.pinned/src/test/cpp/common/uart.h"

#include "tuntap.h"

//#define TRACE_INSTRUCTION 1
//#define TRACE_REG 1
//#include "vexriscvtracer.h" 

class FinkaWorkspace : public Workspace<VFinka>{
public:
	FinkaWorkspace() : Workspace("Finka"){
		// for synchronous reset, ensure reset de-assert delay is more than clock delay
		uint64_t baudRate = 115200;
		uint64_t clockStartDelay = 20000;
		uint64_t resetDeassertDelay = 2 * clockStartDelay;

		/* time processes; clocks */
		int axiPeriod = 1.0e12 / 250.0e6;
		ClockDomain *clk = new ClockDomain(&top->clk, NULL, axiPeriod, clockStartDelay);
		int packetPeriod = 1.0e12 / 322.0e6;
#if 0
		ClockDomain *rx_clk = new ClockDomain(&top->rx_clk, NULL, packetPeriod, clockStartDelay);
		ClockDomain *tx_clk = new ClockDomain(&top->tx_clk, NULL, packetPeriod, clockStartDelay);
#endif
		/* time processes; *synchronous* resets, de-asserting with clocks enabled */
		AsyncReset *rst    = new AsyncReset(&top->rst,    resetDeassertDelay);
#if 0
		AsyncReset *tx_rst = new AsyncReset(&top->tx_rst, resetDeassertDelay);
		AsyncReset *rx_rst = new AsyncReset(&top->rx_rst, resetDeassertDelay);
#endif

		top->uart_txd = 1;

		/* UART */
		UartRx *uartRx = new UartRx(&top->uart_txd, 1.0e12 / baudRate);
		UartTx *uartTx = new UartTx(&top->uart_rxd, 1.0e12 / baudRate);

		/* clock synchronous processes */
	 	TunTapRx *tunTapRx = new TunTapRx(top->s_axis_rx_tdata, &top->s_axis_rx_tkeep, &top->s_axis_rx_tuser, &top->s_axis_rx_tlast, &top->s_axis_rx_tvalid, &top->s_axis_rx_tready);
		/*rx_*/clk->add(tunTapRx);

	 	TunTapTx *tunTapTx = new TunTapTx(top->m_axis_tx_tdata, &top->m_axis_tx_tkeep, &top->m_axis_tx_tuser, &top->m_axis_tx_tlast, &top->m_axis_tx_tvalid, &top->m_axis_tx_tready);
		/*tx_*/clk->add(tunTapTx);

		/* accept incoming AXIS frames from DUT */
		top->m_axis_tx_tready = 1;

		timeProcesses.push_back(clk);
#if 0
		timeProcesses.push_back(rx_clk);
		timeProcesses.push_back(tx_clk);
#endif
		timeProcesses.push_back(rst);
#if 0
		timeProcesses.push_back(rx_rst);
		timeProcesses.push_back(tx_rst);
#endif
		timeProcesses.push_back(uartRx);
		timeProcesses.push_back(uartTx);

		/* JTAG over TCP debug server */
		Jtag *jtag = new Jtag(&top->jtag_tms, &top->jtag_tdi, &top->jtag_tdo, &top->jtag_tck, axiPeriod * 4);
		timeProcesses.push_back(jtag);

		#ifdef TRACE
		//speedFactor = 10e-3;
		//cout << "Simulation capped to " << speedFactor << " of real time"<< endl;
		#endif
	}
};

struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    uint64_t diffInNanos = end_time.tv_sec*1e9 + end_time.tv_nsec -  start_time.tv_sec*1e9 - start_time.tv_nsec;
    return diffInNanos;
}

int main(int argc, char **argv, char **env) {

	Verilated::randReset(2);
	Verilated::commandArgs(argc, argv);

	printf("BOOT\n");
	timespec startedAt = timer_start();

	FinkaWorkspace().run(1e9);

	uint64_t duration = timer_end(startedAt);
	cout << endl << "****************************************************************" << endl;

	exit(0);
}
