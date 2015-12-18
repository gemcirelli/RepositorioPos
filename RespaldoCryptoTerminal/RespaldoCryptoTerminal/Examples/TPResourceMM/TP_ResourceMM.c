/******************************************************************************
* Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPFonts.h>
#include <TPLCD.h>

#include "TPImageRes.h"
#include "TPAnimationRes.h"
#include "TPEnglishRes.h"
#include "TPChineseRes.h"
#if 0
#include "TPPortugueseRes.h"
#include "TPSpanishRes.h"
#include "TPDenishRes.h"
#endif
#include "TP_ResourceMM.h"

/******************************************************************************
* Static Variables
*******************************************************************************/
static E_MultiLanguage AppLanguage = LANGUAGE_ENGLISH;

/******************************************************************************
* Global Variables
*******************************************************************************/
//ST_FONT fontLib[32];

/******************************************************************************
* Global Functions
*******************************************************************************/
/**
 * Get String Resource by resource ID
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param stringID String resource ID in TP_ResIndex.h
 *
 * @return PNULL, no this resource or AppLanguage is not correct,
 *         otherwise pointer to resource
 *
 * @date 2012-3-3
 *
 * @note string resources are encoded in unicode
 */
const uint8* TPRes_GetStringByID(E_StringID stringID)
{
    if(stringID >= STRINGID_MAX)
    {
        return PNULL;
    }
    switch (AppLanguage)
    {
        case LANGUAGE_ENGLISH:
            return TPEnglish[stringID];
        case LANGUAGE_CHINESE:
            return TPChinese[stringID];
        #if 0
        case LANGUAGE_PORTUGUESE:
            return TPPortuguese[stringID];
        case LANGUAGE_SPANISH:
            return TPSpanish[stringID];
        case LANGUAGE_DENISH:
            return TPDenish[stringID];
        #endif

        default:
            return PNULL;
    }
}


/**
 * Get Image Resource by resource ID
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param imageID Image resource ID in TP_ResIndex.h
 *
 * @return PNULL, no this resource, otherwise pointer to resource
 *
 * @date 2012-3-3
 */
uint8* TPRes_GetImageByID(uint32 imageID)
{
    return (uint8*)&TPImageRes[imageID];
}


/**
 * Get Animation Resource by resource ID
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param animationID Animation resource ID in TP_ResIndex.h
 *
 * @return PNULL, no this resource, otherwise pointer to resource
 *
 * @date 2012-3-3
 */
uint8* TPRes_GetAnimationByID(uint32 animationID)
{
    return (uint8*)&TPAnimalRes[animationID];
}

/**
 * Set Application Use Language
 *
 * @author zhangl
 *
 * @version 1.0
 *
 * @param language Application-Use Language, see {@link #E_MultiLanguage}
 *
 * @date 2012-3-3
 *
 * @note Must call this function before call any display related functions or
 *       string resource related functions
 */
void TP_SetAppLanguage(E_MultiLanguage language)
{
    AppLanguage = language;
}

E_MultiLanguage TP_GetAppLanguage()
{
    return AppLanguage;
}

/*End Of File*/
