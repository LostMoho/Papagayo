#ifndef SPINEDOC_H
#define SPINEDOC_H

#include <QString>
#include "lipsyncdoc.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTreeWidgetItem>
#include <QtWidgets>
/**
 * @brief the SpineDoc class is used to manage import/export of spine documents
 * @author @author Jakob Wilson - www.jakobwesley.com
 */
class SpineDoc
{
public:
    SpineDoc(int _width, int _height);  /**creates a new document that is blank*/
    SpineDoc(QString filename = NULL);  /**Creates a new document from an existing json file*/
    ~SpineDoc();
    QString JsonExport();               /**Returns a string representation of the exported json file*/

    void AddVoice(LipsyncVoice* voice, QString parent);       /**Adds a lip sync voice to the document*/
    rapidjson::Document spineData;
    bool exportWords;
    int width;
    int height;

    void Export(QString filename);
    QStandardItemModel* GetModel();

private:
    void AddBones(QString parent);
    void AddSlots();
    void AddSkins();
    void AddWordEvents(LipsyncVoice* voice);
    void AddAnimations(LipsyncVoice* voice);
    QStandardItem* _FindBoneInItem(QStandardItem* root, QString text);
};

#endif // SPINEDOC_H
