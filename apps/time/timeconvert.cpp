//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================
//
// Revision
//		
//		2015/11/09	K.M.Zhu	Add an output of week of year
//
//    2016/04/06  Q.Liu    Add the detailed description for output format.
//
//=============================================================================
#include "BasicFramework.hpp"

#include "TimeString.hpp"
#include "TimeConstants.hpp"

#include "ANSITime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"
#include "GPSWeekZcount.hpp"
#include "JulianDate.hpp"
#include "MJD.hpp"
#include "UnixTime.hpp"
#include "YDSTime.hpp"
#include "SystemTime.hpp"
#include "CommonTime.hpp"

#include <string>
#include <sstream>

#include "CommandOptionWithCommonTimeArg.hpp"

using namespace std;
using namespace gpstk;

class TimCvt : public BasicFramework
{
public:
   TimCvt(char* arg0);

protected:
   virtual void process();
	std::string weekofyear(CommonTime& ct);

private:
   CommandOptionWithCommonTimeArg ANSITimeOption;
   CommandOptionWithCommonTimeArg CivilTimeOption;
   CommandOptionWithCommonTimeArg RinexFileTimeOption;
   CommandOptionWithCommonTimeArg GPSEWSOption;
   CommandOptionWithCommonTimeArg GPSWSOption;
   CommandOptionWithCommonTimeArg GPSWZOption;
   CommandOptionWithCommonTimeArg GPSZ29Option;
   CommandOptionWithCommonTimeArg GPSZ32Option;
   CommandOptionWithCommonTimeArg JDOption;
   CommandOptionWithCommonTimeArg MJDOption;
   CommandOptionWithCommonTimeArg UnixTimeOption;
   CommandOptionWithCommonTimeArg YDSTimeOption;

   CommandOptionWithAnyArg inputFormatOption;
   CommandOptionWithAnyArg inputTimeOption;
   CommandOptionAllOf inputFormatAndTimeOption;

	CommandOptionNoArg outputWeekOfYearOption;
	
   CommandOptionWithNumberArg addOption;
   CommandOptionWithNumberArg subOption;
   CommandOptionWithAnyArg formatOption;
   CommandOptionMutex mutexOption;

   string stringToParse;
   string timeSpec;
};

TimCvt::TimCvt(char* arg0)
      : BasicFramework(arg0, "Converts from a given input time specification"
                       " to other time formats.  Include the quotation marks."
                       "  All year values are four digit years."),

        ANSITimeOption('A', "ansi", "%K", "\"ANSI-Second\""),
        CivilTimeOption('c', "civil", "%m %d %Y %H:%M:%f",
                        "\"Month(numeric) DayOfMonth Year Hour:Minute:Second\""),
        RinexFileTimeOption('R', "rinex-file", "%y %m %d %H %M %S",
                            "\"Year(2-digit) Month(numeric) DayOfMonth Hour Minute Second\""),
        GPSEWSOption('o', "ews", "%E %G %g", 
                     "\"GPSEpoch 10bitGPSweek SecondOfWeek\""),
        GPSWSOption('f', "ws", "%F %g", "\"FullGPSWeek SecondOfWeek\""),
        GPSWZOption('w', "wz", "%F %Z", "\"FullGPSWeek Zcount\""),
        GPSZ29Option(0, "z29", "%E %c", "\"29bitZcount\""),
        GPSZ32Option('Z', "z32", "%C", "\"32bitZcount\""),
        JDOption('j', "julian", "%J", "\"JulianDate\""),
        MJDOption('m', "mjd", "%Q", "\"ModifiedJulianDate\""),
        UnixTimeOption('u',"unixtime", "%U %u",
                       "\"UnixSeconds UnixMicroseconds\""),
        YDSTimeOption('y', "doy", "%Y %j %s",
                      "\"Year DayOfYear SecondsOfDay\""),
        inputFormatOption(0, "input-format", "Time format to use on input"),
        inputTimeOption(0, "input-time",
                        "Time to be parsed by \"input-format\" option"),
		  outputWeekOfYearOption( 'W', "woy", "A switch to print week of year with no argument"),
        addOption('a', "add-offset", "add NUM seconds to specified time"),
        subOption('s', "sub-offset",
                  "subtract NUM seconds from specified time"),
        formatOption('F', "format", "  -F, --format           Time format to use on output, detailed"
				         " are as follows:\n"
				         "\t--ANSITime:\n"
				         "\t      %K         integer seconds since Unix Epoch (00:00, Jan 1, 1970 UTC)\n"
							"\t--CivilTime:\n"
							"\t      %Y         integer 4-digit year\n"
							"\t      %y         integer 2-digit year\n"
							"\t      %m         integer month\n"
							"\t      %b         abbreviated month name string (e.g. \"Jan\")\n"
							"\t      %B         full month name string (e.g.\"January\")\n"
							"\t      %d         integer day-of-month\n"
							"\t      %H         integer hour-of-day\n"
							"\t      %M         integer minute-of-hour\n"
							"\t      %S         integer second-of-minute\n"
							"\t      %f         float second-of-minute\n"
							"\t--Week (GPS/BDS/GAL/QZS):\n"
							"\t      %E         integer GPS Epoch\n"
							"\t      %F         integer full (13-bit) GPS Week\n"
							"\t      %G         integer mod (10-bit) GPS Week\n"
							"\t      %R         integer BDS Epoch\n"
							"\t      %D         integer full BDS Week\n"
							"\t      %e         integer mod BDS Week\n"
							"\t      %T         integer GAL Epoch\n"
							"\t      %L         integer full GAL Week\n"
							"\t      %l         integer mod GAL Week\n"
							"\t      %V         integer QZS Epoch\n"
							"\t      %I         integer full QZS Week\n"
							"\t      %i         integer mod QZS Week\n"
							"\t--WeekSecond (GPS/BDS/GAL/QZS):\n"
							"\t      %w         integer GPS day-of-week\n"
							"\t      %g         float GPS second-of-week\n"
							"\t--GPSWeekZcount:\n"
							"\t      %w         integer GPS day-of-week\n"
							"\t      %z         integer GPS Z-count\n"
							"\t      %Z         integer GPS Z-count\n"
							"\t      %c         integer 29-bit Z-count\n"
							"\t      %C         integer 32-bit Z-count\n"
							"\t--JulianDate:\n"
							"\t      %J         float Julian Date\n"
							"\t--MJD:\n"
							"\t      %Q         float Modified Julian Date\n"
							"\t--UnixTime:\n"
							"\t      %U         integer seconds since Unix Epoch (00:00, Jan 1, 1970 UTC)\n"
							"\t      %u         integer microseconds\n"
							"\t--YDSTime:\n"
							"\t      %Y         integer 4-digit year\n"
							"\t      %y         integer 2-digit year\n"
							"\t      %j         integer day-of-year\n"
							"\t      %s         integer second-of-day\n"
							"\t--Common Identifiers:\n"
							"\t      %P         string TimeSystem\n")
{
   ANSITimeOption.setMaxCount(1);
   CivilTimeOption.setMaxCount(1);
   RinexFileTimeOption.setMaxCount(1);
   GPSEWSOption.setMaxCount(1);
   GPSWSOption.setMaxCount(1);
   GPSWZOption.setMaxCount(1);
   GPSZ29Option.setMaxCount(1);
   GPSZ32Option.setMaxCount(1);
   JDOption.setMaxCount(1);
   MJDOption.setMaxCount(1);
   UnixTimeOption.setMaxCount(1);
   YDSTimeOption.setMaxCount(1);
   formatOption.setMaxCount(1);

   inputFormatOption.setMaxCount(1);
   inputTimeOption.setMaxCount(1);
   inputFormatAndTimeOption.addOption(&inputFormatOption);
   inputFormatAndTimeOption.addOption(&inputTimeOption);

   mutexOption.addOption(&ANSITimeOption);
   mutexOption.addOption(&CivilTimeOption);
   mutexOption.addOption(&RinexFileTimeOption);
   mutexOption.addOption(&GPSEWSOption);
   mutexOption.addOption(&GPSWSOption);
   mutexOption.addOption(&GPSWZOption);
   mutexOption.addOption(&GPSZ29Option);
   mutexOption.addOption(&GPSZ32Option);
   mutexOption.addOption(&JDOption);
   mutexOption.addOption(&MJDOption);
   mutexOption.addOption(&UnixTimeOption);
   mutexOption.addOption(&YDSTimeOption);
   mutexOption.addOption(&inputFormatAndTimeOption);
}

	/// Week of year:
std::string TimCvt::weekofyear(CommonTime& ct)
{
	CivilTime cvt;
	cvt.convertFromCommonTime(ct);
	CivilTime cvt0(cvt.year, 1, 1, 0, 0, 0);

   std::string GPSWeek0, GPSWeek, weekofyear;
	GPSWeek0 = printTime(cvt0.convertToCommonTime(), "%4F");
	GPSWeek  = printTime(ct, "%4F");
	stringstream ss;
	int weekofyear_i = atoi(GPSWeek.c_str()) - atoi(GPSWeek0.c_str()) + 1;
	ss << weekofyear_i;
	weekofyear = ss.str();
	return weekofyear;
}

void TimCvt::process()
{
   CommonTime ct;
   ct.setTimeSystem(TimeSystem::GPS);
   CommandOption *whichOpt = mutexOption.whichOne();

   if (whichOpt)
   {
      CommandOptionWithCommonTimeArg *cta = 
         dynamic_cast<CommandOptionWithCommonTimeArg *>(whichOpt);
      if (cta)
      {
         ct = cta->getTime().front();
         ct.setTimeSystem(TimeSystem::GPS);
      }
      else // whichOpt == &inputFormatAndTimeOption
      {
         mixedScanTime( ct, 
                        inputTimeOption.getValue().front(),
                        inputFormatOption.getValue().front() );
         ct.setTimeSystem(TimeSystem::GPS);
      }
   }
   else
   {
      ct = SystemTime();
      ct.setTimeSystem(TimeSystem::GPS); 
   }

   int i;
   int addOptions = addOption.getCount();
   int subOptions = subOption.getCount();
   for (i = 0; i < addOptions; i++)
      ct += StringUtils::asDouble(addOption.getValue()[i]);
   for (i = 0; i < subOptions; i++)
      ct -= StringUtils::asDouble(subOption.getValue()[i]);


	if (outputWeekOfYearOption.getCount() == 0 &&
		 formatOption.getCount() == 0)
   {
      using StringUtils::leftJustify;
      string eight(8, ' '); // eight spaces
      
      GPSWeekZcount wz(ct);
      CivilTime civ(ct);

      cout << endl
           << eight << leftJustify("Month/Day/Year H:M:S", 32) 
           << CivilTime(ct)  << endl
		
			  << eight << leftJustify("Week of year", 32) 
			  << weekofyear(ct) << endl

           << eight << leftJustify("Modified Julian Date", 32)
           << setprecision(15) << MJD(ct) << endl

           << eight << leftJustify("GPSweek DayOfWeek SecOfWeek", 32)
           << GPSWeekSecond(ct).printf("%G %w % 13.6g") << endl

           << eight << leftJustify("FullGPSweek Zcount", 32)
           << wz.printf("%F % 6z") << endl

           << eight << leftJustify("Year DayOfYear SecondOfDay", 32)
           << YDSTime(ct).printf("%Y %03j % 12.6s") << endl

           << eight << leftJustify("Unix: Second Microsecond", 32)
           << UnixTime(ct).printf("%U % 6u") << endl

           << eight << leftJustify("Zcount: 29-bit (32-bit)", 32)
           << wz.printf("%c (%C)") << endl

           << endl << endl;
   }
	else
	{
 	   if( outputWeekOfYearOption.getCount() != 0 )
	   {
		   cout << weekofyear(ct) << endl;
	   }		
  
      if (formatOption.getCount())
      {
         cout << printTime(ct, formatOption.getValue()[0]) << endl;
      }   
   
	}   
  
	 return;
}

int main(int argc, char* argv[])
{
   TimCvt tc(argv[0]);
   if (!tc.initialize(argc, argv))
      return 0;
   if (!tc.run())
      return 1;
   
   return 0;
}
