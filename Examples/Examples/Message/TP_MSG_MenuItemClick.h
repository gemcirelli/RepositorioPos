
    //MESSAGE
    case MENU_MESSAGE_NEW:
        TP_MsgBoxNew(0,0,0,TRUE);
        break;
    case MENU_MESSAGE_INBOX:
        TP_MsgBoxShow(SMS_BOX_INBOX);
        break;
    case MENU_MESSAGE_OUTBOX:
        TP_MsgBoxShow(SMS_BOX_OUTBOX);
        break;
    case MENU_MESSAGE_SENT:
        TP_MsgBoxShow(SMS_BOX_SENTBOX);
        break;
    case MENU_MESSAGE_DRAFT:
        TP_MsgBoxShow(SMS_BOX_DRAFT);
        break;
    case MENU_MESSAGE_CLEAR:
        TP_MsgMenu_Clear();
        break;
    case MENU_MSGSETTINGS_MSGCENTRE:
        TP_MsgMenu_MSGCentreSet();
        break;
    case MENU_MSGSETTINGS_SAVESMSTO:
        TP_MsgMenu_SaveTo();
        break;

