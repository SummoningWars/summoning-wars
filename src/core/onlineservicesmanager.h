#ifndef ONLINESERVICESMANAGER_H
#define ONLINESERVICESMANAGER_H

#include <OgreSingleton.h>
#include "OgreDataStream.h"

class OnlineServicesManager : public Ogre::Singleton<OnlineServicesManager>
{
public:
    /**
     * @brief The CharacterLite struct
     * Provides a barebones character representation
     */
    struct CharacterLite
    {
        std::string name;
        std::string subtype;
        int level;
        std::string savegame;
    };

    /**
     * @brief The LoginTaskOutData struct
     * Used for handling login data between threads
     */
    struct LoginTaskOutData
    {
        std::string token;
        std::string userName;
        std::string userDataPath;
        std::vector<CharacterLite*> characters;
    };

    /**
     * @brief The TaskData struct
     * Used for handling data beteen threads
     */
    struct TaskData
    {
        std::string TaskName;
        LoginTaskOutData *loginData;
    };

    /**
     * @brief The StatusListener class
     * Listener class for Online taks
     */
    class StatusListener
    {
    public:
        virtual void onLoginFinished(std::vector<OnlineServicesManager::CharacterLite*> &characters) = 0;
        virtual void onLogoutFinished() = 0;
        virtual void onSyncCharFinished() = 0;
    };

public:
    /**
     * @brief OnlineServicesManager
     * @param dataPath Path to the data directory
     */
    OnlineServicesManager(std::string &dataPath);

    /**
     * @brief login Login to the online service
     * @param userName User's Username
     * @param password User's Password
     * @return Return true if loging was successfull, false if otherwise
     */
    bool login(std::string userName, std::string password);

    /**
     * @brief logout Logs the current user out
     * @return Returns true if Logout was successfull, false if otherwise
     */
    bool logout();

    /**
     * @brief userLoggedIn
     * @return Return true if there is a user loggen in currently, false if otherwise
     */
    bool userLoggedIn() { return mUserLoggedIn; }

    /**
     * @brief getUserName
     * @return Returns current username
     */
    const std::string& getUserName() { return mCurrentUsername; }

    /**
     * @brief getUserDataPath
     * @return Returns current Datapath
     */
    const std::string& getUserDataPath() { return mUserDataPath; }

    /**
     * @brief getUserDataResGroupId
     * @return Returns current Ogre ResourceGroupID which holds all images for the avatars
     */
    const std::string& getUserDataResGroupId() { return mUserDataResGroupId; }

    /**
     * @brief syncCharacterData Uploads character data
     * @param charName Chracter name
     * @param data Savegame data
     * @return Returns true if successfull
     */
    bool syncCharacterData(std::string charName, std::string data);

    /**
     * @brief syncCharacterAvatar
     * @param charName
     * @param pathToImage
     * @param ptr
     * @return
     */
    bool syncCharacterAvatar(std::string charName, std::string pathToImage, Ogre::DataStreamPtr ptr);

    void registerLoginStatusListener(StatusListener* l);
    void unregisterLoginStatusListener(StatusListener* l);

    static OnlineServicesManager& getSingleton(void);
    static OnlineServicesManager* getSingletonPtr(void);
private:
    void loginWorker(std::string userName, std::string password, std::string dataPath);
    void onTaskFinished(TaskData *data);

private:
    std::string mCurrentUsername;
    std::string mToken;
    std::string mDataPath;
    std::string mUserDataPath;
    std::string mUserDataResGroupId;
    long mLastSync;
    bool mUserLoggedIn;

    std::vector<StatusListener*> mStatusListener;
};

#endif // ONLINESERVICESMANAGER_H
