#include "Properties.h"
#include <boost/algorithm/string.hpp>


std::string nsGameCore::Properties::getProperty( const std::string& property_name ) const
{
	const std::string canonical_name = property_name;
	auto iter = mProperties.find(canonical_name);
	if (iter != mProperties.end())
	{
		return (*iter).second;
	}
	return "";
	//throw std::runtime_error("Unknown Property: " + canonical_name);
}

void nsGameCore::Properties::setProperty( const std::string& property_name, const std::string& property_value )
{
	const std::string canonical_name = property_name;
	if (mProperties.count(canonical_name))
	{
		mProperties[canonical_name] = property_value;
	}
	else
	{
		throw std::runtime_error("Unknown Property: " + canonical_name);
	}
}

std::vector<std::string> nsGameCore::Properties::getSubProperties( const std::string& property_name ) const
{
	std::string property_value_str = getProperty(property_name);
	std::vector<std::string> values;
	boost::split(values, property_value_str, boost::is_any_of(","));
	return values;
}

std::vector<std::pair<std::string, std::string>> nsGameCore::Properties::getSubPropertyPairs( const std::string& property_name ) const
{
	std::vector<std::pair<std::string,std::string>> result_pairs;
	std::vector<std::string> sub_props = getSubProperties(property_name);
	for (auto iter = sub_props.begin(); iter != sub_props.end(); ++iter)
	{
		std::vector<std::string> split_result;
		boost::split(split_result, *iter, boost::is_any_of(":"));
		if (split_result.size() >= 2)
		{
			result_pairs.push_back(std::make_pair(split_result[0], split_result[1]));
		}
	}
	return result_pairs;
}

std::vector<std::string> nsGameCore::Properties::getSubPropertyNames( const std::string& property_name ) const
{
	std::vector<std::string> result_names;
	std::vector<std::string> sub_props = getSubProperties(property_name);
	for (auto iter = sub_props.begin(); iter != sub_props.end(); ++iter)
	{
		std::vector<std::string> split_result;
		boost::split(split_result, *iter, boost::is_any_of(":"));
		if (split_result.size() >= 1 && !split_result[0].empty())
		{
			result_names.push_back(split_result[0]);
		}
	}
	return result_names;
}