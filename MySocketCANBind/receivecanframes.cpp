#include "receivecanframes.h"

ReceiveCanFrames_t::ReceiveCanFrames_t(QObject *parent) : QObject(parent)
{
  iThreadCounter = 0;
}

void ReceiveCanFrames_t::doWork()
{
  //qDebug() << "QThread";
  while(1)
  {
    usleep(100 * 1000);
    iThreadCounter++;
    emit signalCanMessageReceived(iThreadCounter);
  }
}
