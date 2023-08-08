//
//  DateKiller.h
//  
//
//  Created by Isaac Roberts on 6/13/22.
//

#ifndef DateKiller_h
#define DateKiller_h

#ifndef PAID
#define PAID 0
#endif

#if PAID

#define build_expired false
#define crash_if_unpaid

#else

#if ! DEBUG

#if TARGET_WIN
#pragma message "Will crash in 60 days"
#else
#warning "Will crash in 60 days"
#endif 

#endif

#include <time.h>
//namespace {

#define CI(c) (c-'0')

#define BUILD_YEAR_CH0 (__DATE__[ 7])
#define BUILD_YEAR_CH1 (__DATE__[ 8])
#define BUILD_YEAR_CH2 (__DATE__[ 9])
#define BUILD_YEAR_CH3 (__DATE__[10])

#define BUILD_YEAR 1000*CI(BUILD_YEAR_CH0) + 100*CI(BUILD_YEAR_CH1) + 10*CI(BUILD_YEAR_CH2)+CI(BUILD_YEAR_CH3)

#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')

#define BUILD_MONTH \
	( \
		(BUILD_MONTH_IS_JAN) ? 0 : \
		(BUILD_MONTH_IS_FEB) ? 1 : \
		(BUILD_MONTH_IS_MAR) ? 2 : \
		(BUILD_MONTH_IS_APR) ? 3 : \
		(BUILD_MONTH_IS_MAY) ? 4 : \
		(BUILD_MONTH_IS_JUN) ? 5 : \
		(BUILD_MONTH_IS_JUL) ? 6 : \
		(BUILD_MONTH_IS_AUG) ? 7 : \
		(BUILD_MONTH_IS_SEP) ? 8 : \
		(BUILD_MONTH_IS_OCT) ? 9 : \
		(BUILD_MONTH_IS_NOV) ? 10 : \
		(BUILD_MONTH_IS_DEC) ? 11 : \
		/* error default */    11 \
	)

#define BUILD_DAY_CH0 ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define BUILD_DAY_CH1 (__DATE__[ 5])

#define BUILD_DAY 10*CI(BUILD_DAY_CH0)+CI(BUILD_DAY_CH1)


// Example of __TIME__ string: "21:06:19"
//                              01234567

#define BUILD_HOUR 10*CI(__TIME__[0]) + CI(__TIME__[1])
#define BUILD_MIN 10*CI(__TIME__[3]) + CI(__TIME__[4])
#define BUILD_SEC 10*CI(__TIME__[6]) + CI(__TIME__[7])


	struct If_youre_seeing_this_your_build_has_expired {
		If_youre_seeing_this_your_build_has_expired() {}
		bool obj = 0;
		
		static time_t make_time(int yr, int mon, int day, int hr, int min, int sec)
		{
			struct tm  ttt { .tm_sec = sec, .tm_min = min, .tm_hour = hr, .tm_mday = day, .tm_mon = mon, .tm_year = (yr - 1900) };

			//struct tm  ttt{.tm_year=(yr-1900), .tm_mon=mon, .tm_mday=day, .tm_hour=hr, .tm_min=min, .tm_sec=sec};
			return mktime(&ttt);
		}
		static time_t make_time_macro()
		{
			//BUILD_YEAR, BUILD_MONTH, BUILD_DAY, BUILD_HOUR, BUILD_MIN, BUILD_SEC
			struct tm  ttt{ .tm_sec = BUILD_SEC, .tm_min = BUILD_MIN, .tm_hour = BUILD_HOUR, .tm_mday = BUILD_DAY, .tm_mon = BUILD_MONTH, .tm_year=(BUILD_YEAR-1900) };
			return mktime(&ttt);
		}

		
		static int ctoi(char c)
		{
			if (c==' ')
				return 0;
			jassert(c>='0' && c<='9');
			return c-'0';
		}
		static tm dateToInt(const char* DATE)
		{
			//Thanks StackOverflow
			// Example of __DATE__ string: "Jul 27 2012"
			//                              01234567890
			struct tm dt = {0};

//			std::cout<<"Full date string = "<<DATE<<std::endl;
			
			int yr = 0;
			yr+= ctoi(DATE[ 7]) * 1000;
			yr+= ctoi(DATE[ 8]) * 100;
			yr+= ctoi(DATE[ 9]) * 10;
			yr+= ctoi(DATE[10]) ;
			dt.tm_year = yr-1900;
			
			if (DATE[0] == 'J' && DATE[1] == 'a' && DATE[2] == 'n')
				dt.tm_mon = 0;
			else if (DATE[0] == 'F')
				dt.tm_mon = 1;
			else if  (DATE[0] == 'M' && DATE[1] == 'a' && DATE[2] == 'r')
				dt.tm_mon = 2;
			else if (DATE[0] == 'A' && DATE[1] == 'p')
				dt.tm_mon = 3;
			else if (DATE[0] == 'M' && DATE[1] == 'a' && DATE[2] == 'y')
				dt.tm_mon = 4;
			else if  (DATE[0] == 'J' && DATE[1] == 'u' && DATE[2] == 'n')
				dt.tm_mon = 5;
			else if  (DATE[0] == 'J' && DATE[1] == 'u' && DATE[2] == 'l')
				dt.tm_mon = 6;
			else if  (DATE[0] == 'A' && DATE[1] == 'u')
				dt.tm_mon = 7;
			else if (DATE[0] == 'S')
				dt.tm_mon = 8;
			else if (DATE[0] == 'O')
				dt.tm_mon = 9;
			else if (DATE[0] == 'N')
				dt.tm_mon = 10;
			else if (DATE[0] == 'D')
				dt.tm_mon = 11;

			dt.tm_mday += ((DATE[4] >= '0') ? ctoi(DATE[4]) : 0) * 10;
			dt.tm_mday += ctoi(DATE[ 5]);

//			std::cout<<"Compile time: "<<dt.tm_year+1900<<" - "<<dt.tm_mon+1<<" - "<<dt.tm_mday<<std::endl;
			
			return dt;
		}

		static time_t dateTimeToInt(const char* DATE, const char* TIME)
		{
//			std::cout<<"TIME = "<<TIME<<std::endl;
			//Thanks StackOverflow
			// Example of __DATE__ string: "Jul 27 2012"
			//                              01234567890
			struct tm dt = dateToInt(DATE);
//			std::cout<<"TIME = "<<TIME<<std::endl;
			
			dt.tm_hour += ctoi (TIME[0]) * 10;
			dt.tm_hour += ctoi (TIME[1]);
			dt.tm_min  += ctoi (TIME[3]) * 10;
			dt.tm_min  += ctoi (TIME[4]);
			dt.tm_sec  += ctoi (TIME[6]) * 10;
			dt.tm_sec  += ctoi (TIME[7]);
			return mktime(&dt);
		}
		static bool is_expired_(const char* DATE, const char* TIME)
		{
			auto compile = dateTimeToInt(DATE, TIME);
			auto now = time(0);
//			std::cout<<"in is_expired "<<compile<<std::endl;
//			auto hrdif = std::difftime(now, compile) / 3600.;
//			std::cout<<"hrs since compilation: "<<hrdif<<std::endl;
//			if (hrdif > 0.f)
//				return true;
			
			auto daysDiff = std::difftime(now, compile) / 86400.;
			std::cout<<"days since compilation: "<<daysDiff<<std::endl;
			return (daysDiff > 61);
		}
		static void
		Contact_your_developer_to_get_a_final_build
		(const char* DATE, const char* TIME)
		{
			bool is_build_expired = is_expired_(DATE, TIME);
			assert("Contact your developer to get a final build." && ! is_build_expired);
			if (is_build_expired)
			{
				std::cout<<"Build has expired - Contact your developer to get a final build"<<std::endl;
				If_youre_seeing_this_your_build_has_expired* null = NULL;
				null->obj = true;
			}
		}
	};

//}
#define build_expired return If_youre_seeing_this_your_build_has_expired::is_expired_(__DATE__, __TIME__)

#define crash_if_unpaid If_youre_seeing_this_your_build_has_expired::Contact_your_developer_to_get_a_final_build(__DATE__, __TIME__);

//#define BUILD_TM If_youre_seeing_this_your_build_has_expired::make_time(BUILD_YEAR, BUILD_MONTH, BUILD_DAY, BUILD_HOUR, BUILD_MIN, BUILD_SEC)

#define BUILD_TM If_youre_seeing_this_your_build_has_expired::make_time_macro()

#endif





#endif /* DateKiller_h */
