/*==================================================================/
Logger - header file											 	/
Author: Bartosz Pacuła												/
January, 2019														/
(based on logger by Filip Janiszewski)								/
===================================================================*/
// logger.hpp
//

#define LOGGER_CONFIG_FILENAME "logger_config.xml"
#define LOGGER_OUTPUT_FILENAME "execution.log"

#include "sstream" //for stringstream type

#ifndef IO_HEADERS
	#include "iostream"
	#include "fstream"  //for file output functionality
#endif

#ifndef MEM_CTIME_HEADERS
	#include "memory"   // unique_ptr handling
	#include "ctime"
#endif

#ifndef PUGIXML_HEADER
	#include "pugixml.hpp" // header file for xml support
#endif

//under minGW with gcc there's no support for mutex handling
#ifndef GCC_COMPILER
	#include "thread"	//for multithread handling
	#include "mutex"	//for multithread handling
#endif
//----------------------------------------------------------

namespace logging 
{

	using namespace std;


	class LogPolicyInterface {
	public:
		virtual void openFile(const string& filename) = 0;
		virtual void closeFile() = 0;
		virtual void write(const string& msg) = 0;
	};

	class FileLogPolicy : public LogPolicyInterface {
	private:
		unique_ptr<ofstream> outFile;
	public:
		FileLogPolicy() : outFile(new ofstream) {}
		void openFile(const string& filename);
		void closeFile();
		void write(const string& msg);
		~FileLogPolicy();
	};

	class ConsoleLogPolicy : public LogPolicyInterface {
	public:
		ConsoleLogPolicy() {} //empty imlementation just for compatibility with the logger template class 
		void openFile(const string& filename) {} //empty imlementation just for compatibility with the logger template class
		void closeFile() {} //empty imlementation just for compatibility with the logger template class
		void write(const string& msg);
	};

	void FileLogPolicy::openFile(const string& filename)
	{
		outFile->open(filename,ios::out);
		if(!outFile->is_open())
		{
			string w="\nLogger: Can't open output file!";
			cout << w;
			throw(runtime_error(w));
		}
	}

	void FileLogPolicy::closeFile()
	{
		if(outFile)
			outFile->close();
	}

	void FileLogPolicy::write(const string& msg)
	{
		(*outFile) << msg << "\n"; //writes a message line into the output file
	}

	FileLogPolicy::~FileLogPolicy() //closes the output file on this object destruction
	{
		if(outFile)
			closeFile();
	}

	void ConsoleLogPolicy::write(const string& msg)
	{
		cout << msg << "\n"; 
	}

	enum log_type //types of data to be logged
	{
		trace=1,
		error
	};


	template<typename log_policy> class Logger { //universal logger class
		private:
			unsigned int logLineNumber;
			string getTime();	//gets system time as string
			string getLineHeader();
			stringstream logStream;
			log_policy* policy;
			#ifndef GCC_COMPILER
				mutex writeMutex;
			#endif

			void printImpl();
			template<typename first, typename... rest> void printImpl(first param1, rest ...param);
		public:
			Logger(); // argumentless constructor for console output only
			Logger(const string& filename);
			template<log_type type, typename... Args> void print(Args ...args);
			~Logger();
	};


	template<typename log_policy> Logger<log_policy>::Logger(const string& filename){
		logLineNumber=0;
		policy=new log_policy;
		if(!policy)
		{
			string w="\nLogger: Can't create logger instance!";
			cout << w;
			throw(runtime_error(w));
		}
		policy->openFile(filename);
	}

	template<typename log_policy> Logger<log_policy>::Logger(){
		logLineNumber=0;
		policy=new log_policy;
		if(!policy)
		{
			string w="\nLogger: Can't create logger instance!";
			cout << w;
			throw(runtime_error(w));
		}
	}

	template<typename log_policy> Logger<log_policy>::~Logger(){
		if(policy)
		{
			policy->closeFile();
			delete policy;
		}
	}

	template<typename log_policy> string Logger<log_policy>::getTime()
	{
		string strTime;
		time_t raw_time;
		char str[26];  //helper string to use in ctime_s()
		time(&raw_time); //gets system time
		strTime=ctime_s(str,sizeof(str),&raw_time); //converts to string
		return strTime.substr(0,strTime.size()-1); //returns a time string without newline character
	}

	template<typename log_policy> string Logger<log_policy>::getLineHeader() //generates line header with current line number and time
	{
		stringstream header;
		header.str("");
		header.fill('0');
		header.width(7);
		header << logLineNumber++ << " < " << getTime() << " - "; //logline number and current system time value
		header.fill('0');
		header.width(7);
		header << clock() << " > ~ "; //number of clock ticks elapsed relatively to the program start
		return header.str();
	}

	template<typename log_policy> void Logger<log_policy>::printImpl()
	{
		policy->write(getLineHeader() + logStream.str());
		logStream.str("");
	}

	template<typename log_policy>
		template<typename first,typename... rest> void Logger<log_policy>::printImpl(first param1,rest ...param)
	{
		logStream << param1;
		printImpl(param...); //recursive
	}

	template<typename log_policy>
		template<log_type type, typename... Args> void Logger<log_policy>::print(Args ...args)
	{
		#ifndef GCC_COMPILER
		writeMutex.lock();
		#endif

		switch(type)
		{
			case log_type::trace:
			logStream << "<TRACE> :";
			break;

			case log_type::error:
			logStream << "<ERROR> :";
			break;
		}
		printImpl(args...);

		#ifndef GCC_COMPILER
		writeMutex.unlock();
		#endif
	}

	pair<bool,bool> loadLoggerConfig(const char* filename) //loads logger's configuration from xml file
	{
		using namespace pugi;

		xml_document doc; 
		xml_parse_result result=doc.load_file(filename); //loading xml config file using pugi library function
		if(!result)
		{
			cout << "XML file read error!";
			return make_pair(0,0);
		}

		xml_node data = doc.child("Configuration").child("Data");
		bool level=data.attribute("Level").as_bool();
		bool output=data.attribute("Output").as_bool();

		return make_pair(level,output);
	}

	//----------------------------------------------------------------------------------
	static Logger<FileLogPolicy> *LoggerFLP_ptr; //global pointer for file logging
	static Logger<ConsoleLogPolicy> *LoggerCLP_ptr; //global pointer for console logging
	//----------------------------------------------------------------------------------

	class LogConfig { //helper class for getting logger configuration from file and creating an appropriate logger instance
	public:
		LogConfig();
		~LogConfig();
		bool getLevel() const;
		bool getOutput() const;
	private:
		bool level;
		bool output;
	};

	LogConfig::LogConfig(){

		pair<bool,bool> p=loadLoggerConfig(LOGGER_CONFIG_FILENAME);
		level=p.first;
		output=p.second;
		if(!output)
			LoggerCLP_ptr=new Logger<ConsoleLogPolicy>(); //creating new logger instance
		else if(output)
				LoggerFLP_ptr=new Logger<FileLogPolicy>(LOGGER_OUTPUT_FILENAME);  //creating new logger instance
	}

	LogConfig::~LogConfig(){
		if(LoggerCLP_ptr)
			delete LoggerCLP_ptr;
		if(LoggerFLP_ptr)
			delete LoggerFLP_ptr;
	}

	bool LogConfig::getLevel() const {
		return level;
	}

	bool LogConfig::getOutput() const {
		return output;
	}


	//--------------------------------------------------------------------
	static LogConfig logConfInstance; //creating new logging configuration

	template<log_type type,typename... Args> void _Log(Args ...args) //wrapper function template for printing log message
	{
		if(LoggerCLP_ptr)
			LoggerCLP_ptr->print<type>(args...);
		if(LoggerFLP_ptr)
			LoggerFLP_ptr->print<type>(args...);
	}

	//shorter syntax definitions-------------------------------------
	#define LOG if(!logging::logConfInstance.getLevel()) logging::_Log<logging::log_type::trace>
	#define ELOG if(logging::logConfInstance.getLevel()) logging::_Log<logging::log_type::error>
	//---------------------------------------------------------------

} //end of namespace logging
