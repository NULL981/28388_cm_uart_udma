# TechnologicalAnalysis

> UART loopback example with uDMA

## What is lookback mode in UART?

参考文件：TMS320F2838x Microcontrollers TRM (Rev. B).pdf   4887 / 4991

> The UART can be placed into an internal loopback mode for diagnostic or debug work by setting the LBE
> bit in the UARTCTL register (see ). In loopback mode, data transmitted on the UnTx output is received on
> the UnRx input. Note that the LBE bit should be set before the UART is enabled.  
>
> 我的理解：
>
> ![](.\resource\pics\01_UART_Lookback_mode.PNG)



## What uDMA transfer type in this example?

```c
UDMA_setChannelTransferParams(UDMA_BASE, (UDMA_CHANNEL_UART0_RX | UDMA_PRI_SELECT),
                                 (void *)(UART0_BASE + UART_O_DR), RxDma, UDMA_MODE_BASIC,
                                  size);
```

NOTE: 从上述代码可以看出，本例程中使用的uDMA的传输模式为  UDMA_MODE_BASIC



## uDMA how to work with he UART

## block



![](.\resource\pics\02_UART_uDMA_request.PNG)





