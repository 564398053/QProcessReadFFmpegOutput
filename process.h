#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>
#include <QtDebug>
#include <QTime>
#include <QDir>
#include <QCoreApplication>

// 参考文档：https://zhuanlan.zhihu.com/p/86206831

class Process : public QObject
{
    Q_OBJECT
public:
    explicit Process(QObject *parent = nullptr)
    {
        connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onfinished(int, QProcess::ExitStatus)));
        connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(onReadData()));
        m_process.setReadChannel(QProcess::StandardError);

        QString in = QCoreApplication::applicationDirPath() + "/dancing.mp4";
        QString out = QCoreApplication::applicationDirPath() + "/dancing.mkv";
        // ffmpeg -i in.avi out.mp4
        QStringList argList;
        argList.push_back("-i");
        argList.push_back(in);
        argList.push_back(out);
        argList.push_back("-y");
        m_process.start("ffmpeg", argList);
    }

signals:

private slots:
    void onReadData()
    {
        QString ffmpeglog = m_process.readAllStandardError();
        // 从ffmpeg的输出中提取进度，要点是在编码前捕获 Duration 值，并在编码期间捕获 time=... 值。
        QStringList durationList = ffmpeglog.split("Duration: ");
        if(durationList.size() > 1) {
            QString duration = durationList.at(1);
            durationList = duration.split(", start:");
            if (durationList.size() > 1) {
                duration = durationList.at(0);
                //qDebug() << "Duration: " << duration;

                qint64 totalTimeMS = timeStrToMillisecond(duration);
                m_durationMS = totalTimeMS;
                qDebug()  << "Total time:" << totalTimeMS;
            }
        }

        QStringList timeList = ffmpeglog.split("time=");
        if (timeList.size() > 1) {
            QString time = timeList.at(1);
            timeList = time.split(" bitrate=");
            if (timeList.size() > 1) {
                time = timeList.at(0);
            }

            //qDebug()  << "Time:" << time;
            int timeMS = timeStrToMillisecond(time);
            //qDebug()  << "Time:" << timeMS;
            qDebug() << "Progress: " << int((double(timeMS) / m_durationMS) * 100) << "%";
        }
        // qDebug() <<ffmpeglog;
    }

    void onfinished(int exitCode, QProcess::ExitStatus exitStatus)
    {
        qDebug() << "process finished. exit code: " << exitCode << ", exit status: " << exitStatus;
    }

    static qint64 timeStrToMillisecond(const QString& hhmmss)
    {
        QStringList timeList = hhmmss.split(":");
        qint64 time = 0;
        // 00:00:00.14
        if(timeList.size() == 3) {
            time += timeList.at(0).toInt() * 3600 * 1000;
            time += timeList.at(1).toInt() * 60 * 1000;
            time += (qint64)(timeList.at(2).toDouble() * 60 * 1000);
        }
        return time;
    }

private:
    QProcess m_process;
    qint64 m_durationMS = 0; // video duration in minisecond
};

#endif // PROCESS_H
