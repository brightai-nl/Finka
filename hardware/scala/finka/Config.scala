package finka

import spinal.core._
import spinal.core.sim._

object Config {
  def spinal = SpinalConfig(
    targetDirectory = "build/rtl",
    mode = VHDL,
    // synchronous resets for Xilinx devices
    defaultConfigForClockDomains = ClockDomainConfig(
      resetKind = SYNC
    ),
    // change (un)signed I/Os at toplevel into std_logic_vector
    onlyStdLogicVectorAtTopLevelIo = true
  )
}
