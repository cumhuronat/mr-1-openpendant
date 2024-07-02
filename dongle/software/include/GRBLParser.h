#ifndef GRBL_PARSER_H
#define GRBL_PARSER_H

#include <Arduino.h>
class GRBLParser
{
public:
    static void processMessage(String &statusString);

private:
    static void parseStatus(String &statusString);
    static String getValue(String data, char separator, int index);
};

#endif