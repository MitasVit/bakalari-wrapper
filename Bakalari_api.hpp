#pragma once
/*
C++ Wrapper for Bakaláøi API

Bakaláøi API docs: https://github.com/bakalari-api/bakalari-api-v3

*/


#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <stdexcept>

using namespace std;

namespace Bakalari {
	/*
	Basic Struct class
	*/
	class ABasicStruct {
	public:
		ABasicStruct();
		ABasicStruct(string _id, string _name, string _abbrev);
		~ABasicStruct();

		string* GetId();
		string* GetName();
		string* GetAbbrev();

		void SetId(string _id);
		void SetName(string _name);
		void SetAbbrev(string _abbrev);
	private:
		string* m_id, * m_name, * m_abbrev;
	};

	/*
	Find info in array by ID
	NULL if not found
	*/
	ABasicStruct* FindById(vector<ABasicStruct*> array, string Id);

	/*
	Access module 
	- for getting access tokens
	- for login 
	- login info + options
	*/
	namespace ModuleAccess {

		/*
		School class
		Abbrev = school url
		*/
		class School : public ABasicStruct {
		/*public:
			School();
			School(string _id, string _name, string _url);
			~School();

		private:
			string *m_url;
		protected:
			//hide abbrev methods
			using ABasicStruct::GetAbbrev;
			using ABasicStruct::SetAbbrev;
			using ABasicStruct::ABasicStruct;*/
		};

		/*
		City class
		*/
		class City {
			City();
			City(string _name);
			City(string _name, vector<School*> _schools);
			~City();

			string* GetName();
			vector<School*>* GetSchools();

			void SetName(string _name);
			void SetSchools(vector<School*> _schools);

			/*
			GET https://sluzby.bakalari.cz/api/v1/municipality/$CITY_NAME
			Accept: application/json

			return TRUE if ok
			*/
			bool LoadSchools();
		private:
			string *m_name;
			vector<School*> *m_schools;
		};


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
			TypeText = žák, (the text to add)
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
			time_t* BeginTime, *EndTime;
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
			"WorkDay" 		- pracovní den
			"Weekend" 		- víkend, obvykle se nevyskytuje
			"Celebration" 	- státní státky, významné dny,…
			"Holiday" 		- ostatní prázdniny
			"DirectorDay" 	- øeditelské volno
			"Undefined" 	- obvykle se nevyskytuje - *nejspíše* indikuje chybu serveru nebo špatnì nastavený rozvrh
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

	/*
	News module
	- events, koments, campaigns...
	*/
	/*namespace ModuleNews {
		class Koments {

		};
	}*/

	/*
	ToDo module
	- marks and homeworks
	*/
	/*namespace ModuleMarks {

		class Mark {

		};

		class Homework {

		};

	}*/
}