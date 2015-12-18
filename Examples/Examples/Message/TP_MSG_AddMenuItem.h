    /* Message */
    TP_AddMenuItem(0,
                   MENU_MAIN_MESSAGE,
                   (uint8*)TEXT_MESSAGE,
                   MenuItemPic);

    /* Message->New */
    TP_AddMenuItem(MENU_MAIN_MESSAGE,
                   MENU_MESSAGE_NEW,
                   (uint8*)TEXT_NEW_SMS,
                   MenuItemPic);
    /* Message->InBox */
    TP_AddMenuItem(MENU_MAIN_MESSAGE,
                   MENU_MESSAGE_INBOX,
                   (uint8*)TEXT_INBOX,
                   MenuItemPic);
    /* Message->OutBox */
    TP_AddMenuItem(MENU_MAIN_MESSAGE,
                   MENU_MESSAGE_OUTBOX,
                   (uint8*)TEXT_OUTBOX,
                   MenuItemPic);
    /* Message->SentBox */
    TP_AddMenuItem(MENU_MAIN_MESSAGE,
                   MENU_MESSAGE_SENT,
                   (uint8*)TEXT_SENTBOX,
                   MenuItemPic);
    /* Message->Draft */
    TP_AddMenuItem(MENU_MAIN_MESSAGE,
                   MENU_MESSAGE_DRAFT,
                   (uint8*)TEXT_DRAFTBOX,
                   MenuItemPic);
    /* Message->Msg Settings */
    TP_AddMenuItem(MENU_MAIN_MESSAGE,
                   MENU_MESSAGE_MSGSETTINGS,
                   (uint8*)TEXT_MSG_SETTINGS,
                   MenuItemPic);
    /* Message->Clear Msg*/
    TP_AddMenuItem(MENU_MAIN_MESSAGE,
                   MENU_MESSAGE_CLEAR,
                   (uint8*)TEXT_CLEAE_ALL,
                   MenuItemPic);


    /* Message->MSG Center*/
    TP_AddMenuItem(MENU_MESSAGE_MSGSETTINGS,
                   MENU_MSGSETTINGS_MSGCENTRE,
                   (uint8*)TEXT_MSG_CENTRE2,
                   MenuItemPic);
    /* Message->Msg Settings->Save SMS to*/
    TP_AddMenuItem(MENU_MESSAGE_MSGSETTINGS,
                   MENU_MSGSETTINGS_SAVESMSTO,
                   (uint8*)TEXT_SAVE_SMS_TO,
                   MenuItemPic);


