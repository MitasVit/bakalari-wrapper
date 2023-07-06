#pragma once
#include "Bakalari_api.hpp"

#define WIN32_LEAN_AND_MEAN
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Multi.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <nlohmann/json.hpp>

#include <sstream>

using namespace std;
using json = nlohmann::json;

using namespace curlpp;
using namespace curlpp::options;
using namespace placeholders;
using namespace Options;

static Easy req_provider;
static stringstream buff;

// headers is only bearer token
// with shared request provider and stringstream
// return 1 if ok
// 0  | JSON error
//-1 | Curlpp, logic error
//-2 | Curlpp, runtime error
//-3 | STD error
//-4 | unknown?
// code is response code for HTTP
bool GetJson(Easy *uireq, stringstream *ss, string full_url, list<string> headers, json *j, long *code)
{
	try
	{
		string full = "Accept: application/json";
		headers.push_back(full);

		uireq->setOpt(new Url(full_url.c_str()));
		uireq->setOpt(new WriteStream(ss));
		uireq->setOpt(new HttpHeader(headers));
		uireq->perform();

		long _code;
		uireq->getCurlHandle().getInfo(CURLINFO_RESPONSE_CODE, _code);
		*code = _code;

		cout << "str: " << ss->str() << endl;
		*j = json::parse(ss->str());

		// reset for next request
		uireq->reset();

		ss->str(string());

		return 1;
	}
	catch (json::exception &e)
	{
		cout << "JSON: " << e.what() << endl;
		return 0;
	}
	catch (LogicError &e)
	{
		cout << "CURLPP: " << e.what() << endl;
		return -1;
	}
	catch (RuntimeError &e)
	{
		cout << "CURLPP: " << e.what() << endl;
		return -2;
	}
	catch (exception &e)
	{
		cout << "STD: " << e.what() << endl;
		return -3;
	}
	return -4;
}

bool PostJson(Easy *uireq, stringstream *ss, string full_url, string query, json *j, long *code)
{
	try
	{

		uireq->setOpt(new Url(full_url.c_str()));
		uireq->setOpt(new PostFields(query));
		uireq->setOpt(new WriteStream(ss));
		uireq->perform();

		long _code;
		uireq->getCurlHandle().getInfo(CURLINFO_RESPONSE_CODE, _code);
		*code = _code;

		cout << "str: " << ss->str() << endl;
		*j = json::parse(ss->str());

		// reset for next request
		uireq->reset();

		ss->str(string());

		return 1;
	}
	catch (json::exception &e)
	{
		cout << "JSON: " << e.what() << endl;
		return 0;
	}
	catch (LogicError &e)
	{
		cout << "CURLPP: " << e.what() << endl;
		return -1;
	}
	catch (RuntimeError &e)
	{
		cout << "CURLPP: " << e.what() << endl;
		return -2;
	}
	catch (exception &e)
	{
		cout << "STD: " << e.what() << endl;
		return -3;
	}
	return -4;
}

string secure_str(json j, size_t argc, const char *a, ...)
{
	try
	{
		va_list ap;
		va_start(ap, a);
		json sub = j.at(a);
		for (int i = 0; i < (argc - 1); i++)
		{
			json s = sub.at(va_arg(ap, const char *));
			sub = s;
		}
		va_end(ap);
		if (!sub.is_string())
		{
			return "";
		}
		return sub.get<string>();
	}
	catch (exception &e)
	{
		return "";
	}
}

//-1 if not ok
int secure_int(json j, size_t argc, const char *a, ...)
{
	try
	{
		va_list ap;
		va_start(ap, a);
		json sub = j.at(a);
		for (int i = 0; i < (argc - 1); i++)
		{
			json s = sub.at(va_arg(ap, const char *));
			sub = s;
		}
		va_end(ap);
		if (!sub.is_string())
		{
			return -1;
		}
		return sub.get<int>();
	}
	catch (exception &e)
	{
		return -1;
	}
}

namespace Bakalari
{
	ABasicStruct::ABasicStruct()
	{
		m_id = new string();
		m_abbrev = new string();
		m_name = new string();
	}
	ABasicStruct::ABasicStruct(string _id, string _name, string _abbrev)
	{
		m_id = new string(_id);
		m_abbrev = new string(_abbrev);
		m_name = new string(_name);
	}
	ABasicStruct::~ABasicStruct()
	{
		*m_id = "";
		*m_abbrev = "";
		*m_name = "";
		delete m_id;
		delete m_abbrev;
		delete m_name;
	}

	string *ABasicStruct::GetId()
	{
		return m_id;
	}

	string *ABasicStruct::GetName()
	{
		return m_name;
	}

	string *ABasicStruct::GetAbbrev()
	{
		return m_abbrev;
	}

	void ABasicStruct::SetId(string _id)
	{
		*m_id = _id;
	}

	void ABasicStruct::SetName(string _name)
	{
		*m_name = _name;
	}

	void ABasicStruct::SetAbbrev(string _abbrev)
	{
		*m_abbrev = _abbrev;
	}

	ABasicStruct *FindById(vector<ABasicStruct *> array, string Id)
	{
		for (auto &elem : array)
		{
			if (*elem->GetId() == Id)
			{
				return elem;
			}
		}
		return NULL;
	}

	namespace ModuleAccess
	{

		School::School() : ABasicStruct()
		{
		}

		School::School(string _id, string _name, string _url) : ABasicStruct(_id, _name, _url)
		{
		}

		School::~School()
		{
			*m_url = "";
			delete m_url;
		}

		void School::SetUrl(string _url)
		{
			*m_url = _url;
		}

		string *School::GetUrl()
		{
			return m_url;
		}

		City::City()
		{
			m_name = new string();
			m_schools = new vector<School *>();
		}

		City::City(string _name)
		{
			m_name = new string(_name);
			m_schools = new vector<School *>();
		}

		City::City(string _name, vector<School *> _schools)
		{
			m_name = new string(_name);
			m_schools = new vector<School *>(_schools);
		}

		City::~City()
		{
			*m_name = "";
			m_schools->clear();
			delete m_schools;
			delete m_name;
		}

		string *City::GetName()
		{
			return m_name;
		}

		vector<School *> *City::GetSchools()
		{
			return m_schools;
		}

		void City::SetName(string _name)
		{
			*m_name = _name;
		}

		void City::SetSchools(vector<School *> _schools)
		{
			*m_schools = _schools;
		}

		bool City::LoadSchools()
		{
			json j;
			long *code;

			if (GetJson(&req_provider, &buff, string("https://sluzby.bakalari.cz/api/v1/municipality/" + *m_name).c_str(), {}, &j, code) == 1)
			{
				for (auto &elem : j.at("schools"))
				{
					try
					{
						/*
						School s(...)
						s.Set(...)
						*/
						m_schools->push_back(new School(elem.at("id"), elem.at("name"), elem.at("schoolUrl")));
					}
					catch (exception &e)
					{
						cout << "E: " << e.what() << endl;
						j.clear();
						*code = 0;
						delete code;
						return false;
					}
				}
			}
			else
			{
				cout << "Failed to create request." << endl;
				j.clear();
				*code = 0;
				delete code;
				return false;
			}
			j.clear();
			*code = 0;
			delete code;
			return true;
		}

		Access::Access()
		{
			m_accessToken = new string();
			m_refreshToken = new string();
			m_apiVersion = new string();
			m_appVersion = new string();
			m_school = new School();
		}

		Access::Access(School _school, string _name, string _pwd)
		{
			m_school = new School(_school);
			// init
			m_accessToken = new string();
			m_refreshToken = new string();
			m_apiVersion = new string();
			m_appVersion = new string();

			Login(_name, _pwd);
		}

		Access::Access(School _school, string _refreshToken)
		{
			m_school = new School(_school);
			// init
			m_accessToken = new string();
			m_refreshToken = new string(_refreshToken);
			m_apiVersion = new string();
			m_appVersion = new string();

			RefreshLogin();
		}

		Access::~Access()
		{
			*m_accessToken = "";
			*m_refreshToken = "";
			*m_apiVersion = "";
			*m_appVersion = "";
			m_school = NULL;
			delete m_accessToken;
			delete m_refreshToken;
			delete m_apiVersion;
			delete m_appVersion;
			delete m_school;
		}

		/*
		* Login with Username and Password
		POST /api/login
		Content-Type: application/x-www-form-urlencoded
		return TRUE if ok
		*/
		bool Access::Login(string _name, string _pwd)
		{
			json j;
			long *code;
			try
			{
				// post
				// client_id=ANDR&grant_type=password&username=" + _uname + "&password=" + _upass + "";
				if (PostJson(&req_provider, &buff, *m_school->GetUrl() + "/api/login", ("client_id=ANDR&grant_type=password&username=" + _name + "&password=" + _pwd), &j, code))
				{
					*m_apiVersion = j.at("bak:ApiVersion").get<string>();
					*m_appVersion = j.at("bak:AppVersion").get<string>();
					*m_accessToken = j.at("access_token").get<string>();
					*m_refreshToken = j.at("refresh_token").get<string>();
				}
				else
				{
					cout << "Failed to create request." << endl;
					j.clear();
					*code = 0;
					delete code;
					return false;
				}
				cout << "Request ended with code: " << *code << endl;
			}
			catch (exception &e)
			{
				cout << "E: " << e.what() << endl;
				// we can try to load json with errors
				try
				{
					cout << "error: " << j.at("error").get<string>() << endl;
					cout << "Description: " << j.at("error_description").get<string>() << endl;
				}
				catch (exception &e)
				{
					cout << "E2: " << e.what() << endl;
				}

				j.clear();
				*code = 0;
				delete code;
				return false;
			}

			j.clear();
			*code = 0;
			delete code;
			return true;
		}

		bool Access::Login(School _school, string _name, string _pwd)
		{
			*m_school = _school;
			return Login(_name, _pwd);
		}

		bool Access::Login(School *_school, string _name, string _pwd)
		{
			m_school = _school;
			return Login(_name, _pwd);
		}

		/*
		* Login by refresh token
		POST /api/login
		Content-Type: application/x-www-form-urlencoded
		return TRUE if oks
		client_id=ANDR&grant_type=refresh_token&refresh_token=REFRESHTOKEN
		*/
		bool Access::RefreshLogin()
		{
			json j;
			long *code;
			try
			{
				// post
				if (PostJson(&req_provider, &buff, *m_school->GetUrl() + "/api/login", ("client_id=ANDR&grant_type=refresh_token&refresh_token=" + *m_refreshToken), &j, code))
				{
					*m_apiVersion = j.at("bak:ApiVersion").get<string>();
					*m_appVersion = j.at("bak:AppVersion").get<string>();
					*m_accessToken = j.at("access_token").get<string>();
					//*m_refreshToken = j.at("refresh_token").get<string>();
				}
				else
				{
					cout << "Failed to create request." << endl;
					j.clear();
					*code = 0;
					delete code;
					return false;
				}
				cout << "Request ended with code: " << *code << endl;
			}
			catch (exception &e)
			{
				cout << "E: " << e.what() << endl;
				// we can try to load json with errors
				try
				{
					cout << "error: " << j.at("error").get<string>() << endl;
					cout << "Description: " << j.at("error_description").get<string>() << endl;
				}
				catch (exception &e)
				{
					cout << "E2: " << e.what() << endl;
				}

				j.clear();
				*code = 0;
				delete code;
				return false;
			}

			j.clear();
			*code = 0;
			delete code;
			return true;
		}

		bool Access::RefreshLogin(School _school)
		{
			*m_school = _school;
			return RefreshLogin();
		}

		bool Access::RefreshLogin(School *_school)
		{
			m_school = _school;
			return RefreshLogin();
		}

		/*
		GET https://sluzby.bakalari.cz/api/v1/municipality
		Accept: application/json
		*/
		vector<City *> Access::GetCityList()
		{
			json j;
			long *code;
			vector<City *> cities = {};

			if (GetJson(&req_provider, &buff, string("https://sluzby.bakalari.cz/api/v1/municipality/").c_str(), {}, &j, code) == 1)
			{
				for (auto &elem : j)
				{
					try
					{
						cities.push_back(new City(elem.at("name").get<string>()));
					}
					catch (exception &e)
					{
						cout << "E: " << e.what() << endl;
						j.clear();
						*code = 0;
						delete code;
						return cities;
					}
				}
			}
			else
			{
				cout << "Failed to create request." << endl;
				j.clear();
				*code = 0;
				delete code;
				return cities;
			}
			j.clear();
			*code = 0;
			delete code;
			return cities;
		}

		void Access::SetSchool(School *_school)
		{
			m_school = _school;
		}

		void Access::SetSchool(School _school)
		{
			*m_school = _school;
		}

		School *Access::GetSchool()
		{
			return m_school;
		}

		string *Access::GetRefreshToken()
		{
			return m_refreshToken;
		}

		string *Access::GetAccessToken()
		{
			return m_accessToken;
		}

		string *Access::GetApiVersion()
		{
			return m_apiVersion;
		}

		string *Access::GetAppVersion()
		{
			return m_appVersion;
		}

		Permission::Permission()
		{
			m_name = new string();
			m_rights = new vector<string *>();
		}

		Permission::Permission(string _name, vector<string> _rights)
		{
			m_name = new string(_name);
			m_rights = new vector<string *>();
			for (auto &a : _rights)
			{
				m_rights->push_back(new string(a));
			}
		}

		Permission::~Permission()
		{
			*m_name = "";
			m_rights->clear();
			delete m_rights;
			delete m_name;
		}

		void Permission::SetRights(vector<string> _rights)
		{
			m_rights->clear();
			for (auto &a : _rights)
			{
				m_rights->push_back(new string(a));
			}
		}

		void Permission::SetName(string _name)
		{
			*m_name = _name;
		}

		string *Permission::GetName()
		{
			return m_name;
		}

		vector<string *> *Permission::GetRights()
		{
			return m_rights;
		}

	}

	namespace ModulePeople
	{
		User::User()
		{
			m_uid = new string();
			m_fullName = new string();
			m_userName = new string();
			m_type = new string();
			m_typeText = new string();
			m_studyYear = new int(0);
			m_enabledPerm = new vector<ModuleAccess::Permission *>();
		}

		User::User(string _uid, string _fullName, string _userName, string _type, string _typetext, int _studyYear, vector<ModuleAccess::Permission *> _enabledPerm)
		{
			m_uid = new string(_uid);
			m_fullName = new string(_fullName);
			m_userName = new string(_userName);
			m_type = new string(_type);
			m_typeText = new string();
			m_studyYear = new int(_studyYear);
			m_enabledPerm = new vector<ModuleAccess::Permission *>(_enabledPerm);
		}

		User::User(ModuleAccess::Access *_access) : User()
		{
			GetUserInfo(_access);
		}

		User::~User()
		{
			*m_uid = "";
			*m_fullName = "";
			*m_userName = "";
			*m_type = "";
			*m_typeText = "";
			*m_studyYear = 0;
			m_enabledPerm->clear();
			delete m_uid;
			delete m_fullName;
			delete m_userName;
			delete m_type;
			delete m_typeText;
			delete m_studyYear;
			delete m_enabledPerm;
		}

		bool User::GetUserInfo(ModuleAccess::Access *_access)
		{
			json j;
			long *code;
			if (GetJson(&req_provider, &buff, string(*_access->GetSchool()->GetUrl() + "/api/3/user").c_str(), {}, &j, code) == 1)
			{
				try
				{
					*m_uid = j.at("UserUID").get<string>();
					*m_fullName = j.at("FullName").get<string>();
					*m_userName = j.at("FullUserName").get<string>();
					*m_type = j.at("UserType").get<string>();
					*m_typeText = j.at("UserTypeText").get<string>();
					*m_studyYear = j.at("StudyYear").get<int>();
					for (auto &elem : j.at("EnabledModules"))
					{
						string n = elem.at("Module");
						vector<string> r = {};
						for (auto &rig : elem.at("Rights"))
						{
							r.push_back(rig.get<string>());
						}
						m_enabledPerm->push_back(new ModuleAccess::Permission(n, r));
					}
				}
				catch (exception &e)
				{
					cout << "E: " << e.what() << endl;
					j.clear();
					*code = 0;
					delete code;
					return false;
				}
			}
			else
			{
				cout << "Failed to create request." << endl;
				j.clear();
				*code = 0;
				delete code;
				return false;
			}
			j.clear();
			*code = 0;
			delete code;
			return true;
		}

		void User::SetUid(string _uid)
		{
			*m_uid = _uid;
		}

		void User::SetFullName(string _fullname)
		{
			*m_fullName = _fullname;
		}

		void User::SetUserName(string _username)
		{
			*m_userName = _username;
		}

		void User::SetType(string _type)
		{
			*m_type = _type;
		}

		void User::SetTypeText(string _typeText)
		{
			*m_typeText = _typeText;
		}

		void User::SetStudyYear(int _studyYear)
		{
			*m_studyYear = _studyYear;
		}

		string *User::GetUid()
		{
			return m_uid;
		}

		string *User::GetFullName()
		{
			return m_fullName;
		}

		string *User::GetUsername()
		{
			return m_userName;
		}

		string *User::GetType()
		{
			return m_type;
		}

		string *User::GetTypeText()
		{
			return m_typeText;
		}

		int *User::GetStudyYear()
		{
			return m_studyYear;
		}

		Class::Class() : ABasicStruct()
		{
		}

		Class::Class(string _id, string _name, string _abbrev) : ABasicStruct(_id, _name, _abbrev)
		{
		}

		Teacher::Teacher() : ABasicStruct()
		{
		}

		Teacher::Teacher(string _id, string _name, string _abbrev) : ABasicStruct(_id, _name, _abbrev)
		{
		}

		Group::Group() : ABasicStruct()
		{
			m_class = new Class();
		}

		Group::Group(string _id, string _name, string _abbrev, Class _class) : ABasicStruct(_id, _name, _abbrev)
		{
			m_class = new Class(_class);
		}

		Group::Group(string _id, string _name, string _abbrev, Class *_class) : ABasicStruct(_id, _name, _abbrev)
		{
			m_class = _class;
		}

		void Group::SetClass(Class *_class)
		{
			m_class = _class;
		}

		void Group::SetClass(Class _class)
		{
			*m_class = _class;
		}

		Class *Group::GetClass()
		{
			return m_class;
		}

		Room::Room() : ABasicStruct()
		{
		}

		Room::Room(string _id, string _name, string _abbrev) : ABasicStruct(_id, _name, _abbrev)
		{
		}

		Cycle::Cycle() : ABasicStruct()
		{
		}

		Cycle::Cycle(string _id, string _name, string _abbrev) : ABasicStruct(_id, _name, _abbrev)
		{
		}
	}

	namespace ModuleTime
	{
		Hour::Hour()
		{
			m_id = new int(0);
			m_caption = new string();
			m_beginTime = new tm();
			m_endTime = new tm();
		}

		Hour::Hour(int _id, string _caption, tm _begintime, tm _endtime)
		{
			m_id = new int(_id);
			m_caption = new string(_caption);
			m_beginTime = new tm(_begintime);
			m_endTime = new tm(_endtime);
		}

		Hour::Hour(int _id, string _caption, tm *_begintime, tm *_endtime)
		{
			m_id = new int(_id);
			m_caption = new string(_caption);
			m_beginTime = _begintime;
			m_endTime = _endtime;
		}

		Hour::~Hour()
		{
			*m_id = 0;
			*m_caption = "";
			m_beginTime = NULL;
			m_endTime = NULL;
			delete m_id;
			delete m_caption;
			delete m_beginTime;
			delete m_endTime;
		}

		void Hour::SetId(int _id)
		{
			*m_id = _id;
		}

		void Hour::SetCaption(string _caption)
		{
			*m_caption = _caption;
		}

		void Hour::SetBeginTime(tm _begintime)
		{
			*m_beginTime = _begintime;
		}

		void Hour::SetEndTime(tm _endtime)
		{
			*m_endTime = _endtime;
		}

		int *Hour::GetId()
		{
			return m_id;
		}

		string *Hour::GetCaption()
		{
			return m_caption;
		}

		tm *Hour::GetBeginTime()
		{
			return m_beginTime;
		}

		tm *Hour::GetEndTime()
		{
			return m_endTime;
		}

		Subject::Subject() : ABasicStruct()
		{
		}

		Subject::Subject(string _id, string _name, string _abbrev) : ABasicStruct(_id, _name, _abbrev)
		{
		}

		Lesson::Lesson()
		{
			m_hour = new Hour();
			m_subject = new Subject();
			m_theme = new string();
			m_change = new bool();

			m_groups = new vector<ModulePeople::Group *>();
			m_cycles = new vector<ModulePeople::Cycle *>();
			m_teacher = new ModulePeople::Teacher();
			m_room = new ModulePeople::Room();
		}

		Lesson::~Lesson()
		{
			m_hour = NULL;
			m_subject = NULL;
			*m_theme = "";
			*m_change = false;
			m_groups->clear();
			m_cycles->clear();
			m_teacher = NULL;
			m_room = NULL;

			delete m_hour;
			delete m_subject;
			delete m_theme;
			delete m_change;
			delete m_groups;
			delete m_cycles;
			delete m_teacher;
			delete m_room;
		}

		void Lesson::SetHour(Hour _hour)
		{
			*m_hour = _hour;
		}

		void Lesson::SetSubject(Subject _subject)
		{
			*m_subject = _subject;
		}

		void Lesson::SetTheme(string _theme)
		{
			*m_theme = _theme;
		}

		void Lesson::SetChange(bool _change)
		{
			*m_change = _change;
		}

		void Lesson::SetGroups(vector<ModulePeople::Group> _groups)
		{
			for (auto &a : _groups)
			{
				m_groups->push_back(new ModulePeople::Group(a));
			}
		}

		void Lesson::SetCycles(vector<ModulePeople::Cycle> _cycles)
		{
			for (auto &a : _cycles)
			{
				m_cycles->push_back(new ModulePeople::Cycle(a));
			}
		}

		void Lesson::SetTeacher(ModulePeople::Teacher _teacher)
		{
			*m_teacher = _teacher;
		}

		void Lesson::SetRoom(ModulePeople::Room _room)
		{
			*m_room = _room;
		}

		// void SetHomework(ModuleMarks::Homework _homework);

		Hour *Lesson::GetHour()
		{
			return m_hour;
		}

		Subject *Lesson::GetSubject()
		{
			return m_subject;
		}

		string *Lesson::Getheme()
		{
			return m_theme;
		}

		bool *Lesson::Gethange()
		{
			return m_change;
		}

		vector<ModulePeople::Group *> *Lesson::GetGroups()
		{
			return m_groups;
		}

		vector<ModulePeople::Cycle *> *Lesson::GetCycles()
		{
			return m_cycles;
		}

		ModulePeople::Teacher *Lesson::GetTeacher()
		{
			return m_teacher;
		}

		ModulePeople::Room *Lesson::GetRoom()
		{
			return m_room;
		}

		Day::Day()
		{
			m_lessons = new vector<Lesson *>();
			m_dayOfWeek = new int();
			m_date = new tm();
			m_description = new string();
			m_type = new string();
		}

		Day::Day(string _description, string _type, tm _date, int _dayOfWeek, vector<Lesson> _lessons)
		{
			m_lessons = new vector<Lesson *>();
			m_dayOfWeek = new int(_dayOfWeek);
			m_date = new tm(_date);
			m_description = new string(_description);
			m_type = new string(_type);
			SetLessons(_lessons);
		}

		Day::~Day()
		{
			m_lessons->clear();
			*m_dayOfWeek = 0;
			m_date = NULL;
			*m_description = "";
			*m_type = "";

			delete m_lessons;
			delete m_dayOfWeek;
			delete m_date;
			delete m_description;
			delete m_type;
		}

		void Day::SetLessons(vector<Lesson> _lessons)
		{
			for (auto &a : _lessons)
			{
				m_lessons->push_back(new Lesson(a));
			}
		}

		void Day::SetDayOfWeek(int _dayOfWeek)
		{
			*m_dayOfWeek = _dayOfWeek;
		}

		void Day::SetDate(tm _date)
		{
			*m_date = _date;
		}

		void Day::SetDescription(string _description)
		{
			*m_description = _description;
		}

		void Day::SetType(string _type)
		{
			*m_type = _type;
		}

		vector<Lesson *> *Day::GetLessons()
		{
			return m_lessons;
		}

		int *Day::GetDayOfWeek()
		{
			return m_dayOfWeek;
		}

		tm *Day::GetDate()
		{
			return m_date;
		}

		string *Day::GetDescription()
		{
			return m_description;
		}

		string *Day::GetType()
		{
			return m_type;
		}

		/*
		TimeTable class
		Note: when lesson changed or added it adds:
		"ChangeType" and "Description", see: https://github.com/bakalari-api/bakalari-api-v3/blob/master/moduly/timetable.md
		*/
		class TimeTable
		{
		public:
			TimeTable();
			~TimeTable();

			/*
			GET /api/3/timetable/actual?date=YYYY-MM-dd
			"Content-Type: application/x-www-form-urlencoded"
			"Authorization: Bearer ACCESS_TOKEN"
			*/
			bool LoadByDate(ModuleAccess::Access *_access, tm *daten);

			/*
			GET /api/3/timetable/permanent
			"Content-Type: application/x-www-form-urlencoded"
			"Authorization: Bearer ACCESS_TOKEN"
			*/
			bool LoadPernament(ModuleAccess::Access *_access);

			void SetDays(vector<Day> _days);
			void SetClasses(vector<ModulePeople::Class> _classes);
			void SetGroups(vector<ModulePeople::Group> _groups);
			void SetRooms(vector<ModulePeople::Room> _rooms);
			void SetTeachers(vector<ModulePeople::Teacher> _teachers);
			void SetCycles(vector<ModulePeople::Cycle> _cycles);
			void SetSubjects(vector<Subject> _subjects);

			vector<Day *> *GetDays();
			vector<ModulePeople::Class *> *GetClasses();
			vector<ModulePeople::Group *> *GetGroups();
			vector<ModulePeople::Room *> *GetRooms();
			vector<ModulePeople::Teacher *> *GetTeachers();
			vector<Subject *> *GetSubjects();
			vector<ModulePeople::Cycle *> *GetCycles();

		private:
			vector<Day *> *m_days;
			vector<ModulePeople::Class *> *m_classes;
			vector<ModulePeople::Group *> *m_groups;
			vector<ModulePeople::Room *> *m_rooms;
			vector<ModulePeople::Teacher *> *m_teachers;
			vector<Subject *> *m_subjects;
			vector<ModulePeople::Cycle *> *m_cycles;
		};

	}
}