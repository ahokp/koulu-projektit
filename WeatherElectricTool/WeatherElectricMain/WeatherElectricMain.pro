QT += \
    core \
    gui \
    charts \
    network \
    xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataImporting/datasegmenter.cpp \
    addgraphform.cpp \
    cachefilehandler.cpp \
    dataconnector.cpp \
    datasourcewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    weatherbar.cpp \
    weatherchartbase.cpp \
    weathergraph.cpp \
    DataImporting/dataimporter.cpp \
    DataImporting/xmlfetcher.cpp \
    DataImporting/xmldataimporter.cpp \
    DataImporting/fmidataimporter.cpp \
    DataImporting/fingriddataimporter.cpp \
    weatherpie.cpp

HEADERS += \
    DataImporting/datasegmenter.hh \
    addgraphform.h \
    cachefilehandler.h \
    dataconnector.h \
    datasourcewidget.hh \
    mainwindow.h \
    weatherbar.hh \
    weatherchartbase.hh \
    weathergraph.hh \
    DataImporting/dataimporter.hh \
    DataImporting/xmlfetcher.hh \
    DataImporting/xmldataimporter.hh \
    DataImporting/fmidataimporter.hh \
    DataImporting/fingriddataimporter.hh \
    weatherpie.hh

FORMS += \
    addgraphform.ui \
    mainwindow.ui

# Copy Fingrid API key to build folder
copyApiKey.commands = $(COPY_FILE) \"$$shell_path($$PWD/DataImporting/FINGRID_API_KEY.txt)\" \"$$shell_path($$OUT_PWD)\"
first.depends = $(first) copyApiKey
export(first.depends)
export(copyApiKey.commands)
QMAKE_EXTRA_TARGETS += first copyApiKey

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    dataSets
