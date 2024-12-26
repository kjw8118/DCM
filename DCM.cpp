#include "DCM.h"


#include <fstream>

#include <string>
#include <vector>
#include <map>

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
		
DCM::Unknown::Unknown(int lineIndex, int lineOrder, std::string text)
	: Element(lineIndex, lineOrder, TYPE::UNKNOWN), text(text) {};
	
DCM::Comment::Comment(int lineIndex, int lineOrder, std::string text)
	: Element(lineIndex, lineOrder, TYPE::COMMENT), text(text) {};
	
	
DCM::Format::Format(int lineIndex, int lineOrder, std::string version)
	: Element(lineIndex, lineOrder, TYPE::FORMAT), version(version) {};
		
	
DCM::Function::Function(std::string name, std::string version, std::string longname)
	: name(name), version(version), longname(longname) {};
		
	
DCM::Functions::Functions(int lineIndex, int lineOrder)
	: Element(lineIndex, lineOrder, TYPE::FUNCTIONS) {};
	

	
DCM::Variant::Variant(std::string name)
	: name(name) {};
		
	
DCM::VariantCoding::VariantCoding(int lineIndex, int lineOrder)
	: Element(lineIndex, lineOrder, TYPE::VARIANTCODING) {};
		

DCM::ModuleHeader::ModuleHeader(int lineIndex, int lineOrder, std::string name)
	: Element(lineIndex, lineOrder, TYPE::MODULEHEADER), name(name) {};
		

	
DCM::BaseParameter::BaseParameter(int lineIndex, int lineOrder, int type, std::string name)
	: Element(lineIndex, lineOrder, type), name(name) {};
		

DCM::ArrayBaseParameter::ArrayBaseParameter(int lineIndex, int lineOrder, int type, std::string name, int size_x, int size_y)
	: BaseParameter(lineIndex, lineOrder, type, name), size_x(size_x), size_y(size_y) {};
		

	
DCM::LineBaseParameter::LineBaseParameter(int lineIndex, int lineOrder, int type, std::string name, int size_x)
	: BaseParameter(lineIndex, lineOrder, type, name), size_x(size_x) {};
		
	
DCM::MapBaseParameter::MapBaseParameter(int lineIndex, int lineOrder, int type, std::string name, int size_x, int size_y)
	: LineBaseParameter(lineIndex, lineOrder, type, name, size_x), size_y(size_y) {};
		
	
DCM::Parameter::Parameter(int lineIndex, int lineOrder, std::string name)
	: BaseParameter(lineIndex, lineOrder, TYPE::PARAMETER, name), value(0) {};
		
	
DCM::Array::Array(int lineIndex, int lineOrder, std::string name, int size_x)
	: ArrayBaseParameter(lineIndex, lineOrder, TYPE::ARRAY, name, size_x, 0) {};


DCM::Matrix::Matrix(int lineIndex, int lineOrder, std::string name, int size_x, int size_y)
	: ArrayBaseParameter(lineIndex, lineOrder, TYPE::MATRIX, name, size_x, size_y) {};

	
DCM::CharLine::CharLine(int lineIndex, int lineOrder, std::string name, int size_x)
	: LineBaseParameter(lineIndex, lineOrder, TYPE::CHARLINE, name, size_x) {};


	
DCM::CharMap::CharMap(int lineIndex, int lineOrder, std::string name, int size_x, int size_y)
	: MapBaseParameter(lineIndex, lineOrder, TYPE::CHARMAP, name, size_x, size_y) {};

	
DCM::FixedCharLine::FixedCharLine(int lineIndex, int lineOrder, std::string name, int size_x)
	: LineBaseParameter(lineIndex, lineOrder, TYPE::FIXEDCHARLINE, name, size_x) {};

	
DCM::FixedCharMap::FixedCharMap(int lineIndex, int lineOrder, std::string name, int size_x, int size_y)
	: MapBaseParameter(lineIndex, lineOrder, TYPE::FIXEDCHARMAP, name, size_x, size_y) {};

	
DCM::Distrubution::Distrubution(int lineIndex, int lineOrder, std::string name, int size_x)
	: LineBaseParameter(lineIndex, lineOrder, TYPE::DISTRIBUTION, name, size_x) {};
		

	
DCM::GroupCharLine::GroupCharLine(int lineIndex, int lineOrder, std::string name, int size_x)
	: LineBaseParameter(lineIndex, lineOrder, TYPE::GROUPCHARLINE, name, size_x) {};
		

	
DCM::GroupCharMap::GroupCharMap(int lineIndex, int lineOrder, std::string name, int size_x, int size_y)
	: MapBaseParameter(lineIndex, lineOrder, TYPE::GROUPCHARMAP, name, size_x, size_y) {};
		


	
std::vector<std::string> DCM::stripLine(std::string lineRaw)
{
	bool isOpen = false;
	std::vector<std::string> lineStrip;
	std::string buffer;

	for (auto c : lineRaw)
	{
		if (c == '\"')
		{
			if (!isOpen)
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
			isOpen = !isOpen;

			continue;
		}
		else if ((c == ' ' && !isOpen) || (c == '\n'))
		{
			if (!buffer.empty())
			{
				lineStrip.push_back(buffer);
				buffer.clear();
			}
			isOpen = false;

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

/*

// Initial Header
Comment			: *
Format			: KONSERVIERUNG_FORMAT
Functions		: FUNKTIONEN
VariantCoding	: VARIANTENKODIERUNG
ModuleHeader	: MODULKOPF
Parameter		: FESTWERT
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
void DCM::Parser::parseHeader(std::vector<std::string> lineStrip)
{
	int headerType = Header[lineStrip.at(0)];
	switch (headerType)
	{
	case TYPE::UNKNOWN:
	{
		std::string lineString = "";
		lineString = lineStrip.at(0);
		for (int i = 1; i < lineStrip.size(); i++)
			lineString += " " + lineStrip.at(i);
		pCurrentElement = new Unknown(currentIndex, currentOrder, lineString);
		elements.push_back(pCurrentElement);
		pCurrentElement = nullptr;
		break;
	}
	case TYPE::COMMENT:
	{
		std::string lineComment = "";
		lineComment += lineStrip.at(0);
		for (int i = 1; i < lineStrip.size(); i++)
			lineComment += " " + lineStrip.at(i);
		pCurrentElement = new Comment(currentIndex, currentOrder, lineComment);
		
		elements.push_back(pCurrentElement);
		pCurrentElement = nullptr;
		break;
	}
	case TYPE::FORMAT:
	{
		pCurrentElement = new Format(currentIndex, currentOrder, lineStrip.at(1));
		elements.push_back(pCurrentElement);
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
		pCurrentElement = new Array(currentIndex, currentOrder, lineStrip.at(1));
		if (lineStrip.size() > 4)
		{
			((Array*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((Array*)pCurrentElement)->size_y = std::stoi(lineStrip.at(4));
		}
		else if (lineStrip.size() > 2)
		{
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
		if (lineStrip.size() > 4)
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
		if (lineStrip.size() > 4)
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
		if (lineStrip.size() > 4)
		{
			((GroupCharMap*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((GroupCharMap*)pCurrentElement)->size_y = std::stoi(lineStrip.at(3)); // no have @
		}
		break;
	}
	case TYPE::DISTRIBUTION:
	{
		pCurrentElement = new Distrubution(currentIndex, currentOrder, lineStrip.at(1));
		if (lineStrip.size() > 2)
			((Distrubution*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		break;
	}
	}
}
void DCM::Parser::parseComponent(std::vector<std::string> lineStrip)
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
			((BaseParameter*)pCurrentElement)->variant = { lineStrip.at(1), lineStrip.at(2) };
		else
		{
			// <v-name>=<v-val_i> '=' seperate case
		}
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
		case TYPE::ARRAY:
		case TYPE::MATRIX:
		{
			((ArrayBaseParameter*)pCurrentElement)->unit_x = lineStrip.at(1);
			break;
		}
		case TYPE::CHARLINE:
		case TYPE::CHARMAP:
		case TYPE::FIXEDCHARLINE:
		case TYPE::FIXEDCHARMAP:
		case TYPE::GROUPCHARLINE:
		case TYPE::GROUPCHARMAP:
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
		case TYPE::MATRIX:
		{
			((ArrayBaseParameter*)pCurrentElement)->unit_y = lineStrip.at(1);
			break;
		}
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
		{
			for (int i = 1; i < lineStrip.size(); i++)
				((LineBaseParameter*)pCurrentElement)->point_x.push_back(std::stod(lineStrip.at(i)));
			break;
		}
		case TYPE::CHARMAP:
		case TYPE::FIXEDCHARMAP:
		case TYPE::GROUPCHARMAP:
		{
			for (int i = 1; i < lineStrip.size(); i++)
				((MapBaseParameter*)pCurrentElement)->point_x.push_back(std::stod(lineStrip.at(i)));
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
				((MapBaseParameter*)pCurrentElement)->point_y.push_back(std::stod(lineStrip.at(i)));
			break;
		}
		}
		break;
	}
	case TYPE::VALUE:
	{

		break;
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
		elements.push_back(pCurrentElement);
		switch (pCurrentElement->type)
		{

		}

		pCurrentElement = nullptr;
		break;
	}
	}
}
void DCM::Parser::parseLine(std::string lineRaw)
{
	auto lineStrip = stripLine(lineRaw);

	if (lineStrip.empty())
		return;
	if (pCurrentElement == nullptr)
	{
		parseHeader(lineStrip);
	}
	else
	{
		parseComponent(lineStrip);
	}


}

DCM::Parser::Parser() {};

bool DCM::Parser::open(std::string fpath)
{
	std::fstream file(fpath.c_str(), std::ios::in);
	if (!file.is_open())
	{
		isOpened = false;
		return false;
	}

	isOpened = true;
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


	//std::cout << elements.size() << std::endl;
	return true;
};


std::string DCM::Parser::rebuildUnknown(Unknown* unknown)
{
	std::string text = "";
	if (unknown->type != TYPE::UNKNOWN)
		return text;

	return unknown->text;

}

std::string DCM::Parser::rebuildFunctions(Functions* functions)
{
	std::string text = "";
	if (functions->type != TYPE::FUNCTIONS)
		return text;

	text += "FUNKTIONEN\n";
	for (auto fkt : functions->functions)
	{
		text += "   FKT " + fkt.name + " " + fkt.version + " " + fkt.longname + "\n";
	}
	text += "END";
	return text;
}

std::string DCM::Parser::rebuildComment(Comment* comment)
{
	std::string text = "";
	if (comment->type != TYPE::COMMENT)
		return text;

	return comment->text;

}

std::string DCM::Parser::rebuildFormat(Format* format)
{
	std::string text = "";
	if (format->type != TYPE::FORMAT)
		return text;

	text += "KONSERVIERUNG_FORMAT " + format->version;
	return text;
}


bool DCM::Parser::test()
{
	bool ret = true;
	{
		bool result = typeEnumTest();
		std::cout << "typeEnumTest " << result << std::endl;
		ret &= result;
	}	
	{
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

bool DCM::Parser::typeEnumTest()
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

bool DCM::Parser::stripQuotationTest()
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
		for (auto strip : lineStrip)
		{
			std::cout << strip << ", ";
		}
		std::cout << std::endl;
	}
	std::cout << "\n\n";
	return true;
}

bool DCM::Parser::rebuildUnknownTest()
{
	auto parser = new Parser();
	parser->open("unknownTest.dcm");
	for (auto element : parser->elements)
	{
		std::cout << parser->rebuildUnknown((Unknown*)element) << std::endl;
	}

	return true;
}

bool DCM::Parser::rebuildFunctionsTest()
{
	auto parser = new Parser();
	parser->open("functionsTest.dcm");
	for (auto element : parser->elements)
	{
		std::cout << parser->rebuildFunctions((Functions*)element) << std::endl;
	}

	return true;
}

bool DCM::Parser::rebuildCommentTest()
{
	auto parser = new Parser();
	parser->open("commentTest.dcm");
	for (auto element : parser->elements)
	{
		std::cout << parser->rebuildComment((Comment*)element) << std::endl;
	}

	return true;
}

bool DCM::Parser::rebuildFormatTest()
{
	auto parser = new Parser();
	parser->open("formatTest.dcm");
	for (auto element : parser->elements)
	{
		std::cout << parser->rebuildFormat((Format*)element) << std::endl;
	}

	return true;
}


bool DCM::Parser::parseDCM1Test()
{
	auto parser = new Parser();
	return parser->open("Test_DCM1ex.dcm");

}

bool DCM::Parser::parseDCM2Test()
{
	auto parser = new Parser();
	return parser->open("Test_DCM2.dcm");
	
}