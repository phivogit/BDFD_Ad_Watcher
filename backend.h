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

class backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString imgPath READ getImgPath NOTIFY imgPathChanged)
public:
    explicit backend(QObject *parent = nullptr);
    Q_INVOKABLE void connectBlueStacks();
    Q_INVOKABLE void tap(int x, int y);
    Q_INVOKABLE void captureScreen();

    Q_INVOKABLE void getPos(int x, int y, double width, double height);

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void loop();

    Q_INVOKABLE void log(QString text);
    Q_INVOKABLE QString getImgPath() {
        return imgPath;
    }
    bool isColorSimilar(QColor firstColor, QColor secondColor, int acceptedRange);
    void getData();
    Q_INVOKABLE void startTraining();

    QString solveCaptcha(QString imagePath);
    bool isWrongCaptcha();

    Q_INVOKABLE void updatePort(QString port);
    bool isState1();
    bool isState2();
signals:
    void logUpdated(QString message);
    void imgPathChanged();
private:
    QString adbPortAddress = "127.0.0.1:5555";
    QString adbPath;
    QString imgPath;
    QImage screenImg;
    QTimer *timer;
    QPoint posActivateCaptcha;
    QPoint posCaptchaCheck, posCaptchaTextBox, posCaptchaConfirm;
    QPoint posMainX, posSurroundX1, posSurroundX2, posSurroundX3;
    QPoint posOffX, posSurroundO1, posSurroundO2, posSurroundO3;
    // State: 0 - main screen | 1 - captcha screen | 2 - Ad screen | 3 - GG play ad Popup screen | 4 - ad success screen
    int state = 0;
    bool isTraining = false;

    QColor adButtonColor = QColor(48, 48, 48);
    QPoint adButtonPos = QPoint(839, 614);
    QPoint adButtonPos1 = QPoint(886, 599);
    QPoint adButtonPos2 = QPoint(854, 624);

    QPoint captchaCheckPos = QPoint(546, 552);
    QColor captchaCheckColor = QColor(66, 66, 66);
    QPoint captchaTextBoxPos = QPoint(535, 453);
    QPoint captchaConfirmPos = QPoint(773, 565);
    QColor captchaConfirmColor = QColor(255, 255, 255);

    QPoint popupErrorPos1 = QPoint(1114, 551);
    QColor popupErrorColor1 = QColor(255, 255, 255);

    QPoint adCheckPos1 = QPoint(875, 4);
    QColor adCheckColor1 = QColor(177, 177, 181);

    QPoint state0Check1 = QPoint(858, 334);
    QColor state0Color1 = QColor(66, 66, 66);
    QPoint state0Check2 = QPoint(965, 157);
    QColor state0Color2 = QColor(48, 48, 48);

    QPoint adXButtonPos = QPoint(1244, 36);
    QColor adXButtonColorBlack = QColor(32, 33, 36);
    QPoint adXButtonCheckPos1 = QPoint(1231, 34);
    QPoint adXButtonCheckPos2 = QPoint(1244, 47);
    QColor adXButtonCheckColorWhite = QColor(250, 250, 250);
    QColor adXButtonCheckColorBlack = QColor(44, 43, 48);
    QColor adXButtonCheckColorGray = QColor(80, 80, 80);

    QPoint popupXButtonPos = QPoint(1190, 65);
    QColor popupXButtonColor = QColor(70, 70, 70);
    QPoint popupCheckPos1 = QPoint(36, 70);
    QPoint popupCheckPos2 = QPoint(1208, 65);
    QColor popupCheckColor = QColor(255, 255, 255);

    QPoint adSuccessExitPos = QPoint(1070, 391);
    QPoint adSuccessExitCheck1Pos = QPoint(842, 475);
    QColor adSuccessExitCheck1Color = QColor(255, 255, 255);
    QPoint adSuccessExitCheck2Pos = QPoint(355, 478);
    QPoint adSuccessExitCheck3Pos = QPoint(849, 241);
    QColor adSuccessExitCheck2Color = QColor(66, 66, 66);

    QPoint captchaCorner1 = QPoint(490, 243);
    QPoint captchaCorner2 = QPoint(788, 243);
    QPoint captchaCorner3 = QPoint(490, 390);
    QPoint captchaCorner4 = QPoint(788, 390);

    QPoint wrongCaptchaPos1 = QPoint(475, 308);
    QColor wrongCaptchaCol1 = QColor(108, 108, 108);
    QPoint wrongCaptchaPos2 = QPoint(794, 461);
    QColor wrongCaptchaCol2 = QColor(255, 255, 255);
    QPoint wrongCaptchaPos3 = QPoint(573, 414);
    QColor wrongCaptchaCol3 = QColor(66, 66, 66);
    cv::dnn::Net net;
    bool isModelLoaded = false;
};

#endif // BACKEND_H
