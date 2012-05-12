#include "jni.h"
#include "platform/android/jni/JniHelper.h"
#include "cocos2d.h"

#include "MyDialog.h"

using namespace std;
using namespace cocos2d;

/* implementation of DialogJni
*/
class DialogJni
{
public:
    // weak ref
    static DialogProtocol *delegate;
    static string _title;
    static vector<string> _buttons;
    
public:
    DialogJni(string &title, vector<string> &buttons, DialogProtocol *dialogProtocol);
    ~DialogJni();
    
    void showDialog();    
    static void getClickedButtonAtIndex(int index);
};

DialogProtocol* DialogJni::delegate = NULL;
string DialogJni::_title = "";
vector<string> DialogJni::_buttons = vector<string>();

DialogJni::DialogJni(string &title, vector<string> &buttons, DialogProtocol *dialogProtocol)
{
    _title = title;
    
    vector<string>::iterator iter;
    for (iter = buttons.begin(); iter != buttons.end(); ++iter)
    {
        _buttons.push_back(*iter);
    }
    
    DialogJni::delegate = dialogProtocol;
}

DialogJni::~DialogJni()
{
    _title = "";
    _buttons.clear();
    
    CCLog("DialogJni::~DialogJni() is called");
}

void DialogJni::showDialog()
{
    JniMethodInfo methodInfo;
    
    if (JniHelper::getStaticMethodInfo(methodInfo, 
                                       "org/cocos2dx/lib/Cocos2dxActivity",
                                       "myShowDialog",
                                       "(Ljava/lang/String;[Ljava/lang/String;)V")
        )
    {
        // title
        jstring jstrTitle = methodInfo.env->NewStringUTF(_title.c_str());
        
        // items
        jobjectArray jarrayButton;
        jclass strCls = methodInfo.env->FindClass("java/lang/String");
        jarrayButton = methodInfo.env->NewObjectArray(_buttons.size(), strCls, NULL);
        for (int i = 0; i < _buttons.size(); ++i)
        {
            jstring element = methodInfo.env->NewStringUTF(_buttons[i].c_str());
            methodInfo.env->SetObjectArrayElement(jarrayButton, i, element);
            methodInfo.env->DeleteLocalRef(element);
        }
        
        // invoke java method
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jstrTitle, jarrayButton);
        
        // release reference
        methodInfo.env->DeleteLocalRef(jstrTitle);
        methodInfo.env->DeleteLocalRef(jarrayButton);
    }
}

void DialogJni::getClickedButtonAtIndex(int index)
{
    delegate->buttonClicked(index);
}

// function called by java
extern "C"
{
    // functions called back by java to get the result of dialog
    void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeButtonClicked(JNIEnv *env, jobject thiz, jint index)
    {
        DialogJni::getClickedButtonAtIndex(index);
    }
}

// impelementation of MyDialog

MyDialog::MyDialog() 
: internalObject(NULL) 
{}

MyDialog::~MyDialog() 
{
    CCLog("MyDialog::~MyDialog() is called");
    DialogJni *dialog = (DialogJni*)internalObject;
    CC_SAFE_DELETE(dialog);
}

void MyDialog::initWithItems(string title, vector<string> &buttons, DialogProtocol *dialogProtocol)
{
    internalObject = new DialogJni(title, buttons, dialogProtocol);
}

void MyDialog::popUp()
{
    ((DialogJni*)internalObject)->showDialog();
}
