void
USBDeviceEnumHandler(tDCDInstance *pDevInstance)
{
    uint32_t ui32EPStatus, ui32DataSize;

    //
    // Get the end point 0 status.
    //
    ui32EPStatus = USBEndpointStatus(USB_BASE, USB_EP_0);

    switch(pDevInstance->iEP0State)
    {
        //
        // Handle the status state, this is a transitory state from
        // eUSBStateTx or eUSBStateRx back to eUSBStateIdle.
        //
        case eUSBStateStatus:
        {
            //
            // Just go back to the idle state.
            //
            pDevInstance->iEP0State = eUSBStateIdle;

            //
            // If there is a pending address change then set the address.
            //
            if(pDevInstance->ui32DevAddress & DEV_ADDR_PENDING)
            {
                //
                // Clear the pending address change and set the address.
                //
                pDevInstance->ui32DevAddress &= ~DEV_ADDR_PENDING;
                USBDevAddrSet(USB_BASE, pDevInstance->ui32DevAddress);
            }

            //
            // If a new packet is already pending, we need to read it
            // and handle whatever request it contains.
            //
            if(ui32EPStatus & USB_DEV_EP0_OUT_PKTRDY)
            {
                //
                // Process the newly arrived packet.
                //
                USBDReadAndDispatchRequest(0);
            }
            break;
        }

        //
        // In the IDLE state the code is waiting to receive data from the host.
        //
        case eUSBStateIdle:
        {
            //
            // Is there a packet waiting for us?
            //
            if(ui32EPStatus & USB_DEV_EP0_OUT_PKTRDY)
            {
                //
                // Yes - process it.
                //
                USBDReadAndDispatchRequest(0);
            }
            break;
        }

        //
        // Data is still being sent to the host so handle this in the
        // EP0StateTx() function.
        //
        case eUSBStateTx:
        {
            USBDEP0StateTx(0);
            break;
        }

        //
        // We are still in the middle of sending the configuration descriptor
        // so handle this in the EP0StateTxConfig() function.
        //
        case eUSBStateTxConfig:
        {
            USBDEP0StateTxConfig(0);
            break;
        }

        //
        // Handle the receive state for commands that are receiving data on
        // endpoint zero.
        //
        case eUSBStateRx:
        {
            //
            // Set the number of bytes to get out of this next packet.
            //
            if(pDevInstance->ui32EP0DataRemain > EP0_MAX_PACKET_SIZE)
            {
                //
                // Don't send more than EP0_MAX_PACKET_SIZE bytes.
                //
                ui32DataSize = EP0_MAX_PACKET_SIZE;
            }
            else
            {
                //
                // There was space so send the remaining bytes.
                //
                ui32DataSize = pDevInstance->ui32EP0DataRemain;
            }

            //
            // Get the data from the USB controller end point 0.
            //
            USBEndpointDataGet(USB_BASE, USB_EP_0,
                                   pDevInstance->pui8EP0Data, &ui32DataSize);

            //
            // If there we not more that EP0_MAX_PACKET_SIZE or more bytes
            // remaining then this transfer is complete.  If there were exactly
            // EP0_MAX_PACKET_SIZE remaining then there still needs to be
            // null packet sent before this is complete.
            //
            if(pDevInstance->ui32EP0DataRemain < EP0_MAX_PACKET_SIZE)
            {
                //
                // Return to the idle state.
                //
                pDevInstance->iEP0State =  eUSBStateStatus;

                //
                // If there is a receive callback then call it.
                //
                if((g_ppsDevInfo[0]->psCallbacks->pfnDataReceived) &&
                   (pDevInstance->ui32OUTDataSize != 0))
                {
                    //
                    // Call the custom receive handler to handle the data
                    // that was received.
                    //
                    g_ppsDevInfo[0]->psCallbacks->pfnDataReceived(
                                                g_psDCDInst[0].pvCBData,
                                                pDevInstance->ui32OUTDataSize);

                    //
                    // Indicate that there is no longer any data being waited
                    // on.
                    //
                    pDevInstance->ui32OUTDataSize = 0;
                }
                //
                // Need to ACK the data on end point 0 in this case and set the
                // data end as this is the last of the data.
                //
                USBDevEndpointDataAck(USB_BASE, USB_EP_0, true);

            }
            else
            {
                //
                // Need to ACK the data on end point 0 in this case
                // without setting data end because more data is coming.
                //
                USBDevEndpointDataAck(USB_BASE, USB_EP_0, false);
            }

            //
            // Advance the pointer.
            //
            pDevInstance->pui8EP0Data += ui32DataSize;

            //
            // Decrement the number of bytes that are being waited on.
            //
            pDevInstance->ui32EP0DataRemain -= ui32DataSize;

            break;
        }
        //
        // The device stalled endpoint zero so check if the stall needs to be
        // cleared once it has been successfully sent.
        //
        case eUSBStateStall:
        {
            //
            // If we sent a stall then acknowledge this interrupt.
            //
            if(ui32EPStatus & USB_DEV_EP0_SENT_STALL)
            {
                //
                // Clear the Setup End condition.
                //
                USBDevEndpointStatusClear(USB_BASE, USB_EP_0,
                                              USB_DEV_EP0_SENT_STALL);

                //
                // Reset the global end point 0 state to IDLE.
                //
                pDevInstance->iEP0State = eUSBStateIdle;

            }
            break;
        }
        //
        // Halt on an unknown state, but only in DEBUG mode builds.
        //
        default:
        {
            ASSERT(0);
            break;
        }
    }
}
