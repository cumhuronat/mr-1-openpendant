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
            float wposX = getValue(wpos, ',', 0).toFloat();
            float wposY = getValue(wpos, ',', 1).toFloat();
            float wposZ = getValue(wpos, ',', 2).toFloat();
            if(wposX != currentStatus.wposX){
                bleManager.updateWorkPositionX(wposX);
                currentStatus.wposX = wposX;
            }
            if(wposY != currentStatus.wposY){
                bleManager.updateWorkPositionY(wposY);
                currentStatus.wposY = wposY;
            }
            if(wposZ != currentStatus.wposZ){
                bleManager.updateWorkPositionZ(wposZ);
                currentStatus.wposZ = wposZ;
            }
        }
        else if (segment.startsWith("FS:"))
        {
            String fs = segment.substring(3);
            float feedRate = getValue(fs, ',', 0).toFloat();
            float spindleSpeed = getValue(fs, ',', 1).toFloat();
            if(feedRate != currentStatus.feedRate){
                bleManager.updateFeed(feedRate);
                currentStatus.feedRate = feedRate;
            }
            if(spindleSpeed != currentStatus.spindleSpeed){
                bleManager.updateSpeed(spindleSpeed);
                currentStatus.spindleSpeed = spindleSpeed;
            }
        }
        else if (segment.startsWith("Pn:"))
        {
            String pinState = segment.substring(3);
            bool pinX = pinState.indexOf('X') != -1;
            bool pinY1 = pinState.indexOf("Y1") != -1;
            bool pinY2 = pinState.indexOf("Y2") != -1;
            bool pinZ = pinState.indexOf('Z') != -1;
            bool pinH = pinState.indexOf('H') != -1;
            if(pinX != currentStatus.pinX){
                bleManager.updatePinStateX(pinX);
                currentStatus.pinX = pinX;
            }
            if(pinY1 != currentStatus.pinY1){
                bleManager.updatePinStateY1(pinY1);
                currentStatus.pinY1 = pinY1;
            }
            if(pinY2 != currentStatus.pinY2){
                bleManager.updatePinStateY2(pinY2);
                currentStatus.pinY2 = pinY2;
            }
            if(pinZ != currentStatus.pinZ){
                bleManager.updatePinStateZ(pinZ);
                currentStatus.pinZ = pinZ;
            }
            if(pinH != currentStatus.pinH){
                bleManager.updatePinStateH(pinH);
                currentStatus.pinH = pinH;
            }
        }
        else if (segment.startsWith("Q:"))
        {
            int queueItems = segment.substring(2).toInt();
            if(queueItems != currentStatus.queueItems){
                bleManager.updateQueueItems(queueItems);
                currentStatus.queueItems = queueItems;
            }
        }
        else if (segment.startsWith("WCO:"))
        {
            String wco = segment.substring(4);
            float wcoX = getValue(wco, ',', 0).toFloat();
            float wcoY = getValue(wco, ',', 1).toFloat();
            float wcoZ = getValue(wco, ',', 2).toFloat();
            if(wcoX != currentStatus.wcoX){
                bleManager.updateWorkCoordOffsetX(wcoX);
                currentStatus.wcoX = wcoX;
            }
            if(wcoY != currentStatus.wcoY){
                bleManager.updateWorkCoordOffsetY(wcoY);
                currentStatus.wcoY = wcoY;
            }
            if(wcoZ != currentStatus.wcoZ){
                bleManager.updateWorkCoordOffsetZ(wcoZ);
                currentStatus.wcoZ = wcoZ;
            }
        }
        else if (segment.startsWith("Ov:"))
        {
            String ov = segment.substring(3);
            int ovFeed = getValue(ov, ',', 0).toInt();
            int ovRapid = getValue(ov, ',', 1).toInt();
            int ovSpindle = getValue(ov, ',', 2).toInt();
            if(ovFeed != currentStatus.ovFeed){
                bleManager.updateOverrideFeed(ovFeed);
                currentStatus.ovFeed = ovFeed;
            }
            if(ovRapid != currentStatus.ovRapid){
                bleManager.updateOverrideRapid(ovRapid);
                currentStatus.ovRapid = ovRapid;
            }
            if(ovSpindle != currentStatus.ovSpindle){
                bleManager.updateOverrideSpindle(ovSpindle);
                currentStatus.ovSpindle = ovSpindle;
            }
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