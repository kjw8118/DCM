#pragma once

#include <fstream>

#include <string>
#include <vector>
#include <map>

#include "GIT.h" // From github.com/kjw8118/GITwrap.git

namespace DCM
{
	
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
		TEXT,
		POINT_X,
		POINT_Y,
		DIST_X,
		DIST_Y,
		FUNKTION,
		VAR,
		END,
		MATRIX,
		BOOLEAN,

	};
	
	//std::map<std::string, int> Header;
	class Element
	{
	public:		
		Element(int lineIndex, int &lineOrder, int type);
		int lineIndex = -1;
		int lineOrder = -1;
		int endIndex = -1;
		int type = 0;
		Element(const Element& other)
			: lineIndex(other.lineIndex), lineOrder(other.lineOrder), endIndex(other.endIndex), type(other.type) {};

	};
	class Unknown : public Element
	{
	public:
		Unknown(int lineIndex, int &lineOrder, std::string text);
		std::string text;
		Unknown(const Unknown& other) : Element(other), text(other.text) {};
		void copy(const Unknown& other)
		{
			if (this->type != other.type)
				return;
			this->text = other.text;
		}
	};
	class Comment: public Element
	{
	public:
		Comment(int lineIndex, int &lineOrder, std::string text);
		std::string text;
		Comment(const Comment& other) : Element(other), text(other.text) {};
		void copy(const Comment& other)
		{
			if (this->type != other.type)
				return;
			this->text = other.text;
		}
	};
	class Format : public Element
	{
	public:
		Format(int lineIndex, int &lineOrder, std::string version);
		std::string version;
		Format(const Format& other) : Element(other), version(other.version) {};
		void copy(const Format& other)
		{
			if (this->type != other.type)
				return;
			this->version = other.version;
		}
	};
	class Function
	{
	public:
		Function(std::string name, std::string version, std::string longname);
		std::string name;
		std::string version;
		std::string longname;
		Function(const Function& other) : name(other.name), version(other.version), longname(other.longname) {};
		void copy(const Function& other)
		{
			this->name = other.name;			
			this->version = other.version;
			this->longname = other.longname;
		}
	};
	class Functions : public Element
	{
	public:
		Functions(int lineIndex, int &lineOrder);
		std::vector<Function> functions;
		Functions(const Functions& other) : Element(other), functions(other.functions) {};
		void copy(const Functions& other)
		{
			if (this->type != other.type)
				return;
			this->functions = other.functions;
		}
	};
	class Variant
	{
	public:
		Variant(std::string name);
		std::string name;
		std::vector<std::string> values;
		Variant(const Variant& other) : name(other.name), values(other.values) {};
		void copy(const Variant& other)
		{
			this->name = other.name;
			this->values = other.values;
		}
	};
	class VariantCoding : public Element
	{
	public:
		VariantCoding(int lineIndex, int &lineOrder);
		std::vector<Variant> variants;
		VariantCoding(const VariantCoding& other) : Element(other), variants(other.variants) {};
		void copy(const VariantCoding& other)
		{
			if (this->type != other.type)
				return;
			this->variants = other.variants;
		}
	};
	class ModuleHeader : public Element
	{
	public:
		ModuleHeader(int lineIndex, int &lineOrder, std::string name);
		std::string name;
		std::vector<std::string> texts;
		ModuleHeader(const ModuleHeader& other) : Element(other), name(other.name), texts(other.texts) {};
		void copy(const ModuleHeader& other)
		{
			if (this->type != other.type)
				return;
			this->name = other.name;
			this->texts = other.texts;
		}
	};
	class BaseParameter : public Element
	{
	public:
		BaseParameter(int lineIndex, int &lineOrder, int type, std::string name);
		std::string name;
		std::string langname = "";
		std::string displayname = "";
		//Variant* variant = nullptr;
		//std::pair<std::string, std::string> variant;
		std::string variant = "";		
		//Function* function = nullptr;
		std::string function;
		std::string unit = "";
		BaseParameter(const BaseParameter& other)
			:Element(other), name(other.name), langname(other.langname), displayname(other.displayname),
			variant(other.variant), function(other.function), unit(other.unit) {};
		void copy(const BaseParameter& other)
		{
			if (this->type != other.type)
				return;
			this->name = other.name;
			this->langname = other.langname;
			this->displayname = other.displayname;
			this->variant = other.variant;
			this->function = other.function;
			this->unit = other.unit;
		}		
		
	};
	class ArrayBaseParameter : public BaseParameter
	{
	public:
		ArrayBaseParameter(int lineIndex, int &lineOrder, int type, std::string name, int size_x, int size_y);
		int size_x;
		int size_y;
		//std::vector<double> values;
		std::vector<double> values;
		std::vector<int> dec_values;
		ArrayBaseParameter(const ArrayBaseParameter& other)
			: BaseParameter(other), size_x(other.size_x), size_y(other.size_y), values(other.values), dec_values(other.dec_values) {};
		void copy(const ArrayBaseParameter& other)
		{
			if (this->type != other.type)
				return;
			BaseParameter::copy(other);
			this->size_x = other.size_x;
			this->size_y = other.size_y;
			this->values = other.values;
			this->dec_values = other.dec_values;			
		}
		void copyValue(const ArrayBaseParameter& other)
		{
			if (this->type != other.type)
				return;
			if (this->values.size() != other.values.size())
				return;
			std::copy(other.values.begin(), other.values.end(), this->values.begin());
		}
		
	};
	class LineBaseParameter : public BaseParameter
	{
	public:
		LineBaseParameter(int lineIndex, int &lineOrder, int type, std::string name, int size_x);
		int size_x;
		std::string unit_x = "";
		std::vector<double> point_x;
		std::vector<double> values;
		std::vector<int> dec_point_x;
		std::vector<int> dec_values;
		LineBaseParameter(const LineBaseParameter& other)
			: BaseParameter(other), size_x(other.size_x), unit_x(other.unit_x), point_x(other.point_x), values(other.values),
			dec_point_x(other.dec_point_x), dec_values(other.dec_values) {};
		void copy(const LineBaseParameter& other)
		{
			if (this->type != other.type)
				return;
			BaseParameter::copy(other);
			this->size_x = other.size_x;
			this->unit_x = other.unit_x;
			this->point_x = other.point_x;
			this->values = other.values;
			this->dec_point_x = other.dec_point_x;
			this->dec_values = other.dec_values;
		}
		void copyValue(const LineBaseParameter& other)
		{
			if (this->type != other.type)
				return;
			if (this->values.size() != other.values.size() || this->point_x.size() != other.point_x.size())
				return;
			std::copy(other.point_x.begin(), other.point_x.end(), this->point_x.begin());
			std::copy(other.values.begin(), other.values.end(), this->values.begin());
		}
	};
	class MapBaseParameter : public LineBaseParameter
	{
	public:
		MapBaseParameter(int lineIndex, int &lineOrder, int type, std::string name, int size_x, int size_y);
		int size_y;
		std::string unit_y = "";
		std::vector<double> point_y;
		std::vector<int> dec_point_y;
		MapBaseParameter(const MapBaseParameter& other)
			: LineBaseParameter(other), size_y(other.size_y), unit_y(other.unit_y), point_y(other.point_y), dec_point_y(other.dec_point_y) {};
		void copy(const MapBaseParameter& other)
		{
			if (this->type != other.type)
				return;
			LineBaseParameter::copy(other);
			this->size_y = other.size_y;
			this->unit_y = other.unit_y;
			this->point_y = other.point_y;
			this->dec_point_y = other.dec_point_y;			
		}
		void copyValue(const MapBaseParameter& other)
		{
			if (this->type != other.type)
				return;
			LineBaseParameter::copyValue(other);
			if (this->point_y.size() != other.point_y.size())
				return;
			std::copy(other.point_y.begin(), other.point_y.end(), this->point_y.begin());			
		}
	};
	class Parameter : public BaseParameter // FESTWERT
	{
	public:
		Parameter(int lineIndex, int &lineOrder, std::string name);
		double value;
		int dec_value;
		Parameter(const Parameter& other) : BaseParameter(other), value(other.value), dec_value(other.dec_value) {};
		void copy(const Parameter& other)
		{
			if (this->type != other.type)
				return;
			BaseParameter::copy(other);
			this->value = other.value;
			this->dec_value = other.dec_value;			
		}
		void copyValue(const Parameter& other)
		{
			if (this->type != other.type)
				return;
			this->value = other.value;
		}
	};
	class Boolean : public BaseParameter // FESTWERT
	{
	public:
		Boolean(int lineIndex, int &lineOrder, std::string name);
		std::string text;
		Boolean(const Boolean& other) : BaseParameter(other), text(other.text) {};
		void copy(const Boolean& other)
		{
			if (this->type != other.type)
				return;
			BaseParameter::copy(other);
			this->text = other.text;			
		}
		void copyValue(const Boolean& other)
		{
			if (this->type != other.type)
				return;
			this->text = other.text;
		}
	};
	class Array : public ArrayBaseParameter // FESTWERTEBLOCK
	{
	public:
		Array(int lineIndex, int &lineOrder, std::string name, int size_x = 0);
		Array(const Array& other) : ArrayBaseParameter(other) {};
		void copy(const Array& other)
		{
			if (this->type != other.type)
				return;
			ArrayBaseParameter::copy(other);
		}
		void copyValue(const Array& other)
		{
			ArrayBaseParameter::copy(other);
		}
	};
	class Matrix : public ArrayBaseParameter // FESTWERTEBLOCK
	{
	public:
		Matrix(int lineIndex, int &lineOrder, std::string name, int size_x = 0, int size_y = 0);
		Matrix(const Matrix& other) : ArrayBaseParameter(other) {};
		void copy(const Matrix& other)
		{
			if (this->type != other.type)
				return;
			ArrayBaseParameter::copy(other);
		}
		void copyValue(const Matrix& other)
		{
			ArrayBaseParameter::copy(other);
		}
	};
	class CharLine : public LineBaseParameter // KENNLINIE
	{
	public:
		CharLine(int lineIndex, int &lineOrder, std::string name, int size_x = 0);
		CharLine(const CharLine& other) : LineBaseParameter(other) {};
		void copy(const CharLine& other)
		{
			if (this->type != other.type)
				return;
			LineBaseParameter::copy(other);
		}
		void copyValue(const CharLine& other)
		{
			LineBaseParameter::copy(other);
		}
	};
	class CharMap : public MapBaseParameter // KENNFELD
	{
	public:
		CharMap(int lineIndex, int &lineOrder, std::string name, int size_x = 0, int size_y = 0);		
		CharMap(const CharMap& other) : MapBaseParameter(other) {};
		void copy(const CharMap& other)
		{
			if (this->type != other.type)
				return;
			MapBaseParameter::copy(other);
		}
		void copyValue(const CharMap& other)
		{
			MapBaseParameter::copy(other);
		}
	};
	class FixedCharLine : public LineBaseParameter // FESTKENNLINIE
	{
	public:
		FixedCharLine(int lineIndex, int &lineOrder, std::string name, int size_x = 0);
		FixedCharLine(const FixedCharLine& other) : LineBaseParameter(other) {};
		void copy(const FixedCharLine& other)
		{
			if (this->type != other.type)
				return;
			LineBaseParameter::copy(other);
		}
		void copyValue(const FixedCharLine& other)
		{
			LineBaseParameter::copy(other);
		}
	};
	class FixedCharMap : public MapBaseParameter // FESTKENNFELD
	{
	public:
		FixedCharMap(int lineIndex, int &lineOrder, std::string name, int size_x = 0, int size_y = 0);
		FixedCharMap(const FixedCharMap& other) : MapBaseParameter(other) {};
		void copy(const FixedCharMap& other)
		{
			if (this->type != other.type)
				return;
			MapBaseParameter::copy(other);
		}
		void copyValue(const FixedCharMap& other)
		{
			MapBaseParameter::copy(other);
		}
	};
	class Distribution : public LineBaseParameter
	{
	public:
		Distribution(int lineIndex, int &lineOrder, std::string name, int size_x = 0);
		Distribution(const Distribution& other) : LineBaseParameter(other) {};
		void copy(const Distribution& other)
		{
			if (this->type != other.type)
				return;
			LineBaseParameter::copy(other);
		}
		void copyValue(const Distribution& other)
		{
			LineBaseParameter::copy(other);
		}
	};
	class GroupCharLine : public LineBaseParameter
	{
	public:
		GroupCharLine(int lineIndex, int &lineOrder, std::string name, int size_x = 0);
		//Distrubution* dist_x = nullptr;
		std::string dist_x = "";
		GroupCharLine(const GroupCharLine& other) : LineBaseParameter(other), dist_x(other.dist_x) {};
		void copy(const GroupCharLine& other)
		{
			if (this->type != other.type)
				return;
			this->dist_x = other.dist_x;
			LineBaseParameter::copy(other);
		}
		void copyValue(const GroupCharLine& other)
		{
			LineBaseParameter::copy(other);
		}
		
	};
	class GroupCharMap : public MapBaseParameter
	{
	public:
		GroupCharMap(int lineIndex, int &lineOrder, std::string name, int size_x = 0, int size_y = 0);
		//Distrubution* dist_x = nullptr;
		std::string dist_x = "";
		//Distrubution* dist_y = nullptr;
		std::string dist_y = "";
		GroupCharMap(const GroupCharMap& other) : MapBaseParameter(other), dist_x(other.dist_x), dist_y(other.dist_y) {};
		void copy(const GroupCharMap& other)
		{
			if (this->type != other.type)
				return;
			this->dist_x = other.dist_x;
			this->dist_y = other.dist_y;
			MapBaseParameter::copy(other);
		}
		void copyValue(const GroupCharMap& other)
		{
			MapBaseParameter::copy(other);
		}
		
	};

	std::vector<std::string> stripLine(std::string lineRaw);
	int countDecimalPlaces(const std::string& numberStr);
	std::string toFixed(double value, int precision);

	class Manager
	{
	private:
		GIT* git = nullptr;
		std::string gitName = "";
		std::string gitEmail = "";

		bool isOpened = false;

		std::vector<std::string> lineHistory;
		int currentIndex = 0;
		int currentOrder = 0;
		Element* pCurrentElement = nullptr;

		std::vector<Element*> elements;
		std::vector<ModuleHeader*> moduleHeaders;
		std::vector<Comment*> comments;
		std::vector<Functions*> functions;
		std::vector<Unknown*> unknowns;
		std::vector<Format*> formats;
		std::vector<VariantCoding*> variantCodings;
		std::vector<BaseParameter*> parameters;

		std::string fPath = "";
		std::fstream file;
		
		std::map<std::string, Element*> elementIndex;
		

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


		void putElement(Element* element);

		int calcEndIndex(Element* element);

		
		
	public:
		Manager();
		void open(std::string _fPath);		
		void openWithRepo(std::string _fPath, std::string gitName, std::string gitEmail);
		void openWithRepo(std::string _fPath) { return openWithRepo(_fPath, gitName, gitEmail); };
		
		void createDCM();
		void parse();

		struct EditHistory
		{
			std::chrono::system_clock::time_point date;
			std::string message;
			std::string id;
			// Content from GIT::DiffHunk or std::vector<std::string> ?
			EditHistory() {};
			EditHistory(std::chrono::system_clock::time_point date, std::string message, std::string id)
				: date(date), message(message), id(id) {};
		};

		std::vector<BaseParameter*> collectAllTypeParameters();
		std::vector<Distribution*> collectAxispoint();
		std::vector<Array*> collectValueBlock();
		std::vector<Matrix*> collectValueMatrix();
		std::vector<Parameter*> collectValue();
		std::vector<Parameter*> collectValueBoolean();
		std::vector<LineBaseParameter*> collectCurve();
		std::vector<MapBaseParameter*> collectMap();

		std::string rebuildUnknown(Unknown* unknown);
		std::string rebuildFunctions(Functions* functions);
		std::string rebuildComment(Comment* comment);
		std::string rebuildVariantCoding(VariantCoding* variantCoding);
		std::string rebuildModuleHeader(ModuleHeader* moduleHeader);
		std::string rebuildFormat(Format* format);
		std::string rebuildParameter(Parameter* parameter);
		std::string rebuildBoolean(Boolean* boolean);
		std::string rebuildArray(Array* arr);
		std::string rebuildMatrix(Matrix* matrix);
		std::string rebuildLineBaseParameter(LineBaseParameter* line);
		std::string rebuildMapBaseParameter(MapBaseParameter* map);
		std::string rebuildDistribution(Distribution* dist);

		std::string rebuildElement(Element* element);
		std::string rebuild();
		void saveAsDCM(std::string fname);
		
		void clear();
		std::string getFileName() { return fPath; };

		std::vector<Element*> getElements();
		Element* findElement(std::string name, bool exactmatch=false);
		std::vector<Element*> findElements(std::string name, bool exactmatch = false);
		std::string getRawString();
		std::vector<BaseParameter*> findBaseParameters(std::string name, bool exactmatch = false);
		
		std::vector<EditHistory> getEditHistoryList();
		std::vector<std::string> getRevisionList();

		std::string getContentsAtHistory(EditHistory editHistory);
		std::string getContentsAtHistory(std::string editHistory_id);

		std::string getContentsAtRevision(std::string revision);

		//void forkBranch();
		std::vector<std::pair<bool, std::pair<BaseParameter*, BaseParameter*>>> compareWith(std::vector<BaseParameter*> &otherBaseParameters);
		void copyWith(std::vector<BaseParameter*>& otherBaseParameters);
		std::vector<std::pair<DCM::BaseParameter*, DCM::BaseParameter*>> pairBaseParametersWith(std::vector<BaseParameter*>& otherBaseParameters);
		static bool test();

		// Test Code
		static bool typeEnumTest();
		static bool stripQuotationTest();		
		static bool rebuildUnknownTest();
		static bool rebuildFunctionsTest();
		static bool rebuildCommentTest();
		static bool rebuildFormatTest();
		static bool rebuildParameterTest();
		static bool rebuildArrayTest();
		static bool rebuildMatrixTest();
		static bool rebuildLineBaseParameterTest();
		static bool rebuildMapBaseParameterTest();
		static bool rebuildDistributionTest();

		static bool parseDCM1Test();
		static bool parseDCM2Test();

		//static bool createDCMTest();
	};
};


