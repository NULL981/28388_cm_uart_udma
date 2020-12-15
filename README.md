# 28388_cm_uart_udma
This example showcases how to use UDMA with UART to transfer and receive data
//! This configures the UART in loopback mode and sends and receives data for
//! infinite time.
//! cm_common_config_c28x example needs to be run on C28x1 before running this
//! example on the CM core
//!
//! \b Configuration:
//!  -  Find correct COM port
//!  -  Bits per second = 115200
//!  -  Data Bits = 8
//!  -  Parity = None
//!  -  Stop Bits = 1
//!  -  Hardware Control = None
//!
//! \b External \b Connections \n
//!  - None
//!
//! \b Watch \b Variables \n
//!  - \b TxData   - Data transmitted
//!  - \b RxData   - Data received
//!  - \b errCount - Error count
