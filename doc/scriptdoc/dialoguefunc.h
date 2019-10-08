/**
 * \file dialoguefunc.h
 * \brief Defines functions for creating dialogues and cutscenes.
 */


/**
 * Activates and deactivates Cutscenemode. Cutscenemode has various effects in the whole region:
 * - Monsters AI is deactivated
 * - Players can not give commands as usual
 * - Camera is not centered on the player. Instead all players see the scene through a common camera
 * - black bars are displayed at the top and bottom of the screen
 * 
 * \param mode true activates Cutscenemode, false deactivates it
 * \sa addCameraPosition
 */
void setCutsceneMode(bool mode);

/**
 * Adds a position to the camera position queue. The use of this function is only valid if cutscene mode is active. \n
 * If Cutscenemode is active, all players see the scene trough a common camera that is placed by script. The camera visits the enqueued positions in the order of insertion. Between these positions the camera state is calculated by linear interpolation. The \a time determines the time that is used for the movement between the corresponding position and the previous position. If the camera position queue is empty, the camera wont move.\n
 * After each activation of the Cutscenemode there should be at least one addCameraPosition with \a time set to 0 to initialize the camera. \n
 * The position of the camera is decribed by spherical coordinates.
 * 
 * \param time duration of the camera move between the previous and the given position
 * \param position the point the camera looks at
 * \param phi angle of rotation around the "vertical axis" in degree
 * \param theta angle between line of sight and base plane in degree. Valid values are in the interval [0,90)
 * \param distance distance of the camera to the position the camera looks at
 */
void addCameraPosition(float time, Vector position, float phi, float theta, float distance);

/**
 * Creates a new dialogue and sets it as current dialogue. \n
 * The dialogue is the object that controls the speech and the emotion pictures during a conversation.\n
 * A unit involved in a dialog will not perform any commands given by the AI or the players. The main difference to Cutscenemode is, that only the unit in the dialogue are affected. Another difference to Cutscenemode is, that the camera stays centered at the player. To gain control over camera and other objects, Cutscenemode can be activated additionally. \n
 * Dialogues work with a speech queue that contains mainly text pieces, but also some control commands. The elements of the queue are executed in the ordner of insertion, where each element takes a certain amount of time. Even though dialog scripts sometimes look like timelines, the complete script is still executed immediately. 
 */
void createDialogue();

/**
 * Adds a speaker to the dialogue. The speaker will be referenced by the \a refname by most dialogue related functions. \n
 * It is possible to insert the same \a unitid multiple times with different refnames. The corresponding unit will speak multiple speaker roles then.\n
 * The speaker is added immediately when the function is called. To add the command to the dialogue queue the addSpeakerInDialogue can be used.
 * 
 * \param unitid ID of the unit to add to the dialogue
 * \param refname reference name that will be used for speech. This is not the name that will be displayed in the dialogue window as name of the speaker. If this parameter is omitted the units name is used. A different default refname can be set with the setRefName function.
 * \param force when set to true, the unit is removed from its previous dialogue if it already has one. When set to false, the insertion of the speaker will fail in this case.
 * 
 * \sa removeSpeaker getSpeaker setRefName addSpeakerInDialogue
 */
void addSpeaker(int unitid, string refname, bool force= true);

/**
 * Adds a speaker to the dialogue. The speaker will be referenced by the \a refname by most dialogue related functions. \n
 * The central difference to the addSpeaker is, that this command is added to the dialogue queue and is executed after all previously added texts and commands have been completed.
 * 
 * \param unitid ID of the unit to add to the dialogue
 * \param refname reference name that will be used for speech. This is not the name that will be displayed in the dialogue window as name of the speaker. If this parameter is omitted the units name is used. A different default refname can be set with the setRefName function.
 */
void addSpeakerInDialogue(int unitid, string refname);

/**
 * Removes a speaker from the dialogue. Note that all roles that are spoken by this unit wont have a speaker after this call. \n
 * This command is added to the dialogue queue, thus it is executed after all previously added texts and commands have been completed.
 * 
 * \param unitid ID of the unit to remove from the dialogue
 */
void removeSpeaker(int unitid);

/**
 * Returns the ID of the unit that speak the role given by the \a refname.
 * \param refname name of a speaker role
 * \return ID of the speaker, returns 0 if there is no such refname in the dialogue
 */
int getSpeaker(string refname);

/**
 * Sets the default reference name for a unit. If the unit is a monster/NPC that has no explicit name set, this is also displayed as name in the dialogue. \n
 * If the player starts a dialogue with an NPC, the NPC is added with this \a refname into the dialogue. Players can not speak NPCs that have no reference name. 
 */
void setRefName(int unitid, string refname);

/**
 * Adds a text to the queue of the current dialogue. If there is no speaker assigned to the refname, the text will be skipped. \n
 * When  the text is spoken, an image of the speaker showing the selected \a emotion will be displayed. The position of the speaker (upper left/right, lower left/right) is calculated interally, but can be set explicitely, too. \n
 * Additionally, if the speaker stands still a talk animation is displayed. This animation can be modified by setSpeakerAnimation. 
 * 
 * \param refname reference name of the speaker that should speak the text
 * \param text the displayed text
 * \param emotion emotion of the displayed picture 
 * \param time how long this text is displayed. If this parameter is omitted, it is calculated from the text length
 * 
 * \sa setSpeakerEmotion setSpeakerPosition setSpeakerAnimation
 */
void speak(string refname, string text ,string emotion="normal" ,float time = calculated);

/**
 * Sets the emotion picture of a speaker. Once set, the emotion will remain until it is changed explicitely. \n
 * This command is added to the dialogue queue, thus it is executed after all previously added texts and commands have been completed.
 * 
 * \param refname reference name of the speaker
 * \param emotion emotion of the displayed picture
 */
void setSpeakerEmotion(string refname, string emotion);

/**
 * Sets the animation of a speaker. Similar to the setObjectAnimation, the animation is only displayed, if the speaker is does not do any other action at the moment. \n
 * This command is added to the dialogue queue, thus it is executed after all previously added texts and commands have been completed. This is the main difference to the setObjectAnimation function. \n
 * Although the function has a \a time argument which sets the length of the animation, the command itself is immediately removed from the dialogue queue after execution. So the animation overlaps with the speech that follows, what is usually desired.
 * 
 * \param refname reference name of the speaker
 * \param animation Animation that should be shown by the speaker
 * \param time duration of the animation.
 * \param repeat If set to true, the animation will loop
 */
void setSpeakerAnimation(string refname, string animation, float time, bool repeat = false);

/**
 * Sets the position that is associated with a speaker. The dialogue window has four positions for speakers: "upper_left", "upper_right", "lower_left", "lower_right". Once a position is set for a speaker, he will always appear at the same position. \n
 * In addition, this function assured that the speaker is visible after the call.
 *
 * \param refname reference name of the speaker
 * \param position new position, one of the strings "upper_left", "upper_right", "lower_left", "lower_right"
 */
void setSpeakerPosition(string refname, string position);

/**
 * Inserts a piece of \a luacode into the dialogue queue. This code will be executed after all previously added texts and commands have been completed. \n
 * Be aware that the \a luacode is a new lua script that does not share any local variables with the script calling the addCondition function. Moreover, the current dialogue is an environment variable as well, that is not set in the new script.
 * 
 * \param luacode Code that does a condition check for the event
 */
void executeInDialog(string luacode);

/**
 * Changes the topic base of the current dialogue. The topic base defines the namespace that contains the new topics that are accessed by answers and changeTopic calls. \n
 * Per default the topic base is the refname of the NPC if the player initiated the dialogue and "global" if the dialogue was created by script. In the former case, the topics are chosen from the NPC's namespace, in the latter normal events are called. \n
 * This command is \b not added to the dialogue queue and it is therefore executed immediately when the script runs.
 * 
 * \param topicbase new dialogue topic base
 * \sa addAnswer changeTopic
 */
void setTopicBase(string topicbase);

/**
 * Changes the topic of the dialogue. If the topicbase is not "global", the topic will be chosen from the NPC topic namespace that is given by the topicbase. For that topic the condition is checked, and if the result is true, the topic effect is executed. \n
 * If the topicbase is global a trigger with the name of the topic is created, so essential events are called instead of topics. \n
 * In both cases the called lua script has the same current dialoge environment. \n
 * This command is added to the dialogue queue, thus it is executed after all previously added texts and commands have been completed. Usually this command is the last in the dialogue queue and the resulting topic call refills the dialogue with new content. \n
 * Few topic strings have a special meaning:
 * - \b "end" ends the dialogue
 * - \b "start" restarts the dialogue, only valid if topicbase is not "global"
 * - \b "trade" opens the trade window
 * 
 * \param topic name of a topic (trigger if topicbase=="global")
 */
void changeTopic(string topic);

/**
 * Adds a question into the dialogue queue. The question will always be displayed to the first player that was inserted into the dialogue. \n
 * The answers are added by subsequent addAnswer calls. \n
 * 
 * \param text text of the question
 * \sa addAnswer
 */
void addQuestion(string text);

/**
 * Adds an answer option to a previously added question. The function call is only valid if a question was added directly before. If the user has chosen an answer, the dialogue changes the topic accordingly. \n
 * If the topicbase is "global", all answers are displayed. If the topicbase is not "global", then topic conditions are checked before the question is displayed. If the condition returns false, the answer is not offered. \n
 * 
 * \param text test of the answer
 * \param topic If the answer is chosen, the dialogue topic changes to this topic. If the topicbase is "global" a trigger with the same name is created
 * 
 * \sa changeTopic addQuestion
 */
void addAnswer(string text, string topic);

/**
 * Activates and deactivates the dialogue. An inactive dialogue does not progress with the execution of its queue. The purpose is to suspend a dialogue while showing some scripted actions.
 * This command is \b not added to the dialogue queue and it is therefore executed immediately when the script runs.
 * 
 * \param active activity state of the dialogue
 */
void setDialogueActive(bool active);

/**
 * Allows a unit to speak without a dialogue. The text is displayed immediately in a speech bubble above the unit. \n
 * The speech bubble is only displayed if the speaker is visible on the screen. If cutscene mode is activated, beware that the field of view is narrowed down by the black bars on the top and bottom. \n
 * This can be used on units who have a dialogue. Nevertheless a unit can not speak one text in the dialogue and another text in the speech bubble simulatenously. In this case the text set by unitSpeak is ignored.
 * 
 * \param unitid ID of a unit
 * \param text text that should be spoken
 * \param emotion emotion that is displayed in the speed bubble.
 * \param time how long this text is displayed. If this parameter is omitted, it is calculated from the text length
 */
void unitSpeak( int unitid, string text ,string emotion="normal" ,float time= calculated);

/**
 * Returns all players that fit a certain role. Available roles are:
 * - \b "all" all players
 * - \b "male" all male players
 * - \b "female" all female players
 * - \b "leader" partyleader only
 * - \b "non-leader" all players except the partyleader
 * - \b playerclass all players of the given player class
 * 
 * \param role on of the roles listed above
 * \return list of player ids that fit the role
 */
list<int> getRolePlayers(string role);








