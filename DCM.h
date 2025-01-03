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
		int lineIndex = -1;
		int lineOrder = -1;
		int endIndex = -1;
		int type = -1;
		
		Element(int lineIndex, int& lineOrder, int type) : lineIndex(lineIndex), lineOrder(lineOrder), type(type), endIndex(lineIndex)
		{
			lineOrder++;
		};
		Element(const Element& other)
			: lineIndex(other.lineIndex), lineOrder(other.lineOrder), endIndex(other.endIndex), type(other.type) {};
		Element& operator=(const Element& other)
		{
			if (this != &other)
			{
				lineIndex = other.lineIndex;
				lineOrder = other.lineOrder;
				endIndex = other.endIndex;
				type = other.type;
			}
			return *this;
		}
	};
	class Unknown : public Element
	{
	public:
		std::string text;

		Unknown(int lineIndex, int &lineOrder, std::string text) 
			: Element(lineIndex, lineOrder, TYPE::UNKNOWN), text(text) {};
		Unknown(const Unknown& other)
			: Element(other), text(other.text) {};
		Unknown& operator=(const Unknown& other)
		{
			if (this != &other)
			{
				text = other.text;
			}
			return *this;
		}
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
		std::string text;

		Comment(int lineIndex, int &lineOrder, std::string text)
			: Element(lineIndex, lineOrder, TYPE::COMMENT), text(text) {};
		Comment(const Comment& other)
			: Element(other), text(other.text) {};
		Comment& operator=(const Comment& other)
		{
			if (this != &other)
			{
				text = other.text;
			}
			return *this;
		}
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
		std::string version;
		
		Format(int lineIndex, int &lineOrder, std::string version)
			: Element(lineIndex, lineOrder, TYPE::FORMAT), version(version) {};
		Format(const Format& other) 
			: Element(other), version(other.version) {};
		Format& operator=(const Format& other)
		{
			if (this != &other)
			{
				version = other.version;
			}
			return *this;
		}
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
		std::string name;
		std::string version;
		std::string longname;

		Function(std::string name, std::string version, std::string longname)
			: name(name), version(version), longname(longname) {};
		Function(const Function& other) 
			: name(other.name), version(other.version), longname(other.longname) {};
		Function& operator=(const Function& other)
		{
			if (this != &other)
			{
				name = other.name;
				version = other.version;
				longname = other.longname;
			}
			return *this;
		}
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
		std::vector<Function> functions;
		
		Functions(int lineIndex, int &lineOrder)
			: Element(lineIndex, lineOrder, TYPE::FUNCTIONS) {};
		Functions(const Functions& other)
			: Element(other), functions(other.functions) {};
		Functions& operator=(const Functions& other)
		{
			if (this != &other)
			{
				functions = other.functions;				
			}
			return *this;
		}
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
		std::string name;
		std::vector<std::string> values;
		
		Variant(std::string name)
			: name(name) {};
		Variant(const Variant& other)
			: name(other.name), values(other.values) {};
		Variant& operator=(const Variant& other)
		{
			if (this != &other)
			{
				name = other.name;
				values = other.values;
			}
			return *this;
		}
		void copy(const Variant& other)
		{
			this->name = other.name;
			this->values = other.values;
		}
	};
	class VariantCoding : public Element
	{
	public:
		std::vector<Variant> variants;
		
		VariantCoding(int lineIndex, int &lineOrder)
			: Element(lineIndex, lineOrder, TYPE::VARIANTCODING) {};
		VariantCoding(const VariantCoding& other)
			: Element(other), variants(other.variants) {};
		VariantCoding& operator=(const VariantCoding& other)
		{
			if (this != &other)
			{
				variants = other.variants;				
			}
			return *this;
		}
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
		std::string name;
		std::vector<std::string> texts;

		ModuleHeader(int lineIndex, int &lineOrder, std::string name)
			: Element(lineIndex, lineOrder, TYPE::MODULEHEADER), name(name) {};
		ModuleHeader(const ModuleHeader& other)
			: Element(other), name(other.name), texts(other.texts) {};
		ModuleHeader& operator=(const ModuleHeader& other)
		{
			if (this != &other)
			{
				name = other.name;
				texts = other.texts;
			}
			return *this;
		}
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
		std::string name;
		std::string langname = "";
		std::string displayname = "";
		//Variant* variant = nullptr;
		//std::pair<std::string, std::string> variant;
		std::string variant = "";
		//Function* function = nullptr;
		std::string function;
		std::string unit = "";

		BaseParameter(int lineIndex, int &lineOrder, int type, std::string name)
			: Element(lineIndex, lineOrder, type), name(name) {};
		BaseParameter(const BaseParameter& other)
			:Element(other), name(other.name), langname(other.langname), displayname(other.displayname),
			variant(other.variant), function(other.function), unit(other.unit) {};
		BaseParameter& operator=(const BaseParameter& other)
		{
			if (this != &other)
			{
				name = other.name;
				langname = other.langname;
				displayname = other.displayname;
				variant = other.variant;
				function = other.function;
				unit = other.unit;				
			}
			return *this;
		}
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
		int size_x;
		int size_y;
		//std::vector<double> values;
		std::vector<double> values;
		std::vector<int> dec_values;

		ArrayBaseParameter(int lineIndex, int &lineOrder, int type, std::string name, int size_x, int size_y)
			: BaseParameter(lineIndex, lineOrder, type, name), size_x(size_x), size_y(size_y) {};
		ArrayBaseParameter(const ArrayBaseParameter& other)
			: BaseParameter(other), size_x(other.size_x), size_y(other.size_y), values(other.values), dec_values(other.dec_values) {};
		ArrayBaseParameter& operator=(const ArrayBaseParameter& other)
		{
			if (this != &other)
			{
				size_x = other.size_x;
				size_y = other.size_y;
				values = other.values;
				dec_values = other.dec_values;				
			}
			return *this;
		}
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
		int size_x;
		std::string unit_x = "";
		std::vector<double> point_x;
		std::vector<double> values;
		std::vector<int> dec_point_x;
		std::vector<int> dec_values;

		LineBaseParameter(int lineIndex, int& lineOrder, int type, std::string name, int size_x)
			: BaseParameter(lineIndex, lineOrder, type, name), size_x(size_x) {};
		LineBaseParameter(const LineBaseParameter& other)
			: BaseParameter(other), size_x(other.size_x), unit_x(other.unit_x), point_x(other.point_x), values(other.values),
			dec_point_x(other.dec_point_x), dec_values(other.dec_values) {};
		LineBaseParameter& operator=(const LineBaseParameter& other)
		{
			if (this != &other)
			{
				size_x = other.size_x;
				unit_x = other.unit_x;
				point_x = other.point_x;
				values = other.values;
				dec_point_x = other.dec_point_x;
				dec_values = other.dec_values;
			}
			return *this;
		}
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
		int size_y;
		std::string unit_y = "";
		std::vector<double> point_y;
		std::vector<int> dec_point_y;

		MapBaseParameter(int lineIndex, int &lineOrder, int type, std::string name, int size_x, int size_y)
			: LineBaseParameter(lineIndex, lineOrder, type, name, size_x), size_y(size_y) {};
		MapBaseParameter(const MapBaseParameter& other)
			: LineBaseParameter(other), size_y(other.size_y), unit_y(other.unit_y), point_y(other.point_y), dec_point_y(other.dec_point_y) {};
		MapBaseParameter& operator=(const MapBaseParameter& other)
		{
			if (this != &other)
			{
				size_y = other.size_y;
				unit_y = other.unit_y;
				point_y = other.point_y;
				dec_point_y = other.dec_point_y;				
			}
			return *this;
		}
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
		double value;
		int dec_value;

		Parameter(int lineIndex, int &lineOrder, std::string name)
			: BaseParameter(lineIndex, lineOrder, TYPE::PARAMETER, name), value(0), dec_value(0) {};
		Parameter(const Parameter& other) : BaseParameter(other), value(other.value), dec_value(other.dec_value) {};
		Parameter& operator=(const Parameter& other)
		{
			if (this != &other)
			{
				value = other.value;
				dec_value = other.dec_value;				
			}
			return *this;
		}
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
		std::string text;

		Boolean(int lineIndex, int &lineOrder, std::string name)
			: BaseParameter(lineIndex, lineOrder, TYPE::BOOLEAN, name), text("") {};
		Boolean(const Boolean& other) : BaseParameter(other), text(other.text) {};
		Boolean& operator=(const Boolean& other)
		{
			if (this != &other)
			{
				text = other.text;				
			}
			return *this;
		}
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
		Array(int lineIndex, int &lineOrder, std::string name, int size_x = 0)
			: ArrayBaseParameter(lineIndex, lineOrder, TYPE::ARRAY, name, size_x, 0) {};
		Array(const Array& other) : ArrayBaseParameter(other) {};
		Array& operator=(const Array& other)
		{
			if (this != &other)
			{
				;
			}
			return *this;
		}
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
		Matrix(int lineIndex, int &lineOrder, std::string name, int size_x = 0, int size_y = 0)
			: ArrayBaseParameter(lineIndex, lineOrder, TYPE::MATRIX, name, size_x, size_y) {};
		Matrix(const Matrix& other) : ArrayBaseParameter(other) {};
		Matrix& operator=(const Matrix& other)
		{
			if (this != &other)
			{
				;
			}
			return *this;
		}
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
		CharLine(int lineIndex, int &lineOrder, std::string name, int size_x = 0)
			: LineBaseParameter(lineIndex, lineOrder, TYPE::CHARLINE, name, size_x) {};
		CharLine(const CharLine& other) : LineBaseParameter(other) {};
		CharLine& operator=(const CharLine& other)
		{
			if (this != &other)
			{
				;
			}
			return *this;
		}
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
		CharMap(int lineIndex, int &lineOrder, std::string name, int size_x = 0, int size_y = 0)
			: MapBaseParameter(lineIndex, lineOrder, TYPE::CHARMAP, name, size_x, size_y) {};
		CharMap(const CharMap& other) : MapBaseParameter(other) {};
		CharMap& operator=(const CharMap& other)
		{
			if (this != &other)
			{
				;
			}
			return *this;
		}
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
		FixedCharLine(int lineIndex, int &lineOrder, std::string name, int size_x = 0)
			: LineBaseParameter(lineIndex, lineOrder, TYPE::FIXEDCHARLINE, name, size_x) {};
		FixedCharLine(const FixedCharLine& other) : LineBaseParameter(other) {};
		FixedCharLine& operator=(const FixedCharLine& other)
		{
			if (this != &other)
			{
				;
			}
			return *this;
		}
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
		FixedCharMap(int lineIndex, int &lineOrder, std::string name, int size_x = 0, int size_y = 0)
			: MapBaseParameter(lineIndex, lineOrder, TYPE::FIXEDCHARMAP, name, size_x, size_y) {};
		FixedCharMap(const FixedCharMap& other) : MapBaseParameter(other) {};
		FixedCharMap& operator=(const FixedCharMap& other)
		{
			if (this != &other)
			{
				;
			}
			return *this;
		}
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
		Distribution(int lineIndex, int &lineOrder, std::string name, int size_x = 0)
			: LineBaseParameter(lineIndex, lineOrder, TYPE::DISTRIBUTION, name, size_x) {};
		Distribution(const Distribution& other) : LineBaseParameter(other) {};
		Distribution& operator=(const Distribution& other)
		{
			if (this != &other)
			{
				;
			}
			return *this;
		}
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
		//Distrubution* dist_x = nullptr;
		std::string dist_x = "";

		GroupCharLine(int lineIndex, int &lineOrder, std::string name, int size_x = 0)
			: LineBaseParameter(lineIndex, lineOrder, TYPE::GROUPCHARLINE, name, size_x) {};
		GroupCharLine(const GroupCharLine& other) : LineBaseParameter(other), dist_x(other.dist_x) {};
		GroupCharLine& operator=(const GroupCharLine& other)
		{
			if (this != &other)
			{
				dist_x = other.dist_x;
			}
			return *this;
		}
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
		//Distrubution* dist_x = nullptr;
		std::string dist_x = "";
		//Distrubution* dist_y = nullptr;
		std::string dist_y = "";
		
		GroupCharMap(int lineIndex, int &lineOrder, std::string name, int size_x = 0, int size_y = 0)
			: MapBaseParameter(lineIndex, lineOrder, TYPE::GROUPCHARMAP, name, size_x, size_y) {};
		GroupCharMap(const GroupCharMap& other) : MapBaseParameter(other), dist_x(other.dist_x), dist_y(other.dist_y) {};
		GroupCharMap& operator=(const GroupCharMap& other)
		{
			if (this != &other)
			{
				dist_x = other.dist_x;
				dist_y = other.dist_y;
			}
			return *this;
		}
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


