#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <QObject>
#include <QMouseEvent>
#include <QRandomGenerator>

const int WIDTH  = 640;
const int HEIGHT = 360;

// Define a function to map a value from one range to another
float map(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    // Check if the value is outside of the input range
    if (value < fromLow || value > fromHigh) {
        qWarning("Value is outside of the input range");
        return 0.0f; // Return a default value or handle the error appropriately
    }
    // Perform linear interpolation to map the value to the output range
    return toLow + (value - fromLow) * ((toHigh - toLow) / (fromHigh - fromLow));
}

QColor returnColor(int value) {
    return QColor(255 - (value * 20), 255 - (value * 20), 255 - (value * 20));
}

struct Drop {
    int m_x, m_z, m_len;
    float m_y, m_yspeed;
    Drop() {
        m_x      = QRandomGenerator::global()->bounded(WIDTH); //width
        m_y      = QRandomGenerator::global()->bounded(450) - 500;
        m_z      = QRandomGenerator::global()->bounded(20);
        m_len    = map(m_z, 0, 20, 10, 20);
        m_yspeed = map(m_z, 0, 20,  1, 20);
    }

    void update(float dt = 1/60) {
        m_y = m_y + m_yspeed;
        float grav = map(m_z, 0, 20, 0, 0.2);
        m_yspeed = m_yspeed + (grav*dt);
        if (m_y > HEIGHT) { //height
            m_y = QRandomGenerator::global()->bounded(100) - 200;
            m_yspeed = map(m_z, 0, 20, 4, 10);
        }
    }

    void draw(QPainter *painter) {
        painter->save();
        float thick = map(m_z, 0, 20, 1, 3);
        int roundedThick = qRound(thick);
        QColor color = returnColor(m_z);
        painter->setPen(QPen(color));
        painter->setBrush(QBrush(color));
        painter->drawRect(m_x, m_y, thick, m_len + roundedThick);
        painter->restore();
    }
};


class Window : public QWidget
{
    Q_OBJECT
public:
    Window(int w, int h, QWidget* parent = nullptr) : QWidget(parent)
    {
        setWindowTitle(QStringLiteral("Falling Rain Effect Qt/C++"));
        resize(w, h);
        for(int i = 0; i < 500; ++i) {
            m_drop[i] = new Drop();
        }

        QObject::connect(&m_timer, &QTimer::timeout, this, &Window::animationLoop);
        m_timer.start(1000.f/60.f); //60 times per second
        m_elapsedTimer.start();
    }
    virtual ~Window(){
        delete[] *m_drop;
    }

private:
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    Drop* m_drop[500];

private slots:
    void animationLoop()
    {
        update();
        for(int i = 0; i < 500; ++i) {
            m_drop[i]->update();
        }
    }
private:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter qp(this);
        qp.setBrush(QBrush(QColor(230, 230, 250)));
        qp.drawRect(0,0,size().width(), size().height());

        for (int j = 0; j < 20; ++j) {
            for(int i = 0; i < 500; ++i) {
                if(m_drop[i]->m_z == j) {
                    m_drop[i]->draw(&qp);
                }
            }
        }
    }

};

#include "main.moc"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    Window *w = new Window(WIDTH, HEIGHT);
    w->show();
    a.exec();
}


