
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
 void SpineDoc::AddVoice(LipsyncVoice* voice)
 {

      this->AddBones(); //add the head bone
      this->AddSlots(); //add the animations slots
      this->AddSkins(); //add the mouthshape skin
     if(this->exportWords)         //if desired, export words as events
       this->AddWordEvents(voice);
      this->AddAnimations(voice); //add voice keyframes





 }

 /**-----------------------------------------------------------------------------------*/
 /**
  * @brief SpineDoc::AddBones sets up bones in the file
  */
 void SpineDoc::AddBones()
 {
    Document::AllocatorType& allocator = this->spineData.GetAllocator();

    Value bones(kArrayType);
    Value rootBone(kObjectType);
    rootBone.AddMember("name", "root", allocator);

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
        mouthslot.AddMember("bone", "root", allocator);
        mouthslot.AddMember("attachment", "rest", allocator);

    Value mouthslots(kArrayType);
    mouthslots.PushBack(mouthslot, allocator);


     if(!this->spineData.HasMember("slots"))
        this->spineData.AddMember("slots", mouthslots, allocator);//TODO make array
     else
         this->spineData["slots"].PushBack(mouthslots, allocator);



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
            events.AddMember(Value(word.toStdString().c_str(), allocator).Move(), empty, allocator);

            //add event to keyframes
            Value keyFrame(kObjectType);
            keyFrame.AddMember("time", time, allocator);
            keyFrame.AddMember("name", Value(word.toStdString().c_str(), allocator).Move(), allocator);
            animEvents.PushBack(keyFrame, allocator);
        }
     }

    this->spineData.AddMember("events", events, allocator);


    Value animation(kObjectType);
    Value animations(kObjectType);

    if(this->spineData.HasMember("animations"))
    {
        if(this->spineData["animations"].HasMember("animation"))
        {
            this->spineData["animations"]["animation"].AddMember("events", animEvents, allocator);
        }
        else
        {
            animations.AddMember("animation", animation, allocator);
            this->spineData["animations"].AddMember("animation", animation, allocator);
        }
    }
    else
    {
        animation.AddMember("events", animEvents, allocator);
        animations.AddMember("animation", animation, allocator);
        this->spineData.AddMember("animations", animations, allocator);
    }
 }

/**-----------------------------------------------------------------------------------*/
 /**
  * @brief SpineDoc::AddAnimations - adds all voice animation events
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
        if(this->spineData["animations"].HasMember("animation"))
        {
            if(this->spineData["animations"]["animation"].HasMember("slots"))
            {
                mouthShape.AddMember("attachment", attachment, allocator);
                this->spineData["animations"]["animation"]["slots"].AddMember("MouthShape", mouthShape, allocator);
            }
            else
            {
                mouthShape.AddMember("attachment", attachment, allocator);
                slot.AddMember("MouthShape", mouthShape, allocator);

                this->spineData["animations"]["animation"].AddMember("slots", slot, allocator);//NOTE change
            }
        }
        else
        {
            mouthShape.AddMember("attachment", attachment, allocator);
            slot.AddMember("MouthShape", mouthShape, allocator);
            animation.AddMember("slots", slot, allocator);

            this->spineData["animations"].AddMember("animation", animation, allocator);
        }

    }
    else
    {
        mouthShape.AddMember("attachment", attachment, allocator);
        slot.AddMember("MouthShape", mouthShape, allocator);
        animation.AddMember("slots", slot, allocator);
        animations.AddMember("animation", animation, allocator);
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
/*
 void LipsyncVoice::ExportSpine(QString path)
 {
     QFile	f(path);


     if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
         return;

     QTextStream out(&f);

     //useful variables
     int        fps = 30;
     int		startFrame = 0;
     int		endFrame = 1;
     int        width = 200;
     int        height = 200;
     QString hash("KebD3MyURSyaMpyRinWkDQEAuYk");        //@TODO: make hash compute form phoneme/frame numbers
     double time = 0.0;
     QString	phoneme, nextPhoneme;
     QString    word,   nextWord;
     //get the start and end frames
     if (fPhrases.size() > 0)
     {
         startFrame = fPhrases[0]->fStartFrame;
         endFrame = fPhrases.last()->fEndFrame;
     }


    QString skeleton = "\"skeleton\": {\n\t\"hash\": \"";
    skeleton.append(hash);
    skeleton.append("\",\n\t\"spine\": \"2.1.27\",\n");
    skeleton.append(QString("\t\"width\": %1,\n").arg(width));
    skeleton.append(QString("\t\"height\": %1,\n},\n").arg(height));

    //create bones
    QString bones = "\"bones\": [\n\t{ \"name\": \"root\" }\n],\n";
    //create slots
    QString slot = "\"slots\": [\n\t{ \"name\": \"Mouthshape\", \"bone\": \"root\", \"attachment\": \"rest\" }\n],\n";
    //create skins
    QString skins = "\"skins\": {\n\t\"default\": {\n\t\t\"Mouthshape\": {\n";
        skins.append(QString("\t\t\t\"AI\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

        skins.append(QString("\t\t\t\"E\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

        skins.append(QString("\t\t\t\"FV\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

        skins.append(QString("\t\t\t\"L\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

        skins.append(QString("\t\t\t\"MBP\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

        skins.append(QString("\t\t\t\"O\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

        skins.append(QString("\t\t\t\"U\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

        skins.append(QString("\t\t\t\"WQ\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

        skins.append(QString("\t\t\t\"etc\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

        skins.append(QString("\t\t\t\"rest\": { \"width\":%1").arg(width));
        skins.append(QString(" , \"height\": %1 },\n").arg(height));

    skins.append("\t\t}\n\t}\n},\n");
    //create animations
    QString animations = "\"animations\": {\n\t\"animation\": {\n\t\t\"slots\": {\n\t\t\t\"Mouthshape\": {\n\t\t\t\t\"attachment\": [\n";
    QString events = "\"events\":{\n";
    QString animEvents = "\t\t\"events\":[\n";
    if (startFrame > 1)
    {
        phoneme = "rest";
        out << 1 << ' ' << "rest" << endl;
    }

    for (int frame = startFrame; frame <= endFrame; frame++)
    {
        nextPhoneme = GetPhonemeAtFrame(frame);
        nextWord = GetWordAtFrame(frame);
        time = (double)(frame)/(double)fps;

        //if there is a new word, add it to the events and the animations lists
        if(nextWord != word)
        {
            word = nextWord;
            //add word to events list
            events.append("\t\"");
            events.append(word);
            events.append("\":{},\n");

            //add event to animaitons list
            animEvents.append(QString("\t\t\t{\"time\":%1").arg(time, 0, 'g', 5));
            animEvents.append(",\"name\":\"");
            animEvents.append(word);
            animEvents.append("\"},\n");
        }

        //if there is a new phoneme, add it
        if (nextPhoneme != phoneme)
        {
            if (phoneme == "rest")
            { // export an extra "rest" phoneme at the end of a pause between words or phrases
                animations.append("\t\t\t\t\t{ \"time\": ");
                animations.append(QString("%1").arg(time, 0, 'g', 5));
                animations.append(", \"name\": \"");
                animations.append(phoneme);
                animations.append("\" },\n");

            }
            phoneme = nextPhoneme;

            animations.append("\t\t\t\t\t{ \"time\": ");
            animations.append(QString("%1").arg(time, 0, 'g', 5));
            animations.append(", \"name\": \"");
            animations.append(phoneme);
            animations.append("\" },\n");
        }
    }
    animEvents.append("\t\t]\n");
    animations.append("\t\t\t\t]\n\t\t\t}\n\t\t},\n");
    animations.append(animEvents);
    animations.append("\t}\n}");
    events.append("},\n");
    //write to file

    out<<"{\n";
    out<<skeleton;
    out<<bones;
    out<<slot;
    out<<skins;
    out<<events;
    out<<animations;
    out<<"\n}";

 }



   */
