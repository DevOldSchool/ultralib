#include "PR/os_internal.h"
#include "PRinternal/controller.h"

s32 osEepromLongWrite(OSMesgQueue* mq, u8 address, u8* buffer, int length) {
    s32 ret = 0;

#if BUILD_VERSION < VERSION_I
    if (address > EEPROM_MAXBLOCKS) {
        return -1;
    }
#endif

    while (length > 0) {
#if BUILD_VERSION < VERSION_I
        ret = osEepromWrite(mq, address, buffer);
        if (ret != 0) {
            return ret;
        }
#else
        ERRCK(osEepromWrite(mq, address, buffer));
#endif
        length -= EEPROM_BLOCK_SIZE;
        address++;
        buffer += EEPROM_BLOCK_SIZE;
#if BUILD_VERSION < VERSION_I
        osSetTimer(&__osEepromTimer, 12000 * osClockRate / 1000000, 0, &__osEepromTimerQ, &__osEepromTimerMsg);
#else
        osSetTimer(&__osEepromTimer, OS_USEC_TO_CYCLES(12000), 0, &__osEepromTimerQ, &__osEepromTimerMsg);
#endif
        osRecvMesg(&__osEepromTimerQ, NULL, OS_MESG_BLOCK);
    }

    return ret;
}
