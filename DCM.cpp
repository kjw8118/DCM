#include "DCM.h"


#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <filesystem>

#define DCM_WERTDIV 6

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
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(precision) << value;
	return oss.str();
}


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
		pCurrentElement = new Unknown(lineHistory.back(), (elements.empty() ? nullptr : elements.back()));
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
		pCurrentElement = new Comment(lineHistory.back(), (elements.empty() ? nullptr : elements.back()));
		putElement(pCurrentElement);
		pCurrentElement = nullptr;
		break;
	}
	case TYPE::FORMAT:
	{
		pCurrentElement = new Format(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
		putElement(pCurrentElement);
		pCurrentElement = nullptr;
		break;
	}
	case TYPE::FUNCTIONS:
	{
		pCurrentElement = new Functions((elements.empty() ? nullptr : elements.back()));
		break;
	}
	case TYPE::VARIANTCODING:
	{
		pCurrentElement = new VariantCoding((elements.empty() ? nullptr : elements.back()));
		break;
	}
	case TYPE::MODULEHEADER:
	{
		if (lineStrip.size() > 2)
		{
			pCurrentElement = new ModuleHeader(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
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
				pCurrentElement = new ModuleHeader(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
				moduleHeaders.push_back((ModuleHeader*)pCurrentElement);
			}
			pCurrentElement = nullptr;
		}
		break;
	}
	case TYPE::PARAMETER:
	{
		pCurrentElement = new Parameter(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
		break;
	}
	case TYPE::ARRAY:
	{
		
		if (lineStrip.size() > 4)
		{
			pCurrentElement = new Matrix(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
			((Matrix*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((Matrix*)pCurrentElement)->size_y = std::stoi(lineStrip.at(4)); // have @
		}
		else if (lineStrip.size() > 2)
		{
			pCurrentElement = new Array(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
			((Array*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		}
		break;
	}	
	case TYPE::CHARLINE:
	{
		pCurrentElement = new CharLine(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
		if (lineStrip.size() > 2)
			((CharLine*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		break;
	}
	case TYPE::CHARMAP:
	{
		pCurrentElement = new CharMap(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
		if (lineStrip.size() > 3)
		{
			((CharMap*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((CharMap*)pCurrentElement)->size_y = std::stoi(lineStrip.at(3)); // no have @			
		}
		break;
	}
	case TYPE::FIXEDCHARLINE:
	{
		pCurrentElement = new FixedCharLine(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
		if (lineStrip.size() > 2)
			((FixedCharLine*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		break;
	}
	case TYPE::FIXEDCHARMAP:
	{
		pCurrentElement = new FixedCharMap(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
		if (lineStrip.size() > 3)
		{
			((FixedCharMap*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((FixedCharMap*)pCurrentElement)->size_y = std::stoi(lineStrip.at(3)); // no have @
		}
		break;
	}
	case TYPE::GROUPCHARLINE:
	{
		pCurrentElement = new GroupCharLine(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
		if (lineStrip.size() > 2)
			((GroupCharLine*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
		break;
	}
	case TYPE::GROUPCHARMAP:
	{
		pCurrentElement = new GroupCharMap(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
		if (lineStrip.size() > 3)
		{
			((GroupCharMap*)pCurrentElement)->size_x = std::stoi(lineStrip.at(2));
			((GroupCharMap*)pCurrentElement)->size_y = std::stoi(lineStrip.at(3)); // no have @
		}
		break;
	}
	case TYPE::DISTRIBUTION:
	{
		pCurrentElement = new Distribution(lineStrip.at(1), (elements.empty() ? nullptr : elements.back()));
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
			auto pCurrentBoolean = new Boolean(*(Parameter*)pCurrentElement);
			pCurrentBoolean->text = lineStrip.at(1);			
			pCurrentElement = (Element*)pCurrentBoolean;			
			break;
		}
		case TYPE::BOOLEAN:
		{
			((Boolean*)pCurrentElement)->text = lineStrip.at(1);
			break;
		}
		}
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
		pCurrentElement->lineIndex->putEndIndex(currentIndex-1);
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
		pCurrentElement = new Unknown(lineHistory.back(), (elements.empty() ? nullptr : elements.back()));
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
void DCM::Manager::openWithRepo(std::string _fPath, std::string _gitName, std::string _gitEmail)
{
	if (isOpened)
		return;	

	std::string _fPathAbs = std::filesystem::absolute(_fPath).string();
	std::string _dirPath = std::filesystem::path(_fPathAbs).parent_path().string();
	std::string _fName = std::filesystem::path(_fPathAbs).filename().string();
	
	bool isRepoExist = GIT::isRepoExist(_dirPath);
	git = new GIT(_dirPath, _gitName, _gitEmail);	
	if (!isRepoExist)
	{
		
		std::string _ignoreName = "!" + _fName;
		git->appendGitIgnore({ "*", _ignoreName });
		git->stagingAll();
		git->gitCommit("Init");
	}
	open(_fPath);

	if (isOpened)
	{
		gitName = _gitName;
		gitEmail = _gitEmail;
	}
}
void DCM::Manager::open(std::string _fPath)
{
	if (isOpened)
		return;

	std::string _fPathAbs = std::filesystem::absolute(_fPath).string();
	std::string _fName = std::filesystem::path(_fPathAbs).filename().string();
	fPath = _fPathAbs;
	fName = _fName;
	if(!file.is_open())
		file.open(fPath, std::ios::in | std::ios::out | std::ios::app);
	else if(!file.is_open())
		file.open(fPath, std::ios::in);

	isOpened = file.is_open();

	if (isOpened)
	{
		
		std::ostringstream oss;
		oss << file.rdbuf();
		rawString = new std::string(oss.str());
		std::istringstream iss(*rawString);
		std::string line;
		std::vector<std::string> rawStringListTemp;
		while (std::getline(iss, line))
			rawStringListTemp.push_back(line);
		rawStringList = new std::vector<std::string>(rawStringListTemp.begin(), rawStringListTemp.end());
		
	}
}
void DCM::Manager::cloneFromRepo(std::string _remotePath, std::string _localPath, std::string _gitName, std::string _gitEmail)
{
	if (isOpened)
		return;

	bool isRemoteExist = GIT::isRepoExist(_remotePath);
	if (!isRemoteExist)
		return;
	auto localPathAbs_fs = std::filesystem::absolute(_localPath);
	auto localPathAbs = localPathAbs_fs.string();
	git = GIT::cloneFromRemote(_remotePath, localPathAbs, gitName, gitEmail);
	
	std::vector<std::string> fileList;
	for (const auto& entry : std::filesystem::directory_iterator(localPathAbs))
	{
		if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".dcm" || entry.path().extension() == ".DCM")
			fileList.push_back(entry.path().string());
	}
	if (fileList.size() != 1)
		return;

	auto filePath = (localPathAbs_fs / fileList.back()).string();
	open(filePath);
	if (isOpened)
	{
		gitName = _gitName;
		gitEmail = _gitEmail;
	}


}
void DCM::Manager::createDCM()
{
	clear();
	std::tm localTime;
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());  localtime_s(&localTime, &now);
	std::ostringstream oss;	oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
	putElement((Element*)(new Comment("* encoding=\"euc-kr\"", (elements.empty() ? nullptr : elements.back()))));
	putElement((Element*)(new Comment("* DAMOS format", (elements.empty() ? nullptr : elements.back()))));
	putElement((Element*)(new Comment("* Created by DCM Manager", (elements.empty() ? nullptr : elements.back()))));
	putElement((Element*)(new Comment("* Creation date: " + oss.str(), (elements.empty() ? nullptr : elements.back()))));
	putElement((Element*)(new Comment("*", (elements.empty() ? nullptr : elements.back()))));
	putElement((Element*)(new Unknown("", (elements.empty() ? nullptr : elements.back()))));
	putElement((Element*)(new Format("2.0", (elements.empty() ? nullptr : elements.back()))));
	putElement((Element*)(new Unknown("", (elements.empty() ? nullptr : elements.back()))));

	
}

void DCM::Manager::loadContents(std::string contents)
{
	clear();

	std::istringstream ss(contents);
	std::string lineRaw = "";
	while (std::getline(ss, lineRaw))
	{
		lineHistory.push_back(lineRaw);
		parseLine(lineRaw);
		currentIndex++;
	}
}
void DCM::Manager::parse()
{	
	loadContents(*rawString);	
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
	comments.clear();
	functions.clear();
	unknowns.clear();
	formats.clear();
	variantCodings.clear();
	parameters.clear();

}
void DCM::Manager::close()
{
	clear();
	isOpened = false;
	if (file.is_open())
		file.close();
	delete rawString; rawString = nullptr;
	delete rawStringList; rawStringList = nullptr;

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
		std::transform(name_trnsf.begin(), name_trnsf.end(), name_trnsf.begin(), [](unsigned char c) {return std::tolower(c); });
	for (auto &itr : elementIndex)
	{
			
		std::string key_trnsf = itr.first;
		if (!exactmatch)							
			std::transform(key_trnsf.begin(), key_trnsf.end(), key_trnsf.begin(), [](unsigned char c) {return std::tolower(c); });
			
		if (key_trnsf.compare(name_trnsf) == 0)
			return itr.second; // Found
	}
	// Not found
	return nullptr;
	
}
std::vector<DCM::Element*> DCM::Manager::findElements(std::string name, bool exactmatch)
{
	std::vector<DCM::Element*> ret;
	/*if (name.empty())
		return ret;

	std::string name_trnsf = name;
	if (!exactmatch)
		std::transform(name_trnsf.begin(), name_trnsf.end(), name_trnsf.begin(), [](unsigned char c) {return std::tolower(c); });

	auto itr = std::find_if(elements.begin(), elements.end(), [=](const Element* element) {
		std::string elementName_trnsf;
		switch (element->type)
		{
		case TYPE::PARAMETER:
		case TYPE::BOOLEAN:
		case TYPE::ARRAY:
		case TYPE::MATRIX:
		case TYPE::CHARLINE:
		case TYPE::CHARMAP:
		case TYPE::FIXEDCHARLINE:
		case TYPE::FIXEDCHARMAP:
		case TYPE::DISTRIBUTION:
		case TYPE::GROUPCHARLINE:
		case TYPE::GROUPCHARMAP:
		{
			elementName_trnsf = ((BaseParameter*)element)->name;
			if (!exactmatch)
				std::transform(elementName_trnsf.begin(), elementName_trnsf.end(), elementName_trnsf.begin(), [](unsigned char c) {return std::tolower(c); });
			return name_trnsf == elementName_trnsf;
			break;
		}
		}
		return false;
		});
	if (itr == elements.end())
		return ret;*/
	// impl

	return ret;
}
std::vector<DCM::BaseParameter*> DCM::Manager::findBaseParameters(std::string name, bool exactmatch)
{
	std::vector<DCM::BaseParameter*> ret;

	// impl

	return ret;
}
int DCM::Manager::calcEndIndex(Element* element)
{
	auto endIndex = element->lineIndex->getIndex();

	switch (element->type)
	{
	case TYPE::UNKNOWN:	
	case TYPE::COMMENT:
	case TYPE::FORMAT:
		break;
	case TYPE::FUNCTIONS:
		endIndex += ((Functions*)element)->functions.size() + 1;
		break;
	case TYPE::VARIANTCODING:
		endIndex += ((VariantCoding*)element)->variants.size() + 1;
		break;
	case TYPE::MODULEHEADER:
		endIndex += ((ModuleHeader*)element)->texts.size() + 1;
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
	{
		endIndex += (int)(!((BaseParameter*)element)->langname.empty());
		endIndex += (int)(!((BaseParameter*)element)->displayname.empty());
		endIndex += (int)(!((BaseParameter*)element)->variant.empty());
		endIndex += (int)(!((BaseParameter*)element)->function.empty());
		endIndex += (int)(!((BaseParameter*)element)->unit.empty());		
		switch (element->type)
		{
		case TYPE::PARAMETER:
		case TYPE::BOOLEAN:
		{
			endIndex += 1;
			break;
		}
		case TYPE::ARRAY:
		case TYPE::MATRIX:
		{
			endIndex += (int)(((ArrayBaseParameter*)element)->values.size() / DCM_WERTDIV);
			endIndex += (int)(((ArrayBaseParameter*)element)->values.size() % DCM_WERTDIV != 0);
			break;
		}
		case TYPE::CHARLINE:
		case TYPE::FIXEDCHARLINE:
		case TYPE::DISTRIBUTION:
		case TYPE::GROUPCHARLINE:
		{
			endIndex += (int)(!((LineBaseParameter*)element)->unit_x.empty());			
			endIndex += (int)(((LineBaseParameter*)element)->point_x.size() / DCM_WERTDIV);
			endIndex += (int)(((LineBaseParameter*)element)->point_x.size() % DCM_WERTDIV != 0);
			endIndex += (int)(((LineBaseParameter*)element)->values.size() / DCM_WERTDIV);
			endIndex += (int)(((LineBaseParameter*)element)->values.size() % DCM_WERTDIV != 0); // Distribution의 경우 size == 0 이므로 += 0
			switch (element->type)
			{
			case TYPE::GROUPCHARLINE:
			{
				endIndex += (int)(!((GroupCharLine*)element)->dist_x.empty());
				break;
			}
			}

			break;
		}
		case TYPE::CHARMAP:
		case TYPE::FIXEDCHARMAP:
		case TYPE::GROUPCHARMAP:
		{
			endIndex += (int)(!((MapBaseParameter*)element)->unit_x.empty());
			endIndex += (int)(!((MapBaseParameter*)element)->unit_y.empty());
			auto lineNum_x = (int)(((MapBaseParameter*)element)->point_x.size() / DCM_WERTDIV);
			lineNum_x += (int)(((MapBaseParameter*)element)->point_x.size() % DCM_WERTDIV != 0);
			auto lineNum_y = (int)(((MapBaseParameter*)element)->point_y.size());
			auto lineNum_z = lineNum_x * lineNum_y;
			endIndex += lineNum_x + lineNum_y + lineNum_z;
			switch (element->type)
			{
			case TYPE::GROUPCHARMAP:
			{
				endIndex += (int)(!((GroupCharMap*)element)->dist_x.empty());
				endIndex += (int)(!((GroupCharMap*)element)->dist_y.empty());
				break;
			}
			}

			break;
		}
		}

		endIndex += 1;

		break;
	}
	}

	return endIndex;
}
void DCM::Manager::putElement(Element* element)
{
	if (!elements.empty())
	{
		auto lastElement = elements.back();
		//auto lastIndex = lastElement->endIndex;
		//auto lastOrder = lastElement->lineOrder;

		//element->lineIndex = lastIndex + 1;
		//element->lineOrder = lastOrder + 1;
		element->lineIndex->attachPrev(lastElement->lineIndex);
	}
	else
	{
		//element->lineIndex = 0;
		//element->lineOrder = 0;
		element->lineIndex->detachPrev();
	}
	//auto endIndex = calcEndIndex(element);
	//element->endIndex = endIndex;

	//currentIndex = endIndex + 1;
	currentIndex = element->lineIndex->getEndIndex() + 1;
	//currentOrder = element->lineOrder + 1;
	currentOrder = element->lineIndex->getOrder() + 1;

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

std::string DCM::Manager::getRawString() 
{	
	return *rawString;
}

std::vector<std::string> DCM::Manager::getRawStringList()
{
	return *rawStringList;
}

std::string DCM::Manager::rebuildUnknown(Unknown* unknown)
{
	std::ostringstream oss;
	//std::string text = "";
	if (unknown->type != TYPE::UNKNOWN)
		return oss.str();
	oss << unknown->text + "\n";
	
	return oss.str();

}
std::string DCM::Manager::rebuildFunctions(Functions* functions)
{
	std::ostringstream oss;
	//std::string text = "";
	if (functions->type != TYPE::FUNCTIONS)
		return oss.str();

	oss << "FUNKTIONEN\n";
	for (auto &fkt : functions->functions)
	{
		oss << "   FKT " + fkt.name + " " + fkt.version + " " + fkt.longname + " \n";
	}
	oss << "END\n";

	return oss.str();
}
std::string DCM::Manager::rebuildComment(Comment* comment)
{
	std::ostringstream oss;
	//std::string text = "";
	if (comment->type != TYPE::COMMENT)
		return oss.str();
	oss << comment->text + "\n";

	return oss.str();
}
std::string DCM::Manager::rebuildVariantCoding(VariantCoding* variantCoding)
{
	std::ostringstream oss;
	//std::string text = "";
	if (variantCoding->type != TYPE::VARIANTCODING)
		return oss.str();

	oss << "VARIANTENKODIERUNG\n";
	for (auto &variant : variantCoding->variants)
	{
		oss << "   KRITERIUM " + variant.name;
		for (auto &value : variant.values)
			oss << " " + value;
		oss << "\n";
	}
	oss << "END\n";
	return oss.str();
}
std::string DCM::Manager::rebuildModuleHeader(ModuleHeader* moduleHeader)
{
	std::ostringstream oss;
	//std::string text = "";
	if (moduleHeader->type != TYPE::MODULEHEADER)
		return oss.str();

	oss << "MODULKOPF " + moduleHeader->name + " " + moduleHeader->texts.at(0) + "\n";
	for(int i=1; i<moduleHeader->texts.size(); i++)
		oss << "MODULKOPF " + moduleHeader->texts.at(i) + "\n";	
	oss << "END\n";
	return oss.str();
}
std::string DCM::Manager::rebuildFormat(Format* format)
{
	std::ostringstream oss;
	//std::string text = "";
	if (format->type != TYPE::FORMAT)
		return oss.str();

	oss << "KONSERVIERUNG_FORMAT " + format->version + "\n";
	return oss.str();
}
std::string DCM::Manager::rebuildParameter(Parameter* parameter)
{
	std::ostringstream oss;
	//std::string text = "";
	if (parameter->type != TYPE::PARAMETER)
		return oss.str();

	oss << "FESTWERT " + parameter->name + " \n";
	if (!parameter->langname.empty())
		oss << "   LANGNAME " + parameter->langname + " \n";
	if (!parameter->displayname.empty())
		oss << "   DISPLAYNAME " + parameter->displayname + "\n";
	if (!parameter->variant.empty())
		oss << "   VAR " + parameter->variant + "\n";
	if (!parameter->function.empty())
		oss << "   FUNKTION " + parameter->function + " \n";
	if (!parameter->unit.empty())
		oss << "   EINHEIT_W " + parameter->unit + "\n";	
	oss << "   WERT " + DCM::toFixed(parameter->value, parameter->dec_value) + "\n";
	oss << "END\n";
	
	return oss.str();

}
std::string DCM::Manager::rebuildBoolean(Boolean* boolean)
{
	std::ostringstream oss;
	//std::string text = "";
	if (boolean->type != TYPE::BOOLEAN)
		return oss.str();

	oss << "FESTWERT " + boolean->name + " \n";
	if (!boolean->langname.empty())
		oss << "   LANGNAME " + boolean->langname + " \n";
	if (!boolean->displayname.empty())
		oss << "   DISPLAYNAME " + boolean->displayname + "\n";
	if (!boolean->variant.empty())
		oss << "   VAR " + boolean->variant + "\n";
	if (!boolean->function.empty())
		oss << "   FUNKTION " + boolean->function + " \n";
	if (!boolean->unit.empty())
		oss << "   EINHEIT_W " + boolean->unit + "\n";
	if (!boolean->text.empty())
		oss << "   TEXT " + boolean->text + "\n";	
	oss << "END\n";

	return oss.str();

}
std::string DCM::Manager::rebuildArray(Array* arr)
{
	std::ostringstream oss;
	//std::string text = "";
	if (arr->type != TYPE::ARRAY)
		return oss.str();

	oss << "FESTWERTEBLOCK " + arr->name + " ";
	if (arr->size_x)
		oss << std::to_string(arr->size_x);
	oss << "\n";
	if (!arr->langname.empty())
		oss << "   LANGNAME " + arr->langname + " \n";
	if (!arr->displayname.empty())
		oss << "   DISPLAYNAME " + arr->displayname + "\n";
	if (!arr->variant.empty())
		oss << "   VAR " + arr->variant + "\n";
	if (!arr->function.empty())
		oss << "   FUNKTION " + arr->function + " \n";
	if (!arr->unit.empty())
		oss << "   EINHEIT_W " + arr->unit + "\n";
	
	
	for (int i=0; i<arr->values.size(); i++)
	{
		if (i % 6 == 0)
			oss << "   WERT   ";
		oss << DCM::toFixed(arr->values.at(i), arr->dec_values.at(i)) + "   ";
		if (i % 6 == 5 || i == arr->values.size()-1)
			oss << "\n";
	}
	oss << "END\n";

	return oss.str();
}
std::string DCM::Manager::rebuildMatrix(Matrix* matrix)
{
	std::ostringstream oss;
	//std::string text = "";
	if (matrix->type != TYPE::MATRIX)
		return oss.str();

	oss << "FESTWERTEBLOCK " + matrix->name + " ";
	if (matrix->size_x && matrix->size_y)
		oss << std::to_string(matrix->size_x) + " @ " + std::to_string(matrix->size_y);
	oss << "\n";
	if (!matrix->langname.empty())
		oss << "   LANGNAME " + matrix->langname + " \n";
	if (!matrix->displayname.empty())
		oss << "   DISPLAYNAME " + matrix->displayname + "\n";
	if (!matrix->variant.empty())
		oss << "   VAR " + matrix->variant + "\n";
	if (!matrix->function.empty())
		oss << "   FUNKTION " + matrix->function + " \n";
	if (!matrix->unit.empty())
		oss << "   EINHEIT_W " + matrix->unit + "\n";

	for (int i = 0; i < matrix->size_y; i++)
	{
		for (int j = 0; j < matrix->size_x; j++)
		{
			if (j % 6 == 0)
				oss << "   WERT   ";
			oss << DCM::toFixed(matrix->values.at(i * matrix->size_x + j), matrix->dec_values.at(i * matrix->size_x + j)) + "   ";
			if (j % 6 == 5 || j == matrix->size_x - 1)
				oss << "\n";
		}
	}
	oss << "END\n";

	return oss.str();
}
std::string DCM::Manager::rebuildLineBaseParameter(LineBaseParameter* line)
{
	std::ostringstream oss;
	//std::string text = "";
	switch (line->type)
	{
	case TYPE::CHARLINE:
	{
		oss << "KENNLINIE " + line->name;
		break;
	}
	case TYPE::FIXEDCHARLINE:
	{
		oss << "FESTKENNLINIE " + line->name;
		break;
	}
	case TYPE::GROUPCHARLINE:
	{
		oss << "GRUPPENKENNLINIE " + line->name;
		break;
	}
	default:
		return oss.str();
	}	

	oss << " ";
	if (line->size_x)
		oss << std::to_string(line->size_x);
	oss << "\n";
	if (!line->langname.empty())
		oss << "   LANGNAME " + line->langname + " \n";
	if (!line->displayname.empty())
		oss << "   DISPLAYNAME " + line->displayname + "\n";
	if (!line->variant.empty())
		oss << "   VAR " + line->variant + "\n";
	if (!line->function.empty())
		oss << "   FUNKTION " + line->function + " \n";
	if (!line->unit_x.empty())
		oss << "   EINHEIT_X " + line->unit_x + "\n";
	if (!line->unit.empty())
		oss << "   EINHEIT_W " + line->unit + "\n";
	
	if (line->type == TYPE::GROUPCHARLINE)
		if(!((GroupCharLine*)line)->dist_x.empty()) oss << "*SSTX\t" + ((GroupCharLine*)line)->dist_x + "\n";

	
	for (int i = 0; i < line->point_x.size(); i++)
	{
		if (i % 6 == 0)
			oss << "   ST/X   ";
		oss << DCM::toFixed(line->point_x.at(i), line->dec_point_x.at(i)) + "   ";
		if (i % 6 == 5 || i == line->point_x.size() - 1)
			oss << "\n";
	}

	for (int i = 0; i < line->values.size(); i++)
	{
		if (i % 6 == 0)
			oss << "   WERT   ";
		oss << DCM::toFixed(line->values.at(i), line->dec_values.at(i)) + "   ";
		if (i % 6 == 5 || i == line->values.size() - 1)
			oss << "\n";
	}
	

	oss << "END\n";

	return oss.str();
}
std::string DCM::Manager::rebuildMapBaseParameter(MapBaseParameter* map)
{
	std::ostringstream oss;
	//std::string text = "";
	switch (map->type)
	{
	case TYPE::CHARMAP:
	{
		oss << "KENNFELD " + map->name;
		break;
	}
	case TYPE::FIXEDCHARMAP:
	{
		oss << "FESTKENNFELD " + map->name;
		break;
	}
	case TYPE::GROUPCHARMAP:
	{
		oss << "GRUPPENKENNFELD " + map->name;
		break;
	}
	default:
		return oss.str();
	}

	oss << " ";
	if (map->size_x && map->size_y)
		oss << std::to_string(map->size_x) + " " + std::to_string(map->size_y);
	oss << "\n";
	if (!map->langname.empty())
		oss << "   LANGNAME " + map->langname + " \n";
	if (!map->displayname.empty())
		oss << "   DISPLAYNAME " + map->displayname + "\n";
	if (!map->variant.empty())
		oss << "   VAR " + map->variant + "\n";
	if (!map->function.empty())
		oss << "   FUNKTION " + map->function + " \n";
	if (!map->unit_x.empty())
		oss << "   EINHEIT_X " + map->unit_x + "\n";
	if (!map->unit_y.empty())
		oss << "   EINHEIT_Y " + map->unit_y + "\n";
	if (!map->unit.empty())
		oss << "   EINHEIT_W " + map->unit + "\n";

	if (map->type == TYPE::GROUPCHARMAP)
	{
		if (!((GroupCharMap*)map)->dist_x.empty()) oss << "*SSTX\t" + ((GroupCharMap*)map)->dist_x + "\n";
		if (!((GroupCharMap*)map)->dist_y.empty()) oss << "*SSTY\t" + ((GroupCharMap*)map)->dist_y + "\n";
	}
	
	for (int i = 0; i < map->point_x.size(); i++)
	{
		if (i % 6 == 0)
			oss << "   ST/X   ";
		oss << DCM::toFixed(map->point_x.at(i), map->dec_point_x.at(i)) + "   ";
		if (i % 6 == 5 || i == map->point_x.size() - 1)
			oss << "\n";
	}

	for(int i=0; i<map->point_y.size(); i++)
	{
		auto value_y = map->point_y.at(i);
		auto dec_value_y = map->dec_point_y.at(i);
		oss << "   ST/Y   " + DCM::toFixed(value_y, dec_value_y) + "\n";
		for (int j = 0; j < map->point_x.size(); j++)
		{
			if (j % 6 == 0)
				oss << "   WERT   ";
			oss << DCM::toFixed(map->values.at(i * map->point_x.size() + j), map->dec_values.at(i * map->point_x.size() + j)) + "   ";
			if (j % 6 == 5 || j == map->point_x.size() - 1)
				oss << "\n";
		}		
	}

	oss << "END\n";

	return oss.str();
}
std::string DCM::Manager::rebuildDistribution(Distribution* dist)
{
	std::ostringstream oss;
	//std::string text = "";
	if (dist->type != TYPE::DISTRIBUTION)
		return oss.str();

	oss << "STUETZSTELLENVERTEILUNG " + dist->name + " ";
	if (dist->size_x)
		oss << std::to_string(dist->size_x);
	oss << "\n";
	//oss << "*SST\n";
	if (!dist->langname.empty())
		oss << "   LANGNAME " + dist->langname + " \n";
	if (!dist->displayname.empty())
		oss << "   DISPLAYNAME " + dist->displayname + "\n";
	if (!dist->variant.empty())
		oss << "   VAR " + dist->variant + "\n";
	if (!dist->function.empty())
		oss << "   FUNKTION " + dist->function + " \n";
	if (!dist->unit_x.empty())
		oss << "   EINHEIT_X " + dist->unit_x + "\n";
	
	for (int i = 0; i < dist->point_x.size(); i++)
	{
		if (i % 6 == 0)
			oss << "   ST/X   ";
		oss << DCM::toFixed(dist->point_x.at(i), dist->dec_point_x.at(i)) + "   ";
		if (i % 6 == 5 || i == dist->point_x.size() - 1)
			oss << "\n";
	}

	oss << "END\n";

	return oss.str();
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
std::string DCM::Manager::rebuildElements()
{
	std::ostringstream oss;
	//std::string text = "";
	for (auto element : elements)
		oss << rebuildElement(element);

	return oss.str();
}

/*std::vector<std::string> DCM::Manager::rebuildListFromDiff(std::vector<GIT::DiffResult> diffResults)
{
	auto originList = getRawStringList();
	std::vector<std::string> rebuildList;
	int currIndex = 0;

	for (auto &diffResult : diffResults)
	{
		for (auto &diffHunk : diffResult.diffHunks)
		{
			int startIndex = diffHunk.hunk.old_start-1;
			rebuildList.insert(rebuildList.end(), originList.begin() + currIndex, originList.begin() + startIndex);
			currIndex = startIndex;
			for (auto& diffLine : diffHunk.diffLines)
			{
				switch (diffLine.type)
				{
				case GIT::DiffLine::CONTEXT:
					rebuildList.push_back(originList.at(currIndex++));
					break;
				case GIT::DiffLine::ADDED:
					rebuildList.push_back(diffLine.line);
					break;
				case GIT::DiffLine::DELETED:
					currIndex++;
					break;				
				}
			}
		}
	}
	
	rebuildList.insert(rebuildList.end(), originList.begin() + currIndex, originList.end());
	return rebuildList;
}
std::string DCM::Manager::rebuildFromDiff(std::vector<GIT::DiffResult> diffResults)
{
	auto rebuildList = rebuildListFromDiff(diffResults);
	std::ostringstream oss;
	for (auto line : rebuildList)
		oss << line << '\n';
	return oss.str();

}

std::vector<std::string> DCM::Manager::rebuildListFromEdit(std::string edit_id)
{
	if (git == nullptr)
		return {};
	auto diffResults = git->gitDiffWithCommit(fPath, edit_id);
	return rebuildListFromDiff(diffResults);
}

std::string DCM::Manager::rebuildFromEdit(std::string edit_id)
{
	if (git == nullptr)
		return "";
	auto diffResults = git->gitDiffWithCommit(fPath, edit_id);
	return rebuildFromDiff(diffResults);
}*/

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

std::vector<DCM::Manager::EditHistory> DCM::Manager::getEditHistoryList()
{
	std::vector<DCM::Manager::EditHistory> editHistoryList;
	if (git == nullptr)
		return editHistoryList;

	auto gitLog = git->gitLog();
	for (auto log : gitLog)
	{
		editHistoryList.emplace_back(
			std::chrono::system_clock::time_point(std::chrono::seconds(log.author.when.time)), // Date
			log.message, // Commit message
			log.oid_str // ID
		);
		// Date, Commit message, oid, Content		
	}

	return editHistoryList;
}
std::vector<std::string> DCM::Manager::getRevisionList()
{
	if (git == nullptr)
		return std::vector<std::string>();

	return git->getLocalBranchList();
}

std::string DCM::Manager::getContentsFromFile()
{
	return getRawString();
}

std::string DCM::Manager::getContentsFromEditID(std::string edit_id)
{	
	if (git == nullptr)
		return "";

	return git->gitShowFromCommit(fPath, edit_id);	
}
std::string DCM::Manager::getContentsFromRevision(std::string revision)
{
	if (git == nullptr)
		return "";

	return git->gitShowFromBranch(fPath, revision);
}

std::vector<GIT::DiffResult> DCM::Manager::getDiffWithCurrent()
{

	if (git == nullptr)
		return {};
	auto rawLines = getRawString();
	auto newLines = rebuildElements();
	auto diffResults = git->gitDiffHeadToMemory(fName, newLines);
	//git->printDiffResults(diffResults);

	return diffResults;
}

std::vector<GIT::DiffResult> DCM::Manager::getDiffWithEdit(std::string edit_id)
{
	if (git == nullptr)
		return {};
	auto rawLines = getRawString();
	return git->gitDiffWithCommit(fPath, edit_id);
}

std::vector<std::pair<DCM::BaseParameter*, DCM::BaseParameter*>> DCM::Manager::pairBaseParametersWith(std::vector<DCM::BaseParameter*>& otherBaseParameters)
{	
	std::vector<std::pair<DCM::BaseParameter*, DCM::BaseParameter*>> baseParameterPairs;
	auto myBaseParameters = collectAllTypeParameters();
	
	std::vector<DCM::BaseParameter*> filteredBaseParameters;
	for (auto otherPara : otherBaseParameters)
	{
		auto myPara = std::find_if(myBaseParameters.begin(), myBaseParameters.end(), [=](DCM::BaseParameter* myPara) {return myPara->name == otherPara->name; });
		if (myPara != myBaseParameters.end())					
			baseParameterPairs.push_back({ *myPara, otherPara });
		
	}
	return baseParameterPairs;
}

std::vector<std::pair<bool, std::pair<DCM::BaseParameter*, DCM::BaseParameter*>>> DCM::Manager::compareWith(std::vector<BaseParameter*> &otherBaseParameters)
{	
	auto baseParameterPairs = pairBaseParametersWith(otherBaseParameters);
	std::vector<std::pair<bool, std::pair<DCM::BaseParameter*, DCM::BaseParameter*>>> comparedParameters;
	for (auto paraPair : baseParameterPairs)
	{
		auto myParaString = rebuildElement((Element*)paraPair.first);
		auto otherParaString = rebuildElement((Element*)paraPair.second);
		if (myParaString == otherParaString)
			comparedParameters.push_back({ true, paraPair });
		else
			comparedParameters.push_back({ false, paraPair });
	}

	return comparedParameters;
}
void DCM::Manager::copyWith(std::vector<DCM::BaseParameter*>& otherBaseParameters)
{
	auto baseParameterPairs = pairBaseParametersWith(otherBaseParameters);
	for (auto paraPair : baseParameterPairs)
	{
		auto myPara = paraPair.first;
		auto otherPara = paraPair.second;
		switch (myPara->type)
		{
		case TYPE::PARAMETER:
			((Parameter*)myPara)->copyValue(*(Parameter*)otherPara);
			break;
		case TYPE::BOOLEAN:
			((Boolean*)myPara)->copyValue(*(Boolean*)otherPara);
			break;
		case TYPE::ARRAY:
		case TYPE::MATRIX:
			((ArrayBaseParameter*)myPara)->copyValue(*(ArrayBaseParameter*)otherPara);
			break;
		case TYPE::CHARLINE:		
		case TYPE::FIXEDCHARLINE:
		case TYPE::DISTRIBUTION:
		case TYPE::GROUPCHARLINE:
			((LineBaseParameter*)myPara)->copyValue(*(LineBaseParameter*)otherPara);
			break;
		case TYPE::CHARMAP:
		case TYPE::FIXEDCHARMAP:
		case TYPE::GROUPCHARMAP:
			((MapBaseParameter*)myPara)->copyValue(*(MapBaseParameter*)otherPara);
			break;
		}		
	}
}


void DCM::Manager::removeElement(DCM::Element* element)
{
	int order = element->lineIndex->getOrder();
	delete element;
	elements.erase(elements.begin() + order);

}
void DCM::Manager::removeElement(int order)
{
	if (order < 0 || order >= elements.size())
		return;

	auto element = elements.at(order);
	delete element;
	elements.erase(elements.begin() + order);
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
	manager->open("Test_DCM1ex.dcm");

	std::string result = manager->rebuildElements();
	
	std::cout << "\n\n\t\tRebuild\n\n";
	std::cout << result << std::endl;

	return !result.empty();

}
bool DCM::Manager::parseDCM2Test()
{
	auto manager = new Manager();
	manager->open("Test_DCM2.dcm");

	std::string result = manager->rebuildElements();

	std::cout << "\n\n\t\tRebuild\n\n";
	std::cout << result << std::endl;

	return !result.empty();
}

