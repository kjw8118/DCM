#include "DCM.h"


#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>

namespace DCM {
	std::map<std::string, int> Header = {
		{"*", TYPE::COMMENT},
		{"!", TYPE::COMMENT},
		{".", TYPE::COMMENT},
		{"KONSERVIERUNG_FORMAT", TYPE::FORMAT},
		{"FUNKTIONEN", TYPE::FUNCTIONS},
		{"FKT", TYPE::FUNCTION},
		{"VARIANTENKODIERUNG", TYPE::VARIANTCODING},
		{"KRITERIUM", TYPE::VARIANT},
		{"MODULKOPF", TYPE::MODULEHEADER},
		{"FESTWERT", TYPE::PARAMETER},
		{"FESTWERTEBLOCK", TYPE::ARRAY},
		{"KENNLINIE", TYPE::CHARLINE},
		{"KENNFELD", TYPE::CHARMAP},
		{"FESTKENNLINIE", TYPE::FIXEDCHARLINE},
		{"FESTKENNFELD", TYPE::FIXEDCHARMAP},
		{"GRUPPENKENNLINIE", TYPE::GROUPCHARLINE},
		{"GRUPPENKENNFELD", TYPE::GROUPCHARMAP},
		{"STUETZSTELLENVERTEILUNG", TYPE::DISTRIBUTION},
		{"LANGNAME", TYPE::LANGNAME},
		{"DISPLAYNAME", TYPE::DISPLAYNAME},
		{"VAR", TYPE::VAR},
		{"FUNKTION", TYPE::FUNKTION},
		{"EINHEIT_X", TYPE::UNIT_X},
		{"EINHEIT_Y", TYPE::UNIT_Y},
		{"EINHEIT_W", TYPE::UNIT_W},
		{"ST/X", TYPE::POINT_X},
		{"ST/Y", TYPE::POINT_Y},
		{"WERT", TYPE::VALUE},
		{"TEXT", TYPE::TEXT},
		{"*SSTX", TYPE::DIST_X},
		{"*SSTY", TYPE::DIST_Y},
		{"END", TYPE::END},
	};

}

DCM::Element::Element(int lineIndex, int& lineOrder, int type)
	: lineIndex(lineIndex), lineOrder(lineOrder), type(type)
{
	lineOrder++;
};	
DCM::Unknown::Unknown(int lineIndex, int &lineOrder, std::string text)
	: Element(lineIndex, lineOrder, TYPE::UNKNOWN), text(text) {};
DCM::Comment::Comment(int lineIndex, int &lineOrder, std::string text)
	: Element(lineIndex, lineOrder, TYPE::COMMENT), text(text) {};
DCM::Format::Format(int lineIndex, int &lineOrder, std::string version)
	: Element(lineIndex, lineOrder, TYPE::FORMAT), version(version) {};
DCM::Function::Function(std::string name, std::string version, std::string longname)
	: name(name), version(version), longname(longname) {};
DCM::Functions::Functions(int lineIndex, int &lineOrder)
	: Element(lineIndex, lineOrder, TYPE::FUNCTIONS) {};
DCM::Variant::Variant(std::string name)
	: name(name) {};	
DCM::VariantCoding::VariantCoding(int lineIndex, int &lineOrder)
	: Element(lineIndex, lineOrder, TYPE::VARIANTCODING) {};
DCM::ModuleHeader::ModuleHeader(int lineIndex, int &lineOrder, std::string name)
	: Element(lineIndex, lineOrder, TYPE::MODULEHEADER), name(name) {};	
DCM::BaseParameter::BaseParameter(int lineIndex, int &lineOrder, int type, std::string name)
	: Element(lineIndex, lineOrder, type), name(name) {};
DCM::ArrayBaseParameter::ArrayBaseParameter(int lineIndex, int &lineOrder, int type, std::string name, int size_x, int size_y)
	: BaseParameter(lineIndex, lineOrder, type, name), size_x(size_x), size_y(size_y) {};	
DCM::LineBaseParameter::LineBaseParameter(int lineIndex, int &lineOrder, int type, std::string name, int size_x)
	: BaseParameter(lineIndex, lineOrder, type, name), size_x(size_x) {};
DCM::MapBaseParameter::MapBaseParameter(int lineIndex, int &lineOrder, int type, std::string name, int size_x, int size_y)
	: LineBaseParameter(lineIndex, lineOrder, type, name, size_x), size_y(size_y) {};
DCM::Parameter::Parameter(int lineIndex, int &lineOrder, std::string name)
	: BaseParameter(lineIndex, lineOrder, TYPE::PARAMETER, name), value(0), dec_value(0) {};
DCM::Boolean::Boolean(int lineIndex, int &lineOrder, std::string name)
	: BaseParameter(lineIndex, lineOrder, TYPE::BOOLEAN, name), text("") {};
DCM::Array::Array(int lineIndex, int &lineOrder, std::string name, int size_x)
	: ArrayBaseParameter(lineIndex, lineOrder, TYPE::ARRAY, name, size_x, 0) {};
DCM::Matrix::Matrix(int lineIndex, int &lineOrder, std::string name, int size_x, int size_y)
	: ArrayBaseParameter(lineIndex, lineOrder, TYPE::MATRIX, name, size_x, size_y) {};	
DCM::CharLine::CharLine(int lineIndex, int &lineOrder, std::string name, int size_x)
	: LineBaseParameter(lineIndex, lineOrder, TYPE::CHARLINE, name, size_x) {};	
DCM::CharMap::CharMap(int lineIndex, int &lineOrder, std::string name, int size_x, int size_y)
	: MapBaseParameter(lineIndex, lineOrder, TYPE::CHARMAP, name, size_x, size_y) {};	
DCM::FixedCharLine::FixedCharLine(int lineIndex, int &lineOrder, std::string name, int size_x)
	: LineBaseParameter(lineIndex, lineOrder, TYPE::FIXEDCHARLINE, name, size_x) {};	
DCM::FixedCharMap::FixedCharMap(int lineIndex, int &lineOrder, std::string name, int size_x, int size_y)
	: MapBaseParameter(lineIndex, lineOrder, TYPE::FIXEDCHARMAP, name, size_x, size_y) {};	
DCM::Distribution::Distribution(int lineIndex, int &lineOrder, std::string name, int size_x)
	: LineBaseParameter(lineIndex, lineOrder, TYPE::DISTRIBUTION, name, size_x) {};	
DCM::GroupCharLine::GroupCharLine(int lineIndex, int &lineOrder, std::string name, int size_x)
	: LineBaseParameter(lineIndex, lineOrder, TYPE::GROUPCHARLINE, name, size_x) {};	
DCM::GroupCharMap::GroupCharMap(int lineIndex, int &lineOrder, std::string name, int size_x, int size_y)
	: MapBaseParameter(lineIndex, lineOrder, TYPE::GROUPCHARMAP, name, size_x, size_y) {};
		


	
std::vector<std::string> DCM::stripLine(std::string lineRaw)
{
	bool isQuoOpen = false;
	std::vector<std::string> lineStrip;
	std::string buffer;

	for (auto c : lineRaw)
	{
		if (c == '\"')
		{
			if (!isQuoOpen)
			{
				if (!buffer.empty())
				{
					lineStrip.push_back(buffer);
					buffer.clear();
				}
				buffer += c;				
			}
			else
			{
				buffer += c;
				lineStrip.push_back(buffer);
				buffer.clear();
			}
			isQuoOpen = !isQuoOpen;

			continue;
		}
		else if ((c == ' ' && !isQuoOpen) || (c == '\t' && !isQuoOpen) || (c == '\n'))
		{
			if (!buffer.empty())
			{
				lineStrip.push_back(buffer);
				buffer.clear();
			}
			isQuoOpen = false;

			continue;
		}
		else
		{
			buffer += c;

			continue;
		}

	}
	if (!buffer.empty())
	{
		lineStrip.push_back(buffer);
		buffer.clear();
	}

	return lineStrip;
}
int DCM::countDecimalPlaces(const std::string& numberStr)
{
	size_t dotPos = numberStr.find('.');
	if (dotPos == std::string::npos) {	
		return 0;
	}
	std::string decimalPart = numberStr.substr(dotPos + 1);
	return decimalPart.length();
}


std::string DCM::toFixed(double value, int precision)
{
#if __cplusplus >= 202002L
#include <format>
	return std::format("{:.{}f}", value, precision);
}
#else
#include <sstream>
#include <iomanip>
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(precision) << value;
	return oss.str();
}
#endif

/*

// Initial Header
Comment			: *
Format			: KONSERVIERUNG_FORMAT
Functions		: FUNKTIONEN
VariantCoding	: VARIANTENKODIERUNG
ModuleHeader	: MODULKOPF
Parameter		: FESTWERT
Boolean			: FE
Array			: FESTWERTEBLOCK
Matrix			: FESTWERTEBLOCK
CharLine		: KENNLINIE
CharMap			: KENNFELD
FixedCharLine	: FESTKENNLINIE
FixedCharMap	: FESTKENNFELD
GroupCharLine	: GRUPPENKENNLINIE
GroupCharMap	: GRUPPENKENNFELD
Distribution	: STUETZSTELLENVERTEILUNG

// Component Header
Function		: FKT
Variant			: KRITERIUM
Langname		: LANGNAME
Displayname		: DISPLAYNAME
Variant			: VAR
Function		: FUNKTION
Unit X			: EINHEIT_X
Unit Y			: EINHEIT_Y
Unit Z			: EINHEIT_W
Point X			: ST/X
Point Y			: ST/Y
Value			: WERT

Distribution X	: *SSTX
Distribution Y	: *SSTY

End				: END

*/
void DCM::Manager::parseHeader(std::vector<std::string> lineStrip)
{
	int headerType = Header[lineStrip.at(0)];
	switch (headerType)
	{
	case TYPE::UNKNOWN:
	{
		//std::string lineString = "";
		//lineString = lineStrip.at(0);
		//for (int i = 1; i < lineStrip.size(); i++)
		//	lineString += " " + lineStrip.at(i);
		pCurrentElement = new Unknown(currentIndex, currentOrder, lineHistory.back());
		putElement(pCurrentElement);
		pCurrentElement = nullptr;
		break;
	}
	case TYPE::COMMENT:
	{
		//std::string lineComment = "";
		//lineComment += lineStrip.at(0);
		//for (int i = 1; i < lineStrip.size(); i++)
		//	lineComment += " " + lineStrip.at(i);
		pCurrentElement = new Comment(currentIndex, currentOrder, lineHistory.back());
		putElement(pCurrentElement);
		pCurrentElement = nullptr;
		break;
	}
	case TYPE::FORMAT:
	{
		pCurrentElement = new Format(currentIndex, currentOrder, lineStrip.at(1));
		putElement(pCurrentElement);
		pCurrentElement = nullptr;
		break;
	}
	case TYPE::FUNCTIONS:
	{
		pCurrentElement = new Functions(currentIndex, currentOrder);
		break;
	}
	case TYPE::VARIANTCODING:
	{
		pCurrentElement = new VariantCoding(currentIndex, currentOrder);
		break;
	}
	case TYPE::MODULEHEADER:
	{
		if (lineStrip.size() > 2)
		{
			pCurrentElement = new ModuleHeader(currentIndex, currentOrder, lineStrip.at(1));
			((ModuleHeader*)pCurrentElement)->texts.push_back(lineStrip.at(2));
			moduleHeaders.push_back((ModuleHeader*)pCurrentElement);
			pCurrentElement = nullptr;
		}
		else
		{
			if (!moduleHeaders.empty())
			{
				moduleHeaders.back()->texts.push_back(lineStrip.at(1));
			}
			else
			{
				pCurrentElement = new ModuleHeader(currentIndex, currentOrder, lineStrip.at(1));
				moduleHeaders.push_back((ModuleHeader*)pCurrentElement);
			}
			pCurrentElement = nullptr;
		}
		break;
	}
	case TYPE::PARAMETER:
	{
		pCurrentElement = new Parameter(currentIndex, currentOrder, lineStrip.at(1));
		break;
	}
	case TYPE::ARRAY:
	{
		
		if (lineStrip.size() > 4)
		{
			pCurrentElement = new Matrix(currentIndex, currentOrder, lineStrip.at(1));
			((Matrix*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((Matrix*)pCurrentElement)->size_y = std::stoi(lineStrip.at(4)); // have @
		}
		else if (lineStrip.size() > 2)
		{
			pCurrentElement = new Array(currentIndex, currentOrder, lineStrip.at(1));
			((Array*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		}
		break;
	}	
	case TYPE::CHARLINE:
	{
		pCurrentElement = new CharLine(currentIndex, currentOrder, lineStrip.at(1));
		if (lineStrip.size() > 2)
			((CharLine*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		break;
	}
	case TYPE::CHARMAP:
	{
		pCurrentElement = new CharMap(currentIndex, currentOrder, lineStrip.at(1));
		if (lineStrip.size() > 3)
		{
			((CharMap*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((CharMap*)pCurrentElement)->size_y = std::stoi(lineStrip.at(3)); // no have @			
		}
		break;
	}
	case TYPE::FIXEDCHARLINE:
	{
		pCurrentElement = new FixedCharLine(currentIndex, currentOrder, lineStrip.at(1));
		if (lineStrip.size() > 2)
			((FixedCharLine*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		break;
	}
	case TYPE::FIXEDCHARMAP:
	{
		pCurrentElement = new FixedCharMap(currentIndex, currentOrder, lineStrip.at(1));
		if (lineStrip.size() > 3)
		{
			((FixedCharMap*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((FixedCharMap*)pCurrentElement)->size_y = std::stoi(lineStrip.at(3)); // no have @
		}
		break;
	}
	case TYPE::GROUPCHARLINE:
	{
		pCurrentElement = new GroupCharLine(currentIndex, currentOrder, lineStrip.at(1));
		if (lineStrip.size() > 2)
			((GroupCharLine*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		break;
	}
	case TYPE::GROUPCHARMAP:
	{
		pCurrentElement = new GroupCharMap(currentIndex, currentOrder, lineStrip.at(1));
		if (lineStrip.size() > 3)
		{
			((GroupCharMap*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((GroupCharMap*)pCurrentElement)->size_y = std::stoi(lineStrip.at(3)); // no have @
		}
		break;
	}
	case TYPE::DISTRIBUTION:
	{
		pCurrentElement = new Distribution(currentIndex, currentOrder, lineStrip.at(1));
		if (lineStrip.size() > 2)
			((Distribution*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		break;
	}
	}
}
void DCM::Manager::parseComponent(std::vector<std::string> lineStrip)
{
	
	int componentType = Header[lineStrip.at(0)];	
	switch (componentType)
	{
	case TYPE::UNKNOWN:
	{
		break;
	}
	case TYPE::FUNCTION:
	{
		((Functions*)pCurrentElement)->functions.emplace_back(lineStrip.at(1), lineStrip.at(2), lineStrip.at(3));
		break;
	}
	case TYPE::VARIANT:
	{
		Variant variant(lineStrip.at(1));
		for (int i = 2; i < lineStrip.size(); i++)
			variant.values.push_back(lineStrip.at(i));
		((VariantCoding*)pCurrentElement)->variants.push_back(variant);
		break;
	}
	case TYPE::FUNKTION:
	{		
		((BaseParameter*)pCurrentElement)->function = lineStrip.at(1);
		break;
	}
	case TYPE::VAR:
	{
		if (lineStrip.size() > 2)		
			((BaseParameter*)pCurrentElement)->variant = lineStrip.at(1);		
		break;
	}
	case TYPE::LANGNAME:
	{		
		((BaseParameter*)pCurrentElement)->langname = lineStrip.at(1);
		break;
	}
	case TYPE::DISPLAYNAME:
	{
		((BaseParameter*)pCurrentElement)->displayname = lineStrip.at(1);
		break;
	}
	case TYPE::UNIT_W:
	{
		((BaseParameter*)pCurrentElement)->unit = lineStrip.at(1);
		break;
	}
	case TYPE::UNIT_X:
	{
		switch (pCurrentElement->type)
		{		
		case TYPE::CHARLINE:
		case TYPE::CHARMAP:
		case TYPE::FIXEDCHARLINE:
		case TYPE::FIXEDCHARMAP:
		case TYPE::GROUPCHARLINE:
		case TYPE::GROUPCHARMAP:
		case TYPE::DISTRIBUTION:
		{
			((LineBaseParameter*)pCurrentElement)->unit_x = lineStrip.at(1);
			break;
		}
		}		
		break;
	}
	case TYPE::UNIT_Y:
	{
		switch (pCurrentElement->type)
		{				
		case TYPE::CHARMAP:
		case TYPE::FIXEDCHARMAP:
		case TYPE::GROUPCHARMAP:
		{
			((MapBaseParameter*)pCurrentElement)->unit_y = lineStrip.at(1);
			break;
		}
		}
		break;
	}
	case TYPE::POINT_X:
	{
		switch (pCurrentElement->type)
		{
		case TYPE::CHARLINE:
		case TYPE::FIXEDCHARLINE:
		case TYPE::GROUPCHARLINE:
		case TYPE::DISTRIBUTION:
		case TYPE::CHARMAP:
		case TYPE::FIXEDCHARMAP:
		case TYPE::GROUPCHARMAP:
		{
			for (int i = 1; i < lineStrip.size(); i++)
			{
				((LineBaseParameter*)pCurrentElement)->point_x.push_back(std::stod(lineStrip.at(i)));
				((LineBaseParameter*)pCurrentElement)->dec_point_x.push_back(countDecimalPlaces(lineStrip.at(i)));
			}
			break;
		}		
		}
		break;
	}
	case TYPE::POINT_Y:
	{
		switch (pCurrentElement->type)
		{
		case TYPE::CHARMAP:
		case TYPE::FIXEDCHARMAP:
		case TYPE::GROUPCHARMAP:
		{
			for (int i = 1; i < lineStrip.size(); i++)
			{
				((MapBaseParameter*)pCurrentElement)->point_y.push_back(std::stod(lineStrip.at(i)));
				((MapBaseParameter*)pCurrentElement)->dec_point_y.push_back(countDecimalPlaces(lineStrip.at(i)));
			}
			break;
		}
		}
		break;
	}
	case TYPE::VALUE:
	{
		switch (pCurrentElement->type)
		{
		case TYPE::PARAMETER:
		{
			((Parameter*)pCurrentElement)->value = std::stod(lineStrip.at(1));
			((Parameter*)pCurrentElement)->dec_value = countDecimalPlaces(lineStrip.at(1));
			break;
		}
		case TYPE::ARRAY:
		case TYPE::MATRIX:
		{
			for (int i = 1; i < lineStrip.size(); i++)
			{
				((ArrayBaseParameter*)pCurrentElement)->values.push_back(std::stod(lineStrip.at(i)));
				((ArrayBaseParameter*)pCurrentElement)->dec_values.push_back(countDecimalPlaces(lineStrip.at(i)));
			}
			break;
		}
		case CHARLINE:
		case FIXEDCHARLINE:
		case GROUPCHARLINE:
		case CHARMAP:
		case FIXEDCHARMAP:
		case GROUPCHARMAP:
		{
			for (int i = 1; i < lineStrip.size(); i++)
			{
				((LineBaseParameter*)pCurrentElement)->values.push_back(std::stod(lineStrip.at(i)));
				((LineBaseParameter*)pCurrentElement)->dec_values.push_back(countDecimalPlaces(lineStrip.at(i)));
			}
			break;
		}		
		}
		break;
	}
	case TYPE::TEXT:
	{
		switch (pCurrentElement->type)
		{
		case TYPE::PARAMETER:
		{
			auto pCurrentParameter = (Parameter*)pCurrentElement;
			auto pCurrentBoolean = new Boolean(pCurrentParameter->lineIndex, pCurrentParameter->lineOrder, pCurrentParameter->name);
			pCurrentBoolean->displayname = pCurrentParameter->displayname;
			pCurrentBoolean->function = pCurrentParameter->function;
			pCurrentBoolean->langname = pCurrentParameter->langname;
			pCurrentBoolean->unit = pCurrentParameter->unit;
			pCurrentBoolean->variant = pCurrentParameter->variant;
			pCurrentBoolean->text = lineStrip.at(1);
			delete pCurrentElement;
			pCurrentElement = (Element*)pCurrentBoolean;			
			break;
		}
		case TYPE::BOOLEAN:
		{
			((Boolean*)pCurrentElement)->text = lineStrip.at(1);
			break;
		}
		}
	}
	case TYPE::DIST_X:
	{
		switch (pCurrentElement->type)
		{
		case TYPE::GROUPCHARLINE:
		{
			((GroupCharLine*)pCurrentElement)->dist_x = lineStrip.at(1);
			break;
		}
		case TYPE::GROUPCHARMAP:
		{
			((GroupCharMap*)pCurrentElement)->dist_x = lineStrip.at(1);
			break;
		}		
		}
		break;
	}
	case TYPE::DIST_Y:
	{
		switch (pCurrentElement->type)
		{
		case TYPE::GROUPCHARMAP:
		{
			((GroupCharMap*)pCurrentElement)->dist_y = lineStrip.at(1);
			break;
		}
		}
		break;
	}
	case TYPE::END:
	{
		putElement(pCurrentElement);		
		pCurrentElement = nullptr;
		break;
	}
	}
}
void DCM::Manager::parseLine(std::string lineRaw)
{
	auto lineStrip = stripLine(lineRaw);

	if (lineStrip.empty())
	{
		pCurrentElement = new Unknown(currentIndex, currentOrder, lineHistory.back());
		putElement(pCurrentElement);
		pCurrentElement = nullptr;
		return;
	}
	if (pCurrentElement == nullptr)
	{
		parseHeader(lineStrip);		
	}
	else
	{
		parseComponent(lineStrip);
	}


}

DCM::Manager::Manager()
{

}

bool DCM::Manager::open(std::string fpath, int mode)
{
	file.open(fpath.c_str(), mode);
	if (!file.is_open())
	{
		isOpened = false;
		return false;
	}

	isOpened = true;
	
	return true;
};
void DCM::Manager::createDCM()
{
	clear();
	std::tm localTime;
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());  localtime_s(&localTime, &now);
	std::ostringstream oss;	oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
	putElement((Element*)(new Comment(currentIndex++, currentOrder, "* encoding=\"euc-kr\"")));
	putElement((Element*)(new Comment(currentIndex++, currentOrder, "* DAMOS format")));
	putElement((Element*)(new Comment(currentIndex++, currentOrder, "* Created by DCM Manager")));
	putElement((Element*)(new Comment(currentIndex++, currentOrder, "* Creation date: " + oss.str())));
	putElement((Element*)(new Comment(currentIndex++, currentOrder, "*")));
	putElement((Element*)(new Unknown(currentIndex++, currentOrder, "")));
	putElement((Element*)(new Format(currentIndex++, currentOrder, "2.0")));
	putElement((Element*)(new Unknown(currentIndex++, currentOrder, "")));

	
}
void DCM::Manager::parse()
{
	if (isOpened)
	{
		lineHistory.clear();
		pCurrentElement = nullptr;
		std::string lineRaw = "";
		currentIndex = 0;

		while (std::getline(file, lineRaw))
		{
			lineHistory.push_back(lineRaw);
			parseLine(lineRaw);
			currentIndex++;
		}
	}
}

void DCM::Manager::clear()
{
	lineHistory.clear();
	currentIndex = 0;
	currentOrder = 0;
	pCurrentElement = nullptr;
	for (auto element : elements)
	{
		if (element != nullptr)
			delete element;
	}
	elements.clear();
	moduleHeaders.clear();
	isOpened = false;
	if (file.is_open())
		file.close();

}
std::vector<DCM::Element*> DCM::Manager::getElements()
{
	return elements; 
};
DCM::Element* DCM::Manager::findElement(std::string name, bool exactmatch)
{
	if (name.empty())
		return nullptr;	
	
	std::string name_trnsf = name;
	if(!exactmatch)
		std::transform(name_trnsf.begin(), name_trnsf.end(), name_trnsf.begin(), std::tolower);
	for (auto &itr : elementIndex)
	{
			
		std::string key_trnsf = itr.first;
		if (!exactmatch)							
			std::transform(key_trnsf.begin(), key_trnsf.end(), key_trnsf.begin(), std::tolower);
			
		if (key_trnsf.compare(name_trnsf) == 0)
			return itr.second; // Found
	}
	// Not found
	return nullptr;
	
}
std::vector<DCM::Element*> DCM::Manager::findElements(std::string name, bool exactmatch)
{
	std::vector<DCM::Element*> ret;
	if (name.empty())
		return ret;

	std::string name_trnsf = name;
	if (!exactmatch)
		std::transform(name_trnsf.begin(), name_trnsf.end(), name_trnsf.begin(), std::tolower);
}
void DCM::Manager::putElement(Element* element)
{
	elements.push_back(element);	
	switch (element->type)
	{
	case TYPE::UNKNOWN:
		unknowns.push_back((Unknown*)element);
		break;
	case TYPE::COMMENT:
		comments.push_back((Comment*)element);
		break;
	case TYPE::FORMAT:
		formats.push_back((Format*)element);
		break;
	case TYPE::FUNCTIONS:
		functions.push_back((Functions*)element);
		break;
	case TYPE::VARIANTCODING:
		variantCodings.push_back((VariantCoding*)element);
		break;
	case TYPE::MODULEHEADER:
		moduleHeaders.push_back((ModuleHeader*)element);
		break;
	case TYPE::PARAMETER:
	case TYPE::BOOLEAN:
	case TYPE::ARRAY:
	case TYPE::MATRIX:
	case TYPE::CHARLINE:
	case TYPE::CHARMAP:
	case TYPE::FIXEDCHARLINE:
	case TYPE::FIXEDCHARMAP:
	case TYPE::GROUPCHARLINE:
	case TYPE::GROUPCHARMAP:
	case TYPE::DISTRIBUTION:
		parameters.push_back((BaseParameter*)element);
		elementIndex[((BaseParameter*)element)->name] = element;
		break;
	}
}
std::string DCM::Manager::rebuildUnknown(Unknown* unknown)
{
	std::string text = "";
	if (unknown->type != TYPE::UNKNOWN)
		return text;

	return unknown->text + "\n";

}

std::string DCM::Manager::rebuildFunctions(Functions* functions)
{
	std::string text = "";
	if (functions->type != TYPE::FUNCTIONS)
		return text;

	text += "FUNKTIONEN\n";
	for (auto &fkt : functions->functions)
	{
		text += "   FKT " + fkt.name + " " + fkt.version + " " + fkt.longname + " \n";
	}
	text += "END\n";
	return text;
}

std::string DCM::Manager::rebuildComment(Comment* comment)
{
	std::string text = "";
	if (comment->type != TYPE::COMMENT)
		return text;

	return comment->text + "\n";

}

std::string DCM::Manager::rebuildVariantCoding(VariantCoding* variantCoding)
{
	std::string text = "";
	if (variantCoding->type != TYPE::VARIANTCODING)
		return text;

	text += "VARIANTENKODIERUNG\n";
	for (auto &variant : variantCoding->variants)
	{
		text += "   KRITERIUM " + variant.name;
		for (auto &value : variant.values)
			text += " " + value;
		text += "\n";
	}
	text += "END\n";
	return text;
}

std::string DCM::Manager::rebuildModuleHeader(ModuleHeader* moduleHeader)
{
	std::string text = "";
	if (moduleHeader->type != TYPE::MODULEHEADER)
		return text;

	text += "MODULKOPF " + moduleHeader->name + " " + moduleHeader->texts.at(0) + "\n";
	for(int i=1; i<moduleHeader->texts.size(); i++)
		text += "MODULKOPF " + moduleHeader->texts.at(i) + "\n";	
	text += "END\n";
	return text;
}

std::string DCM::Manager::rebuildFormat(Format* format)
{
	std::string text = "";
	if (format->type != TYPE::FORMAT)
		return text;

	text += "KONSERVIERUNG_FORMAT " + format->version + "\n";
	return text;
}

std::string DCM::Manager::rebuildParameter(Parameter* parameter)
{
	std::string text = "";
	if (parameter->type != TYPE::PARAMETER)
		return text;

	text += "FESTWERT " + parameter->name + " \n";
	if (!parameter->langname.empty())
		text += "   LANGNAME " + parameter->langname + " \n";
	if (!parameter->displayname.empty())
		text += "   DISPLAYNAME " + parameter->displayname + "\n";
	if (!parameter->variant.empty())
		text += "   VAR " + parameter->variant + "\n";
	if (!parameter->function.empty())
		text += "   FUNKTION " + parameter->function + " \n";
	if (!parameter->unit.empty())
		text += "   EINHEIT_W " + parameter->unit + "\n";	
	text += "   WERT " + DCM::toFixed(parameter->value, parameter->dec_value) + "\n";
	text += "END\n";
	
	return text;

}
std::string DCM::Manager::rebuildBoolean(Boolean* boolean)
{
	std::string text = "";
	if (boolean->type != TYPE::BOOLEAN)
		return text;

	text += "FESTWERT " + boolean->name + " \n";
	if (!boolean->langname.empty())
		text += "   LANGNAME " + boolean->langname + " \n";
	if (!boolean->displayname.empty())
		text += "   DISPLAYNAME " + boolean->displayname + "\n";
	if (!boolean->variant.empty())
		text += "   VAR " + boolean->variant + "\n";
	if (!boolean->function.empty())
		text += "   FUNKTION " + boolean->function + " \n";
	if (!boolean->unit.empty())
		text += "   EINHEIT_W " + boolean->unit + "\n";
	if (!boolean->text.empty())
		text += "   TEXT " + boolean->text + "\n";	
	text += "END\n";

	return text;

}

std::string DCM::Manager::rebuildArray(Array* arr)
{
	std::string text = "";
	if (arr->type != TYPE::ARRAY)
		return text;

	text += "FESTWERTEBLOCK " + arr->name + " ";
	if (arr->size_x)
		text += std::to_string(arr->size_x);
	text += "\n";
	if (!arr->langname.empty())
		text += "   LANGNAME " + arr->langname + " \n";
	if (!arr->displayname.empty())
		text += "   DISPLAYNAME " + arr->displayname + "\n";
	if (!arr->variant.empty())
		text += "   VAR " + arr->variant + "\n";
	if (!arr->function.empty())
		text += "   FUNKTION " + arr->function + " \n";
	if (!arr->unit.empty())
		text += "   EINHEIT_W " + arr->unit + "\n";
	
	
	for (int i=0; i<arr->values.size(); i++)
	{
		if (i % 6 == 0)
			text += "   WERT   ";
		text += DCM::toFixed(arr->values.at(i), arr->dec_values.at(i)) + "   ";
		if (i % 6 == 5 || i == arr->values.size()-1)
			text += "\n";
	}
	text += "END\n";

	return text;
}
std::string DCM::Manager::rebuildMatrix(Matrix* matrix)
{
	std::string text = "";
	if (matrix->type != TYPE::MATRIX)
		return text;

	text += "FESTWERTEBLOCK " + matrix->name + " ";
	if (matrix->size_x && matrix->size_y)
		text += std::to_string(matrix->size_x) + " @ " + std::to_string(matrix->size_y);
	text += "\n";
	if (!matrix->langname.empty())
		text += "   LANGNAME " + matrix->langname + " \n";
	if (!matrix->displayname.empty())
		text += "   DISPLAYNAME " + matrix->displayname + "\n";
	if (!matrix->variant.empty())
		text += "   VAR " + matrix->variant + "\n";
	if (!matrix->function.empty())
		text += "   FUNKTION " + matrix->function + " \n";
	if (!matrix->unit.empty())
		text += "   EINHEIT_W " + matrix->unit + "\n";

	for (int i = 0; i < matrix->size_y; i++)
	{
		for (int j = 0; j < matrix->size_x; j++)
		{
			if (j % 6 == 0)
				text += "   WERT   ";
			text += DCM::toFixed(matrix->values.at(i * matrix->size_x + j), matrix->dec_values.at(i * matrix->size_x + j)) + "   ";
			if (j % 6 == 5 || j == matrix->size_x - 1)
				text += "\n";
		}
	}
	text += "END\n";

	return text;
}


std::string DCM::Manager::rebuildLineBaseParameter(LineBaseParameter* line)
{
	std::string text = "";
	switch (line->type)
	{
	case TYPE::CHARLINE:
	{
		text += "KENNLINIE " + line->name;
		break;
	}
	case TYPE::FIXEDCHARLINE:
	{
		text += "FESTKENNLINIE " + line->name;
		break;
	}
	case TYPE::GROUPCHARLINE:
	{
		text += "GRUPPENKENNLINIE " + line->name;
		break;
	}
	default:
		return text;
	}	

	text += " ";
	if (line->size_x)
		text += std::to_string(line->size_x);
	text += "\n";
	if (!line->langname.empty())
		text += "   LANGNAME " + line->langname + " \n";
	if (!line->displayname.empty())
		text += "   DISPLAYNAME " + line->displayname + "\n";
	if (!line->variant.empty())
		text += "   VAR " + line->variant + "\n";
	if (!line->function.empty())
		text += "   FUNKTION " + line->function + " \n";
	if (!line->unit_x.empty())
		text += "   EINHEIT_X " + line->unit_x + "\n";
	if (!line->unit.empty())
		text += "   EINHEIT_W " + line->unit + "\n";
	
	if (line->type == TYPE::GROUPCHARLINE)
		if(!((GroupCharLine*)line)->dist_x.empty()) text += "*SSTX\t" + ((GroupCharLine*)line)->dist_x + "\n";

	
	for (int i = 0; i < line->point_x.size(); i++)
	{
		if (i % 6 == 0)
			text += "   ST/X   ";
		text += DCM::toFixed(line->point_x.at(i), line->dec_point_x.at(i)) + "   ";
		if (i % 6 == 5 || i == line->point_x.size() - 1)
			text += "\n";
	}

	for (int i = 0; i < line->values.size(); i++)
	{
		if (i % 6 == 0)
			text += "   WERT   ";
		text += DCM::toFixed(line->values.at(i), line->dec_values.at(i)) + "   ";
		if (i % 6 == 5 || i == line->values.size() - 1)
			text += "\n";
	}
	

	text += "END\n";

	return text;
}
std::string DCM::Manager::rebuildMapBaseParameter(MapBaseParameter* map)
{
	std::string text = "";
	switch (map->type)
	{
	case TYPE::CHARMAP:
	{
		text += "KENNFELD " + map->name;
		break;
	}
	case TYPE::FIXEDCHARMAP:
	{
		text += "FESTKENNFELD " + map->name;
		break;
	}
	case TYPE::GROUPCHARMAP:
	{
		text += "GRUPPENKENNFELD " + map->name;
		break;
	}
	default:
		return text;
	}

	text += " ";
	if (map->size_x && map->size_y)
		text += std::to_string(map->size_x) + " " + std::to_string(map->size_y);
	text += "\n";
	if (!map->langname.empty())
		text += "   LANGNAME " + map->langname + " \n";
	if (!map->displayname.empty())
		text += "   DISPLAYNAME " + map->displayname + "\n";
	if (!map->variant.empty())
		text += "   VAR " + map->variant + "\n";
	if (!map->function.empty())
		text += "   FUNKTION " + map->function + " \n";
	if (!map->unit_x.empty())
		text += "   EINHEIT_X " + map->unit_x + "\n";
	if (!map->unit_y.empty())
		text += "   EINHEIT_Y " + map->unit_y + "\n";
	if (!map->unit.empty())
		text += "   EINHEIT_W " + map->unit + "\n";

	if (map->type == TYPE::GROUPCHARMAP)
	{
		if (!((GroupCharMap*)map)->dist_x.empty()) text += "*SSTX\t" + ((GroupCharMap*)map)->dist_x + "\n";
		if (!((GroupCharMap*)map)->dist_y.empty()) text += "*SSTY\t" + ((GroupCharMap*)map)->dist_y + "\n";
	}
	
	for (int i = 0; i < map->point_x.size(); i++)
	{
		if (i % 6 == 0)
			text += "   ST/X   ";
		text += DCM::toFixed(map->point_x.at(i), map->dec_point_x.at(i)) + "   ";
		if (i % 6 == 5 || i == map->point_x.size() - 1)
			text += "\n";
	}

	for(int i=0; i<map->point_y.size(); i++)
	{
		auto value_y = map->point_y.at(i);
		auto dec_value_y = map->dec_point_y.at(i);
		text += "   ST/Y   " + DCM::toFixed(value_y, dec_value_y) + "\n";
		for (int j = 0; j < map->point_x.size(); j++)
		{
			if (j % 6 == 0)
				text += "   WERT   ";
			text += DCM::toFixed(map->values.at(i * map->point_x.size() + j), map->dec_values.at(i * map->point_x.size() + j)) + "   ";
			if (j % 6 == 5 || j == map->point_x.size() - 1)
				text += "\n";
		}		
	}

	text += "END\n";

	return text;
}

std::string DCM::Manager::rebuildDistribution(Distribution* dist)
{
	std::string text = "";
	if (dist->type != TYPE::DISTRIBUTION)
		return text;

	text += "STUETZSTELLENVERTEILUNG " + dist->name + " ";
	if (dist->size_x)
		text += std::to_string(dist->size_x);
	text += "\n";
	text += "*SST\n";
	if (!dist->langname.empty())
		text += "   LANGNAME " + dist->langname + " \n";
	if (!dist->displayname.empty())
		text += "   DISPLAYNAME " + dist->displayname + "\n";
	if (!dist->variant.empty())
		text += "   VAR " + dist->variant + "\n";
	if (!dist->function.empty())
		text += "   FUNKTION " + dist->function + " \n";
	if (!dist->unit_x.empty())
		text += "   EINHEIT_X " + dist->unit_x + "\n";
	
	for (int i = 0; i < dist->point_x.size(); i++)
	{
		if (i % 6 == 0)
			text += "   ST/X   ";
		text += DCM::toFixed(dist->point_x.at(i), dist->dec_point_x.at(i)) + "   ";
		if (i % 6 == 5 || i == dist->point_x.size() - 1)
			text += "\n";
	}

	text += "END\n";

	return text;
}

std::string DCM::Manager::rebuildElement(Element* element)
{
	switch (element->type)
	{
	case TYPE::UNKNOWN:
		return rebuildUnknown((Unknown*)element);		
	case TYPE::FUNCTIONS:
		return rebuildFunctions((Functions*)element);
	case TYPE::COMMENT:
		return rebuildComment((Comment*)element);
	case TYPE::VARIANTCODING:
		return rebuildVariantCoding((VariantCoding*)element);
	case TYPE::MODULEHEADER:
		return rebuildModuleHeader((ModuleHeader*)element);
	case TYPE::FORMAT:
		return rebuildFormat((Format*)element);
	case TYPE::PARAMETER:	
		return rebuildParameter((Parameter*)element);
	case TYPE::BOOLEAN:
		return rebuildBoolean((Boolean*)element);
	case TYPE::ARRAY:
		return rebuildArray((Array*)element);
	case TYPE::MATRIX:
		return rebuildMatrix((Matrix*)element);
	case TYPE::CHARLINE:
	case TYPE::FIXEDCHARLINE:
	case TYPE::GROUPCHARLINE:
		return rebuildLineBaseParameter((LineBaseParameter*)element);
	case TYPE::CHARMAP:
	case TYPE::FIXEDCHARMAP:
	case TYPE::GROUPCHARMAP:
		return rebuildMapBaseParameter((MapBaseParameter*)element);
	case TYPE::DISTRIBUTION:
		return rebuildDistribution((Distribution*)element);
	default:
		return "";
	}
}
std::string DCM::Manager::rebuild()
{
	std::string text = "";
	for (auto element : elements)
		text += rebuildElement(element);

	return text;
}

void DCM::Manager::saveAsDCM(std::string fname)
{
	std::fstream wfile;
	wfile.open(fname.c_str(), std::ios::out);
	if (!wfile.is_open())
		return;

	for (auto element : elements)
		wfile << rebuildElement(element);

	wfile.close();
}

std::vector<DCM::BaseParameter*> DCM::Manager::collectAllTypeParameters() 
{
	return parameters; 
}
std::vector<DCM::Distribution*> DCM::Manager::collectAxispoint()
{
	std::vector<DCM::Distribution*> ret;
	for (auto para : parameters)
	{
		if (para->type == DCM::TYPE::DISTRIBUTION)
			ret.push_back((DCM::Distribution*)para);
	}
	return ret;
}
std::vector<DCM::Array*> DCM::Manager::collectValueBlock()
{
	std::vector<DCM::Array*> ret;
	for (auto para : parameters)
	{
		if (para->type == DCM::TYPE::ARRAY)
			ret.push_back((DCM::Array*)para);
	}
	return ret;
}
std::vector<DCM::Matrix*> DCM::Manager::collectValueMatrix()
{
	std::vector<DCM::Matrix*> ret;
	for (auto para : parameters)
	{
		if (para->type == DCM::TYPE::MATRIX)
			ret.push_back((DCM::Matrix*)para);
	}
	return ret;
}
std::vector<DCM::Parameter*> DCM::Manager::collectValue()
{
	std::vector<DCM::Parameter*> ret;
	for (auto para : parameters)
	{
		if (para->type == DCM::TYPE::PARAMETER)
			ret.push_back((DCM::Parameter*)para);
	}
	return ret;
}
std::vector<DCM::Parameter*> DCM::Manager::collectValueBoolean()
{
	std::vector<DCM::Parameter*> ret;
	for (auto para : parameters)
	{
		if (para->type == DCM::TYPE::BOOLEAN)
			ret.push_back((DCM::Parameter*)para);
	}
	return ret;
}
std::vector<DCM::LineBaseParameter*> DCM::Manager::collectCurve()
{
	std::vector<DCM::LineBaseParameter*> ret;
	for (auto para : parameters)
	{
		switch (para->type)
		{
		case DCM::TYPE::CHARLINE:
		case DCM::TYPE::FIXEDCHARLINE:
		case DCM::TYPE::GROUPCHARLINE:
		case DCM::TYPE::DISTRIBUTION:
			ret.push_back((DCM::LineBaseParameter*)para);
		}
	}
	return ret;
}
std::vector<DCM::MapBaseParameter*> DCM::Manager::collectMap()
{
	std::vector<DCM::MapBaseParameter*> ret;
	for (auto para : parameters)
	{
		switch (para->type)
		{
		case DCM::TYPE::CHARMAP:
		case DCM::TYPE::FIXEDCHARMAP:
		case DCM::TYPE::GROUPCHARMAP:		
			ret.push_back((DCM::MapBaseParameter*)para);
		}
	}
	return ret;
}



bool DCM::Manager::test()
{
	bool ret = true;
	if(false){
		bool result = typeEnumTest();
		std::cout << "typeEnumTest " << result << std::endl;
		ret &= result;
	}	
	if(false){
		bool result = stripQuotationTest();
		std::cout << "stripQuoationTest " << result << std::endl;
		ret &= result;
	}
	{
		bool result = parseDCM1Test();
		std::cout << "parseDCM1Test " << result << std::endl;
		ret &= result;
	}
	{
		bool result = parseDCM2Test();
		std::cout << "parseDCM2Test " << result << std::endl;
		ret &= result;
	}

	return ret;
}
bool DCM::Manager::typeEnumTest()
{
	auto file = std::ifstream("headerTest.dcm");
	if (!file.is_open())
		return false;
	std::string line;
	int i = 0;
	std::cout << "\n\n";
	while (std::getline(file, line))
	{
		auto lineStrip = stripLine(line);
		auto header = lineStrip.at(0);
		auto type = Header[header];
		std::cout << type << "\t-> " << i << "\t: " << header;
		if (i != type)
		{
			std::cout << " not matched !!" << std::endl;
			std::cout << "\n\n";
			return false;			
		}
		else
		{
			i++;
			std::cout << std::endl;
		}
	}
	std::cout << "\n\n";
	return true;
}
bool DCM::Manager::stripQuotationTest()
{
	auto file = std::ifstream("quotationMarksTest.dcm");
	if (!file.is_open())
		return false;
	std::string line;
	int i = 0;
	std::cout << "\n\n";
	while (std::getline(file, line))
	{
		auto lineStrip = stripLine(line);
		std::cout << line << "\t>> ";
		for (auto &strip : lineStrip)
		{
			std::cout << strip << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << "\n\n";
	return true;
}
bool DCM::Manager::rebuildUnknownTest()
{
	auto manager = new Manager();
	manager->open("unknownTest.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildUnknown((Unknown*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::rebuildFunctionsTest()
{
	auto manager = new Manager();
	manager->open("functionsTest.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildFunctions((Functions*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::rebuildCommentTest()
{
	auto manager = new Manager();
	manager->open("commentTest.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildComment((Comment*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::rebuildFormatTest()
{
	auto manager = new Manager();
	manager->open("formatTest.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildFormat((Format*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::rebuildParameterTest()
{
	auto manager = new Manager();
	manager->open("Test_DCM2.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildParameter((Parameter*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::rebuildArrayTest()
{
	auto manager = new Manager();
	manager->open("Test_DCM2.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildArray((Array*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::rebuildMatrixTest()
{
	auto manager = new Manager();
	manager->open("Test_DCM2.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildMatrix((Matrix*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::rebuildLineBaseParameterTest()
{
	auto manager = new Manager();
	manager->open("Test_DCM2.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildLineBaseParameter((LineBaseParameter*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::rebuildMapBaseParameterTest()
{
	auto manager = new Manager();
	manager->open("Test_DCM2.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildMapBaseParameter((MapBaseParameter*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::rebuildDistributionTest()
{
	auto manager = new Manager();
	manager->open("Test_DCM2.dcm");
	for (auto element : manager->elements)
	{
		std::cout << manager->rebuildDistribution((Distribution*)element) << std::endl;
	}

	return true;
}
bool DCM::Manager::parseDCM1Test()
{
	auto manager = new Manager();
	bool result = manager->open("Test_DCM1ex.dcm");

	std::cout << "\n\n\t\tRebuild\n\n";

	std::cout << manager->rebuild();

	return result;

}
bool DCM::Manager::parseDCM2Test()
{
	auto manager = new Manager();
	bool result = manager->open("Test_DCM2.dcm");

	std::cout << "\n\n\t\tRebuild\n\n";

	std::cout << manager->rebuild();
	
	return result;
}

