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

#include "benchmarktab.h"
#include "sumwarshelper.h"

#include "Ogre.h"
#include "OgrePlatformInformation.h"

#include "Poco/Environment.h"

#include "CEGUI/CEGUIWindowManager.h"
#include "CEGUI/CEGUI.h"

#include <eventsystem.h>
#include <debugpanel.h>


using namespace CEGUI;
using Poco::Environment;

CEGUI::String BenchmarkTab::WidgetTypeName = "BenchmarkTab";



BenchmarkTab::BenchmarkTab(const CEGUI::String& type, const CEGUI::String& name): CEGUI::Window(type, name), DebugTab(), Ogre::LogListener()
{
	setText("Benchmark");

	m_tabLayout = CEGUI::WindowManager::getSingleton().loadWindowLayout("BenchmarkTab.layout");
	m_tabLayout->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	m_tabLayout->setSize(UVector2(UDim(1.0f, 0.0f), UDim(1.0f, 0.0f)));
	this->addChildWindow(m_tabLayout);

	m_CapsBox = static_cast<CEGUI::MultiLineEditbox*>(m_tabLayout->getChild("BenchmarkTab/ResultsEditbox"));
	m_startBenchmarkButton = static_cast<CEGUI::PushButton*>(m_tabLayout->getChild("BenchmarkTab/StartButton"));
	
	m_ogreRoot = Ogre::Root::getSingletonPtr();

	m_log = Ogre::LogManager::getSingleton().getLog(SumwarsHelper::userPath() + "/BenchLog.log");// Ogre::LogManager::getSingleton().createLog("BenchLog.log");
	m_log->addListener(this);
	m_log->logMessage("-------------------------");

	m_startBenchmarkButton->subscribeEvent(PushButton::EventClicked, CEGUI::Event::Subscriber(&BenchmarkTab::handleStartBenchmark, this));
}

bool BenchmarkTab::handleStartBenchmark(const CEGUI::EventArgs& e)
{
	m_log->logMessage(Environment::osName() + " " + Environment::osVersion() + " " + Environment::osArchitecture());
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

void BenchmarkTab::messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName)
{
	m_CapsBox->appendText(message.c_str());
}


void BenchmarkTab::update(OIS::Keyboard *keyboard, OIS::Mouse *mouse)
{
}
