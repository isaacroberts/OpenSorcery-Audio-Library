//
//  UnlockUtilities.cpp
//  
//
//  Created by Isaac Roberts on 10/16/21.
//

// Created by Isaac Roberts


#include "UnlockUtil.h"

/*
 The input-maker for this is at Documents > scripts > ob.py
 */
String UnlockUtil::deobfuscate(String input)
{
	constexpr int amt = 1;
	String output ="";
	for (int n=0; n < input.length(); ++n)
	{
//		i = ord(c) - amt - 32
//		i = i % (127-32)
//		i = i + 32
		if (input[n]=='\n')
			output += '\n';
		else
			output += char(     ((
						int(char(input[n])) - char(amt) - char(32)
					  ) % 95 + 95 ) % 95 + 32);
		
//		output += char(int(char(input[n]))-char(1));
	}
	return output;
}
//Having two functions makes it harder to crack
String UnlockUtil::deob(String input)
{
	constexpr int amt = 1;
	String output ="";
	for (int n=0; n < input.length(); ++n)
	{
		if (input[n]=='\n')
			output += '\n';
		else
			output += char(     ((
						int(char(input[n])) - char(amt) - char(32)
					  ) % 95 + 95 ) % 95 + 32);
		//		output += char(int(char(input[n]))-char(1));
	}
	return output;
}
String UnlockUtil::deob1(String input)
{
	constexpr int amt = 1;
	String output ="";
	for (int n=0; n < input.length(); ++n)
	{
		if (input[n]=='\n')
			output += '\n';
		else
			output += char(     ((
						int(char(input[n])) - char(amt) - char(32)
					  ) % 95 + 95 ) % 95 + 32);
		//		output += char(int(char(input[n]))-char(1));
	}
	return output;
}
String UnlockUtil::deob2(String input)
{
	constexpr int amt = 2;
	String output ="";
	for (int n=0; n < input.length(); ++n)
	{
		if (input[n]=='\n')
			output += '\n';
		else
			output += char(     ((
						int(char(input[n])) - char(amt) - char(32)
					  ) % 95 + 95 ) % 95 + 32);
//		output += char(int(char(input[n]))-char(2));
	}
	return output;
}
String UnlockUtil::deob3(String input)
{
	constexpr int amt = 3;
	String output ="";
	for (int n=0; n < input.length(); ++n)
	{
		if (input[n]=='\n')
			output += '\n';
		else
			output += char(     ((
						int(char(input[n])) - char(amt) - char(32)
					  ) % 95 + 95 ) % 95 + 32);
//		output += char(int(char(input[n]))-char(3));
	}
	return output;
}
String UnlockUtil::deob4(String input)
{
	constexpr int amt = 4;
	String output ="";
	for (int n=0; n < input.length(); ++n)
	{
		if (input[n]=='\n')
			output += '\n';
		else
			output += char(     ((
						int(char(input[n])) - char(amt) - char(32)
					  ) % 95 + 95 ) % 95 + 32);
//		output += char(int(char(input[n]))-char(4));
	}
	return output;
}
String UnlockUtil::deob5(String input)
{
	constexpr int amt = 5;
	String output ="";
	for (int n=0; n < input.length(); ++n)
	{
		if (input[n]=='\n')
			output += '\n';
		else
			output += char(     ((
						int(char(input[n])) - char(amt) - char(32)
					  ) % 95 + 95 ) % 95 + 32);
//		output += char(int(char(input[n]))-char(4));
	}
	return output;
}

//This one's for dev purposes
String UnlockUtil::deob0(String input)
{
	return input;
}
