#include "backend.h"
backend::backend(QObject *parent)
    : QObject{parent}
{
    adbPath = QDir::currentPath() + "/adb.exe";
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &backend::newloop);

    // Start OCR server
    QString pythonPath = QDir::currentPath() + "/ocr_env/Scripts/python.exe";
    QString scriptPath = QDir::currentPath() + "/ocr.py";
    ocrProcess = new QProcess(this);
    ocrProcess->start(pythonPath, QStringList() << scriptPath);
    ocrProcess->waitForReadyRead(15000); // wait until "OCR server ready"
    log("OCR server started");
}

backend::~backend(){
    if (timer && timer->isActive()) {
        timer->stop();
    }a
    if (ocrProcess && ocrProcess->state() != QProcess::NotRunning) {
        log("Stopping OCR server...");
        ocrProcess->terminate(); // Send termination signal (SIGTERM/WM_CLOSE)
        if (!ocrProcess->waitForFinished(3000)) { // Wait up to 3 seconds
            ocrProcess->kill();
        }
    }
    if (adbConnected) {
        QProcess adbProcess;
        QStringList args;
        args << "disconnect" << adbPortAddress;
        adbProcess.start(adbPath, args);
        adbProcess.waitForFinished(2000);
        log("Disconnected from BlueStacks ADB.");
    }
}
void backend::captureScreen() {
    QProcess process;
    QStringList args;
    args << "-s" << adbPortAddress << "shell" << "screencap" << "-p" << "/sdcard/screenshot.png";
    process.start(adbPath, args);
    process.waitForFinished();

    QProcess pullProcess;
    QStringList pullArgs;
    pullArgs << "-s" << adbPortAddress << "pull" << "/sdcard/screenshot.png" << "screenshot.png";
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
    if (adbConnected) {
        args << "disconnect" << adbPortAddress;
    } else {
        args << "connect" << adbPortAddress;
    }
    adbConnected = !adbConnected;
    process.start(adbPath, args);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    backend::log("Output: " + output);
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
    timer->stop();
}
int backend::checkState() {
    captureScreen();
    QPoint adWatchButtonLoc = findTemplate(screenImg, QString("adWatchButton"));
    QPoint XButton1Loc = findTemplate(screenImg, QString("XButton1"));
    QPoint XButton1Loc_black = findTemplate(screenImg, QString("XButton1_black"));
    QPoint XButton1Loc_skip = findTemplate(screenImg, QString("XButton1_skip"));
    QPoint XButton2Loc = findTemplate(screenImg, QString("XButton2"));
    QPoint captchaTextbox = findTemplate(screenImg, QString("captchaTextbox"));
    QPoint successScreen = findTemplate(screenImg, QString("successScreen"));
    QPoint adContinueScreen = findTemplate(screenImg, QString("adContinueScreen"));
    QPoint adWaitingScreen = findTemplate(screenImg, QString("adWaitingScreen"));
    if (adContinueScreen.x()!=-1) {
        return 5;
    } else if (XButton2Loc.x()!= -1) {
        return 3;
    } else if (XButton1Loc.x()!=-1 || XButton1Loc_black.x() != -1 || XButton1Loc_skip.x()!=-1) {
        if ((XButton1Loc.x()<100 && XButton1Loc.x()>0) || (XButton1Loc_black.x() <100 && XButton1Loc_black.x() > 0) || (XButton1Loc_skip.x()<100 && XButton1Loc_skip.x() > 0)) {
            return 21;
        } else {
            return 22;
        }
    } else if (adWaitingScreen.x()!=-1) {
        return 20;
    } else if (adWatchButtonLoc.x()!=-1) {
        return 0;
    } else if (captchaTextbox.x() != -1) {
        return 1;
    } else if (successScreen.x()!= -1) {
        return 4;
    }
    return 2;
}
void backend::newloop() {
    int checkedState = checkState();
    if (checkedState == 22) {
        log("state 2: Ad / Waiting state");
        tap(XButton1.x(), XButton1.y());
        log("Tap at XButton1");
    } else if (checkedState == 21) {
        log("state 2: Ad / Waiting state");
        tap(XButton1_left.x(), XButton1_left.y());
        log("Tap at XButton1_left");
    } else if (checkedState == 20) {
        log("state 2: ad waiting screen");
    } else if (checkedState == 0) {
        log("state 0: main screen");
        tap(adWatchButton.x(), adWatchButton.y());
        log("Tapped at adWatchButton");
    } else if (checkedState == 1) {
        log("state 1: captcha");
        QRect captchaRec = QRect(captchaCorner1, captchaCorner2).normalized();
        QImage captchaImg = screenImg.copy(captchaRec);
        captchaImg.save("temp_captcha.png");
        QString code = solveCaptcha(QDir::currentPath() + "/temp_captcha.png");
        if (code.length() == 5) {
            tap(captchaTextbox.x(), captchaTextbox.y());
            log("Tapped at captchaTextbox");
            QThread::msleep(300);
            QProcess process;
            QStringList args;
            args << "-s" << adbPortAddress << "shell" << "input" << "text" << code;
            process.start(adbPath, args);
            process.waitForFinished();
            tap(captchaConfirm.x(), captchaConfirm.y());
            log("Tapped at captchaConfirm");
        }
        else {
            tap(XButton1.x(), XButton1.y());
            log("Tapped at XButton1. Captcha retry.");
        }
    } else if (checkedState == 3) {
        log("state 3: Ad popup");
        tap(XButton2.x(), XButton2.y());
        log("Tap at XButton2");
    } else if (checkedState == 4) {
        log("State 4: Success screen");
        tap(XButton1.x(), XButton1.y());
        log("Tapped at XButton1");
    } else if (checkedState == 5) {
        log("State 5: Continue Ad screen");
        tap(continueAdButton.x(), continueAdButton.y());
        log("Tapped at continue button.");
    } else {
        log("Undefined state");
    }
}

void backend::setLocXButton1(QPoint point) {
    XButton1 = point;
}
void backend::setLocXButton1_left(QPoint point) {
    XButton1_left = point;
}

void backend::setLocXButton2(QPoint point) {
    XButton2 = point;
}
void backend::setAdWatchButton(QPoint point) {
    adWatchButton = point;
}
void backend::setCaptchaTextbox(QPoint point) {
    captchaTextbox = point;
};
void backend::setCaptchaConfirm(QPoint point) {
    captchaConfirm = point;
};
void backend::updatePort(QString port) {
    adbPortAddress = port;
}
void backend::setContinueAd(QPoint point) {
    continueAdButton = point;
}


QString backend::solveCaptcha(QString imagePath) {
    if (imagePath.startsWith("file:///")) imagePath = imagePath.remove(0, 8);
    imagePath = QDir::toNativeSeparators(imagePath);
    // URL encode backslashes
    QString encoded = imagePath.replace("\\", "%5C");

    QProcess process;
    process.start("curl", QStringList() << "-s" << ("http://localhost:9119/?path=" + encoded));
    process.waitForFinished(10000);

    QString result = process.readAllStandardOutput().trimmed();

    log("Captcha prediction: " + result);
    return result;
}

bool backend::matchTemplate(QImage sourceTemplate, QString templateName) {
    // Convert to 3 channels of RGB instead of 4 channels
    QImage converted = sourceTemplate.convertToFormat(QImage::Format_RGB888);
    // Convert to cv::Mat. constBits() returns const uchar* type. const_cast turns to non-const uchar* type. converted.bytesPerLine is the byte per line so that it doesn't turn the invisible padding bytes into the image.
    cv::Mat image(converted.height(), converted.width(), CV_8UC3, const_cast<uchar*>(converted.constBits()), converted.bytesPerLine());

    QDir templatePath("./templates");
    QStringList filter;
    filter << QString("*%1*").arg(templateName);
    QFileInfoList files = templatePath.entryInfoList(filter, QDir::Files);
    for (const QFileInfo &file : files) {
        cv::Mat temp = cv::imread(file.absoluteFilePath().toStdString(), cv::IMREAD_COLOR);
        if (temp.empty()) continue;
        if (temp.cols > image.cols || temp.rows > image.rows) continue;
        cv::cvtColor(temp, temp, cv::COLOR_BGR2RGB);
        cv::Mat results;
        cv::matchTemplate(image, temp, results, cv::TM_CCOEFF_NORMED);

        double maxVal;
        cv::Point maxLoc;
        cv::minMaxLoc(results, nullptr, &maxVal, nullptr, &maxLoc);
        if (maxVal >= 0.8) return true;
    }
    return false;
}

QPoint backend::findTemplate(QImage sourceTemplate, QString templateName) {
    QImage converted = sourceTemplate.convertToFormat(QImage::Format_RGB888);
    // Convert to cv::Mat. constBits() returns const uchar* type. const_cast turns to non-const uchar* type. converted.bytesPerLine is the byte per line so that it doesn't turn the invisible padding bytes into the image.
    cv::Mat image(converted.height(), converted.width(), CV_8UC3, const_cast<uchar*>(converted.constBits()), converted.bytesPerLine());
    QDir templatePath("./templates");
    QStringList filter;
    filter << QString("*%1*").arg(templateName);
    QFileInfoList files = templatePath.entryInfoList(filter, QDir::Files);
    for (const QFileInfo &file : files) {
        cv::Mat temp = cv::imread(file.absoluteFilePath().toStdString(), cv::IMREAD_COLOR);
        if (temp.empty()) continue;
        if (temp.cols > image.cols || temp.rows > image.rows) continue;
        cv::cvtColor(temp, temp, cv::COLOR_BGR2RGB);
        cv::Mat results;
        cv::matchTemplate(image, temp, results, cv::TM_CCOEFF_NORMED);

        double maxVal;
        cv::Point maxLoc;
        cv::minMaxLoc(results, nullptr, &maxVal, nullptr, &maxLoc);
        if (maxVal >= 0.8) return QPoint(maxLoc.x, maxLoc.y);
    }
    return QPoint(-1, -1);
}

void backend::log(QString text) {
    qDebug() << text;
    emit logUpdated(QString("%1").arg(text));
}

void backend::tap(int x, int y) {
    QProcess process;
    QStringList args;
    args << "-s" << adbPortAddress << "shell" << "input" << "tap" << QString::number(x) << QString::number(y);

    process.start(adbPath, args);
    process.waitForFinished();
}
void backend::captureWidget(QString widgetName) {
    QRect rectangle = QRect(captureWidget1, captureWidget2).normalized();
    QDir directory = QDir("./templates");
    if (!directory.exists()) {
        if (!directory.mkpath(".")) {
            log("Can't create templates directory.");
        }
    }
    QImage croppedImg = screenImg.copy(rectangle);
    QString filePath = directory.filePath(widgetName+".png");
    if (croppedImg.save(filePath)) {
        log(QString("Saved %1.png successfully").arg(widgetName));
    } else {
        log(QString("Failed to save %1.png").arg(widgetName));
    }
}
QPoint backend::getPos(int x, int y, double width, double height, int mode, int clickCount, QString widgetName) {
    if (screenImg.isNull()) log("No Image");
    int realX = x * (screenImg.width() / width);
    int realY = y * (screenImg.height() / height);
    if (realX < 0 || realX > screenImg.width() || realY < 0 || realY > screenImg.height()) {
        log("Position out of bounds.");
    } else {
        QColor color = screenImg.pixelColor(realX, realY);
        if (mode == 0) {
            log(QString("Pixel: %1 x %2 | Color: Hex %3 | RGB %4, %5, %6").arg(realX).arg(realY).arg(color.name()).arg(color.red()).arg(color.green()).arg(color.blue()));
            return QPoint(realX, realY);
        } else if (mode == 1) {
            setLocXButton1(QPoint(realX, realY));
            log(QString("Set X button 1 to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 2) {
            setLocXButton2(QPoint(realX,realY));
            log(QString("Set X button 2 to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 3) {
            setAdWatchButton(QPoint(realX,realY));
            log(QString("Set Ad Watch Button to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 4) {
            setCaptchaTextbox(QPoint(realX,realY));
            log(QString("Set Captcha Textbox to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 5) {
            setCaptchaConfirm(QPoint(realX,realY));
            log(QString("Set Captcha Confirm to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 6) {
            if (clickCount == 1) captureWidget1 = QPoint(realX, realY);
            else if (clickCount == 2) {
                captureWidget2 = QPoint(realX, realY);
                captureWidget(widgetName);
            }
        } else if (mode == 7) {
            setContinueAd(QPoint(realX,realY));
            log(QString("Set Continue Ad Button to pixel %1 x %2").arg(realX).arg(realY));
        } else if (mode == 8) {
            setLocXButton1_left(QPoint(realX, realY));
            log(QString("Set X Button (left) to pixel %1 x %2").arg(realX).arg(realY));
        }
    }
    return QPoint(-1, -1);
}


