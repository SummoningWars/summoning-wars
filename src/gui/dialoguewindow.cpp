/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dialoguewindow.h"
#include "ceguiutility.h"
#include "debug.h"

DialogueWindow::DialogueWindow(Document* doc, Scene* scene, const std::string& ceguiSkinName)
	: Window (doc)
	, m_ceguiSkinName (ceguiSkinName)
{
	m_scene = scene;
	CEGUI::Window* label;

	// The CharInfo window and holder
	CEGUI::FrameWindow* dialog_wnd = static_cast<CEGUI::FrameWindow*> (CEGUIUtility::loadLayoutFromFile ("dialogwindow.layout"));
	if (!dialog_wnd)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "dialogwindow.layout");
	}

	CEGUI::Window* dialog_wnd_holder = CEGUIUtility::loadLayoutFromFile ("dialogwindow_holder.layout");
	if (!dialog_wnd_holder)
	{
		SW_DEBUG ("WARNING: Failed to load [%s]", "dialogwindow_holder.layout");
	}
	
	CEGUI::Window* wndHolder = CEGUIUtility::getWindowForLoadedLayoutEx (dialog_wnd_holder, "DialogWindow_Holder");
	CEGUI::Window* wndDialogWindow = CEGUIUtility::getWindowForLoadedLayoutEx (dialog_wnd, "DialogWindow");
	if (wndHolder && wndDialogWindow)
	{
		CEGUIUtility::addChildWidget (wndHolder, wndDialogWindow);
	}
	else
	{
		if (!wndHolder) SW_DEBUG ("ERROR: Unable to get the window holder for dialog screen.");
		if (!wndDialogWindow) SW_DEBUG ("ERROR: Unable to get the window for dialog screen.");
	}

	m_window = dialog_wnd_holder;

	CEGUI::FrameWindow* frame_widget = static_cast<CEGUI::FrameWindow*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueLowerBar"));
	frame_widget->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));

	frame_widget = static_cast<CEGUI::FrameWindow*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueUpperBar"));
	frame_widget->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));

	CEGUI::ScrollablePane* pane;
	pane = static_cast<CEGUI::ScrollablePane*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueLowerBar/SpeakerTextPane0"));
	pane->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));

	pane = static_cast<CEGUI::ScrollablePane*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueLowerBar/SpeakerTextPane1"));
	pane->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueLowerBar/SpeakerTextPane0/SpeakerTextLabel0");
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));

	label = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueLowerBar/SpeakerTextPane1/SpeakerTextLabel1");
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));

	CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueLowerBar/DialogueSkipAllButton"));
	btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&DialogueWindow ::onTextClicked, this));
}

void DialogueWindow::update()
{
	Player* player = m_document->getLocalPlayer();
	Region* reg = player->getRegion();
	Dialogue* dia =  reg->getDialogue( player->getDialogueId() );
	
	bool bar_vis = false;
	if (reg->getCutsceneMode () == true || player->getDialogueId() != 0)
	{
		bar_vis = true;
	}
	
	if (m_document->getGUIState()->m_shown_windows & Document::TRADE)
	{
		bar_vis = false;
	}
	
	CEGUI::FrameWindow* upper_bar = static_cast<CEGUI::FrameWindow*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueUpperBar"));
	CEGUI::FrameWindow* lower_bar = static_cast<CEGUI::FrameWindow*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueLowerBar"));
	if (upper_bar->isVisible() != bar_vis)
	{
		upper_bar->setVisible(bar_vis);
		lower_bar->setVisible(bar_vis);
		m_window->setVisible (bar_vis);
		SW_DEBUG ("Set both lower and upper bar visibility to %d", bar_vis);
		
		// note: remove this (only debug)
		if (bar_vis)
		{
			SW_DEBUG ("object visibility set to true: %s", upper_bar->getNamePath ().c_str ());
			CEGUI::Window *tmp;
			tmp = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow");
			SW_DEBUG ("object visibility of %s is %d", tmp->getNamePath ().c_str (), tmp->isVisible ());
			tmp = CEGUIUtility::getWindow ("GameScreen/DialogWindow_Holder");
			SW_DEBUG ("object visibility of %s is %d", tmp->getNamePath ().c_str (), tmp->isVisible ());
			if (! tmp->isVisible ())
			{
				tmp->setVisible (true);
			}
			tmp = CEGUIUtility::getWindow ("GameScreen");
			SW_DEBUG ("object visibility of %s is %d", tmp->getNamePath ().c_str (), tmp->isVisible ());
		}
	}
	
	if (bar_vis)
	{
		CEGUI::Window* skipButton = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueLowerBar/DialogueSkipAllButton");
		
		CEGUI::Window* wimage;
		CEGUI::Window* wname;
		CEGUI::Window* wtext;
		CEGUI::ScrollablePane* wpane;
		
		if (dia != 0)
		{
			skipButton->setVisible(true);
			// Schleife fuer die moeglichen Sprecher eines Dialogs
			std::string image, name, text;
			
			WorldObject* wo;
			Creature* cr;
			
			std::stringstream stream;
			
			Dialogue::SpeakerState* spstate;
			for (int i=0; i<Dialogue::NR_POSITIONS; i++)
			{
				stream.str("");
				stream << "DialogWindow/DialogueLowerBar/SpeakerEmotionImage" << i;
				wimage = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
						
				stream.str("");
				stream << "DialogWindow/DialogueLowerBar/SpeakerNameLabel" << i;
				wname = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
				
				stream.str("");
				stream << "DialogWindow/DialogueLowerBar/SpeakerTextPane" << i << "/SpeakerTextLabel" << i;
				wtext = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
				
				
				stream.str("");
				stream << "DialogWindow/DialogueLowerBar/SpeakerTextPane";
				stream << i;
				wpane = static_cast<CEGUI::ScrollablePane*> (CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str()));
				
				
				spstate = dia->getSpeakerState((Dialogue::Position) i);
				if (spstate != 0)
				{
					// Sprecher ist aktiv
					wo = reg->getObject(spstate->m_id);
					cr = dynamic_cast<Creature*>(wo);
					if (cr == 0)
						continue;
					
					
					name = cr->getName().getTranslation().c_str();
					
					
					if (wname->getText() != (CEGUI::utf8*) name.c_str())
					{
						wname->setText((CEGUI::utf8*) name.c_str());
						SW_DEBUG ("Set text of widget (a) [%s] to [%s]", wname->getNamePath ().c_str (), wname->getText ().c_str ());
					}
					
					if (!wname->isVisible())
					{
						wname->setVisible(true);
					}
					
					text = cr->getSpeakText().m_text.getTranslation();
					
					if (text == "" || cr->getSpeakText().m_in_dialogue == false)
					{
						wtext->setVisible(false);
					}
					else
					{
						// remove everything after ##
						size_t pos = text.find("##");
						if (pos != std::string::npos)
						{
							text.erase(pos);
						}
						const CEGUI::Font* fnt = wtext->getFont();
						
						if (wtext->getText() != (CEGUI::utf8*) text.c_str())
						{
							// TODO: investigate need to adjust size
#if 0
							CEGUI::UVector2 size = CEGUIUtility::getWidgetSize (wtext);
							
							CEGUIUtility::Rectf isize = wtext->getUnclippedInnerRect ();
							FormatedText txt = CEGUIUtility::fitTextToWindow((CEGUI::utf8*)text.c_str(), isize.getWidth(), CEGUIUtility::WordWrapCentred, fnt);
							float height = PixelAligned(txt.lines * fnt->getLineSpacing());
							size.d_y = CEGUI::UDim(0.0, height);
							CEGUIUtility::setWidgetSize (wtext, size);
							wtext->setText((CEGUI::utf8*)txt.text.c_str());
#else
							wtext->setText((CEGUI::utf8*)text.c_str());
							SW_DEBUG ("Set text of widget (b) [%s] to [%s]", wname->getNamePath ().c_str (), wname->getText ().c_str ());
#endif
						}
						wtext->setVisible(true);
						
						// Scrolling des Texts
						float height = CEGUIUtility::getWidgetSize (wtext).d_y.d_offset;
						if (height > 4*fnt->getLineSpacing())
						{
							int initiallines = 2;
							int lines = (int) (height / fnt->getLineSpacing() + 0.5);
							float timesplit = initiallines+lines;
							float frac = cr->getSpeakText().m_displayed_time / cr->getSpeakText().m_time;
							
							float scrollpos = 0.0;
							if (frac > (timesplit - initiallines)/timesplit)
							{
								scrollpos = 1.0;
							}
							else if (frac > initiallines/timesplit)
							{
								scrollpos =(frac * timesplit - initiallines) / (timesplit - initiallines*2);
							}
							
							if (wpane->getVerticalScrollPosition() != scrollpos)
							{
								wpane->setVerticalScrollPosition(scrollpos);
							}
							
						}
					}
					
					std::string image = cr->getEmotionImage(cr->getSpeakText().m_emotion);
					if (wimage->getProperty("Image") != image)
					{
						wimage->setProperty("Image",image);
					}
					if (!wimage->isVisible())
					{
						wimage->setVisible(true);
					}
					
				}
				else
				{
					wimage->setVisible(false);
					wname->setVisible(false);
					wtext->setVisible(false);
				}
			}
		}
		else
		{
			for (int i=0; i<Dialogue::NR_POSITIONS; i++)
			{
				std::stringstream stream;
				stream.str("");
				stream << "DialogWindow/DialogueLowerBar/SpeakerEmotionImage" << i;
				wimage = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
						
				stream.str("");
				stream << "DialogWindow/DialogueLowerBar/SpeakerNameLabel" << i;
				wname = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
				
				stream.str("");
				stream << "DialogWindow/DialogueLowerBar/SpeakerTextPane" << i << "/SpeakerTextLabel" << i;
				wtext = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, stream.str());
				
				stream.str("");
				stream << "DialogWindow/DialogueLowerBar/SpeakerTextPane" << i;
				
				wimage->setVisible(false);
				wname->setVisible(false);
				wtext->setVisible(false);
			}
			skipButton->setVisible(false);
		}
	}

	updateSpeechBubbles();
}

void DialogueWindow::updateTranslation()
{
	
}

void DialogueWindow::updateSpeechBubbles()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* game_screen =  CEGUIUtility::getWindow ("GameScreen");
	
	int picsize = 50;
	
	// Zaehler wie viele Labels existieren
	static int lcount =0;
	
	// Zaehler, wie viele Antwortmoeglichkeiten existieren
	static int acount =0;
	
	Player* player = m_document->getLocalPlayer();
	Region* reg = player->getRegion();
	
	if (player ==0 || player->getRegion() ==0)
		return;
	
	Dialogue* dia =  reg->getDialogue( player->getDialogueId() );
	
	CEGUI::Window* label;
	CEGUI::Window* image;
	CEGUI::FrameWindow* ques;
	CEGUI::FrameWindow* speakframe;
	
	// Objekte im Umkreis von 20 Meter holen
	std::list<WorldObject*> objs;
	std::list<WorldObject*>::iterator it;
	
	float r = 20;
	bool alldia;
	alldia = (player->getRegion()->getCutsceneMode () == true) || (player->getDialogueId() != 0);
	if (alldia)
		r = 1000;
	
	Shape s;
	s.m_center = player->getShape()->m_center;
	s.m_type = Shape::RECT;
	s.m_extent = Vector(r,r);
	player->getRegion()->getObjectsInShape(&s,&objs,WorldObject::LAYER_ALL,WorldObject::CREATURE);
	
	std::pair<float,float> pos;
	
	Creature* cr;
	
	int nr =0;
	std::string text;
	
	std::stringstream stream;
	bool bar_vis = false;
	if (alldia)
	{
		bar_vis = true;
	}
	
	for (it = objs.begin(); it != objs.end(); ++it)
	{
		// nur Kreaturen behandeln
		if (!(*it)->isCreature())
			continue;
		
		cr = static_cast<Creature*>(*it);
		pos = m_scene->getProjection(cr->getShape()->m_center,2.5f);
		
		
		// nur Kreaturen behandeln, die wirklich zu sehen sind
		if (!alldia && ( pos.first <0 || pos.first >1 || pos.second <0 || pos.second >1))
			continue;
				
		text = cr->getSpeakText().m_text.getTranslation();
		
		if (text == "")
			continue;

		
		// Wenn der Spieler sich in einem Gespraech befindet, die dazugehoerigen Texte darstellen nicht hier darstellen
		// diese werden in den Balken dargestellt
		if (cr->getDialogueId() !=0 && cr->getDialogueId() == player->getDialogueId() && cr->getSpeakText().m_in_dialogue)
			continue;
		
		if (nr >= lcount)
		{
			lcount ++;
			
			stream.str("");
			stream << "SpeechFrame" << nr;
			
			
			speakframe = (CEGUI::FrameWindow*) win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "FrameWindow"), stream.str());
			speakframe->setProperty("FrameEnabled","false");
			speakframe->setProperty("TitlebarEnabled","false");
			speakframe->setProperty("CloseButtonEnabled","false");
			CEGUIUtility::addChildWidget (game_screen, speakframe);
			speakframe->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
			speakframe->setVisible(false);
			CEGUIUtility::setWidgetSize (speakframe, CEGUI::UVector2(CEGUI::UDim(0,200), CEGUI::UDim(0,picsize+10)));
			
			stream.str("");
			stream << "SpeechLabel" << nr;
			
			label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), stream.str());
			CEGUIUtility::addChildWidget (speakframe, label);
			label->setProperty("FrameEnabled", "true");
			label->setProperty("BackgroundEnabled", "true");
			CEGUIUtility::setWidgetSize (label, CEGUI::UVector2(CEGUI::UDim(0,80), CEGUI::UDim(0,picsize)));
			label->setPosition(CEGUI::UVector2(CEGUI::UDim(0,picsize+10), CEGUI::UDim(0,5)));
			
			label->setText("");
			SW_DEBUG ("Set text of widget (c) [%s] to [%s]", label->getNamePath ().c_str (), label->getText ().c_str ());
			label->setAlpha(0.9);
			
			stream.str("");
			stream << "SpeechIamge" << nr;
			
			image = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticImage"), stream.str());
			CEGUIUtility::addChildWidget (speakframe, image);
			image->setProperty("BackgroundEnabled", "true");
			CEGUIUtility::setWidgetSize (image, CEGUI::UVector2(CEGUI::UDim(0,picsize), CEGUI::UDim(0,picsize)));
			image->setPosition(CEGUI::UVector2(CEGUI::UDim(0,5), CEGUI::UDim(0,5)));
		}
		else
		{
			stream.str("");
			stream << "GameScreen/SpeechFrame" << nr;
			
			speakframe = static_cast<CEGUI::FrameWindow*>( CEGUIUtility::getWindow (stream.str()) );
			
			stream.str("");
			stream << "GameScreen/SpeechFrame" << nr << "/SpeechLabel" << nr;
			label = CEGUIUtility::getWindow (stream.str());
			
			stream.str("");
			stream << "GameScreen/SpeechFrame" << nr << "/SpeechIamge" << nr;
			image = CEGUIUtility::getWindow (stream.str());
			
		}
		
			
			
		if (label->getText() != (CEGUI::utf8*) text.c_str())
		{
			SW_DEBUG ("diff label [%s], [%s]", label->getText ().c_str (), text.c_str ());

			const CEGUI::Font* font = label->getFont();
			
			float width = font->getTextExtent((CEGUI::utf8*) text.c_str())+15;
			float height = font->getFontHeight() +15;
			CEGUIUtility::Rectf rect = game_screen->getInnerRectClipper();

			float maxwidth = rect.getWidth()/4;
			FormatedText txt = CEGUIUtility::fitTextToWindow((CEGUI::utf8*) text.c_str(), maxwidth, CEGUIUtility::WordWrapLeftAligned, font);
			size_t lines = txt.lines;
			label->setText((CEGUI::utf8*) txt.text.c_str());
			SW_DEBUG ("Set text of widget (d) [%s] to [%s]", label->getNamePath ().c_str (), label->getText ().c_str ());

			// Test to see whether the text can fit a single line
			if (txt.lines <= 1)
			{
				// Single line
				CEGUIUtility::setWidgetSize (label, CEGUI::UVector2(CEGUI::UDim(0,width),  CEGUI::UDim(0,picsize)));
				CEGUIUtility::setWidgetSize (speakframe, CEGUI::UVector2(CEGUI::UDim(0,picsize+15+width), CEGUI::UDim(0,picsize+10)));
				image->setPosition(CEGUI::UVector2(CEGUI::UDim(0,5), CEGUI::UDim(0,5)));
			}
			else
			{
				// Multi-line
				rect.setWidth(maxwidth-15);
				
				height = lines * font->getFontHeight() +15;
				
				if (height < picsize)
				{
					CEGUIUtility::setWidgetSize (label, CEGUI::UVector2(CEGUI::UDim(0,maxwidth),  CEGUI::UDim(0,picsize)));
					CEGUIUtility::setWidgetSize (speakframe, CEGUI::UVector2(CEGUI::UDim(0,picsize+15+maxwidth), CEGUI::UDim(0,picsize+10)));
					image->setPosition(CEGUI::UVector2(CEGUI::UDim(0,5), CEGUI::UDim(0,5)));
				}
				else
				{
					CEGUIUtility::setWidgetSize (label, CEGUI::UVector2(CEGUI::UDim(0,maxwidth),  CEGUI::UDim(0,height)));
					CEGUIUtility::setWidgetSize (speakframe, CEGUI::UVector2(CEGUI::UDim(0,picsize+15+maxwidth), CEGUI::UDim(0,height+10)));
					image->setPosition(CEGUI::UVector2(CEGUI::UDim(0,5), CEGUI::UDim(0,(height-picsize)/2 + 5)));
				}
			}
		}
		
		float width = speakframe->getPixelSize().d_width;
		float height = speakframe->getPixelSize().d_height;
		speakframe->setPosition(CEGUI::UVector2(CEGUI::UDim(pos.first,-width/2), CEGUI::UDim(pos.second,-height)));
		
		speakframe->setVisible(true);
		
		// Emotionimage setzen
		std::string imagestr = cr->getEmotionImage(cr->getSpeakText().m_emotion);
		if (image->getProperty("Image") != imagestr)
		{
			image->setProperty("Image",imagestr);
		}
		
		nr++;
		
	}

	// restliche Label verstecken
	for (; nr<lcount; nr++)
	{
		stream.str("");
		stream << "GameScreen/SpeechFrame" << nr;
		speakframe = static_cast<CEGUI::FrameWindow*>(CEGUIUtility::getWindow (stream.str()));
			
		speakframe->setVisible(false);
	}
	
	// Fenster fuer eine Frage
	Dialogue::Question* question = 0;
	if (dia != 0)
	{
		question = dia->getQuestion();
	}
	if (question !=0 && question->m_active)
	{
		bool can_answer = dia->playerCanAnswer(player->getId());

		int wflags = m_document->getGUIState()->m_shown_windows;
		if (wflags != (Document::QUESTIONBOX | Document::CHAT ) || (wflags & Document::QUESTIONBOX) == 0)
		{
			m_document->getGUIState()->m_shown_windows |= Document::QUESTIONBOX;
			m_document->getGUIState()->m_shown_windows &= (Document::QUESTIONBOX | Document::CHAT );
			wflags = m_document->getGUIState()->m_shown_windows;
			m_document->setModified(m_document->getModified() | Document::WINDOWS_MODIFIED);
		}
		
		if (acount ==0)
		{
			ques = (CEGUI::FrameWindow*) win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "FrameWindow"), "QuestionWindow");
			ques->setProperty("FrameEnabled","false");
			ques->setProperty("TitlebarEnabled","false");
			ques->setProperty("CloseButtonEnabled","false");
			CEGUIUtility::addChildWidget (game_screen, ques);
			ques->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
			ques->setVisible(false);
			
			label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), "QuestionLabel");
			CEGUIUtility::addChildWidget (ques, label);
			label->setProperty("FrameEnabled", "false");
			label->setProperty("BackgroundEnabled", "false");
		}
		else
		{
			ques = (CEGUI::FrameWindow*)CEGUIUtility::getWindow ("GameScreen/QuestionWindow");
			label = (CEGUI::Window*) CEGUIUtility::getWindow ("GameScreen/QuestionWindow/QuestionLabel");
			label->setProperty("HorzFormatting", "WordWrapLeftAligned");
		}
		
		nr =0;
		
		// Groesse des Fensters ermitteln
		float height=5,width=0;
		float elemheight, elemwidth;
		int lines;
		float horzoffset = 10;
		
		CEGUIUtility::Rectf rect = game_screen->getInnerRectClipper();
		float maxwidth = rect.getWidth()/4;
		rect.setWidth(maxwidth-15);
		
		const CEGUI::Font* font = label->getFont();
		float lineheight = font->getFontHeight();
		
		CEGUI::utf8* ctext;
		ctext = (CEGUI::utf8*) question->m_text.getTranslation().c_str();
		elemwidth =font->getTextExtent(ctext);
		elemheight = lineheight;
		
		if (elemwidth > maxwidth)
		{
			elemwidth = maxwidth;
						lines = CEGUIUtility::fitTextToWindow((CEGUI::utf8*)ctext,rect.getWidth(), CEGUIUtility::WordWrapLeftAligned, font).lines;
			elemheight = lines * lineheight;
		}
		width = MathHelper::Max(width,elemwidth);
		
		if (label->getText() != ctext)
		{
			label->setText((CEGUI::utf8*)ctext);
			SW_DEBUG ("Set text of widget (e) [%s] to [%s]", label->getNamePath ().c_str (), label->getText ().c_str ());
		}
		
		label->setPosition(CEGUI::UVector2(CEGUI::UDim(0,horzoffset), CEGUI::UDim(0,height)));
		CEGUIUtility::setWidgetSize (label, CEGUI::UVector2(cegui_reldim(1.0f), CEGUI::UDim(0,elemheight)));
		height += elemheight + 20;
		
		// Insert answers
		std::list < std::pair<TranslatableString, std::string> >::iterator it;
		for (it = question->m_answers.begin(); it != question->m_answers.end(); ++it)
		{
			stream.str("");
			stream << "AnswerLabel" << nr;
			
			if (nr >= acount)
			{
				acount ++;
				label = win_mgr.createWindow (CEGUIUtility::getWidgetWithSkin (m_ceguiSkinName, "StaticText"), stream.str());
				CEGUIUtility::addChildWidget (ques, label);
				label->setProperty("FrameEnabled", "false");
				label->setProperty("BackgroundEnabled", "false");
				label->setMousePassThroughEnabled (false);
				label->setID(nr);
//				label->setAlwaysOnTop (true);
				label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onAnswerClicked, this));
				label->subscribeEvent(CEGUI::Window::EventMouseEntersArea, CEGUI::Event::Subscriber(&DialogueWindow::onAnswerEnterArea, this));
				label->subscribeEvent(CEGUI::Window::EventMouseLeavesArea, CEGUI::Event::Subscriber(&DialogueWindow::onAnswerLeaveArea, this));
			}
			else
			{
				stream.str ("");
				stream << "GameScreen/QuestionWindow/AnswerLabel" << nr;
				//DEBUG ("Inserting (existing) answer label [%s]", stream.str ().c_str ());
				label = CEGUIUtility::getWindow (stream.str());
				// Augustin Preda, 2014.01.19: for some reason, there are several always on top windows, and not all can be at the front.
				// These options however must be :-)

				label->moveToFront ();
			}
			
			CEGUI::String cstring = (CEGUI::utf8*) it->first.getTranslation().c_str();
			
			// Anzahl Antworten bei Voting eintragen
			if (question->m_weighted_answers.count(nr) > 0)
			{
				stream.str("");
				stream << " (" << (int) question->m_weighted_answers[nr] <<")";
				cstring += (CEGUI::utf8*) stream.str().c_str();
			}
			
			elemwidth =font->getTextExtent(cstring);
			elemheight = lineheight+5;

			if (elemwidth > maxwidth)
			{
				elemwidth = maxwidth;
				lines = CEGUIUtility::fitTextToWindow((CEGUI::utf8*)cstring.c_str(),maxwidth, CEGUIUtility::WordWrapLeftAligned, font).lines;
				elemheight = lines * lineheight;
			}
			width = MathHelper::Max(width,elemwidth);

			CEGUI::String selectionColour ("");
			if (label->isUserStringDefined ("selcol"))
			{
				selectionColour = label->getUserString ("selcol");
			}
			CEGUI::String expectedString;
			expectedString.append (selectionColour);
			expectedString.append (cstring);

			if (label->getText() != expectedString)
			{
				SW_DEBUG ("Updating text of widget (f) [%s] to [%s] (prev [%s])", label->getNamePath ().c_str (), expectedString.c_str (), label->getText ().c_str ());
				label->setText((CEGUI::utf8*)expectedString.c_str());
			}
			
			label->setPosition(CEGUI::UVector2(CEGUI::UDim(0,horzoffset), CEGUI::UDim(0,height)));
			CEGUIUtility::setWidgetSize (label, CEGUI::UVector2(cegui_reldim(1.0f), CEGUI::UDim(0,elemheight)));

			label->setVisible(true);

			float alpha = 1.0;
			if (!can_answer)
			{
				alpha = 0.5;
			}
			if (label->getAlpha() != alpha)
			{
				label->setAlpha(alpha);
			}

			height += elemheight + 5;
			nr++;
		}
		
		width += 2* horzoffset;
		
		//DEBUG ("Setting position of question window: (%.2f, %.2f), %.2f", 0.5f, -width/2, 0.2);
		ques->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-width/2), cegui_reldim(0.2)));
		//DEBUG ("Setting size of question window: (%.2f, %.2f)", width, height);
		CEGUIUtility::setWidgetSizeAbs (ques, width, height);
		
		ques->setVisible(true);

		// restliche Antwortlabels ausblenden
		for (; nr<acount; nr++)
		{
			stream.str("");
			stream << "GameScreen/QuestionWindow/AnswerLabel" << nr;
			
			label = CEGUIUtility::getWindow (stream.str());
			label->setVisible(false);
		}
	}
	else if (acount !=0)
	{
		m_document->getGUIState()->m_shown_windows &= ~Document::QUESTIONBOX;
		ques = (CEGUI::FrameWindow*) CEGUIUtility::getWindow ("GameScreen/QuestionWindow");
		ques->setVisible(false);
		
	}
}

bool DialogueWindow::onAnswerClicked(const CEGUI::EventArgs& evt)
{
	SW_DEBUG ("DialogueWindow::onAnswerClicked");
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	m_document->onAnswerClick(id);
	return true;
}

bool DialogueWindow::onAnswerEnterArea(const CEGUI::EventArgs& evt)
{
	//DEBUG ("DialogueWindow::onAnswerEnterArea");
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	CEGUI::String lineText = "[colour='FFFFDD66']";
	SW_DEBUG ("Entered area for: %s (%s)", we.window->getNamePath ().c_str (), we.window->getTextVisual ().c_str ());
//	we.window->setTextParsingEnabled (true);
	we.window->setText ("-");
	we.window->invalidate (true);
	we.window->setUserString ("selcol", lineText);
	
	//CEGUI::System::getSingletonPtr ()->getDefaultGUIContext ().markAsDirty ();
	//DEBUG ("Entered area for: %s", we.window->getTextVisual ().c_str ());
	//XXX, TODO
	return true;
}

bool DialogueWindow::onAnswerLeaveArea(const CEGUI::EventArgs& evt)
{
	//DEBUG ("DialogueWindow::onAnswerLeaveArea");
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	//DEBUG ("Left area for: %s", we.window->getText ().c_str ());
	SW_DEBUG ("Left area for: %s (%s)", we.window->getNamePath ().c_str (), we.window->getTextVisual ().c_str ());
	CEGUI::String lineText = "[colour='FFFFFFFF']";
	we.window->setText ("-");
	we.window->invalidate (true);
	we.window->setUserString ("selcol", lineText);
	return true;
}

bool DialogueWindow::onTextClicked(const CEGUI::EventArgs& evt)
{
	SW_DEBUG ("DialogueWindow::onTextClicked");

  const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	CEGUI::Window* btn = CEGUIUtility::getWindowForLoadedLayoutEx (m_window, "DialogWindow/DialogueLowerBar/DialogueSkipAllButton");
	bool skipAll = false;
	if (we.window == btn)
	{
		skipAll = true;
	}
	
	m_document->onSkipDialogueTextClicked(skipAll);
	return true;
}

