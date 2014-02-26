#pragma once
#include <string>
#include <vector>
#include <map>

namespace nsGameCore
{

class Properties
{
public:
	std::string getProperty(const std::string& property_name) const;

	void setProperty(const std::string& property_name, const std::string& property_value);

	std::vector<std::string> getSubProperties(const std::string& property_name) const;

	std::vector<std::string> getSubPropertyNames(const std::string& property_name) const;

	std::vector<std::pair<std::string, std::string>> getSubPropertyPairs(const std::string& property_name) const;

	template <typename T>
	T getPropertyValue(const std::string& property_name)
	{
		return boost::lexical_cast<T>(getProperty(property_name));
	}

	template <typename T>
	void setPropertyValue(const std::string& property_name, const T& value)
	{
		setProperty(property_name, boost::lexical_cast<std::string>(value));
	}

	template <typename T> 
	T getPropertySubValue(const std::string& property_name, const std::string& sub_name) const
	{
		std::vector<std::string> values = getSubProperties(property_name);

		for (auto iter = values.begin(); iter != values.end(); ++iter)
		{
			std::vector<std::string> value_split;
			boost::split(value_split, (*iter), boost::is_any_of(":"));
			if (value_split.size() == 2 && boost::iequals(value_split[0], sub_name ))
			{
				return boost::lexical_cast<T>(value_split[1]);
			}
		}
		throw std::runtime_error("Unknown property: ");
	}

	template <typename T> 
	void setPropertySubValue(const std::string& property_name, const std::string& sub_name, const T& value) 
	{
		bool was_found = false;
		std::vector<std::string> values = getSubProperties(property_name);
		std::string mod_value_string;
		for (auto iter = values.begin(); iter != values.end(); ++iter)
		{
			std::vector<std::string> value_split;
			boost::split(value_split, (*iter), boost::is_any_of(":"));
			if (value_split.size() == 2 && boost::iequals(value_split[0], sub_name ))
			{
				//return boost::lexical_cast<T>(value_split[1]);
				value_split[1] = boost::lexical_cast<std::string>(value);
				was_found = true;	
			}
			if (!mod_value_string.empty())
			{
				mod_value_string += ",";
			}
			mod_value_string += value_split[0] + ":" + value_split[1];
		}
		
		mProperties[property_name] = mod_value_string;
		if (!was_found)
		{
			throw std::runtime_error("Unknown property: ");
		}
		
	}
	template <class T> 
	void addProperty(const std::string& name, const std::string& value_name, const T& value)
	{
		addProperty(name, value_name + ":" + boost::lexical_cast<std::string>(value));
	}
	void addProperty(const std::string& name, const std::string& value_str)
	{
		std::string& current_value = mProperties[name];
		if (current_value.empty())
		{
			current_value = value_str;
		}
		else
		{
			current_value += "," +value_str;
		}
	}
protected:
	std::map<std::string, std::string> mProperties;
};

template <class T>
class ValueRef
{
public:
	ValueRef(Properties& props, const std::string property_name, const std::string& value_name)
		:mrProperties(props)
		,mPropertyName(property_name)
		,mValueName(value_name)
	{
		mValue = props.getPropertySubValue<T>(mPropertyName, mValueName);
	}

	~ValueRef()
	{
		mrProperties.setPropertySubValue<T>(mPropertyName, mValueName, mValue);
	}

	T& getRef() 
	{
		return mValue;
	}

	T get() const
	{
		return mValue;
	}

	void operator=(const T& v)
	{
		mValue = v;
	}

	bool operator==(const T& v) const
	{
		return mValue == v;
	}
	T operator+=(const T& v)
	{
		return (mValue+=v);
	}
	T operator-=(const T& v)
	{
		return (mValue-=v);
	}
	std::string getString() const
	{
		return boost::lexical_cast<std::string>(mValue);
	}
	bool operator>(const T& rhs)
	{
		return mValue > rhs;
	}

	bool operator<(const T& rhs)
	{
		return mValue < rhs;
	}

	bool operator>(const ValueRef& rhs)
	{
		return mValue > rhs.mValue;
	}

	bool operator<(const ValueRef& rhs)
	{
		return mValue < rhs.mValue;
	}

	bool operator==(const ValueRef& rhs)
	{
		return mValue == rhs.mValue;
	}
protected:
	Properties& mrProperties;
	const std::string mPropertyName;
	const std::string mValueName;
	T mValue;
	
};

}