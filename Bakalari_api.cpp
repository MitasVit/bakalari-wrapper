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

//headers is only bearer token
//with shared request provider and stringstream
//return 1 if ok
//0  | JSON error
//-1 | Curlpp, logic error
//-2 | Curlpp, runtime error
//-3 | STD error
//-4 | unknown?
//code is response code for HTTP
bool GetJson(Easy *uireq, stringstream *ss, string full_url, list<string> headers, json* j, long* code) {
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

		//reset for next request
		uireq->reset();


		ss->str(string());

		return 1;
	}
	catch (json::exception& e)
	{
		cout << "JSON: " << e.what() << endl;
		return 0;
	}
	catch (LogicError& e)
	{
		cout << "CURLPP: " << e.what() << endl;
		return -1;
	}
	catch (RuntimeError& e)
	{
		cout << "CURLPP: " << e.what() << endl;
		return -2;
	}
	catch (exception& e)
	{
		cout << "STD: " << e.what() << endl;
		return -3;
	}
	return -4;
}


string secure_str(json j, size_t argc, const char* a, ...)
{
    try {
        va_list ap;
        va_start(ap, a);
        json sub = j.at(a);
        for (int i = 0; i < (argc - 1); i++) {
            json s = sub.at(va_arg(ap, const char*));
            sub = s;
        }
        va_end(ap);
        if (!sub.is_string()) {
            return "";
        }
        return sub.get<string>();
    }
    catch (exception& e)
    {
        return "";
    }
}

//-1 if not ok
int secure_int(json j, size_t argc, const char* a, ...)
{
    try {
        va_list ap;
        va_start(ap, a);
        json sub = j.at(a);
        for (int i = 0; i < (argc - 1); i++) {
            json s = sub.at(va_arg(ap, const char*));
            sub = s;
        }
        va_end(ap);
        if (!sub.is_string()) {
            return -1;
        }
        return sub.get<int>();
    }
    catch (exception& e)
    {
        return -1;
    }
}



namespace Bakalari {
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

		string* ABasicStruct::GetId()
		{
			return m_id;
		}

		string* ABasicStruct::GetName()
		{
			return m_name;
		}

		string* ABasicStruct::GetAbbrev()
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

		ABasicStruct* FindById(vector<ABasicStruct*> array, string Id)
		{
			for (auto& elem : array)
			{
				if (*elem->GetId() == Id) {
					return elem;
				}
			}
			return NULL;
	}

	namespace ModuleAccess {

	

		City::City()
		{
			m_name = new string();
			m_schools = new vector<School*>();
			}

			City::City(string _name)
			{
				m_name = new string(_name);
				m_schools = new vector<School*>();
			}

			City::City(string _name, vector<School*> _schools)
			{
				m_name = new string(_name);
				m_schools = new vector<School*>(_schools);
			}

			City::~City()
			{
				
			}

			string* City::GetName()
			{
				return m_name;
			}

			vector<School*>* City::GetSchools()
			{
				return m_schools;
			}

			void City::SetName(string _name)
			{
				*m_name = _name;
			}

			void City::SetSchools(vector<School*> _schools)
			{
				*m_schools = _schools;
			}

			bool City::LoadSchools()
			{
				/*
				GET https://sluzby.bakalari.cz/api/v1/municipality/$CITY_NAME
			Accept: application/json
				*/
				json j;
				long* code;
				
				if (GetJson(&req_provider, &buff, string("https://sluzby.bakalari.cz/api/v1/municipality/" + *m_name).c_str(), {}, &j, code)) {
						for (auto& elem : j.at("schools"))
						{
							try {
								School s;
								s.SetId(elem.at("id"));
								s.SetName(elem.at("name"));
								s.SetAbbrev(elem.at("schoolUrl"));
								m_schools->push_back(new School(s));
							}
							catch (exception& e) {
								return false;
							}
						}
				}
				else {
					return false;
				}
				return true;

			}


		/*
		Acces class
		- for private login (per user)
		+ info about bakalari API
		*/
		class Access {
			Access();
			Access(string _name, string _pwd);
			Access(string _refreshToken);
			~Access();

			/*
			* Login with Username and Password
			POST /api/login
			Content-Type: application/x-www-form-urlencoded
			return TRUE if ok
			*/
			bool Login(string _name, string _pwd);

			/*
			* Login by refresh token
			POST /api/login
			Content-Type: application/x-www-form-urlencoded
			return TRUE if oks
			*/
			bool RefreshLogin();

			/*
			GET https://sluzby.bakalari.cz/api/v1/municipality
			Accept: application/json
			*/
			vector<City*> GetCityList();

			void SetSchool(School* _school);
			void SetSchool(School _school);

			School* GetSchool();
			string* GetRefreshToken();
			string* GetAccessToken();
			string* GetApiVersion();
			string* GetAppVersion();
		private:
			string* m_accessToken, * m_refreshToken, * m_apiVersion, * m_appVersion;
			School* m_school;//for school url...
		};

		/*
		Permission class
		*/
		class Permission {
		public:
			Permission();
			Permission(string _name, vector<string> _rights);
			~Permission();

			void SetRights(vector<string> _rights);
			void SetName(string _name);

			string* GetName();
			vector<string*>* GetRights();

		private:
			string* m_name;
			vector<string*>* rights;
		};

	}

	/*
	People module
	- User, teachers...
	- Classes, organizations, groups.., Rooms
	*/
	namespace ModulePeople {

		/*
		User class
		*/
		class User {
		public:
			User();
			User(string _uid, string _fullName, string _userName, string _type, int _studyYear, vector<ModuleAccess::Permission*> _enabledPerm);
			User(ModuleAccess::Access* _access);
			~User();

			/*
			GET /api/3/user
			Content-Type: application/x-www-form-urlencoded
			"Authorization: Bearer ACCESS_TOKEN"
			*/
			bool GetUserInfo(ModuleAccess::Access* _access);

			void SetUid(string _uid);
			void SetFullName(string _fullname);
			void SetUserName(string _username);
			void SetType(string _type);
			void SetTypeText(string _typeText);
			void SetStudyYear(int _studyYear);

			string* GetUid();
			string* GetFullName();
			string* GetUserName();
			string* GetType();
			string* GetTypeText();
			int* GetStudyYear();

		private:
			string* m_uid, * m_fullName, * m_userName, * m_type, * m_typeText;
			/*
			Type = student, parents, ... (just a type in English)
			TypeText = ��k, (the text to add)
			*/
			int* m_studyYear;
			vector<ModuleAccess::Permission*>* m_enabledPerm;
		};

		/*
		Class class
		*/
		class Class : public ABasicStruct {

		};

		/*
		Teacher class
		*/
		class Teacher : public ABasicStruct {

		};

		/*
		Groups class
		*/
		class Group : public ABasicStruct {
			Group();
			Group(string _id, string _name, string _abbrev, Class _class);
			Group(string _id, string _name, string _abbrev, Class* _class);

			void SetClass(Class* _class);
			void SetClass(Class _class);

			Class* GetClass();
		private:
			Class* m_class;
		};

		/*
		Room class
		*/
		class Room : public ABasicStruct {

		};

		/*
		Cycle class
		*/
		class Cycle : public ABasicStruct {

		};
	}

	/*
	Time module
	- subjects, hours...
	- timetable
	- lessons
	- absences, substitutions
	*/
	namespace ModuleTime {
		class Hour {
		public:
			Hour();
			Hour(int _id, string _caption, time_t _begintime, time_t _endtime);
			~Hour();

			void SetId(int _id);
			void SetCaption(string _caption);
			void SetBeginTime(time_t _begintime);
			void SetEndTime(time_t _endtime);

			int* GetId();
			string* GetCaption();
			time_t* GetBeginTime();
			time_t* GetEndTime();
		private:
			int* Id;
			string* Caption;
			time_t* BeginTime, * EndTime;
		};

		class Subject : public ABasicStruct {

		};

		/*
		class Lesson
		*/
		class Lesson {
		public:
			Lesson();
			~Lesson();


			void SetHour(Hour _hour);
			void SetSubject(Subject _subject);
			void SetTheme(string _theme);
			void SetChange(bool _change);
			void SetGroups(vector<ModulePeople::Group> _groups);
			void SetCycles(vector<ModulePeople::Cycle> _groups);
			void SetTeacher(ModulePeople::Teacher _teacher);
			void SetRoom(ModulePeople::Room _room);
			//void SetHomework(ModuleMarks::Homework _homework);

			Hour* GetHour();
			Subject* GetSubject();
			string* Getheme();
			bool* Gethange();
			vector<ModulePeople::Group*>* GetGroups();
			vector<ModulePeople::Cycle>* GetCycles();
			ModulePeople::Teacher* GetTeacher();
			ModulePeople::Room* GetRoom();
			//ModuleMarks::Homework* GetHomework();

		private:
			Hour* m_hour;
			Subject* m_subject;
			string* m_theme;
			bool* m_change;

			vector<ModulePeople::Group*>* m_groups;
			vector<ModulePeople::Cycle>* m_cycles;
			ModulePeople::Teacher* m_teacher;
			ModulePeople::Room* m_room;

			//ModuleMarks::Homework* m_homework;
		};

		class Day {
		public:
			Day();
			Day(string _description, string _type, time_t _date, int _dayOfWeek, vector<Lesson> _lessons);
			~Day();

			void SetLessons(vector<Lesson> _lessons);
			void SetDayOfWeek(int _dayOfWeek);
			void SetDate(time_t _date);
			void SetDescription(string _description);
			void SetType(string _type);

			vector<Lesson*> GetLessons();
			int GetDayOfWeek();
			time_t* GetDate();
			string* GetDescription();
			string* GetType();
		private:
			vector<Lesson*> m_lessons;
			int m_dayOfWeek;
			time_t* m_date;
			string* m_description, * m_type;
			/*
			Day types:
			"WorkDay" 		- pracovn� den
			"Weekend" 		- v�kend, obvykle se nevyskytuje
			"Celebration" 	- st�tn� st�tky, v�znamn� dny,�
			"Holiday" 		- ostatn� pr�zdniny
			"DirectorDay" 	- �editelsk� volno
			"Undefined" 	- obvykle se nevyskytuje - *nejsp�e* indikuje chybu serveru nebo �patn� nastaven� rozvrh
			*/
		};

		/*
		TimeTable class
		Note: when lesson changed or added it adds:
		"ChangeType" and "Description", see: https://github.com/bakalari-api/bakalari-api-v3/blob/master/moduly/timetable.md
		*/
		class TimeTable {
		public:
			TimeTable();
			~TimeTable();


			/*
			GET /api/3/timetable/actual?date=YYYY-MM-dd
			"Content-Type: application/x-www-form-urlencoded"
			"Authorization: Bearer ACCESS_TOKEN"
			*/
			bool LoadByDate(ModuleAccess::Access* _access, time_t* daten);

			/*
			GET /api/3/timetable/permanent
			"Content-Type: application/x-www-form-urlencoded"
			"Authorization: Bearer ACCESS_TOKEN"
			*/
			bool LoadPernament(ModuleAccess::Access* _access);


			void SetDays(vector<Day> _days);
			void SetClasses(vector<ModulePeople::Class> _classes);
			void SetGroups(vector<ModulePeople::Group> _groups);
			void SetRooms(vector<ModulePeople::Room> _rooms);
			void SetTeachers(vector<ModulePeople::Teacher> _teachers);
			void SetCycles(vector<ModulePeople::Cycle> _cycles);
			void SetSubjects(vector<Subject> _subjects);

			vector<Day*>* GetDays();
			vector<ModulePeople::Class*>* GetClasses();
			vector<ModulePeople::Group*>* GetGroups();
			vector<ModulePeople::Room*>* GetRooms();
			vector<ModulePeople::Teacher*>* GetTeachers();
			vector<Subject*>* GetSubjects();
			vector<ModulePeople::Cycle*>* GetCycles();

		private:
			vector<Day*>* m_days;
			vector<ModulePeople::Class*>* m_classes;
			vector<ModulePeople::Group*>* m_groups;
			vector<ModulePeople::Room*>* m_rooms;
			vector<ModulePeople::Teacher*>* m_teachers;
			vector<Subject*>* m_subjects;
			vector<ModulePeople::Cycle*>* m_cycles;
		};

	}
}