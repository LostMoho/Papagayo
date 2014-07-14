#-------------------------------------------------
#
# Project created by QtCreator 2014-06-03T18:13:04
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Papagayo
TEMPLATE = app


SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/lipsyncdoc.cpp \
    src/waveformview.cpp \
    src/mouthview.cpp \
    src/audioextractor.cpp \
    src/breakdowndialog.cpp

INCLUDEPATH += src

win32 {

INCLUDEPATH += libsndfile_1.0.25/Win32/include

LIBS += -LC:\Users\mclifton\Documents\QtDevelopment\Papagayo\libsndfile_1.0.25\Win32\lib -llibsndfile-1
#LIBS += -Llibsndfile_1.0.25\Win32\lib -llibsndfile-1

MY_DLL_FILES += LC:\Users\mclifton\Documents\QtDevelopment\Papagayo\libsndfile_1.0.25\Win32\bin\libsndfile-1.dll

} else {

SOURCES += \
    libsndfile_1.0.19/aiff.c \
    libsndfile_1.0.19/alaw.c \
    libsndfile_1.0.19/au.c \
    libsndfile_1.0.19/audio_detect.c \
    libsndfile_1.0.19/avr.c \
    libsndfile_1.0.19/broadcast.c \
    libsndfile_1.0.19/caf.c \
    libsndfile_1.0.19/chunk.c \
    libsndfile_1.0.19/command.c \
    libsndfile_1.0.19/common.c \
    libsndfile_1.0.19/dither.c \
    libsndfile_1.0.19/double64.c \
    libsndfile_1.0.19/dwd.c \
    libsndfile_1.0.19/dwvw.c \
    libsndfile_1.0.19/file_io.c \
    libsndfile_1.0.19/flac.c \
    libsndfile_1.0.19/float32.c \
    libsndfile_1.0.19/gsm610.c \
    libsndfile_1.0.19/htk.c \
    libsndfile_1.0.19/ima_adpcm.c \
    libsndfile_1.0.19/ima_oki_adpcm.c \
    libsndfile_1.0.19/interleave.c \
    libsndfile_1.0.19/ircam.c \
    libsndfile_1.0.19/macbinary3.c \
    libsndfile_1.0.19/macos.c \
    libsndfile_1.0.19/mat4.c \
    libsndfile_1.0.19/mat5.c \
    libsndfile_1.0.19/mpc2k.c \
    libsndfile_1.0.19/ms_adpcm.c \
    libsndfile_1.0.19/nist.c \
    libsndfile_1.0.19/ogg.c \
    libsndfile_1.0.19/paf.c \
    libsndfile_1.0.19/pcm.c \
    libsndfile_1.0.19/pvf.c \
    libsndfile_1.0.19/raw.c \
    libsndfile_1.0.19/rf64.c \
    libsndfile_1.0.19/rx2.c \
    libsndfile_1.0.19/sd2.c \
    libsndfile_1.0.19/sds.c \
    libsndfile_1.0.19/sndfile.c \
    libsndfile_1.0.19/strings.c \
    libsndfile_1.0.19/svx.c \
    libsndfile_1.0.19/txw.c \
    libsndfile_1.0.19/ulaw.c \
    libsndfile_1.0.19/voc.c \
    libsndfile_1.0.19/vox_adpcm.c \
    libsndfile_1.0.19/w64.c \
    libsndfile_1.0.19/wav_w64.c \
    libsndfile_1.0.19/wav.c \
    libsndfile_1.0.19/wve.c \
    libsndfile_1.0.19/xi.c \
    libsndfile_1.0.19/G72x/g72x.c \
    libsndfile_1.0.19/G72x/g721.c \
    libsndfile_1.0.19/G72x/g723_16.c \
    libsndfile_1.0.19/G72x/g723_24.c \
    libsndfile_1.0.19/G72x/g723_40.c \
    libsndfile_1.0.19/GSM610/add.c \
    libsndfile_1.0.19/GSM610/code.c \
    libsndfile_1.0.19/GSM610/decode.c \
    libsndfile_1.0.19/GSM610/gsm_create.c \
    libsndfile_1.0.19/GSM610/gsm_decode.c \
    libsndfile_1.0.19/GSM610/gsm_destroy.c \
    libsndfile_1.0.19/GSM610/gsm_encode.c \
    libsndfile_1.0.19/GSM610/gsm_option.c \
    libsndfile_1.0.19/GSM610/long_term.c \
    libsndfile_1.0.19/GSM610/lpc.c \
    libsndfile_1.0.19/GSM610/preprocess.c \
    libsndfile_1.0.19/GSM610/rpe.c \
    libsndfile_1.0.19/GSM610/short_term.c \
    libsndfile_1.0.19/GSM610/table.c \
    libsndfile_1.0.19/g72x_src.c

HEADERS += \
    libsndfile_1.0.19/common.h \
    libsndfile_1.0.19/config.h \
    libsndfile_1.0.19/ima_oki_adpcm.h \
    libsndfile_1.0.19/sf_unistd.h \
    libsndfile_1.0.19/sfconfig.h \
    libsndfile_1.0.19/sfendian.h \
    libsndfile_1.0.19/sndfile.h \
    libsndfile_1.0.19/test_main.h \
    libsndfile_1.0.19/wav_w64.h \
    libsndfile_1.0.19/G72x/g72x_priv.h \
    libsndfile_1.0.19/G72x/g72x.h \
    libsndfile_1.0.19/GSM610/config.h \
    libsndfile_1.0.19/GSM610/gsm.h \
    libsndfile_1.0.19/GSM610/gsm610_priv.h \

INCLUDEPATH += libsndfile_1.0.19

}

HEADERS += src/mainwindow.h \
    src/lipsyncdoc.h \
    src/waveformview.h \
    src/audioextractor.h \
    src/mouthview.h \
    src/pg_config.h \
    src/breakdowndialog.h

FORMS += src/mainwindow.ui \
    src/breakdowndialog.ui

OTHER_FILES += \
    ToDo.txt

RESOURCES += \
    rsrc/papagayo.qrc

ICON = mac/papagayo.icns

QMAKE_INFO_PLIST = mac/Info.plist

RC_FILE = win/papagayo.rc

OBJECTIVE_SOURCES +=

extra_libs.files = MY_DLL_FILES
extra_libs.path = $$DESTDIR

INSTALLS += extra_libs
