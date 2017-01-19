#pragma once
//------------------------------------------------------------------------------------------
#ifdef __APPLE__
#define __BNN_RUN_MAIN_WINDOW __BNN_RUN_MAIN_WINDOW_MAC
#else
#define __BNN_RUN_MAIN_WINDOW __BNN_RUN_MAIN_WINDOW_WIN
#endif


#define __BNN_RUN_MAIN_WINDOW_WIN(MainWindowClass, argc, argv) \
{ \
    QSurfaceFormat format; \
    format.setDepthBufferSize(24); \
    format.setStencilBufferSize(8); \
    format.setVersion(4, 5); \
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); \
    format.setProfile(QSurfaceFormat::CoreProfile); \
    format.setSamples(4); \
    QSurfaceFormat::setDefaultFormat(format); \
    QApplication a(argc, argv); \
    MainWindowClass w; \
    w.show(); \
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, \
                                      w.size(), \
                                      qApp->desktop()->availableGeometry())); \
    return a.exec(); \
}

#define __BNN_RUN_MAIN_WINDOW_MAC(MainWindowClass, argc, argv) \
{ \
    QSurfaceFormat format; \
    format.setDepthBufferSize(24); \
    format.setStencilBufferSize(8); \
    format.setVersion(4, 1); \
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); \
    format.setProfile(QSurfaceFormat::CoreProfile); \
    format.setSamples(4); \
    QSurfaceFormat::setDefaultFormat(format); \
    QApplication a(argc, argv); \
    MainWindowClass w; \
    w.show(); \
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, \
                                      w.size(), \
                                      qApp->desktop()->availableGeometry())); \
    return a.exec(); \
}


