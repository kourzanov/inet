/*
 * VoIPSender.h
 *
 *  Created on: 25/gen/2011
 *      Author: Adriano
 */



#ifndef VOIPSENDER_H_
#define VOIPSENDER_H_

#include <string.h>

#include "INETDefs.h"

#include "UDPSocket.h"
#include "IPvXAddressResolver.h"
#include "VoipPacket_m.h"

class VoIPSender : public cSimpleModule
{
    UDPSocket socket;
    //has the sender been initialized?
    bool initialized_;

    //source
    // FIXME: use volatile parameters for talk and silence duration
    double    durTalk;
    double    durSil;
    double    scaleTalk;
    double    shapeTalk;
    double    scaleSil;
    double    shapeSil;
    bool      isTalk;

    // FIXME: be more specific with the name of this self message
    cMessage* selfSource;
    //sender
    // FIXME questi non dovrebbero essere interi     //FIXME Translate!!!
    int    talkID;
    int    nframes;
    int    frameID;
    int    nframes_tmp;
    int    talkFrameSize;
    double samplingTime;

    // ----------------------------
    // FIXME: it is unclear what is this self message used for
    cMessage *selfSender;

    simtime_t timestamp;
    int localPort;
    int destPort;
    IPvXAddress destAddress;
    simtime_t stopTime;

    void talkspurt(double dur);
    void selectPeriodTime();
    void sendVoIPPacket();

  public:
    ~VoIPSender();
    VoIPSender();

  protected:
    virtual int numInitStages() const {return 4;}
    void initialize(int stage);
    void handleMessage(cMessage *msg);
};

#endif /* VOIPSENDER_H_ */
