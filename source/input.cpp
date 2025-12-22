#include <3ds.h>
#include <string>

std::string getTextInput(const std::string& hint, int maxLen = 256)
{
    SwkbdState swkbd;
    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, maxLen);
    swkbdSetHintText(&swkbd, hint.c_str());

    static char buffer[257] = {0};
    SwkbdButton button = swkbdInputText(&swkbd, buffer, sizeof(buffer));

    if (button == SWKBD_BUTTON_LEFT)
        return "";

    return std::string(buffer);
}