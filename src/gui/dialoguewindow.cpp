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

DialogueWindow::DialogueWindow(Document* doc, Scene* scene)
	: Window(doc)
{
	m_scene = scene;
	CEGUI::Window* label;
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* game_screen =  win_mgr.getWindow("GameScreen");
	CEGUI::ScrollablePane* pane;
	
	// Oberer und Unterer Balken
	CEGUI::FrameWindow* lower_bar = (CEGUI::FrameWindow*) win_mgr.createWindow("TaharezLook/FrameWindow", "DialogueLowerBar");
	lower_bar->setProperty("FrameEnabled","false");
	lower_bar->setProperty("TitlebarEnabled","false");
	lower_bar->setProperty("CloseButtonEnabled","false");
	lower_bar->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim( 0.85f)));
	lower_bar->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 0.15f)));
	lower_bar->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));
	lower_bar->setAlwaysOnTop(true);
	lower_bar->setVisible(false);
	lower_bar->setWantsMultiClickEvents(false);
	
	game_screen->addChildWindow(lower_bar);
	
	CEGUI::FrameWindow* upper_bar = (CEGUI::FrameWindow*) win_mgr.createWindow("TaharezLook/FrameWindow", "DialogueUpperBar");
	upper_bar->setProperty("FrameEnabled","false");
	upper_bar->setProperty("SizingEnabled","false");
	upper_bar->setProperty("TitlebarEnabled","false");
	upper_bar->setProperty("CloseButtonEnabled","false");
	upper_bar->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim( 0.0f)));
	upper_bar->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 0.15f)));
	upper_bar->setAlwaysOnTop(true);
	upper_bar->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));
	upper_bar->setVisible(false);
	upper_bar->setWantsMultiClickEvents(false);
	
	game_screen->addChildWindow(upper_bar);
	
	label = win_mgr.createWindow("TaharezLook/StaticImage", "UpperBarBackground");
	upper_bar->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "true");
	label->setProperty("MousePassThroughEnabled", "true");
	label->moveToBack ();
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.00f), cegui_reldim( 0.00f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 0.95f)));
	
	label = win_mgr.createWindow("TaharezLook/StaticImage", "LowerBarBackground");
	lower_bar->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "true");
	label->setProperty("MousePassThroughEnabled", "true");
	label->moveToBack ();
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.00f), cegui_reldim( 0.05f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 0.95f)));
	
	
	label = win_mgr.createWindow("TaharezLook/StaticImage", "SpeakerEmotionImage0");
	lower_bar->addChildWindow(label);
	label->setProperty("FrameEnabled", "true");
	label->setProperty("BackgroundEnabled", "true");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.01f), cegui_reldim( 0.05f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.10f), cegui_reldim( 0.9f)));
	
	label = win_mgr.createWindow("TaharezLook/StaticImage", "SpeakerEmotionImage1");
	lower_bar->addChildWindow(label);
	label->setProperty("FrameEnabled", "true");
	label->setProperty("BackgroundEnabled", "true");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.89f), cegui_reldim( 0.05f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.10f), cegui_reldim( 0.9f)));
	
	label = win_mgr.createWindow("TaharezLook/StaticText", "SpeakerNameLabel0");
	lower_bar->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "true");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim(0.05f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.3f), cegui_reldim( 0.22f)));
	
	
	label = win_mgr.createWindow("TaharezLook/StaticText", "SpeakerNameLabel1");
	lower_bar->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "true");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.58f), cegui_reldim(0.05f)));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.3f), cegui_reldim( 0.22f)));
	
	
	pane = static_cast<CEGUI::ScrollablePane*> (win_mgr.createWindow("TaharezLook/ScrollablePaneNoBar", "SpeakerTextPane0"));
	lower_bar->addChildWindow(pane);
	pane->setPosition(CEGUI::UVector2(cegui_reldim(0.12f), cegui_reldim(0.27f)));
	pane->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));
	pane->setSize(CEGUI::UVector2(cegui_reldim(0.38f), cegui_reldim( 0.66f)));
	
	label = win_mgr.createWindow("TaharezLook/StaticText", "SpeakerTextLabel0");
	pane->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "true");
	label->setFont("DejaVuSerif-10");
	//label->setProperty("HorzFormatting", "WordWrapLeftAligned");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.99f), cegui_reldim( 0.99f)));
	label->setWantsMultiClickEvents(false);
	
	pane = static_cast<CEGUI::ScrollablePane*> (win_mgr.createWindow("TaharezLook/ScrollablePaneNoBar", "SpeakerTextPane1"));
	lower_bar->addChildWindow(pane);
	pane->setPosition(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim(0.27f)));
	pane->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));
	pane->setSize(CEGUI::UVector2(cegui_reldim(0.38f), cegui_reldim( 0.66f)));
	
	label = win_mgr.createWindow("TaharezLook/StaticText", "SpeakerTextLabel1");
	pane->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "true");
	label->setFont("DejaVuSerif-10");
	//label->setProperty("HorzFormatting", "WordWrapLeftAligned");
	label->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)));
	label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onTextClicked, this));
	label->setSize(CEGUI::UVector2(cegui_reldim(0.99f), cegui_reldim( 0.99f)));
	label->setWantsMultiClickEvents(false);
}

void DialogueWindow::update()
{
	CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
	
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
	
	CEGUI::FrameWindow* upper_bar = static_cast<CEGUI::FrameWindow*>( win_mgr.getWindow("DialogueUpperBar"));
	CEGUI::FrameWindow* lower_bar = static_cast<CEGUI::FrameWindow*>( win_mgr.getWindow("DialogueLowerBar"));
	if (upper_bar->isVisible() != bar_vis)
	{
		upper_bar->setVisible(bar_vis);
		lower_bar->setVisible(bar_vis);
	}
	
	if (bar_vis)
	{
		CEGUI::Window* wimage;
		CEGUI::Window* wname;
		CEGUI::Window* wtext;
		CEGUI::ScrollablePane* wpane;
		
		if (dia != 0)
		{
			// Schleife fuer die moeglichen Sprecher eines Dialogs
			std::string image, name, text;
			
			WorldObject* wo;
			Creature* cr;
			
			std::stringstream stream;
			
			Dialogue::SpeakerState* spstate;
			for (int i=0; i<Dialogue::NR_POSITIONS; i++)
			{
				stream.str("");
				stream << "SpeakerEmotionImage";
				stream << i;
				wimage = win_mgr.getWindow(stream.str());
						
				stream.str("");
				stream << "SpeakerNameLabel";
				stream << i;
				wname = win_mgr.getWindow(stream.str());
				
				stream.str("");
				stream << "SpeakerTextLabel";
				stream << i;
				wtext = win_mgr.getWindow(stream.str());
				
				
				stream.str("");
				stream << "SpeakerTextPane";
				stream << i;
				wpane = static_cast<CEGUI::ScrollablePane*> (win_mgr.getWindow(stream.str()));
				
				
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
						CEGUI::Font* fnt = wtext->getFont();
						
						if (wtext->getText() != (CEGUI::utf8*) text.c_str())
						{
							CEGUI::UVector2 size = wtext->getSize();
							CEGUI::Rect isize = wtext->getUnclippedInnerRect ();
							FormatedText txt = CEGUIUtility::fitTextToWindow((CEGUI::utf8*)text.c_str(), isize.getWidth(), CEGUIUtility::WordWrapCentred, fnt);
							float height = PixelAligned(txt.lines * fnt->getLineSpacing());
							size.d_y = CEGUI::UDim(0.0, height);
							wtext->setSize(size);
							wtext->setText((CEGUI::utf8*)txt.text.c_str());
						}
						wtext->setVisible(true);
						
						// Scrolling des Texts
						float height = wtext->getSize().d_y.d_offset;
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
				stream << "SpeakerEmotionImage";
				stream << i;
				wimage = win_mgr.getWindow(stream.str());
						
				stream.str("");
				stream << "SpeakerNameLabel";
				stream << i;
				wname = win_mgr.getWindow(stream.str());
				
				stream.str("");
				stream << "SpeakerTextLabel";
				stream << i;
				wtext = win_mgr.getWindow(stream.str());
				
				stream.str("");
				stream << "SpeakerTextPane";
				stream << i;
				
				wimage->setVisible(false);
				wname->setVisible(false);
				wtext->setVisible(false);
			}
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
	CEGUI::Window* game_screen =  win_mgr.getWindow("GameScreen");
	
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
			stream << "SpeechFrame";
			stream << nr;
			
			
			speakframe = (CEGUI::FrameWindow*) win_mgr.createWindow("TaharezLook/FrameWindow", stream.str());
			speakframe->setProperty("FrameEnabled","false");
			speakframe->setProperty("TitlebarEnabled","false");
			speakframe->setProperty("CloseButtonEnabled","false");
			game_screen->addChildWindow(speakframe);
			speakframe->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
			speakframe->setVisible(false);
			speakframe->setSize(CEGUI::UVector2(CEGUI::UDim(0,200), CEGUI::UDim(0,picsize+10)));
			
			stream.str("");
			stream << "SpeechLabel";
			stream << nr;
			
			label = win_mgr.createWindow("TaharezLook/StaticText", stream.str());
			speakframe->addChildWindow(label);
			label->setProperty("FrameEnabled", "true");
			label->setProperty("BackgroundEnabled", "true");
			label->setSize(CEGUI::UVector2(CEGUI::UDim(0,80), CEGUI::UDim(0,picsize)));
			label->setPosition(CEGUI::UVector2(CEGUI::UDim(0,picsize+10), CEGUI::UDim(0,5)));
			
			label->setText("");
			label->setAlpha(0.9);
			
			stream.str("");
			stream << "SpeechIamge";
			stream << nr;
			
			image = win_mgr.createWindow("TaharezLook/StaticImage", stream.str());
			speakframe->addChildWindow(image);
			image->setProperty("BackgroundEnabled", "true");
			image->setSize(CEGUI::UVector2(CEGUI::UDim(0,picsize), CEGUI::UDim(0,picsize)));
			image->setPosition(CEGUI::UVector2(CEGUI::UDim(0,5), CEGUI::UDim(0,5)));
		}
		else
		{
			stream.str("");
			stream << "SpeechFrame";
			stream << nr;
			
			speakframe = static_cast<CEGUI::FrameWindow*>( win_mgr.getWindow(stream.str()) );
			
			stream.str("");
			stream << "SpeechLabel";
			stream << nr;
			label = win_mgr.getWindow(stream.str());
			
			stream.str("");
			stream << "SpeechIamge";
			stream << nr;
			image = win_mgr.getWindow(stream.str());
			
		}
		
			
			
		if (label->getText() != (CEGUI::utf8*) text.c_str())
		{

			CEGUI::Font* font = label->getFont();

			
			
			float width = font->getTextExtent((CEGUI::utf8*) text.c_str())+15;
			float height = font->getFontHeight() +15;
			CEGUI::Rect rect = game_screen->getInnerRectClipper();

			float maxwidth = rect.getWidth()/4;
                        //CEGUI::Rect maxWidthRect;
                        //maxWidthRect.setWidth(maxwidth);
			
                        FormatedText txt = CEGUIUtility::fitTextToWindow((CEGUI::utf8*) text.c_str(), maxwidth, CEGUIUtility::WordWrapLeftAligned, font);
			size_t lines = txt.lines;
			label->setText((CEGUI::utf8*) txt.text.c_str());
			
			// Testen ob der Text auf eine Zeile passt
			if (!txt.lines > 1)
			{
				// einzelne Zeile
				label->setSize(CEGUI::UVector2(CEGUI::UDim(0,width),  CEGUI::UDim(0,picsize)));
				speakframe->setSize(CEGUI::UVector2(CEGUI::UDim(0,picsize+15+width), CEGUI::UDim(0,picsize+10)));
				image->setPosition(CEGUI::UVector2(CEGUI::UDim(0,5), CEGUI::UDim(0,5)));
			}
			else
			{
				// mehrere Zeilen
				rect.setWidth(maxwidth-15);
				
				height = lines * font->getFontHeight() +15;
				
				if (height < picsize)
				{
					label->setSize(CEGUI::UVector2(CEGUI::UDim(0,maxwidth),  CEGUI::UDim(0,picsize)));
					speakframe->setSize(CEGUI::UVector2(CEGUI::UDim(0,picsize+15+maxwidth), CEGUI::UDim(0,picsize+10)));
					image->setPosition(CEGUI::UVector2(CEGUI::UDim(0,5), CEGUI::UDim(0,5)));
				}
				else
				{
					label->setSize(CEGUI::UVector2(CEGUI::UDim(0,maxwidth),  CEGUI::UDim(0,height)));
					speakframe->setSize(CEGUI::UVector2(CEGUI::UDim(0,picsize+15+maxwidth), CEGUI::UDim(0,height+10)));
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
		stream << "SpeechFrame";
		stream << nr;
		speakframe = static_cast<CEGUI::FrameWindow*>(win_mgr.getWindow(stream.str()));
			
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
			ques = (CEGUI::FrameWindow*) win_mgr.createWindow("TaharezLook/FrameWindow", "QuestionWindow");
			ques->setProperty("FrameEnabled","false");
			ques->setProperty("TitlebarEnabled","false");
			ques->setProperty("CloseButtonEnabled","false");
			game_screen->addChildWindow(ques);
			ques->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Window::consumeEvent, (Window*) this));
			ques->setVisible(false);
			
			label = win_mgr.createWindow("TaharezLook/StaticText", "QuestionLabel");
			ques->addChildWindow(label);
			label->setProperty("FrameEnabled", "false");
			label->setProperty("BackgroundEnabled", "false");
		}
		else
		{
			ques = (CEGUI::FrameWindow*)win_mgr.getWindow("QuestionWindow");
			label = (CEGUI::Window*) win_mgr.getWindow("QuestionLabel");
			label->setProperty("HorzFormatting", "WordWrapLeftAligned");
		}
		
		nr =0;
		
		// Groesse des Fensters ermitteln
		float height=5,width=0;
		float elemheight, elemwidth;
		int lines;
		float horzoffset = 10;
		
		CEGUI::Rect rect = game_screen->getInnerRectClipper();
		float maxwidth = rect.getWidth()/4;
		rect.setWidth(maxwidth-15);
		
		CEGUI::Font* font = label->getFont();
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
		}
		
		label->setPosition(CEGUI::UVector2(CEGUI::UDim(0,horzoffset), CEGUI::UDim(0,height)));
		label->setSize(CEGUI::UVector2(cegui_reldim(1.0f), CEGUI::UDim(0,elemheight)));
		
		height += elemheight + 20;
		
		// Antworten einfuegen
		std::list < std::pair<TranslatableString, std::string> >::iterator it;
		for (it = question->m_answers.begin(); it != question->m_answers.end(); ++it)
		{
			stream.str("");
			stream << "AnswerLabel";
			stream << nr;
			
			if (nr >= acount)
			{
				acount ++;
				label = win_mgr.createWindow("TaharezLook/StaticText", stream.str());
				ques->addChildWindow(label);
				label->setProperty("FrameEnabled", "false");
				label->setProperty("BackgroundEnabled", "false");
				//label->setProperty("HorzFormatting", "WordWrapLeftAligned");
				label->setID(nr);
				label->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&DialogueWindow::onAnswerClicked, this));
			}
			else
			{
				label = win_mgr.getWindow(stream.str());
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


			if (label->getText() != cstring)
			{
				label->setText((CEGUI::utf8*)cstring.c_str());
			}
			
			label->setPosition(CEGUI::UVector2(CEGUI::UDim(0,horzoffset), CEGUI::UDim(0,height)));
			label->setSize(CEGUI::UVector2(cegui_reldim(1.0f), CEGUI::UDim(0,elemheight)));

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
		
		ques->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,-width/2), cegui_reldim(0.2)));
		ques->setSize(CEGUI::UVector2(CEGUI::UDim(0,width), CEGUI::UDim(0,height)));
		
		ques->setVisible(true);

		// restliche Antwortlabels ausblenden
		for (; nr<acount; nr++)
		{
			stream.str("");
			stream << "AnswerLabel";
			stream << nr;
			
			label = win_mgr.getWindow(stream.str());
			label->setVisible(false);
		}
	}
	else if (acount !=0)
	{
		m_document->getGUIState()->m_shown_windows &= ~Document::QUESTIONBOX;
		ques = (CEGUI::FrameWindow*) win_mgr.getWindow("QuestionWindow");
		ques->setVisible(false);
		
	}
}

bool DialogueWindow::onAnswerClicked(const CEGUI::EventArgs& evt)
{
	const CEGUI::MouseEventArgs& we =
			static_cast<const CEGUI::MouseEventArgs&>(evt);
	unsigned int id = we.window->getID();
	
	m_document->onAnswerClick(id);
	return true;
}


bool DialogueWindow::onTextClicked(const CEGUI::EventArgs& evt)
{
	m_document->onSkipDialogueTextClicked();
	return true;
}

