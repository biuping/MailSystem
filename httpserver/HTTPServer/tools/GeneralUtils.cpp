#include "GeneralUtils.h"

size_t GeneralUtil::strReplaceFirst(rstring& whole, const rstring& target, const rstring& newstr)
{
	size_t pos = whole.find(target);
	whole.replace(pos, target.size(), newstr);

	return pos;
}
