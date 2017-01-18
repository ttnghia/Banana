//------------------------------------------------------------------------------------------
//            .-..-.
//           (-o/\o-)
//          /`""``""`\
//          \ /.__.\ /
//           \ `--` /                                                 Created on: 9/23/2016
//            `)  ('                                                    Author: Nghia Truong
//         ,  /::::\  ,
//         |'.\::::/.'|
//        _|  ';::;'  |_
//       (::)   ||   (::)                       _.
//        "|    ||    |"                      _(:)
//         '.   ||   .'                       /::\
//           '._||_.'                         \::/
//            /::::\                         /:::\
//            \::::/                        _\:::/
//             /::::\_.._  _.._  _.._  _.._/::::\
//             \::::/::::\/::::\/::::\/::::\::::/
//               `""`\::::/\::::/\::::/\::::/`""`
//                    `""`  `""`  `""`  `""`
//------------------------------------------------------------------------------------------
#ifndef __MANGO_MACRO_H__
#define __MANGO_MACRO_H__
//------------------------------------------------------------------------------------------
#ifdef __APPLE__
#define __MANGO_RUN_MAIN_WINDOW __MANGO_RUN_MAIN_WINDOW_MAC
#else
#define __MANGO_RUN_MAIN_WINDOW __MANGO_RUN_MAIN_WINDOW_WIN
#endif


#define __MANGO_RUN_MAIN_WINDOW_WIN(MainWindowClass, argc, argv) \
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

#define __MANGO_RUN_MAIN_WINDOW_MAC(MainWindowClass, argc, argv) \
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



//------------------------------------------------------------------------------------------
#endif // __MANGO_MACRO_H__
