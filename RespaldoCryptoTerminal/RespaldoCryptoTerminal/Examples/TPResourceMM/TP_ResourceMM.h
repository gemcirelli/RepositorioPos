#ifndef TP_RESOURCE_MM/*Include Guard Start*/
#define TP_RESOURCE_MM

/******************************************************************************
* Nested Include Headers
*******************************************************************************/
#include "TPResIndex.h"

/******************************************************************************
* Type Definitions
*******************************************************************************/
/**
 * Enumeration of supported language
 */
typedef enum MultiLanguageTag
{
    LANGUAGE_NULL = 0,

    LANGUAGE_ENGLISH,
    LANGUAGE_CHINESE,
    LANGUAGE_PORTUGUESE,
    LANGUAGE_SPANISH,
    LANGUAGE_DENISH,

    LANGUAGE_MAX_NUM
}E_MultiLanguage;

typedef enum
{
    STRINGID_MENU = 0,
    STRINGID_CONFIRM,
    STRINGID_CANCEL,
    STRINGID_EXIT,
    STRINGID_BACK,
    STRINGID_LANGUAGE,
    STRINGID_SELECT_EXIT,
    STRINGID_MAX
}E_StringID;

/******************************************************************************
* Global Function Prototypes
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
extern const uint8*
TPRes_GetStringByID(E_StringID stringID);

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
extern uint8*
TPRes_GetImageByID(uint32 imageID);

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
extern uint8*
TPRes_GetAnimationByID(uint32 animationID);

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
void TP_SetAppLanguage(E_MultiLanguage language);

E_MultiLanguage TP_GetAppLanguage(void);

#endif/*Include Guard End*/
/*End Of File*/
