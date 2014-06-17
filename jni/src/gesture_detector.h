
class QGestureRecognizer
{
public:
    enum ResultFlag
    {
        Ignore           = 0x0001,

        MayBeGesture     = 0x0002,
        TriggerGesture   = 0x0004,
        FinishGesture    = 0x0008,
        CancelGesture    = 0x0010,

        ResultState_Mask = 0x00ff,

        ConsumeEventHint        = 0x0100,
        // StoreEventHint          = 0x0200,
        // ReplayStoredEventsHint  = 0x0400,
        // DiscardStoredEventsHint = 0x0800,

        ResultHint_Mask = 0xff00
    };
    Q_DECLARE_FLAGS(Result, ResultFlag)

    QGestureRecognizer();
    virtual ~QGestureRecognizer();

    virtual QGesture *create(QObject *target);
    virtual Result recognize(QGesture *state, QObject *watched, QEvent *event) = 0;
    virtual void reset(QGesture *state);

    static Qt::GestureType registerRecognizer(QGestureRecognizer *recognizer);
    static void unregisterRecognizer(Qt::GestureType type);
};


class QPanGestureRecognizer : public QGestureRecognizer
{
public:
    QPanGestureRecognizer();

    QGesture *create(QObject *target);
    QGestureRecognizer::Result recognize(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);
};

class QPinchGestureRecognizer : public QGestureRecognizer
{
public:
    QPinchGestureRecognizer();

    QGesture *create(QObject *target);
    QGestureRecognizer::Result recognize(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);
};

class QSwipeGestureRecognizer : public QGestureRecognizer
{
public:
    QSwipeGestureRecognizer();

    QGesture *create(QObject *target);
    QGestureRecognizer::Result recognize(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);
};

class QTapGestureRecognizer : public QGestureRecognizer
{
public:
    QTapGestureRecognizer();

    QGesture *create(QObject *target);
    QGestureRecognizer::Result recognize(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);
};

class QTapAndHoldGestureRecognizer : public QGestureRecognizer
{
public:
    QTapAndHoldGestureRecognizer();

    QGesture *create(QObject *target);
    QGestureRecognizer::Result recognize(QGesture *state, QObject *watched, QEvent *event);
    void reset(QGesture *state);
};
