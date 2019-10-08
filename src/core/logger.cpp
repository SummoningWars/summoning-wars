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

#include "logger.h"
#include <fstream>
#include <iostream>
#include <string>

void Log::writeMessage(const char* message, int loglevel)
{
	if (m_stream != 0 && (loglevel & m_loglevel))
	{
		if (loglevel & MSG_ERROR)
			(*m_stream) << "[E] ";
		else if (loglevel & MSG_WARNING)
			(*m_stream) << "[W] ";
		else if (loglevel & MSG_INFO)
			(*m_stream) << "[I] ";
		else if (loglevel & MSG_DEBUG)
			(*m_stream) << "[D] ";
		(*m_stream) << message;
		(*m_stream) << "\n";
		if (m_config & ALWAYS_FLUSH)
		{
			flush();
		}
	}
}

FileLog::FileLog(std::string filename, int loglevel ,int config )
	: Log(NULL, loglevel, config)
{
	std::ofstream* file = new std::ofstream;
	file->open(filename.c_str());
	if (file->is_open())
	{
		setStream(file);
	}
	else
	{
		delete file;
	}
}

FileLog::~FileLog()
{
	if (m_stream != 0)
	{
		static_cast<std::ofstream*>(m_stream)->close();
		delete m_stream;
	}
}

StdOutLog::StdOutLog(int loglevel,int config)
	: Log(&(std::cout), loglevel, config)
{
	
}

LogManager* LogManager::s_instance=0;

LogManager::~LogManager()
{
	std::map<std::string, Log*>::iterator it;
	for (it = m_logs.begin(); it != m_logs.end(); ++it)
	{
		delete it->second;
	}
	m_logs.clear();
}

void LogManager::writeMessage(const char* message, int loglevel)
{
	std::map<std::string, Log*>::iterator it;
	for (it = m_logs.begin(); it != m_logs.end(); ++it)
	{
		it->second->writeMessage(message,loglevel);
	}
}

void LogManager::addLog(std::string logname, Log* log)
{
	if (m_logs.count(logname) != 0)
	{
		delete m_logs[logname];
	}
	m_logs[logname] = log;
}

void LogManager::deleteLog(std::string logname)
{
	if (m_logs.count(logname) > 0)
	{
		delete m_logs[logname];
		m_logs.erase(logname);
	}
}

Log* LogManager::getLog(std::string logname)
{
	std::map<std::string, Log*>::iterator it;
	it = m_logs.find(logname);
	if (it != m_logs.end())
	{
		return it->second;
	}
	return 0;
}

