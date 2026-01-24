#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>

#define TEST_UI


static inline QString getFileName(const QString& path) {
    QFileInfo fileInfo(path);
    return fileInfo.fileName();
}
// 封装⼀个 "宏" 作为打印⽇志的⽅式.
#define TAG QString("[%1:%2]").arg(getFileName(__FILE__),QString::number(__LINE__))
// 默认情况下，qDebug在输出字符串时会⾃动加上引号，可能会⼲扰调试信息的可读性
// noquote是qDebug类的⼀个成员函数，⽤于指定输出时不⾃动为字符串添加引号

#define LOG() qDebug().noquote() << TAG

// 从指定⽂件中, 读取所有的⼆进制内容. 得到⼀个 QByteArray
static inline QByteArray loadFileToByteArray(const QString& path) {
    QFile file(path);
    bool ok = file.open(QFile::ReadOnly);
    if (!ok) {
        LOG() << "⽂件打开失败!";
        return QByteArray();
    }
    QByteArray content = file.readAll();
    file.close();
    return content;
}

// 把 QByteArray 中的内容, 写⼊到某个指定⽂件⾥
static inline void writeByteArrayToFile(const QString& path, const QByteArray&
                                                                 content) {
    QFile file(path);
    bool ok = file.open(QFile::WriteOnly);
    if (!ok) {
        LOG() << "⽂件打开失败!";
        return;
    }
    file.write(content);
    file.flush();
    file.close();
}

// 根据 QByteArray, 转成 QIcon
static inline QIcon makeIcon(const QByteArray& byteArray) {
    QPixmap pixmap;
    pixmap.loadFromData(byteArray);
    QIcon icon(pixmap);
    return icon;
}

// 根据 QByteArray, 转成圆形的 QIcon
static inline QIcon makeCircleIcon(const QByteArray& byteArray, int radius) {
    QPixmap pixmap;
    pixmap.loadFromData(byteArray);
    if (pixmap.isNull()) {
        return QIcon();
    }
    // 把 pixmap 缩放到指定的 2*radius ⼤⼩. IgnoreAspectRatio 忽略⻓宽⽐;
    // SmoothTransformation 平滑缩放, 获得更⾼的图⽚质量, 但是会牺牲⼀定速度.
    pixmap = pixmap.scaled(2*radius, 2*radius, Qt::IgnoreAspectRatio,
                           Qt::SmoothTransformation);
    // 构造绘图设置，可以理解成画图的画布
    QPixmap output = QPixmap(pixmap.size());
    output.fill(Qt::transparent); // 设置透明背景
    QPainter painter(&output);
    painter.setRenderHint(QPainter::Antialiasing);
    // 创建圆形路径
    QPainterPath path;
    path.addEllipse(0, 0, 2*radius, 2*radius);
    // 设置裁剪路径，裁剪路径的作⽤是限制绘图操作的范围，只有在裁剪路径内的区域才会被裁剪
    painter.setClipPath(path);
    // 绘制圆形图⽚
    painter.drawPixmap(0, 0, pixmap);
    // 结束绘制：end()内部会释放与绘图设备相关的资源，确保绘图命令都能够被正确执⾏
    painter.end();
    QIcon icon(output);
    return icon;
}

#endif // UTIL_H
