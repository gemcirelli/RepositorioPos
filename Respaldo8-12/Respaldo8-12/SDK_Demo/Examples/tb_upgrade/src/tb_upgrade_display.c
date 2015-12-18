/******************************************************************************
* Include Headers
*******************************************************************************/
#include <TPTypedef.h>
#include <TPBase.h>
#include <TPFonts.h>
#include <TPKeypad.h>
#include <TPLCD.h>
#include <TPUpgrade.h>

#include "TP_DemoDefine.h"
#include "TP_ListBox.h"
#include "TP_DisplayAPI.h"
#include "tb_upgrade.h"

#include "./tb_upgrade_display.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

/******************************************************************************
* Manifest Constants
*******************************************************************************/
//#define ALIGN_CENTER 0x00
//#define ALIGN_LEFT   0x01
//#define ALIGN_RIGHT  0x02

#define CHAR_WIDTH  (6)
#define CHAR_HEIGHT (12)

/******************************************************************************
* Constants
*******************************************************************************/
static const char QueryDoStartNewOne[] =
{
    "Ultimo proceso de actualizacion no se completo\nPulse Si para comenzar uno nuevo\nPulse No para continuar el ultimo"
};

static const char QueryDoUseBP[] =
{
    "Chequeado Punto de Control\nComenzar Desde Ese Punto?"
};

static const char QueryDoResume[] =
{
    "Ultimo proceso de actualizacion no finalizado\nContinuarlo?"
};

static const char QueryDoRestart[] =
{
    "Reiniciar la actualizacion ahora?"
};

static const char QueryDoClearData[]  =
{
    "Le gustaria cancelar permanentemente?"
};

static const char QueryDoCancelDownload[] =
{
    "Presione \"Ok\" para detener o \"Cancelar\" para continuar"
};

/******************************************************************************
* Private Variables
*******************************************************************************/
static int32 ProgressInd = 0;

/******************************************************************************
* Private Functions
*******************************************************************************/
static void displayLine(const char* pLine, uint8 style, uint8 yStart)
{
    uint16 lineWidth;
    uint16 areaWidth;
    uint8  xPos;

    lineWidth = strlen(pLine) * CHAR_WIDTH;
    areaWidth = 114 - 13 + 1;
    xPos      = 13;
    if (style == ALIGN_MIDDLE)
    {
        xPos += ((areaWidth - lineWidth) / 2);
    }
    else if (style == ALIGN_RIGHT)
    {
        xPos += (areaWidth - lineWidth);
    }

    TP_ScrGotoxyEx(xPos, yStart);
    TP_LcdPrintf((uint8*)"%s\x0", pLine);
}


static void displayButton(void)
{
    TP_ScrGotoxyEx(13, 49);
    TP_LcdPrintf((uint8*)"Yes");

    TP_ScrGotoxyEx(103, 49);
    TP_LcdPrintf((uint8*)"No");
}


static void addDots(char* pLine)
{
    uint8 nDot;

    nDot = 0;
    while (nDot++ < ProgressInd)
    {
        strcat(pLine, ".");
    }
}


static void initDisplayPage(void)
{
    TP_ScrAttrSet(0);
    TP_ScrSpaceSet(0, 0);
    TP_SetDisplayArea(0, 0, 127, 63);

    TP_LcdFreeze();

    TP_ScrDrawRect(10, 2, 117, 63);
    TP_ScrClrRect(11, 3, 116, 62);
    TP_SetDisplayArea(13, 5, 114, 60);

    displayLine("Actualizacion", ALIGN_MIDDLE, 5);
}


//»æÖÆ¹ö¶¯Ìõ
static void drawScrBar(TP_ListBoxRect* ScrRect, int32 StartLine, float BlockHight)
{
    TP_SetDisplayArea( ScrRect->left, ScrRect->top, ScrRect->right,ScrRect->bottom );
    TP_ScrClsDisplayArea();
    TP_ScrDrawRect( ScrRect->left, ScrRect->top, ScrRect->right,ScrRect->bottom);

    if ( BlockHight < 1 )
    {
        TP_ScrFillRect(ScrRect->left,
                       (uint32)ScrRect->top +
                       (uint32)((float)StartLine * BlockHight),
                       ScrRect->right,
                       (uint32)ScrRect->top +
                       (uint32)((float)StartLine * BlockHight) +1);
    }
    else
    {
        TP_ScrFillRect(ScrRect->left,
                       (uint32)ScrRect->top +
                       (uint32)((float)StartLine * BlockHight),
                       ScrRect->right,
                       (uint32)ScrRect->top +
                       (uint32)((float)(StartLine + 1) * BlockHight));
    }
}


static uint8 showQueryDialog(const char* pQueryStr, uint32 timeout)
{
    TP_ListBoxRect borderRect;
    uint8 yPos = 5;
    uint8 userKey;
    int32 StartLine = 0;
    float BlockHight = 0;
    int32 LineCount = 0;
    int32 MaxRow = 0;
    int32 MaxCharInRow = 0;
    int32 CopyLen = 0;
    char* (*strAddrBuf)[] = PNULL;
    char* temp = PNULL;

    TP_AllocMemory( sizeof(char*) * 10, (void**)&strAddrBuf );
    if (PNULL == strAddrBuf )
    {
        return 0xFB;
    }
    memset( strAddrBuf, 0, sizeof(char*) * 10);

    TP_AllocMemory(256, (void**)&temp);
    if (PNULL == temp )
    {
        TP_FreeMemory((void**)&strAddrBuf );

        return 0xFB;
    }
    memset(temp, 0, 256);

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 1);
    borderRect.left   = 112;
    borderRect.top    = yPos;
    borderRect.right  = 117;
    borderRect.bottom = 47;
    TP_SetDisplayArea(12, yPos + 2, 110, 45);

    MaxRow       = TP_GetMaxRowInScreen();
    MaxCharInRow = TP_GetMaxCharInRow();

    LineCount = SplitTextToBuff((uint8*)pQueryStr, strAddrBuf, 10);
    LineCount = min(LineCount, 10);
    if (LineCount > MaxRow)
    {
        BlockHight   = (float)(47 - yPos) / (float)( LineCount - MaxRow + 1);
    }

    CopyLen = min(strlen((char*)(*strAddrBuf)[StartLine]),
                  MaxRow * (MaxCharInRow + 1));
    memcpy(temp, (*strAddrBuf)[StartLine], CopyLen);
    temp[CopyLen] = 0;

    TP_ShowTextSingleLine(12, yPos + 2, 110, 45,
                          CFONT, ALIGN_LEFT, (uint8*)"%s", temp);

    TP_ScrUpdate();

    if (timeout > 0)
    {
        TP_TimerSet(0, timeout);
    }
    while (TRUE)
    {
        if (TP_Kbhit() == 0xFF)
        {
            timeout = 0;

            userKey = TP_GetKey();
            TP_Kbflush();
            if (userKey == UPGRADE_KEY_OK)
            {
                TP_FreeMemory((void**)&strAddrBuf);
                TP_FreeMemory((void**)&temp);

                return 0x00;
            }
            else if (userKey == UPGRADE_KEY_CANCEL)
            {
                TP_FreeMemory((void**)&strAddrBuf);
                TP_FreeMemory((void**)&temp);
                TP_ScrFontSet( CFONT );

                return 0x01;
            }
            else if (userKey == TP_KEY_UP ||
                     userKey == TP_KEY_DOWN )
            {
                if (userKey == TP_KEY_DOWN)
                {
                    if (StartLine < (LineCount - MaxRow))
                    {
                        ++StartLine;
                    }
                }
                else
                {
                    if (StartLine > 0 )
                    {
                        --StartLine;
                    }
                }

                CopyLen = min( strlen((char*)(*strAddrBuf)[StartLine]),
                               MaxRow * (MaxCharInRow + 1));
                memcpy(temp, (*strAddrBuf)[StartLine], CopyLen);
                temp[CopyLen] = 0;

                TP_LcdFreeze();

                TP_ShowTextSingleLine(12, yPos + 2, 110, 45,
                                      CFONT, ALIGN_LEFT,(uint8*)"%s", temp);

                TP_ScrUpdate();
            }
        }

        if (timeout > 0)
        {
            if (TP_TimerCheck(0) == 0)
            {
                TP_FreeMemory((void**)&strAddrBuf);
                TP_FreeMemory((void**)&temp);

                return 0x02;
            }
        }
    }
}

/******************************************************************************
* Protected Functions
*******************************************************************************/
void tb_upgrade_display_ShowProgress(TB_Upgrade_UpgradeStepE step,
                                     uint32 currValue, uint32 maxValue)
{
    uint8 temp1[64];
    uint8 temp2[64];
    uint8 yPos = 5;

    initDisplayPage();

    if (step == TB_UPGRADE_STEP_CONNECT)
    {
        sprintf((char*)temp1, "%s", "Conectando");
        addDots((char*)temp1);

        sprintf((char*)temp2, "Intentos: %d / %d", currValue, maxValue);
    }
    else if (step == TB_UPGRADE_STEP_DOWNLOAD)
    {
        sprintf((char*)temp1, "%s", "Descargando");
        addDots((char*)temp1);

        sprintf((char*)temp2, "%d ~ %d", currValue + 1, maxValue + 1);
    }
    else if (step == TB_UPGRADE_STEP_WRITE)
    {
        sprintf((char*)temp1, "%s", "Escribiendo");
        addDots((char*)temp1);

        sprintf((char*)temp2, "%d / %d", currValue + 1, maxValue + 1);
    }

    yPos += (CHAR_HEIGHT + 5);
    displayLine((char*)temp1,
                ALIGN_LEFT, yPos);

    yPos += (CHAR_HEIGHT + 1);
    displayLine((char*)temp2,
                ALIGN_MIDDLE, yPos);

    TP_ScrUpdate();

    ProgressInd++;
    ProgressInd = (ProgressInd % 4);
}


void tb_upgrade_display_PromptGetVersion(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Reportando version del software...",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}

void tb_upgrade_display_PromptStart(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Iniciando...",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptCheckingData(char* message)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine(message, ALIGN_LEFT, yPos);
    displayLine("Cancelar",
                ALIGN_RIGHT, 49);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptQuitDownload(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Desconectando...",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}

void tb_upgrade_display_PromptBeforeUpgrade(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Validar datos descargados y actualizar...",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptSuccess(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Actualizacion exitosa!",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptFail(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Fallo actualizacion!",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptFinish(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Reportando al servidor...",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptServiceRegisting(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Registrando servicio...",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}



void tb_upgrade_display_PromptError(uint8 result)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);

    switch (result)
    {
    case 0x01:
        displayLine("Error de inicio de actualizacion\nIntentar mas tarde",
                    ALIGN_LEFT, yPos);
        break;

    case 0x02:
        displayLine("Error de conexion al servidor\nIntentar mas tarde",
                    ALIGN_LEFT, yPos);
        break;

    case 0x03:
        displayLine("El archivo no existe\nChequear la URL",
                    ALIGN_LEFT, yPos);
        break;

    case 0x04:
        displayLine("La descarga fallo\nIntentar mas tarde",
                    ALIGN_LEFT, yPos);
        break;

    case 0x05:
        displayLine("Escritura a memoria flash fallo\nIntentar mas tarde",
                    ALIGN_LEFT, yPos);
        break;

    case 0x06:
        displayLine("La actualizacion reiniciara en el proximo encendido",
                    ALIGN_LEFT, yPos);
        break;

    case 0x07:
        displayLine("Datos de actualizacion invalidos\nChequear la URL",
                    ALIGN_LEFT, yPos);
        break;

    case 0x08:
        displayLine("El software a actualizar no es de este producto!",
                    ALIGN_LEFT, yPos);
        break;

    case 0x09:
        displayLine("El software a actualizar no es de esta terminal!",
                    ALIGN_LEFT, yPos);
        break;

    case 0x0A:
        displayLine("La ultima version se encuentra instalada!",
                    ALIGN_LEFT, yPos);
        break;

    case 0x0B:
        displayLine("La actualizacion no es para esta version del software!",
                    ALIGN_LEFT, yPos);
        break;
    }

    TP_ScrUpdate();
}

void tb_upgrade_display_PromptErrorVersion(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("La ultima version se encuentra instalada!",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptErrorProtocol(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("El software a actualizar no es de este producto!",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptCheckingBP(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Validando punto de control...",
                ALIGN_LEFT, yPos);
    displayLine("Cancelar",
                ALIGN_RIGHT, 49);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptInvalidBP(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Punto de control invalido\nIniciar desde el principio ahora",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}


void tb_upgrade_display_PromptClearData(void)
{
    uint8 yPos = 5;

    initDisplayPage();

    yPos += (CHAR_HEIGHT + 5);
    displayLine("Limpiando datos de actualizacion...",
                ALIGN_LEFT, yPos);

    TP_ScrUpdate();
}


uint8 tb_upgrade_display_QueryDoStartNewOne(void)
{
    uint8 result;

    result = showQueryDialog(QueryDoStartNewOne, 0);
    if (result == 0xFB)
    {
        result = 0x00;
    }

    return result;
}


uint8 tb_upgrade_display_QueryDoUseBP(void)
{
    uint8 result;

    result = showQueryDialog(QueryDoUseBP, 0);
    if (result == 0xFB)
    {
        result = 0x00;
    }

    return result;
}


uint8 tb_upgrade_display_QueryDoRestart(void)
{
    uint8 result;

    result = showQueryDialog(QueryDoRestart, 3 * 1000);
    if (result == 0x02 ||
        result == 0xFB)
    {
        result = 0x00;
    }

    return result;
}


uint8 tb_upgrade_display_QueryDoResume(void)
{
    uint8 result;

    result = showQueryDialog(QueryDoResume, 3 * 1000);
    if (result == 0x02 ||
        result == 0xFB)
    {
        result = 0x00;
    }

    return result;
}


uint8 tb_upgrade_display_QueryDoClearData(void)
{
    uint8 result;

    result = showQueryDialog(QueryDoClearData, 0);
    if (result == 0xFB)
    {
        result = 0x00;
    }

    return result;
}


uint8 tb_upgrade_display_QueryDoCancelDownload(void)
{
    uint8 result;

    result = showQueryDialog(QueryDoCancelDownload, 0);
    if (result == 0xFB)
    {
        result = 0x00;
    }

    return result;
}
