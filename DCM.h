#pragma once

#include <fstream>

#include <string>
#include <vector>
#include <map>

#include <iostream>

namespace DCM
{
	
	/*
	
	Comment			: *
	Format			: KONSERVIERUNG_FORMAT
	Functions		: FUNKTIONEN
	Function		: FKT
	VariantCoding	: VARIANTENKODIERUNG
	Variant			: KRITERIUM
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
	
	*/

	enum TYPE
	{
		UNKNOWN = 0,
		COMMENT,
		FORMAT,
		FUNCTIONS,
		FUNCTION,
		VARIANTCODING,
		VARIANT,
		MODULEHEADER,
		PARAMETER,
		ARRAY,
		CHARLINE,
		CHARMAP,
		FIXEDCHARLINE,
		FIXEDCHARMAP,
		DISTRIBUTION,
		GROUPCHARLINE,
		GROUPCHARMAP,
		LANGNAME,
		DISPLAYNAME,
		UNIT_W,
		UNIT_X,
		UNIT_Y,
		VALUE,
		POINT_X,
		POINT_Y,
		DIST_X,
		DIST_Y,
		FUNKTION,
		VAR,
		END,
		MATRIX,

	};
	
	//std::map<std::string, int> Header;
	class Element
	{
	public:		
		Element(int lineIndex, int &lineOrder, int type);
		int lineIndex = -1;
		int lineOrder = -1;
		int type = 0;

	};
	class Comment: public Element
	{
	public:
		Comment(int lineIndex, int lineOrder, std::string text);
		std::string text;
	};
	class Format : public Element
	{
	public:
		Format(int lineIndex, int lineOrder, std::string version);
		std::string version;
	};
	class Function
	{
	public:
		Function(std::string name, std::string version, std::string longname);
		std::string name;
		std::string version;
		std::string longname;
	};
	class Functions : public Element
	{
	public:
		Functions(int lineIndex, int lineOrder);
		std::vector<Function> functions;
	};

	class Variant
	{
	public:
		Variant(std::string name);
		std::string name;
		std::vector<std::string> values;
	};
	class VariantCoding : public Element
	{
	public:
		VariantCoding(int lineIndex, int lineOrder);
		std::vector<Variant> variants;
	};

	class ModuleHeader : public Element
	{
	public:
		ModuleHeader(int lineIndex, int lineOrder, std::string name);
		std::string name;
		std::vector<std::string> texts;
	};

	class BaseParameter : public Element
	{
	public:
		BaseParameter(int lineIndex, int lineOrder, int type, std::string name);
		std::string name;
		std::string langname = "";
		std::string displayname = "";
		//Variant* variant = nullptr;
		std::pair<std::string, std::string> variant;
		//Function* function = nullptr;
		std::string function;
		std::string unit = "";
		
		
	};

	class ArrayBaseParameter : public BaseParameter
	{
	public:
		ArrayBaseParameter(int lineIndex, int lineOrder, int type, std::string name, int size_x, int size_y);
		int size_x;
		int size_y;
		std::string unit_x = "";
		std::string unit_y = "";
		std::vector<double> values;
		
	};

	class LineBaseParameter : public BaseParameter
	{
	public:
		LineBaseParameter(int lineIndex, int lineOrder, int type, std::string name, int size_x);
		int size_x;
		std::string unit_x = "";
		std::vector<double> point_x;
		std::vector<double> values;
	};

	class MapBaseParameter : public LineBaseParameter
	{
	public:
		MapBaseParameter(int lineIndex, int lineOrder, int type, std::string name, int size_x, int size_y);
		int size_y;
		std::string unit_y = "";
		std::vector<double> point_y;
		std::vector<std::vector<double>> values;
	};

	class Parameter : public BaseParameter // FESTWERT
	{
	public:
		Parameter(int lineIndex, int lineOrder, std::string name);
		double value;
	};

	class Array : public ArrayBaseParameter // FESTWERTEBLOCK
	{
	public:
		Array(int lineIndex, int lineOrder, std::string name, int size_x = 0);
		
	};

	class Matrix : public ArrayBaseParameter // FESTWERTEBLOCK
	{
	public:
		Matrix(int lineIndex, int lineOrder, std::string name, int size_x = 0, int size_y = 0);
		
	};

	class CharLine : public LineBaseParameter // KENNLINIE
	{
	public:
		CharLine(int lineIndex, int lineOrder, std::string name, int size_x = 0);
		
	};

	class CharMap : public MapBaseParameter // KENNFELD
	{
	public:
		CharMap(int lineIndex, int lineOrder, std::string name, int size_x = 0, int size_y = 0);		
		
	};

	class FixedCharLine : public LineBaseParameter // FESTKENNLINIE
	{
	public:
		FixedCharLine(int lineIndex, int lineOrder, std::string name, int size_x = 0);
		
	};

	class FixedCharMap : public MapBaseParameter // FESTKENNFELD
	{
	public:
		FixedCharMap(int lineIndex, int lineOrder, std::string name, int size_x = 0, int size_y = 0);

	};

	class Distrubution : public LineBaseParameter
	{
	public:
		Distrubution(int lineIndex, int lineOrder, std::string name, int size_x = 0);		
		
	};

	class GroupCharLine : public LineBaseParameter
	{
	public:
		GroupCharLine(int lineIndex, int lineOrder, std::string name, int size_x = 0);
		//Distrubution* dist_x = nullptr;
		std::string dist_x = "";
		
	};

	class GroupCharMap : public MapBaseParameter
	{
	public:
		GroupCharMap(int lineIndex, int lineOrder, std::string name, int size_x = 0, int size_y = 0);
		//Distrubution* dist_x = nullptr;
		std::string dist_x = "";
		//Distrubution* dist_y = nullptr;
		std::string dist_y = "";
		
	};

	std::vector<std::string> stripLine(std::string lineRaw);

	class Parser
	{
	private:
		bool isOpened = false;

		std::vector<std::string> lineHistory;
		int currentIndex = 0;
		int currentOrder = 0;
		Element* pCurrentElement = nullptr;

		std::vector<Element*> elements;
		std::vector<ModuleHeader*> moduleHeaders;				

		

		

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
		void parseHeader(std::vector<std::string> lineStrip);
		void parseComponent(std::vector<std::string> lineStrip);
		void parseLine(std::string lineRaw);		
	public:
		Parser();
		bool open(std::string fpath);
		static bool test();

		// Test Code
		static bool typeEnumTest();
		static bool stripQuotationTest();
		static bool parseDCM1Test();
		static bool parseDCM2Test();
	};
};

