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

// Utility for CEGUI cross-version compatibility
#include "ceguiutility.h"

#include "benchmarktab.h"
#include "sumwarshelper.h"

#include "Ogre.h"
#include "OgrePlatformInformation.h"

#ifdef CEGUI_07
#include "CEGUI/CEGUIWindowManager.h"
#else
#include "CEGUI/WindowManager.h"
#endif

#include "CEGUI/CEGUI.h"

#include <eventsystem.h>
#include <debugpanel.h>


using namespace CEGUI;

CEGUI::String BenchmarkTab::WidgetTypeName = "BenchmarkTab";



BenchmarkTab::BenchmarkTab(const CEGUI::String& type, const CEGUI::String& name): CEGUI::Window(type, name), DebugTab(), Ogre::LogListener()
{
	setText("Benchmark");

	m_tabLayout = CEGUIUtility::loadLayoutFromFile ("benchmarktab.layout");
	m_tabLayout->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
  CEGUIUtility::setWidgetSizeRel (m_tabLayout, 1.0f, 1.0f);
  CEGUIUtility::addChildWidget(this, m_tabLayout);

	m_CapsBox = static_cast<CEGUI::MultiLineEditbox*>(m_tabLayout->getChild("ResultsEditbox"));
	m_startBenchmarkButton = static_cast<CEGUI::PushButton*>(m_tabLayout->getChild("StartButton"));
	
	m_ogreRoot = Ogre::Root::getSingletonPtr();

	m_log = Ogre::LogManager::getSingleton().getLog(SumwarsHelper::userPath() + "/BenchLog.log");// Ogre::LogManager::getSingleton().createLog("BenchLog.log");
	m_log->addListener(this);
	m_log->logMessage("-------------------------");

	m_startBenchmarkButton->subscribeEvent(PushButton::EventClicked, CEGUI::Event::Subscriber(&BenchmarkTab::handleStartBenchmark, this));
}

bool BenchmarkTab::handleStartBenchmark(const CEGUI::EventArgs& e)
{
    // TODO: Retriev OS information here
    //m_log->logMessage(Environment::osName() + " " + Environment::osVersion() + " " + Environment::osArchitecture());
	m_log->logMessage("\n---------------------");


	Ogre::PlatformInformation::log(m_log);
	const_cast<Ogre::RenderSystemCapabilities*>(m_ogreRoot->getRenderSystem()->getCapabilities())->log(m_log);
	
	m_log->logMessage("\n---------------------");
	m_log->logMessage("ogre.cfg");
	
	std::string line;
	std::ifstream cfgfile ("ogre.cfg");
	if (cfgfile.is_open())
	{
		while ( cfgfile.good() )
		{
			std::getline(cfgfile,line);
			m_log->logMessage(line.c_str());
		}
		cfgfile.close();
	}

	m_log->logMessage("\n");
	EventSystem::doString("startBenchmark()");
	DebugPanel::getSingleton().toogleVisibility();

	return true;
}

	
#if OGRE_VERSION_MAJOR == 1
#if OGRE_VERSION_MINOR == 7
void BenchmarkTab::messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName)
{
        m_CapsBox->appendText(message.c_str());
}
#endif
#if OGRE_VERSION_MINOR >= 8
void BenchmarkTab::messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName, bool& skipThisMessage)
{
	if (!skipThisMessage)
    {
        m_CapsBox->appendText(message.c_str());
    }
}
#endif
#endif

void BenchmarkTab::update(OIS::Keyboard *keyboard, OIS::Mouse *mouse)
{
}
