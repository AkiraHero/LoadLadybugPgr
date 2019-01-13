#include <LoadLadyBugPgr.h>
using namespace std;
#include <string>
#include <QString>
#include <QDir>
#include <fstream>
#include <QDebug>
int main(int argc, char *argv[])
{
    int64_t basetime = 1542729600000L;
    QString path = "I://1121//jxl";
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.pgr";
    QStringList  files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    ofstream outputtime;
    outputtime.open("timestamp.csv",std::ios::app);
    for(int i = 0; i != files.size(); i++){
        PGR pgrReader;
        qDebug() << files[i];
        string filepath = (path + "/" + files[i]).toStdString();
        LadybugError e1 = pgrReader(filepath);
        assert(e1 == LADYBUG_OK);
        int imagenum = pgrReader.getImageNum();
        for(int j =7576; j != imagenum; j++){
            pgrReader(j);
            int64 timestamp = (int64)pgrReader.getulSeconds() * 1e6L + (int64)pgrReader.getulMicroSeconds();
//            qDebug()<<QString().fromStdString(filepath)<<firsttimestamp<<pgrReader.getulSeconds() << pgrReader.getulMicroSeconds();
            int timestamp_short = int(timestamp/1e3 - basetime);
            if(timestamp_short < 49307880)
                continue;
            outputtime << timestamp_short << std::endl;
            for (int k = 0; k != 6; k++){
                QString savepath = QString("I:\\1121\\jxl\\dataset_tmp\\ladybug\\camera%1\\").arg(k);
                if(!QDir(savepath).exists())
                    QDir(savepath).mkpath(savepath);
                auto img = pgrReader.getRectifiedImage(k);
                string imgname = savepath.append(QString("%1.png").arg(timestamp_short)).toStdString();
                LadybugError e = pgrReader.saveRectifiedImg(imgname.c_str(), &img, LADYBUG_FILEFORMAT_PNG);
                assert(e == LADYBUG_OK);

            }
            if (j % 100 == 0)
                qDebug() << "frame:" << j << "time" << timestamp;
        }
        outputtime.close();
        break;
    }


    return 0;
}
