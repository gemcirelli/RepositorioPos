    /* Call Record */
    TP_AddMenuItem(0,
                   MENU_MAIN_CALLRECORD,
                   (uint8*)TEXT_CALLRECORD,
                   MenuItemPic);
	/* Call Record->Missed Calls */
    TP_AddMenuItem(MENU_MAIN_CALLRECORD,
                   MENU_CALLRECORD_MISSEDCALLS,
                   (uint8*)TEXT_MISSED_CALLS,
                   MenuItemPic);
    /* Call Record->Received Calls */
    TP_AddMenuItem(MENU_MAIN_CALLRECORD,
                   MENU_CALLRECORD_RECEIVEDCALLS,
                   (uint8*)TEXT_RECEIVED_CALLS,
                   MenuItemPic);
    /* Call Record->Dialed Calls */
    TP_AddMenuItem(MENU_MAIN_CALLRECORD,
                   MENU_CALLRECORD_DIALLEDCALLS,
                   (uint8*)TEXT_DAILED_CALLS,
                   MenuItemPic);

    /* Call Record->Call Info */
    TP_AddMenuItem(MENU_MAIN_CALLRECORD,
                   MENU_CALLRECORD_CLEARALL,
                   (uint8*)TEXT_CLEAR_ALL_CALLS,
                   MenuItemPic);
