#pragma once

#include <fstream>

#include <string>
#include <vector>
#include <map>
#include <list>

#include "GIT.h" // From github.com/kjw8118/GITwrap.git

namespace DCM
{
	class LineIndex
	{
	private:
		LineIndex* prev = nullptr;
		LineIndex* next = nullptr;
		int index;
		int length;
		int order;

		void updateIndex()
		{
			if (prev != nullptr)
				index = prev->getEndIndex() + 1;
			else
				index = 0;
		}
		void updateOrder()
		{
			if (prev != nullptr)
				order = prev->getOrder() + 1;
			else
				order = 0;
		}
		void update()
		{
			updateIndex();
			updateOrder();

			if (index < 0 || length < 0) // 아직 정보가 없는 상태
				return;

			//int curIndex = index + length;
			//int curOrder = order + 1;
			auto itr = next;
			while (itr != nullptr)
			{
				//itr->index = curIndex;
				//itr->order = curOrder;
				//curIndex += itr->length;
				//curOrder += 1;
				itr->updateIndex();
				itr->updateOrder();

				itr = itr->next;
			}
		}
	public:
		LineIndex(int index = 0, int length = 1)
			: index(index), length(length), order(0), prev(nullptr), next(nullptr) {};
		LineIndex(LineIndex* _prev, LineIndex* next = nullptr)
			: prev(_prev), next(next)
		{
			updateIndex();
			updateOrder();

			if (next != nullptr)
#ifndef max
				length = std::max(1, next->getIndex() - index);
#else
				length = max(1, next->getIndex() - index);
#endif
			else
				length = 1;

			attachPrev(prev);
			attachNext(next);
		}
		int getIndex()
		{
			updateIndex();
			return index;
		};
		int getEndIndex()
		{
			return getIndex() + length - 1;
		}
		int getOrder()
		{
			updateOrder();
			return order;
		}
		void putLength(int len)
		{
			length = len;
			update();
		}
		void putEndIndex(int endIndex)
		{
			length = endIndex - getIndex() + 1;
			update();
		}
		/* attch와 detach는 하나의 연결리스트만 존재한다고 가정하고 작동한다 */
		/* 현재의 prev를 until까지 떼어내고 없앤다 즉 until 이전까지는 없애지 않는다 왜냐하면 새로운 prev가 될 수 있으므로 */
		void detachPrev(LineIndex* until = nullptr)
		{
			if (prev != nullptr)
			{
				std::list<LineIndex*> trashBin;
				auto itr = prev;
				while (itr != until && itr != nullptr && itr != this)
				{
					trashBin.push_front(itr);
					itr = itr->prev;
				}
				for (auto trash : trashBin)
					delete trash;
				prev = nullptr;

				updateIndex();
				updateOrder();

				//update();
			}
		}
		/* 현재의 prev를 detach하고 새로운 _prev를 붙인다 */
		void attachPrev(LineIndex* _prev)
		{
			if (_prev == prev)
				return;

			detachPrev(_prev);
			prev = _prev;
			if (prev != nullptr)
			{
				prev->attachNext(this);
				update();
			}
		}
		void detachNext(LineIndex* until = nullptr)
		{
			std::list<LineIndex*> trashBin;
			auto itr = next;
			while (itr != until && itr != nullptr && itr != this)
			{
				trashBin.push_front(itr);
				itr = itr->next;
			}
			for (auto trash : trashBin)
				delete trash;
			next = nullptr;
			//update();
		}
		void attachNext(LineIndex* _next)
		{
			if (_next == next)
				return;

			detachNext(_next);
			next = _next;
			if (next != nullptr)
			{
				next->attachPrev(this);
				update();
			}
		}
		/*  */
		void attach(LineIndex* _prev, LineIndex* _next)
		{
			//prev = _prev;
			//next = _next;
			attachPrev(_prev);
			attachNext(_next);
		}
		void detach(LineIndex* _prev, LineIndex* _next)
		{
			detachPrev(_prev);
			detachNext(_next);
		}

		void independent()
		{
			prev->detachNext(this); // 삭제 없이 연결만 끊기 prev->next = nullptr
			next->detachPrev(this); // 자신과 연결 안끊고 attach하면 자신이 삭제될 수 있음

			prev->attachNext(next); //
			next->attachPrev(prev); // prev->attachNext(next)에 next->attachPrev 있어서 사실상 중복

			prev = nullptr;
			next = nullptr;

			updateIndex();
			updateOrder();
		}
		void resetIndex()
		{
			if (prev == nullptr)
				index = 0;
			update();
		}
		LineIndex* findIndex(int line)
		{
			int curEndIndex = getEndIndex();
			int curIndex = getIndex();

			if (line < curIndex)
				return prev->findIndex(line);
			else if (line >= curIndex && line <= curEndIndex)
				return this;
			else
				return next->findIndex(line);
		}
		LineIndex(const LineIndex& other)
			: index(other.index), length(other.length), order(other.order), prev(other.prev), next(other.next) {};
		LineIndex& operator=(const LineIndex& other)
		{
			if (this != &other)
			{
				index = other.index;
				length = other.length;
				order = other.order;
				prev = other.prev; // 기존 연결관계를 어떻게 할지 안 정함
				next = other.next;
			}
			return *this;
		}

	};

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
		LineIndex* lineIndex = nullptr;
		int type = -1;
	
	protected:
		Element(int type, Element* prev) : type(type)
		{
			if (prev != nullptr) lineIndex = new LineIndex(prev->lineIndex);
			else lineIndex = new LineIndex(nullptr);
		};
		Element(const Element& other)
			: type(other.type), lineIndex(other.lineIndex) {};
		Element& operator=(const Element& other)
		{
			if (this != &other)
			{
				lineIndex = other.lineIndex;
				type = other.type;
			}
			return *this;
		}	
	};
	class Unknown : public Element
	{
	public:
		std::string text;

		Unknown(std::string text, Element* prev=nullptr) 
			: Element(TYPE::UNKNOWN, prev), text(text) {};
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

		Comment(std::string text, Element* prev = nullptr)
			: Element(TYPE::COMMENT, prev), text(text) {};
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
		
		Format(std::string version, Element* prev = nullptr)
			: Element(TYPE::FORMAT, prev), version(version) {};
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
		
		Functions(Element* prev = nullptr)
			: Element(TYPE::FUNCTIONS, prev) {};
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
		
		VariantCoding(Element* prev = nullptr)
			: Element(TYPE::VARIANTCODING, prev) {};
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

		ModuleHeader(std::string name, Element* prev = nullptr)
			: Element(TYPE::MODULEHEADER, prev), name(name) {};
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

	protected:
		BaseParameter(int type, std::string name, Element* prev)
			: Element(type, prev), name(name) {};
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
			
		
	};
	class ArrayBaseParameter : public BaseParameter
	{
	public:
		int size_x;
		int size_y;
		//std::vector<double> values;
		std::vector<double> values;
		std::vector<int> dec_values;

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

	protected:
		ArrayBaseParameter(int type, std::string name, int size_x, int size_y, Element* prev)
			: BaseParameter(type, name, prev), size_x(size_x), size_y(size_y) {};
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

	protected:
		LineBaseParameter(int type, std::string name, int size_x, Element* prev)
			: BaseParameter(type, name, prev), size_x(size_x) {};
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
		
	};
	class MapBaseParameter : public LineBaseParameter
	{
	public:
		int size_y;
		std::string unit_y = "";
		std::vector<double> point_y;
		std::vector<int> dec_point_y;

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

	protected:
		MapBaseParameter(int type, std::string name, int size_x, int size_y, Element* prev)
			: LineBaseParameter(type, name, size_x, prev), size_y(size_y) {};
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
		
	};
	class Parameter : public BaseParameter // FESTWERT
	{
	public:
		double value;
		int dec_value;

		Parameter(std::string name, Element* prev = nullptr)
			: BaseParameter(TYPE::PARAMETER, name, prev), value(0), dec_value(0) {};
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

		Boolean(std::string name, Element* prev = nullptr)
			: BaseParameter(TYPE::BOOLEAN, name, prev), text("") {};
		Boolean(const Boolean& other) : BaseParameter(other), text(other.text) {};
		Boolean(const Parameter& other) : BaseParameter(other), text("") { type = TYPE::BOOLEAN; };
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
		Array(std::string name, Element* prev = nullptr)
			: ArrayBaseParameter(TYPE::ARRAY, name, 0, 0, prev) {};
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
		Matrix(std::string name, Element* prev = nullptr)
			: ArrayBaseParameter(TYPE::MATRIX, name, 0, 0, prev) {};
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
		CharLine(std::string name, Element* prev = nullptr)
			: LineBaseParameter(TYPE::CHARLINE, name, 0, prev) {};
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
		CharMap(std::string name, Element* prev = nullptr)
			: MapBaseParameter(TYPE::CHARMAP, name, 0, 0, prev) {};
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
		FixedCharLine(std::string name, Element* prev = nullptr)
			: LineBaseParameter(TYPE::FIXEDCHARLINE, name, 0, prev) {};
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
		FixedCharMap(std::string name, Element* prev = nullptr)
			: MapBaseParameter(TYPE::FIXEDCHARMAP, name, 0, 0, prev) {};
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
		Distribution(std::string name, Element* prev = nullptr)
			: LineBaseParameter(TYPE::DISTRIBUTION, name, 0, prev) {};
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

		GroupCharLine(std::string name, Element* prev = nullptr)
			: LineBaseParameter(TYPE::GROUPCHARLINE, name, 0, prev) {};
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
		
		GroupCharMap(std::string name, Element* prev = nullptr)
			: MapBaseParameter(TYPE::GROUPCHARMAP, name, 0, 0, prev) {};
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
		std::string fName = "";
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
		void cloneFromRepo(std::string _remotePath, std::string _localPath, std::string _gitName, std::string _gitEmail);

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
		std::vector<GIT::DiffResult> getDiffWithCurrent();
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
		std::vector<std::string> getRawStringList() { return lineHistory; };

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


