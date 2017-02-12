#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include <QThread>
#include "processcanmessage.h"

//can socket
int iCanSocId = -1;
pthread_mutex_t CanReadMutex;


//thread function
pthread_t WaitForIncommingFrameId = -1;
void* fnWaitForIncommingFrame (void* unused)
{
  struct can_frame frame_rd;
  int recvbytes = 0;
  int iMessageCounter = -1;
  XMC_LMOCan_t ReceivedCanMsg;
  memset(&ReceivedCanMsg,0, sizeof(XMC_LMOCan_t));
  while (1)
  {
    //usleep(100 * 1000);
    if(iCanSocId < 0) break;
    //fprintf (stderr, "Wating for frames in thread\n");
    recvbytes = read(iCanSocId, &frame_rd, sizeof(struct can_frame));

    if(recvbytes)
    {
      pthread_mutex_lock(&CanReadMutex);
      memset(&ReceivedCanMsg,0, sizeof(XMC_LMOCan_t));
      //fprintf(stderr,"in thread: dlc = %d, data = %s\n", frame_rd.can_dlc,frame_rd.data);
      ReceivedCanMsg.can_identifier  = frame_rd.can_id;
      ReceivedCanMsg.can_data_length = frame_rd.can_dlc;

      int u8SizeToCopy = 0;
      if (ReceivedCanMsg.can_data_length < (signed)sizeof(ReceivedCanMsg.can_data))
           u8SizeToCopy = ReceivedCanMsg.can_data_length;
       else
           u8SizeToCopy = sizeof(ReceivedCanMsg.can_data);
       memcpy(ReceivedCanMsg.can_data, frame_rd.data, u8SizeToCopy);

      pMainWindow->onCanMessageReceived(iMessageCounter++, &ReceivedCanMsg);
      pthread_mutex_unlock(&CanReadMutex);
    }

  }
  return NULL;
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initialize and open can socket
    //open socket
    iCanSocId = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (iCanSocId < 0)
    {
      qDebug() << "Problem to open the socket";
    }

    struct sockaddr_can addr;
    struct ifreq ifr;
    addr.can_family = AF_CAN;
    //strcpy(ifr.ifr_name, "vcan0");  //virtual can
    strcpy(ifr.ifr_name, "can0");   //real can

    if (ioctl(iCanSocId, SIOCGIFINDEX, &ifr) < 0)
    {
      qDebug() << "Problem to find index";
    }

    addr.can_ifindex = ifr.ifr_ifindex;
    //
    if(bind(iCanSocId, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
      qDebug() << "Problem to bind";
    }

    //Create thread for reading frames using pthread

    if ((pthread_create (&WaitForIncommingFrameId, NULL, &fnWaitForIncommingFrame, NULL)) == 0)
    {
       qDebug() << "Thread Created";
    }
    else
    {
      qDebug() << "Problem to Create Thread";
    }

    //Create thread for reading frames using QThread
    QThread *ReceiveCanFramesThread = new QThread;
    ReceiveCanFrames_t *ReceiveCanFrames = new ReceiveCanFrames_t;
    ReceiveCanFrames->moveToThread(ReceiveCanFramesThread);

    connect(ReceiveCanFramesThread, SIGNAL(started()),            ReceiveCanFrames,       SLOT(doWork()));
    connect(ReceiveCanFrames,       SIGNAL(signalWorkFinished()), ReceiveCanFramesThread, SLOT(quit()));

    connect(ReceiveCanFramesThread, SIGNAL(finished()),           ReceiveCanFrames,       SLOT(deleteLater()));
    connect(ReceiveCanFramesThread, SIGNAL(finished()),           ReceiveCanFramesThread, SLOT(deleteLater()));

    //when frames is received:
    connect(ReceiveCanFrames, SIGNAL(signalCanMessageReceived(int)), this,                SLOT(onCanMessageReceivedFromQThread(int)));
    ReceiveCanFramesThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_BtnRead_clicked()
{
    struct can_frame frame_rd;
    int recvbytes = 0;
    qDebug() << "Wating for frames";

    //recvbytes = read(iCanSocId, &frame_rd, sizeof(struct can_frame));
    //recvbytes = recv(iCanSocId, &frame_rd, sizeof(struct can_frame), 0);
    recvbytes = read(iCanSocId, &frame_rd, sizeof(struct can_frame));
    if(recvbytes)
    {
        fprintf(stderr,"dlc = %d, data = %s\n", frame_rd.can_dlc,frame_rd.data);

    }

}

void MainWindow::onCanMessageReceived(int iCounter, XMC_LMOCan_t *ReceivedCanMsg)
{
    static uint64_t u64LastValue = 0;
    uint64_t u64CurrentValue = 0;
    u64CurrentValue = *((uint64_t *)ReceivedCanMsg->can_data);
    qDebug() << "MessageCounter:"
             << iCounter
             << ReceivedCanMsg->can_identifier
             << ReceivedCanMsg->can_data_length
             << QByteArray((const char *)ReceivedCanMsg->can_data, ReceivedCanMsg->can_data_length).toHex()
             << "Diff:"
             << (u64CurrentValue-u64LastValue);
    u64LastValue = u64CurrentValue;
}

void MainWindow::onCanMessageReceivedFromQThread(int iCounter)
{
   //qDebug() << "QThread MessageCounter" << iCounter;
}
