#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QPoint>
#include <QColor>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QDebug>
#include <windows.h>
#include <QImage>
#include <QDir>
#include <QDateTime>
#include <QRect>
#include <QThread>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QString>
#include <QDebug>
class backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString imgPath READ getImgPath NOTIFY imgPathChanged)
public:
    explicit backend(QObject *parent = nullptr);
    ~backend();
    Q_INVOKABLE void connectBlueStacks();
    Q_INVOKABLE void tap(int x, int y);
    Q_INVOKABLE void captureScreen();
    Q_INVOKABLE void captureWidget(QString);


    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void newloop();
    Q_INVOKABLE void log(QString text);
    Q_INVOKABLE QString getImgPath() {
        return imgPath;
    }
    bool isColorSimilar(QColor firstColor, QColor secondColor, int acceptedRange);

    QString solveCaptcha(QString imagePath);
    bool isWrongCaptcha();

    Q_INVOKABLE QPoint getPos(int x, int y, double width, double height, int mode = 0, int clickCount = 0, QString widgetName = "");
    Q_INVOKABLE void updatePort(QString port);
    Q_INVOKABLE void setLocXButton1(QPoint point);
    Q_INVOKABLE void setLocXButton1_left(QPoint point);
    Q_INVOKABLE void setLocXButton2(QPoint point);
    Q_INVOKABLE void setAdWatchButton(QPoint point);
    Q_INVOKABLE void setCaptchaTextbox(QPoint point);
    Q_INVOKABLE void setCaptchaConfirm(QPoint point);
    Q_INVOKABLE void setContinueAd(QPoint point);
    bool isState1();
    bool isState2();
signals:
    void logUpdated(QString message);
    void imgPathChanged();
private:
    bool matchTemplate(QImage sourceImg, QString templateName);
    QPoint findTemplate(QImage sourceTemplate, QString templateName);
    QPoint XButton1 = QPoint(857, 49);
    QPoint XButton1_left = QPoint(53, 49);
    QPoint XButton2 = QPoint(856, 388);
    QPoint adWatchButton = QPoint(211, 623);
    QPoint captchaTextbox = QPoint(328, 886);
    QPoint captchaConfirm = QPoint(598, 1000);
    QPoint continueAdButton = QPoint(618, 902);
    QPoint captureWidget1 = QPoint(0, 0);
    QPoint captureWidget2 = QPoint(0, 0);
    QPoint captchaCorner1 = QPoint(300, 680);
    QPoint captchaCorner2 = QPoint(595, 827);
    QString widgetName;
    int checkState();

    QProcess *ocrProcess = nullptr;
    QString adbPortAddress = "127.0.0.1:5555";
    QString adbPath;
    QString imgPath;
    QImage screenImg;
    QTimer *timer;
    // State: 0 - main screen | 1 - captcha screen | 2 - Ad screen | 3 - GG play ad Popup screen | 4 - ad success screen | 5 - Ad continue screen
    int state = 0;
    int adbConnected = 0;
    int time = 0;
    cv::dnn::Net net;
    bool isModelLoaded = false;
};

#endif // BACKEND_H
