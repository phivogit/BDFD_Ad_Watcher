#include "backend.h"
#include <QDebug>
#include <windows.h>
#include <QImage>
#include <QDir>
#include <QDateTime>
#include <QRect>
#include <QThread>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>
#include <QString>
#include <QDebug>
backend::backend(QObject *parent)
    : QObject{parent}
{
    adbPath = "C:\\Users\\hi\\AppData\\Local\\Android\\Sdk\\platform-tools\\adb.exe";
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &backend::loop);
    try {
        net = cv::dnn::dnn4_v20211220::readNetFromONNX("captcha_solver.onnx");
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        if (net.empty()) {
            log("AI model loaded but empty");
            isModelLoaded = false;
        } else {
            log("AI Model Loaded.");
            isModelLoaded = true;
        }
    } catch (const cv::Exception& e ){
        log("Crashed loading AI model: " + QString::fromStdString(e.what()));
        isModelLoaded = false;
    }
}

void backend::updatePort(QString port) {
    adbPortAddress = port;
}
bool backend::isColorSimilar(QColor firstColor, QColor secondColor, int acceptedRange) {
    if ((firstColor.red() - secondColor.red()) > acceptedRange || (firstColor.red() - secondColor.red()) < -acceptedRange) return false;
    if ((firstColor.green() - secondColor.green()) > acceptedRange || (firstColor.green() - secondColor.green()) < -acceptedRange) return false;
    if ((firstColor.blue() - secondColor.blue()) > acceptedRange || (firstColor.blue() - secondColor.blue()) < -acceptedRange) return false;
    return true;
}
void backend::log(QString text) {
    qDebug() << text;
    emit logUpdated(QString("%1").arg(text));
}
void backend::tap(int x, int y) {
    QProcess process;
    QStringList args;
    args << "-s" << "127.0.0.1:5555" << "shell" << "input" << "tap" << QString::number(x) << QString::number(y);

    process.start(adbPath, args);
    process.waitForFinished();
}

void backend::captureScreen() {
    QProcess process;
    QStringList args;
    args << "-s" << "127.0.0.1:5555" << "shell" << "screencap" << "-p" << "/sdcard/screenshot.png";
    process.start(adbPath, args);
    process.waitForFinished();

    QProcess pullProcess;
    QStringList pullArgs;
    pullArgs << "-s" << "127.0.0.1:5555" << "pull" << "/sdcard/screenshot.png" << "screenshot.png";
    pullProcess.start(adbPath, pullArgs);
    pullProcess.waitForFinished();

    QImage img;
    if (img.load("screenshot.png")) {
        screenImg = img;
        imgPath = "file:///" + QDir::currentPath() + "/screenshot.png?r=" + QString::number(QDateTime::currentMSecsSinceEpoch());
        emit imgPathChanged();
    } else {
        log("Failed to decode bluestacks screen image");

    }
}

void backend::connectBlueStacks() {
    QProcess process;
    QStringList args;
    args << "connect" << adbPortAddress;
    process.start(adbPath, args);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    backend::log("Output: " + output);
}

void backend::getPos(int x, int y, double width, double height) {
    // x, y is the position of the click in QML
    // width, height is the window's size
    if (screenImg.isNull()) log("No Image");
    int realX = x * (screenImg.width() / width);
    int realY = y * (screenImg.height() / height);
    if (realX < 0 || realX > screenImg.width() || realY < 0 || realY > screenImg.height()) {
        log("Position out of bounds.");
    } else {
        QColor color = screenImg.pixelColor(realX, realY);
        log(QString("Pixel: %1 x %2 | Color: Hex %3 | RGB %4, %5, %6").arg(realX).arg(realY).arg(color.name()).arg(color.red()).arg(color.green()).arg(color.blue()));
    }
}

void backend::startTraining() {
    if (timer->isActive()) log("The training loop is currently active.");
    else {
        log("Loop started.");
        isTraining = true;
        state = 0;
        timer->start(2000);
    }
}

void backend::start() {
    if (timer->isActive()) log("The loop is currently active.");
    else {
        log("Loop started.");
        state = 0;
        timer->start(2000);
    }
}
void backend::stop() {
    log("Loop stopped.");
    isTraining = false;
    timer->stop();
}

void backend::loop() {
    log(QString("State %1").arg(state));
    captureScreen();
    if (isTraining) {
        if (state == 0) {
            if (isColorSimilar(screenImg.pixelColor(adButtonPos1), adButtonColor, 2) && isColorSimilar(screenImg.pixelColor(adButtonPos2), adButtonColor, 2)) {
                tap(adButtonPos.x(), adButtonPos.y());
                state = 1;
            }
        } else if (state == 1) {
            getData();
            tap(adSuccessExitPos.x(), adSuccessExitPos.y());
            state = 0;
        }
    } else {
        if (screenImg.isNull()) {
            log("Failed to capture the screen");
            stop();
        }
        if (state == 0) {
            // Check for ad activate button
            if (isColorSimilar(screenImg.pixelColor(adButtonPos1), adButtonColor, 2) && isColorSimilar(screenImg.pixelColor(adButtonPos2), adButtonColor, 2)) {
                tap(adButtonPos.x(), adButtonPos.y());
            }
            // Check if it's state 1 or state 2 after clicking
            if (!(isColorSimilar(screenImg.pixelColor(state0Check1), state0Color1, 2) && isColorSimilar(screenImg.pixelColor(state0Check2), state0Color2, 2))) {
                if (isColorSimilar(screenImg.pixelColor(captchaCheckPos), captchaCheckColor, 3)) {
                    state = 1;
                } else {state = 2;}
            }
        } else if (state == 1) {
            QRect rect(captchaCorner1.x(), captchaCorner1.y(), captchaCorner2.x()-captchaCorner1.x(), captchaCorner3.y() - captchaCorner1.y());
            QImage captchaImg = screenImg.copy(rect);
            captchaImg.save("temp_captcha.png");

            QString code = solveCaptcha(QDir::currentPath() + "/temp_captcha.png");
            if (code.length() == 3) {
                tap(captchaTextBoxPos.x(), captchaTextBoxPos.y());
                QThread::msleep(300);

                QProcess process;
                QStringList args;
                args << "-s" << "127.0.0.1:5555" << "shell" << "input" << "text" << code;

                process.start(adbPath, args);
                process.waitForFinished();
                tap(captchaConfirmPos.x(), captchaConfirmPos.y());
            }
            // Questioning state
            state = 5;
        } else if (state == 2) {
            // if there's x button
            if ((isColorSimilar(screenImg.pixelColor(adXButtonPos), adXButtonColorBlack, 3) ||
                 isColorSimilar(screenImg.pixelColor(adXButtonPos), adXButtonCheckColorWhite, 3)) &&
                ((isColorSimilar(screenImg.pixelColor(adXButtonCheckPos1), adXButtonCheckColorWhite, 3) && isColorSimilar(screenImg.pixelColor(adXButtonCheckPos2), adXButtonCheckColorWhite, 3)) ||
                (isColorSimilar(screenImg.pixelColor(adXButtonCheckPos1), adXButtonCheckColorBlack, 3) && isColorSimilar(screenImg.pixelColor(adXButtonCheckPos2), adXButtonCheckColorBlack, 3)) ||
                (isColorSimilar(screenImg.pixelColor(adXButtonCheckPos1), adXButtonCheckColorGray, 3) && isColorSimilar(screenImg.pixelColor(adXButtonCheckPos2), adXButtonCheckColorGray, 3)))) {
                tap(adXButtonPos.x(), adXButtonPos.y());
            }
            if (isColorSimilar(screenImg.pixelColor(adSuccessExitCheck1Pos), adSuccessExitCheck1Color, 3) &&
                isColorSimilar(screenImg.pixelColor(adSuccessExitCheck2Pos), adSuccessExitCheck2Color, 3) &&
                isColorSimilar(screenImg.pixelColor(adSuccessExitCheck2Pos), adSuccessExitCheck2Color, 3))
                state = 4;
            if (isColorSimilar(screenImg.pixelColor(popupXButtonPos), popupXButtonColor, 3) &&
                isColorSimilar(screenImg.pixelColor(popupCheckPos1), popupCheckColor, 3) &&
                isColorSimilar(screenImg.pixelColor(popupCheckPos2), popupCheckColor, 3))
                state = 3;
            if (isColorSimilar(screenImg.pixelColor(popupErrorPos1), popupErrorColor1, 2) &&
                !(isColorSimilar(screenImg.pixelColor(state0Check1), state0Color1, 2) && isColorSimilar(screenImg.pixelColor(state0Check2), state0Color2, 2)))
                tap(1010, 232);
        } else if (state == 3) {
            if (isColorSimilar(screenImg.pixelColor(popupXButtonPos), popupXButtonColor, 3) &&
                isColorSimilar(screenImg.pixelColor(popupCheckPos1), popupCheckColor, 3) &&
                isColorSimilar(screenImg.pixelColor(popupCheckPos2), popupCheckColor, 3)) {
                tap(popupXButtonPos.x(), popupXButtonPos.y());
                state = 2;
            } else {
                log("Undefined state");
            }
        } else if (state == 4) {
            if (isColorSimilar(screenImg.pixelColor(adSuccessExitCheck1Pos), adSuccessExitCheck1Color, 3) &&
                isColorSimilar(screenImg.pixelColor(adSuccessExitCheck2Pos), adSuccessExitCheck2Color, 3) &&
                isColorSimilar(screenImg.pixelColor(adSuccessExitCheck2Pos), adSuccessExitCheck2Color, 3)) {
                tap(adSuccessExitPos.x(), adSuccessExitPos.y());
            }
            if (isColorSimilar(screenImg.pixelColor(state0Check1), state0Color1, 2) && isColorSimilar(screenImg.pixelColor(state0Check2), state0Color2, 2))
                state = 0;
        } else if (state == 5) {
            //is captcha wrong?
            if (isColorSimilar(screenImg.pixelColor(wrongCaptchaPos1), wrongCaptchaCol1, 2) &&
                isColorSimilar(screenImg.pixelColor(wrongCaptchaPos2), wrongCaptchaCol2, 2) &&
                isColorSimilar(screenImg.pixelColor(wrongCaptchaPos3), wrongCaptchaCol3, 2)) {
                tap(adSuccessExitPos.x(), adSuccessExitPos.y());
            } else if (isColorSimilar(screenImg.pixelColor(state0Check1), state0Color1, 2) &&
                       isColorSimilar(screenImg.pixelColor(state0Check2), state0Color2, 2)) {
                state = 0;
            } else {
                state = 2;
            }

        }
    }
}

// TRAINING DATA

void backend::getData() {
    QRect rect(captchaCorner1.x(), captchaCorner1.y(), captchaCorner2.x()-captchaCorner1.x(), captchaCorner3.y() - captchaCorner1.y());
    QImage crop = screenImg.copy(rect);
    QString fileName = "raw_images/" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png";
    if (!crop.save(fileName)) {
        log("Failed to save image");
    }
}

QString backend::solveCaptcha(QString imagePath) {
    if (!isModelLoaded) return "ERROR_LOAD";

    // 1. Path Cleanup
    if (imagePath.startsWith("file:///")) imagePath = imagePath.remove(0, 8);
    int queryIndex = imagePath.indexOf("?");
    if (queryIndex != -1) imagePath = imagePath.left(queryIndex);

    // 2. Load & Check Image
    cv::Mat img = cv::imread(imagePath.toStdString());
    if (img.empty()) {
        log("❌ Error: Could not read image path: " + imagePath);
        return "";
    }

    // 3. Create Blob (Standard NCHW)
    cv::Mat blob;
    cv::dnn::blobFromImage(img, blob, 1.0/255.0, cv::Size(128, 64), cv::Scalar(), true, false);

    // 4. Set Input (Matches the name from your python log: 'inputs:0')
    net.setInput(blob, "inputs:0");

    // 5. Forward Pass
    std::vector<cv::Mat> outs;
    try {
        net.forward(outs);
    } catch (cv::Exception &e) {
        log("❌ Inference Crash: " + QString::fromStdString(e.what()));
        return "";
    }

    if (outs.empty()) return "ERROR_NO_OUTPUT";

    // 6. Decode (The Array of 30)
    float* data = (float*)outs[0].data;
    QString result = "";

    for (int i = 0; i < 3; i++) {
        int bestNum = -1;
        float maxProb = -100.0f;
        for (int j = 0; j < 10; j++) {
            int index = (i * 10) + j;
            if (data[index] > maxProb) {
                maxProb = data[index];
                bestNum = j;
            }
        }
        result += QString::number(bestNum);
    }

    log("Captcha Prediction: " + result);
    return result;
}
