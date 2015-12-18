/*******************************************************************************
 * ÎÄ¼þÎ»ÖÃ: TP_ParamsBackUp.c
 * ÎÄ¼þËµÃ÷: ²ÎÊý¹ÜÀiÆ÷Êý¾Ý±¸·ÝAPIµÄÊµÏÖ
 * ´´½¨ÈÕÆu: 2012Äê03ÔÂ16ÈÕ
 * ´´½¨ÈË	     : ÎâÒ«Èª 
 * ÐÞ¸ÄÈÕÆu:
 * ÐÞ¸ÄËµÃ÷:
 * ÐÞ¸ÄÈÕÆu:
 * ±¸×¢	     :
	1. ½«²ÎÊý¹ÜÀiÆ÷Äu²¿ËùoÐÊý¾ÝÒÔÊý¾Ýa÷·½Ê½½»oeeÏ²ãÒµÎn±¸·Ý
	2. eÏ²ãÒµÎnÊäÈëiêÕûµÄ²ÎÊýÊý¾Ýa÷£¬²ÎÊý¹ÜÀiÆ÷i¨¹ý·ÖÎöa÷Êý¾Ý½øÐÐ¼oÔØ²ÎÊý¡£
******************************************************************************/
#include "TPTypedef.h"
#include "TP_ParamsBackUp.h"
#include "TP_ParamsManager.h"
#include "TP_Public.h"


extern int32 TP_ParamsItems_ToBuffer(uint8* dataBuffer, uint32 length);
extern int32 TP_LoadParams_FromBuffer(const uint8* dataStream, uint32 length);

/**
 * º¯ Êý TP_BakeUp_ParamsItem() ¹¦ÄÜÃèÊö£º½«²ÎÊý¹ÜÀiÆ÷Äu²¿ËùoÐ²ÎÊýÒÔa÷·½Ê½Êä³ö
 * @param dataBuffer, eÏ²ãÒµÎnÌa¹©µÄÐ´»º³åÇø;
 * @param length, eÏ²ãÒµÎnÌa¹©µÄÐ´»º³å³¤¶È;
 * @return >0 Êý¾Ý³¤¶È
	<=0,»º³åÇø²»×ã
 * @author  ÎâÒ«Èª
 * @date 20120315
 * @note 
 */
int32 TP_BakeUp_ParamsItem(uint8* dataBuffer, uint32 length)
{
	return TP_ParamsItems_ToBuffer(dataBuffer, length);
}

/**
 * º¯ Êý TP_ParamsItem_LoadFrom_BakeUp() ¹¦ÄÜÃèÊö£º½«²ÎÊý¹ÜÀiÆ÷Äu²¿ËùoÐ²ÎÊýÒÔa÷·½Ê½Êä³ö
 * @param dataStream, eÏ²ãÒµÎnÌa¹©µÄ²ÎÊýÊý¾Ýa÷;
 * @param length, eÏ²ãÒµÎnÌa¹©µÄ²ÎÊýÊý¾Ýa÷µÄÊý¾Ý³¤¶È;
 * @return >0 ¼oÔØµÄ²ÎÊý¸öÊý
	<=0,Êý¾ÝÒì³£
 * @author  ÎâÒ«Èª
 * @date 20120315
 * @note 
 	½ö×÷iâ²¿a÷¼oÔØ,²»±£´æµ½flashÎÄ¼þ,ÈôÒª±£´æÔòÐè½o×Åµ÷oÃTP_Save_AllParamsItem_To_File();
 */
int32 TP_ParamsItem_LoadFrom_BakeUp(const uint8* dataStream, uint32 length)
{
	return TP_LoadParams_FromBuffer(dataStream, length);
}

