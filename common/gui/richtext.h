#ifndef RICHTEXT_H
#define RICHTEXT_H

#include "../platform.h"
#include "../ustring.h"

#define RICHTEXT_TEXT		0
#define RICHTEXT_ICON		1

// Rich text part
class RichPart
{
public:
	int m_type;
	int m_icon;
	UString m_text;
	int texlen() const;	//each icon counts as 1
	int rawlen() const;	//icon tags are counted
	std::string texval() const;
	RichPart substr(int start, int length) const;
	RichPart();
	RichPart(const RichPart& original);
	RichPart(const char* cstr);
	RichPart(UString ustr);
	RichPart& operator=(const RichPart &original);
	RichPart(int type, int subtype);
};

class RichText
{
public:
	std::list<RichPart> m_part;
	std::string rawstr() const;
	int texlen() const;	//each icon counts as 1
	int rawlen() const;	//icon tags are counted
	RichText pwver() const;	//asterisk-mask password std::string
	RichText();
	RichText(const RichPart& part);
	RichText(const RichText& original);
	RichText(const char* cstr);
	RichText& operator=(const RichText &original);
	RichText operator+(const RichText &other);
	RichText substr(int start, int length) const;
	//RichText parsetags(int* caret) const;
};

RichText ParseTags(RichText original, int* caret);

#endif
