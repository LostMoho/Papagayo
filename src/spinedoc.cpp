
/**
 * @brief the SpineDoc class is used to manage import/export of spine documents
 * @author Jakob Wilson - www.jakobwesley.com
 */

#include "spinedoc.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <QMessageBox>

using namespace rapidjson;
using namespace std;


/**-----------------------------------------------------------------------------------*/
/**
 * @brief SpineDoc::SpineDoc overloaded constructor for createing a new spine skeleton
 */
SpineDoc::SpineDoc(int _width, int _height)
{
    this->width = _width;
    this->height = _height;
    this->exportWords = true;

    this->spineData.Parse("{ }");//create a blank doc
    Document::AllocatorType& allocator = this->spineData.GetAllocator();

    Value skeleton(kObjectType);
    skeleton.AddMember("hash", "KebD3MyURSyaMpyRinWkDQEAuYk", allocator);
    skeleton.AddMember("spine", "2.1.27", allocator);
    skeleton.AddMember("width", this->width, allocator);
    skeleton.AddMember("height", this->height, allocator);

    this->spineData.AddMember("skeleton", skeleton, allocator);
}

/**-----------------------------------------------------------------------------------*/
/**
 * @brief SpineDoc::SpineDoc Creates a Spine document from a file
 * @param filename - the file to open
 */
SpineDoc::SpineDoc(QString filename)
{
    QFile	f(filename);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(0, "error", "Could not open json file");
        return;
    }

    qDebug()<<"Reading json";
    QTextStream in(&f);
    QString json = in.readAll();

    if(this->spineData.Parse(json.toStdString().c_str()).HasParseError())
    {
        qDebug()<<"Could not parse json file!!";
        this->spineData.Parse("{ }");       //create an empty file
        QMessageBox::information(0, "error", "Could not open json file");
    }

    //width and height to match default images
    this->width = 200;
    this->height = 200;
    this->exportWords = true;
}


/**-----------------------------------------------------------------------------------*/
/**
 * @brief SpineDoc::~SpineDoc - Destructor for any cleanup of allocated objects/data
 */
SpineDoc::~SpineDoc()
{

}

/**-----------------------------------------------------------------------------------*/
/**
 * @brief SpineDoc::JsonExport writes the json document to a Qstring and returns it
 * @return QString - the formatted json file
 */
QString SpineDoc::JsonExport()
{
   StringBuffer buffer;
   Writer<StringBuffer> writer(buffer);
   this->spineData.Accept(writer);
   return QString(buffer.GetString());
}


/**-----------------------------------------------------------------------------------*/
/**
  * @brief SpineDoc::AddVoice Adds a LipsyncVoice to the spine document. This means that a user can have multiple characters in a converstation and export their voices to
  * different skeletons for conversations with multiple characters
  * @param voice
  * @TODO: break down into several smaller methods
  */
 void SpineDoc::AddVoice(LipsyncVoice* voice, QString parent = NULL)
 {


     this->AddBones(parent); //add the head bone
     this->AddSlots(); //add the animations slots               //problem in slots section
     this->AddSkins(); //add the mouthshape skin
     this->AddAnimations(voice); //add voice keyframes
     if(this->exportWords)         //if desired, export words as events
       this->AddWordEvents(voice);

 }

 /**-----------------------------------------------------------------------------------*/
 /**
  * @brief SpineDoc::AddBones sets up bones in the file
  */
 void SpineDoc::AddBones(QString parent = NULL)
 {
    Document::AllocatorType& allocator = this->spineData.GetAllocator();

    Value bones(kArrayType);
    Value rootBone(kObjectType);
    rootBone.AddMember("name", "mouth", allocator);
    if(parent != NULL)
        rootBone.AddMember("parent", Value(parent.toStdString().c_str(), allocator).Move(), allocator);

    if(this->spineData.HasMember("bones"))
        this->spineData["bones"].PushBack(rootBone, allocator);
    else
    {
        bones.PushBack(rootBone, allocator);
        this->spineData.AddMember("bones", bones, allocator);
    }


 }

 /**-----------------------------------------------------------------------------------*/
 /**
  * @brief SpineDoc::AddSlots adds necessary slot positions to file
  */
 void SpineDoc::AddSlots()
 {
     Document::AllocatorType& allocator = this->spineData.GetAllocator();


     Value mouthslot(kObjectType);
        mouthslot.AddMember("name", "MouthShape", allocator);
        mouthslot.AddMember("bone", "mouth", allocator);
        mouthslot.AddMember("attachment", "rest", allocator);


     if(!this->spineData.HasMember("slots"))
     {
         Value mouthslots(kArrayType);
         mouthslots.PushBack(mouthslot, allocator);

        this->spineData.AddMember("slots", mouthslots, allocator);//TODO make array
     }
     else
         this->spineData["slots"].PushBack(mouthslot, allocator);



 }

/**-----------------------------------------------------------------------------------*/
 /**
  * @brief SpineDoc::AddSkins adds a skin with all phonemes
  */
 void SpineDoc::AddSkins()
 {
     Document::AllocatorType& allocator = this->spineData.GetAllocator();

     //add all phonems to mouthshape
    Value phonemes(kObjectType);
    string vals[] = {"AI", "E", "FV", "L", "MBP", "O", "U", "WQ", "etc", "rest"};
    int i;
    for(i = 0; i < 10; i++)
    {
        Value phoneme(kObjectType);
        phoneme.AddMember("width", this->width, allocator);
        phoneme.AddMember("height", this->height, allocator);

        phonemes.AddMember(Value(vals[i].c_str(), allocator).Move(), phoneme , allocator);
    }

    if(!this->spineData.HasMember("skins") || !this->spineData["skins"].HasMember("MouthShape"))
    {
        Value def(kObjectType);
        def.AddMember("MouthShape", phonemes, allocator);

        if(this->spineData.HasMember("skins"))
               this->spineData["skins"].AddMember("default", def, allocator);
        else
        {
            Value skins(kObjectType);
            skins.AddMember("default", def, allocator);
            this->spineData.AddMember("skins", skins, allocator);
        }
    }
 }

 /**-----------------------------------------------------------------------------------*/
 /**
  * @brief SpineDoc::AddWordEvents - adds word events that display in spine
  */
 void SpineDoc::AddWordEvents(LipsyncVoice* voice)
 {
     Document::AllocatorType& allocator = this->spineData.GetAllocator();
     QString    word,   nextWord;

      string animName = voice->fName.toStdString();
      qDebug()<<"Set name to "<<animName.c_str();

     int startFrame = 0;
     int endFrame = 1;
     int        fps = 30;
     double time = 0.0f;
     word = voice->GetWordAtFrame(0);
     if (voice->fPhrases.size() > 0)
     {
         startFrame = voice->fPhrases[0]->fStartFrame;
         endFrame = voice->fPhrases.last()->fEndFrame;
     }

     Value events(kObjectType);
     Value animEvents(kArrayType);


     for (int frame = startFrame; frame <= endFrame; frame++)
     {
        nextWord = voice->GetWordAtFrame(frame);
        time = (double)(frame)/(double)fps;

        if(nextWord != word && nextWord != "")
        {
            //add event to events list
            word = nextWord;
            Value empty(kObjectType);

            if(this->spineData.HasMember("events"))
                this->spineData["events"].AddMember(Value(word.toStdString().c_str(), allocator).Move(), empty, allocator);
            else
                events.AddMember(Value(word.toStdString().c_str(), allocator).Move(), empty, allocator);

            //add event to keyframes
            Value keyFrame(kObjectType);
            keyFrame.AddMember("time", time, allocator);
            keyFrame.AddMember("name", Value(word.toStdString().c_str(), allocator).Move(), allocator);
            animEvents.PushBack(keyFrame, allocator);
        }
     }

     if(!this->spineData.HasMember("events"))
        this->spineData.AddMember("events", events, allocator);


    Value animation(kObjectType);
    Value animations(kObjectType);

    if(this->spineData.HasMember("animations"))
    {
        if(this->spineData["animations"].HasMember(animName.c_str()))
        {
            this->spineData["animations"][animName.c_str()].AddMember("events", animEvents, allocator);
        }
        else
        {
            animations.AddMember( Value(animName.c_str(), allocator).Move(), animation, allocator);
            this->spineData["animations"].AddMember(Value(animName.c_str(), allocator).Move(), animation, allocator);
        }
    }
    else
    {
        animation.AddMember("events", animEvents, allocator);
        animations.AddMember(Value(animName.c_str(), allocator).Move(), animation, allocator);
        this->spineData.AddMember("animations", animations, allocator);
    }
 }

/**-----------------------------------------------------------------------------------*/
 /**
  * @brief SpineDoc::AddAnimations - adds all voice animation events
  * @param voice - the voice we want to add to the animation
  */
 void SpineDoc::AddAnimations(LipsyncVoice* voice)
 {   
     Document::AllocatorType& allocator = this->spineData.GetAllocator();
     QString    phoneme,   nextPhoneme;

     int startFrame = 0;
     int endFrame = 1;
     int fps = 30;
     double time = 0.0f;
     phoneme = voice->GetPhonemeAtFrame(0);
     string animName = voice->fName.toStdString();
     qDebug()<<"Set Animation name to "<<animName.c_str();
     if (voice->fPhrases.size() > 0)
     {
         startFrame = voice->fPhrases[0]->fStartFrame;
         endFrame = voice->fPhrases.last()->fEndFrame;
     }

     Value attachment(kArrayType);

     for (int frame = startFrame; frame <= endFrame; frame++)
     {
        nextPhoneme = voice->GetPhonemeAtFrame(frame);
        time = (double)(frame)/(double)fps;

        if(nextPhoneme != phoneme)
        {
            phoneme = nextPhoneme;
            //add event to keyframes
            Value keyFrame(kObjectType);
            keyFrame.AddMember("time", time, allocator);
            keyFrame.AddMember("name", Value(phoneme.toStdString().c_str(), allocator).Move(), allocator);
            attachment.PushBack(keyFrame, allocator);
        }
     }





    // @todo: create a method for handling finding and inserting nested items so this code isn'y necessary

    Value mouthShape(kObjectType);
    Value slot(kObjectType);
    Value animation(kObjectType);
    Value animations(kObjectType);


    if(this->spineData.HasMember("animations"))
    {
        if(this->spineData["animations"].HasMember(animName.c_str()))
        {
            if(this->spineData["animations"][animName.c_str()].HasMember("slots"))
            {
                mouthShape.AddMember("attachment", attachment, allocator);
                this->spineData["animations"][animName.c_str()]["slots"].AddMember("MouthShape", mouthShape, allocator);
            }
            else
            {
                mouthShape.AddMember("attachment", attachment, allocator);
                slot.AddMember("MouthShape", mouthShape, allocator);

                this->spineData["animations"][animName.c_str()].AddMember("slots", slot, allocator);//NOTE change
            }
        }
        else
        {
            mouthShape.AddMember("attachment", attachment, allocator);
            slot.AddMember("MouthShape", mouthShape, allocator);
            animation.AddMember("slots", slot, allocator);

            this->spineData["animations"].AddMember(Value(animName.c_str(), allocator).Move(), animation, allocator);
        }

    }
    else
    {
        mouthShape.AddMember("attachment", attachment, allocator);
        slot.AddMember("MouthShape", mouthShape, allocator);
        animation.AddMember("slots", slot, allocator);
        animations.AddMember(Value(animName.c_str(), allocator).Move(), animation, allocator);
        this->spineData.AddMember("animations", animations, allocator);
    }
 }

 /**-----------------------------------------------------------------------------------*/
 /**
 * @brief SpineDoc::Export exports the document in it's current state
 * @param filename - the file path to export to
 */
void SpineDoc::Export(QString filename)
{
    StringBuffer sb;
   PrettyWriter<StringBuffer> writer(sb);
   this->spineData.Accept(writer);    // Accept() traverses the DOM and generates Handler events.

   QFile	f(filename);

   if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
       return;

   QTextStream out(&f);

   out<<sb.GetString();

}

/**-----------------------------------------------------------------------------------*/

/**
 * @brief SpineDoc::GetModel creates a standard item model that cen be used with a tree view to represent the bonse structure of the file
 * @return QStandardModel - a model representing the bone structure
 */
QStandardItemModel* SpineDoc::GetModel()
{
    QStandardItemModel* model = new QStandardItemModel();
    if(!this->spineData.HasMember("bones") || !this->spineData["bones"].IsArray())
        return model;


    int length = this->spineData["bones"].Size();

    QStandardItem *rootItem = model->invisibleRootItem();
    QStandardItem *parentItem;
    for(int i = 0; i < length; i++)
    {
        QStandardItem* item = new QStandardItem(QString(this->spineData["bones"][i]["name"].GetString()));

        if((this->spineData["bones"][i].HasMember("parent")))
        {
            parentItem = this->_FindBoneInItem(rootItem, QString(this->spineData["bones"][i]["parent"].GetString()));
            parentItem->appendRow(item);
        }
        else
        {
            rootItem->appendRow(item);
        }

    }
    return model;
}

/**
 * @brief SpineDoc::_FindBoneInItem Finds a particular item by name in a tree of items
 * @param root - the root item
 * @param text - the neam of the item we want to find
 * @return  QStandardItem - the item with that name or NULL if that item was not found
 */
QStandardItem* SpineDoc::_FindBoneInItem(QStandardItem* root, QString text)
{

    QStandardItem* ret;
    for(int i = 0; i < root->rowCount(); i++)
    {
        //check if item is a match
        if(root->child(i)->text() == text)
            return root->child(i);

        //else search children
         ret =  _FindBoneInItem(root->child(i), text);
        if(ret != NULL)
            return ret;

    }
    return NULL;

}

/**-----------------------------------------------------------------------------------*/

