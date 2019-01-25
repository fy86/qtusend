#include <QCoreApplication>
#include <QTimer>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QUdpSocket>
#include <QDir>
#include <QProcess>

void scanfx()
{
    FILE *fp;
    fp=fopen("a.txt","rt");
    if(fp==NULL)return;
    int i,j;
    fscanf(fp,"%x%x",&i,&j);
    fclose(fp);
    printf("%d %d\n",i,j);
}

void testdirname()
{
    QDir dir("////a.ta/");
    qDebug("dir dirname:%s   path:%s",dir.dirName().toLatin1().data(),dir.path().toLatin1().data());
}
void testProcess()
{
    QProcess *p=new QProcess();
    QStringList sl;
    //sl<<"/home/c/tmp/test.sh";
    sl<<"ls";
    p->start(QString("/bin/sh"),sl);
    p->waitForFinished();

    QByteArray ba=p->readAllStandardOutput();
    qDebug(" output : %s",ba.data());
}
void testFileRead()
{
    QFile f(QString("/home/c/tmp/t.txt"));
    QByteArray ba;
    ba.resize(20);
    if(f.exists()){
        if(f.open(QIODevice::ReadOnly)){
            ba=f.readAll();
            ba.resize(3);
            qDebug(" file readall ba.len : %d",ba.size());
            //ba.at(0)='z';
        }

    }
    else{
        qDebug(" file not found");
    }
    char b8=0;
    ba.append(b8);
    qDebug("   file : %s  len:%d",ba.data(),ba.size());
    ba.data()[0]='z';
    qDebug("   file : %s  len:%d",ba.data(),ba.size());

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile file;
    QByteArray line;
    int d[30];
    int sn;
    char buf[20];
    QUdpSocket us;

    qDebug("hello");
    testdirname();
    testProcess();
    testFileRead();

    int start=-1,end=10000;

    switch(argc){
    case 4:
        end=QString(argv[3]).toInt();
    case 3:
        start=QString(argv[2]).toInt();
    case 2:
        file.setFileName(argv[1]);
        qDebug("start:%d end:%d name:%s",start,end,file.fileName().toLatin1().data());
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
            while(!file.atEnd()){
                line=file.readLine();
                QString str(line.data());
                QStringList list;
                list= str.split(QRegExp("\\s+"),QString::SkipEmptyParts);
                int len = list.size();

                if(len<13)continue;
                sn=list.at(0).toInt();
                if(sn<start)continue;
                if(sn>end)continue;

                for(int i=0;i<len;i++){
                    d[i]=list.at(i).toInt(0,16);
                    qDebug("i:%d len:%d list .. %s %02x",i,len,list.at(i).toLatin1().data(),list.at(i).toUInt(0,16));

                }
                qDebug(" line : %s",str.toLatin1().data());

                //buf[0]=0x55;
                //buf[1]=0xaa;
                //
                buf[0]=0xaa;
                buf[1]=0x55;
                //for(int i=0;i<4;i++)buf[2+i]=d[4+i];

                // uart id32.err
                for(int i=0;i<4;i++)buf[2+3-i]=d[4+i];

                for(int i=0;i<8;i++)buf[6+i]=d[11+i];
                buf[14]=0;
                buf[15]=0x88;

                //us.writeDatagram(buf,16,QHostAddress("239.255.43.21"),7755);
                us.writeDatagram(buf,16,QHostAddress("127.0.0.1"),7755);

            }

        }
        //scanfx();
        break;
    default:
        if(argc<2) break;
        ///// argc >=5
        buf[0]=0xaa;buf[1]=0x55;
        for(int i=0;i<4;i++){
            //buf[2+i]=QString(argv[i+1]).toUInt(0,16);
            buf[2+3-i]=QString(argv[i+1]).toUInt(0,16);
        }
        for(int i=0;i<8;i++){
            if((6+i)>argc)break;
            buf[6+i]=QString(argv[5+i]).toUInt(0,16);
        }
        buf[14]=0;
        buf[15]=0x88;
        for(int i=0;i<4;i++)qDebug(" id32 : %02x",0x0ff & buf[2+i]);
        for(int i=0;i<8;i++)qDebug(" data8:%d : %02x",i,0x0ff & buf[6+i]);
        //us.writeDatagram(buf,16,QHostAddress("239.255.43.21"),7755);
        us.writeDatagram(buf,16,QHostAddress("127.0.0.1"),7755);
        break;
    }

    QTimer::singleShot(0,&a,SLOT(quit()));
    return a.exec();
}
