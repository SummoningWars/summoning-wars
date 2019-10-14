#include "onlineservicesmanager.h"
#include "config.h"
#include "debug.h"
#include "tinyxml.h"
#include "OgreRoot.h"

#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/network/protocol/http/client.hpp>
#include <boost/filesystem.hpp>

#include <OgreResourceGroupManager.h>

using namespace boost;

//std::string backend_url = "http://localhost:8081";
std::string backend_url = "http://sumwars-backend.appspot.com";

template<> OnlineServicesManager* Ogre::Singleton<OnlineServicesManager>::SUMWARS_OGRE_SINGLETON = 0;

OnlineServicesManager::OnlineServicesManager(std::string &dataPath)
{
    mDataPath = dataPath;
    mUserLoggedIn = false;
    mUserDataPath = "";
    mUserDataResGroupId = "Savegame";
    mCurrentUsername = "Default";
    mToken = "";
}

bool OnlineServicesManager::syncCharacterData(std::string charName, std::string data)
{
    if(Ogre::Root::getSingletonPtr())
    {
        unsigned long timeSinceLastSync = Ogre::Root::getSingleton().getTimer()->getMilliseconds() - mLastSync;
        if(timeSinceLastSync > 2000)
        {

        }
    }
    return true;
}

bool OnlineServicesManager::syncCharacterAvatar(std::string charName, std::string pathToImage, Ogre::DataStreamPtr ptr)
{
    return true;
}

bool OnlineServicesManager::login(std::string userName, std::string password)
{
    boost::thread(boost::bind(&OnlineServicesManager::loginWorker, this, userName, password, mDataPath));

    return true;
}

void OnlineServicesManager::loginWorker(std::string userName, std::string password, std::string dataPath)
{
    OnlineServicesManager::TaskData *dat = new OnlineServicesManager::TaskData();
    dat->TaskName = "LoginTask";
    dat->loginData = new LoginTaskOutData();
    dat->loginData->userName = userName;

    network::http::client client;
    network::http::client::request request("http://sumwars-backend.appspot.com/api/remote_login?username=" + userName + "&password=" + password);
    network::http::client::response response = client.get(request);
    std::stringstream str;
    str << network::http::body(response);

    std::string temp = str.str();
    if(temp == "401")
    {
        std::cout << "Username or password is wrong" << std::endl;
        //this->cancel();
    }
    else
    {
        TiXmlDocument doc;
        if(!doc.Parse(temp.c_str()))
        {
            std::cerr << doc.ErrorDesc() << std::endl;
        }

        TiXmlElement* root = doc.FirstChildElement();
        if(root == NULL)
        {
            std::cerr << "Failed to load file: No root element." << std::endl;
            doc.Clear();
        }

        boost::filesystem::path p(dataPath);
        p /= "save/" + userName;
        dat->loginData->userDataPath = p.string();

        boost::filesystem::create_directory(p);

        for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement())
        {
            std::string val = elem->Value();
            if(val == "token")
            {
                dat->loginData->token = elem->Attribute("value");
            }
            else if(val == "Character")
            {
                OnlineServicesManager::CharacterLite *cl = new OnlineServicesManager::CharacterLite();
                elem->Attribute("level", &cl->level);
                cl->subtype = elem->Attribute("subtype");
                cl->name = elem->Attribute("name");
                cl->savegame = std::string(elem->GetText());
                dat->loginData->characters.push_back(cl);
                boost::filesystem::path tempSave = p /= ("/" + cl->name + ".sav");
                boost::filesystem::path tempPNG = p /= ("/" + cl->name + ".png");

                // TODO: check if savegames in the cloud are newer or older than the local ones and ask which ones to use

                std::ofstream f;
                f.open(tempSave.string().c_str());
                f << cl->savegame;
                f.close();
            }
        }

        std::cout << "Login finished. Retrieved " << dat->loginData->characters.size() << " character infos.";
        this->onTaskFinished(dat);
    }
}

bool OnlineServicesManager::logout()
{

    return true;
}

void OnlineServicesManager::onTaskFinished(TaskData *data)
{
    if(data->TaskName == "LoginTask")
    {
        mToken = data->loginData->token;
        mUserDataPath = data->loginData->userDataPath;
        mUserDataResGroupId = "Savegame_" + data->loginData->userName;
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mUserDataPath, "FileSystem", mUserDataResGroupId);
        Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(mUserDataResGroupId);
        mCurrentUsername = data->loginData->userName;
        mUserLoggedIn = true;

        std::vector<StatusListener*>::iterator iter;
        for(iter = mStatusListener.begin(); iter != mStatusListener.end(); iter++)
            (*iter)->onLoginFinished(data->loginData->characters);
    }
    else if(data->TaskName == "Logout Task")
    {
        /*mUserDataPath = "";
        Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(mUserDataResGroupId);
        mUserDataResGroupId = "Savegame";
        mCurrentUsername = "Default";
        mUserLoggedIn = false;

        std::vector<StatusListener*>::iterator iter;
        for(iter = mStatusListener.begin(); iter != mStatusListener.end(); iter++)
            (*iter)->onLogoutFinished();*/
    }
    else if(data->TaskName == "Sync Character Task")
    {/*
        std::vector<StatusListener*>::iterator iter;
        for(iter = mStatusListener.begin(); iter != mStatusListener.end(); iter++)
            (*iter)->onSyncCharFinished();*/
    }
    else if(data->TaskName == "Sync Character Avatar Task")
    {
        // do nothing for now
    }

}

void OnlineServicesManager::registerLoginStatusListener(StatusListener* l)
{
    std::vector<StatusListener*>::iterator iter;
    for(iter = mStatusListener.begin(); iter != mStatusListener.end(); iter++)
        if(*iter == l)
            return;

    mStatusListener.push_back(l);
}

void OnlineServicesManager::unregisterLoginStatusListener(StatusListener* l)
{
    std::vector<StatusListener*>::iterator iter;
    for(iter = mStatusListener.begin(); iter != mStatusListener.end(); iter++)
        if(*iter == l)
            mStatusListener.erase(iter);
}


OnlineServicesManager* OnlineServicesManager::getSingletonPtr(void)
{
    return SUMWARS_OGRE_SINGLETON;
}

OnlineServicesManager& OnlineServicesManager::getSingleton(void)
{
    assert( SUMWARS_OGRE_SINGLETON );
    return ( *SUMWARS_OGRE_SINGLETON );
}
