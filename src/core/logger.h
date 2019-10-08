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

#pragma once

#include <iostream>
#include <map>

/**
 * \class Log
 * \brief Klasse fuer ein zu schreibendes Log, Daten koennen auf den Bildschirm oder auf Dateien geschrieben werden
 */
class Log
{
	public:
		
		/**
		* Auflistung verschiedener Einstellungen welche Nachrichten geloggt werden sollen
		*/
		enum LogLevel
		{
			LOGLEVEL_ERROR = 0x1,
			LOGLEVEL_WARNING = 0x011,
			LOGLEVEL_INFO = 0x0111,
			LOGLEVEL_DEBUG = 0x1111,
			
			LOGLEVEL_WARNING_ONLY = 0x10,
			LOGLEVEL_INFO_ONLY = 0x100,
			LOGLEVEL_DEBUG_ONLY = 0x1000,
		};
		
		/**
		* Auflistung verschiedener Einstufung von Nachrichten
		*/
		enum MessageLogLevel
		{
			MSG_ERROR = 0x1,
			MSG_WARNING = 0x10,
			MSG_INFO = 0x100,
			MSG_DEBUG = 0x1000,
		};
		
		/**
		 * Verschiedene Einstellungsflags des Logs
		 */
		enum Configuration
		{
			ALWAYS_FLUSH = 0x4,
			
			DEFAULT_CONFIG = 0x4
		};
		
		/**
		 * \brief Konstruktor
		 * \param stream Ausgabestrom auf den die Logmeldungen geschrieben werden
		 * \param loglevel Loglevel des Logs
		 * \param config Bitmaske mit Configuration Bits
		 */
		Log(std::ostream* stream, int loglevel = LOGLEVEL_ERROR,int config = DEFAULT_CONFIG)
		{
			m_stream = stream;
			m_loglevel = loglevel;
			m_config = config;
		}
		
		/**
		 * \brief Destruktor
		 */
		virtual ~Log()
		{
		}
		
		/**
		 * \brief Gibt das Loglevel aus
		 * \return Loglevel
		 */
		int getLoglevel()
		{
			return m_loglevel;
		}
		
		/**
		 * \brief setzt das Loglevel
		 * \param loglevel neues Loglevel
		 */
		void setLoglevel(int loglevel)
		{
			m_loglevel = loglevel;
		}
		
		/**
		 * \brief Schreibt die Nachricht in das Log
		 */
		void writeMessage(const char* message, int loglevel);
		
		/**
		 * Schreibt Daten die eventuell gepuffert sind auf den Zielort
		 */
		void flush()
		{
			if (m_stream != 0)
				m_stream->flush();
		}
	
	private:
		/**
		 * Bitmaske mit allen Config Bits
		 */
		int m_config;
		
		/**
		 * Bitmaske, die einstellt welche Nachrichten geloggt werden
		 */
		int m_loglevel;
		
	protected:
		
		/**
		 * \brief Setzt den Zielort der Ausgabe
		 * \param stream Ausgabestrom
		 */
		void setStream(std::ostream* stream)
		{
			m_stream = stream;
		}
		
		/**
		 * Zielort, an den die Daten geschrieben werden
		 */
		std::ostream* m_stream;
};

/**
 * \class FileLog
 * \brief Klasse fuer ein Log, das in eine Datei geschrieben wird
 */
class FileLog : public Log
{
	public:
		/**
		 * \brief Konstruktor
		 * \param filename Name der Datei
		 * \param loglevel Loglevel des Logs
		 * \param config Bitmaske mit Configuration Bits
		 */
		FileLog(std::string filename, int loglevel = LOGLEVEL_ERROR,int config = DEFAULT_CONFIG);
		
		/**
		 * \brief Destruktor
		 */
		virtual ~FileLog();
};

/**
 * \class StdOutLog
 * Klasse fuer ein Log, das auf stdout geschrieben wird
 */
class StdOutLog : public Log
{
	public:
		/**
		 * \brief Konstruktor
		 * \param loglevel Loglevel des Logs
		 * \param config Bitmaske mit Configuration Bits
		 */
		StdOutLog(int loglevel = LOGLEVEL_ERROR,int config = DEFAULT_CONFIG);
		
		/**
		 * \brief Destruktor
		 */
		virtual ~StdOutLog()
		{
			
		}
};


/**
 * \class LogManager
 * \brief Klasse fuer die Verwaltung mehrerer Logs
 */
class LogManager
{
	public:
		/**
		 * \brief Gibt die einzige Instanz der Klasse zurueck
		 * \return LogManager Instanz
		 */
		static LogManager& instance()
		{
			if (s_instance == 0)
			{
				s_instance = new LogManager;
			}
			return *s_instance;
		}
		
		/**
		 * \brief Loescht die Instanz der Klasse
		 */
		static void cleanup()
		{
			if (s_instance != 0)
				delete s_instance;
			s_instance =0;
		}
		
		/**
		 * \brief Destruktor
		 */
		~LogManager();
		
		/**
		 * \brief Schreibt eine Nachricht in alle Logs
		 * \param message Nachricht
		 * \param loglevel Loglevel
		 */
		void writeMessage(const char* message, int loglevel);
		
		/**
		 * \brief Erzeugt ein neues Log
		 * \param logname Name des Logs
		 * \param log Log das hinzugefuegt wird
		 */
		void addLog(std::string logname, Log* log);
		
		/**
		 * \brief Loescht das Log mit dem angegebenen Name
		 * \param logname Name eines Logs
		 */
		void deleteLog(std::string logname);
		
		/**
		 * \brief Gibt ein Log aus
		 */
		Log* getLog(std::string logname);
		
	private:
		/**
		 * \brief Konstruktor
		 */
		LogManager()
		{
			
		}
		
		/**
		 * \brief einzige Instanz der Klassse
		 */
		static LogManager* s_instance;
		
		/**
		 * \brief alle Logs, geordnet nach Name
		 */
		std::map<std::string, Log*> m_logs;
		
};



