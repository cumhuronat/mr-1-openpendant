#include "GRBLParser.h"
#include "BLEManager.h"
struct StatusData
{
    char status[10];
    float wposX, wposY, wposZ;
    float feedRate, spindleSpeed;
    bool pinX, pinY1, pinY2, pinZ, pinH;
    int queueItems;
    float wcoX, wcoY, wcoZ;
    int ovFeed, ovRapid, ovSpindle;
};

StatusData currentStatus;

void GRBLParser::parseStatus(String &statusString)
{
    statusString = statusString.substring(1, statusString.length() - 2);

    int segmentCount = 0;
    String segments[10]; // Assuming maximum 10 segments

    int startIndex = 0;
    int pipeIndex = statusString.indexOf('|');

    while (pipeIndex != -1)
    {
        segments[segmentCount++] = statusString.substring(startIndex, pipeIndex);
        startIndex = pipeIndex + 1;
        pipeIndex = statusString.indexOf('|', startIndex);
    }
    segments[segmentCount++] = statusString.substring(startIndex);

    for (int i = 0; i < segmentCount; i++)
    {
        String segment = segments[i];

        if (i == 0)
        {
            char newStatus[10];
            strncpy(newStatus, segment.c_str(), sizeof(currentStatus.status) - 1);
            if(strcmp(newStatus, currentStatus.status) != 0){
                bleManager.updateMachineState(newStatus);
            }
            strncpy(currentStatus.status, newStatus, sizeof(currentStatus.status) - 1);
        }
        else if (segment.startsWith("WPos:"))
        {
            String wpos = segment.substring(5);
            currentStatus.wposX = getValue(wpos, ',', 0).toFloat();
            currentStatus.wposY = getValue(wpos, ',', 1).toFloat();
            currentStatus.wposZ = getValue(wpos, ',', 2).toFloat();
        }
        else if (segment.startsWith("FS:"))
        {
            String fs = segment.substring(3);
            currentStatus.feedRate = getValue(fs, ',', 0).toFloat();
            currentStatus.spindleSpeed = getValue(fs, ',', 1).toFloat();
        }
        else if (segment.startsWith("Pn:"))
        {
            String pinState = segment.substring(3);
            currentStatus.pinX = pinState.indexOf('X') != -1;
            currentStatus.pinY1 = pinState.indexOf("Y1") != -1;
            currentStatus.pinY2 = pinState.indexOf("Y2") != -1;
            currentStatus.pinZ = pinState.indexOf('Z') != -1;
            currentStatus.pinH = pinState.indexOf('H') != -1;
        }
        else if (segment.startsWith("Q:"))
        {
            currentStatus.queueItems = segment.substring(2).toInt();
        }
        else if (segment.startsWith("WCO:"))
        {
            String wco = segment.substring(4);
            currentStatus.wcoX = getValue(wco, ',', 0).toFloat();
            currentStatus.wcoY = getValue(wco, ',', 1).toFloat();
            currentStatus.wcoZ = getValue(wco, ',', 2).toFloat();
        }
        else if (segment.startsWith("Ov:"))
        {
            String ov = segment.substring(3);
            currentStatus.ovFeed = getValue(ov, ',', 0).toInt();
            currentStatus.ovRapid = getValue(ov, ',', 1).toInt();
            currentStatus.ovSpindle = getValue(ov, ',', 2).toInt();
        }
    }
}

String GRBLParser::getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void GRBLParser::processMessage(String &statusString)
{
    if (statusString.startsWith("<"))
    {
        parseStatus(statusString);
    }
}