
	//CallRecod
	case MENU_CALLRECORD_MISSEDCALLS :
        ShowCallRecordList(2);
        break;
    case MENU_CALLRECORD_RECEIVEDCALLS :
        ShowCallRecordList(1);
        break;
    case MENU_CALLRECORD_DIALLEDCALLS :
        ShowCallRecordList(0);
        break;
    case MENU_CALLRECORD_CLEARALL :
        TP_CallRecordMenu_Clear();
        break;



