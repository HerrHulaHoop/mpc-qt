// generic helper module for general-use static functions
#ifndef HELPERS_H
#define HELPERS_H
#include <QObject>
#include <QWidget>
#include <QList>
#include <QUrl>
#include <QUuid>
#include <QOpenGLWidget>

class QFileDialog;
class QLocalServer;
class QOpenGLTexture;

namespace Helpers {
    QString toDateFormat(double time);
    enum DisabledTrack { NothingDisabled, DisabledAudio, DisabledVideo };
    enum Subtitles { NoSubtitles, SubtitlesPresent, SubtitlesDisabled, };
    enum FileType { AudioFile, VideoFile };
    QString parseFormat(QString fmt, QString fileName, DisabledTrack disabled,
                        Subtitles subtitles, double timeNav, double timeBegin,
                        double timeEnd);
}

class JsonServer : public QObject
{
    Q_OBJECT
public:
    explicit JsonServer(QObject *parent = 0);
    bool sendPayload(const QByteArray &payload);

private:
    void listen();

signals:
    void payloadReceived(const QByteArray &payload);

private slots:
    void server_newConnection();

private:
    QString socketName;
    QLocalServer *server;
};

class LogoDrawer : public QObject {
    Q_OBJECT
public:
    explicit LogoDrawer(QObject *parent = 0);
    ~LogoDrawer();
    void setLogoUrl(const QString &filename);
    void resizeGL(int w, int h);
    void paintGL(QOpenGLWidget *widget);

private:
    void regenerateTexture();

private:
    QRectF logoLocation;
    QImage logo;
    QString logoUrl;
};

class LogoWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit LogoWidget(QWidget *parent = 0);
    ~LogoWidget();
    void setLogo(const QString &filename);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

private:
    LogoDrawer *logoDrawer;
    QString logoUrl;
};

class DisplayNode;
class DisplayParser {
public:
    DisplayParser();
    ~DisplayParser();

    void takeFormatString(QString fmt);
    QString parseMetadata(QVariantMap metaData, QString displayString,
                          Helpers::FileType fileType);
private:
    DisplayNode *node;
};

class TrackInfo {
public:
    TrackInfo() {}
    TrackInfo(const QUrl &url, const QUuid &list, const QUuid &item);
    QUrl url;
    QUuid list;
    QUuid item;
    QVariantMap toVMap() const;
    void fromVMap(const QVariantMap &map);
    bool operator ==(const TrackInfo &track) const;
};

class MouseState {
public:
    MouseState();
    MouseState(const MouseState &m);
    MouseState(int button, int mod, bool press);

    // Components
    int button;
    int mod;
    bool press;

    // to Qt notation functions
    Qt::MouseButtons mouseButtons() const;
    Qt::KeyboardModifiers keyModifiers() const;
    bool isPress();
    bool isWheel();

    // I/O functions
    QString toString() const;
    QVariantMap toVMap() const;
    void fromVMap(const QVariantMap &map);

    // Hashing-related functions
    uint mouseHash() const;
    bool operator ==(const MouseState &other) const;
    bool operator !() const;        // le saef bull eyediom faec

    // Conversion functions
    static MouseState fromWheelEvent(QWheelEvent *event);
    static MouseState fromMouseEvent(QMouseEvent *event, bool press);

    // Display mapping vars
    static QStringList buttonToText;
    static QStringList modToText;
    static QStringList multiModToText;
    static QStringList pressToText;
};

inline uint qHash(const MouseState &m, uint seed) {
    Q_UNUSED(seed);
    return m.mouseHash();
}

typedef QHash<MouseState, QAction*> MouseStateMap;

class Command {
public:
    Command();
    Command(QAction *a, MouseState mf, MouseState mw);

    // Components
    QAction *action;
    QKeySequence keys;      // taken from the QAction in constructor
    MouseState mouseFullscreen;
    MouseState mouseWindowed;

    // I/O functions
    QString toString() const;
    QVariantMap toVMap() const;
    void fromVMap(const QVariantMap &map);

    // Conversion functions
    void fromAction(QAction *a);
};



#endif // HELPERS_H
