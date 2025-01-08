#pragma once

#include "LineIndex.h"

#include <string>
#include <vector>

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

	class Element
	{
	public:
		LineIndex* lineIndex = nullptr;
		int type = -1;
		~Element();
	protected:
		Element(int type, Element* prev);
		Element(const Element& other);
		Element& operator=(const Element& other);
	};
	class Unknown : public Element
	{
	public:
		std::string text;

		Unknown(std::string text, Element* prev = nullptr);
		Unknown(const Unknown& other);
		Unknown& operator=(const Unknown& other);
		void copy(const Unknown& other);
	};
	class Comment : public Element
	{
	public:
		std::string text;

		Comment(std::string text, Element* prev = nullptr);
		Comment(const Comment& other);
		Comment& operator=(const Comment& other);
		void copy(const Comment& other);
	};
	class Format : public Element
	{
	public:
		std::string version;

		Format(std::string version, Element* prev = nullptr);
		Format(const Format& other);
		Format& operator=(const Format& other);
		void copy(const Format& other);
	};
	class Function
	{
	public:
		std::string name;
		std::string version;
		std::string longname;

		Function(std::string name, std::string version, std::string longname);
		Function(const Function& other);
		Function& operator=(const Function& other);
		void copy(const Function& other);
	};
	class Functions : public Element
	{
	public:
		std::vector<Function> functions;

		Functions(Element* prev = nullptr);
		Functions(const Functions& other);
		Functions& operator=(const Functions& other);
		void copy(const Functions& other);
	};
	class Variant
	{
	public:
		std::string name;
		std::vector<std::string> values;

		Variant(std::string name);
		Variant(const Variant& other);
		Variant& operator=(const Variant& other);
		void copy(const Variant& other);
	};
	class VariantCoding : public Element
	{
	public:
		std::vector<Variant> variants;

		VariantCoding(Element* prev = nullptr);
		VariantCoding(const VariantCoding& other);
		VariantCoding& operator=(const VariantCoding& other);
		void copy(const VariantCoding& other);
	};
	class ModuleHeader : public Element
	{
	public:
		std::string name;
		std::vector<std::string> texts;

		ModuleHeader(std::string name, Element* prev = nullptr);
		ModuleHeader(const ModuleHeader& other);
		ModuleHeader& operator=(const ModuleHeader& other);
		void copy(const ModuleHeader& other);
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

		void copy(const BaseParameter& other);

	protected:
		BaseParameter(int type, std::string name, Element* prev);
		BaseParameter(const BaseParameter& other);
		BaseParameter& operator=(const BaseParameter& other);


	};
	class ArrayBaseParameter : public BaseParameter
	{
	public:
		int size_x;
		int size_y;
		//std::vector<double> values;
		std::vector<double> values;
		std::vector<int> dec_values;

		void copy(const ArrayBaseParameter& other);
		void copyValue(const ArrayBaseParameter& other);
	protected:
		ArrayBaseParameter(int type, std::string name, int size_x, int size_y, Element* prev);
		ArrayBaseParameter(const ArrayBaseParameter& other);
		ArrayBaseParameter& operator=(const ArrayBaseParameter& other);


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

		void copy(const LineBaseParameter& other);
		void copyValue(const LineBaseParameter& other);

	protected:
		LineBaseParameter(int type, std::string name, int size_x, Element* prev);
		LineBaseParameter(const LineBaseParameter& other);
		LineBaseParameter& operator=(const LineBaseParameter& other);

	};
	class MapBaseParameter : public LineBaseParameter
	{
	public:
		int size_y;
		std::string unit_y = "";
		std::vector<double> point_y;
		std::vector<int> dec_point_y;

		void copy(const MapBaseParameter& other);
		void copyValue(const MapBaseParameter& other);

	protected:
		MapBaseParameter(int type, std::string name, int size_x, int size_y, Element* prev);
		MapBaseParameter(const MapBaseParameter& other);
		MapBaseParameter& operator=(const MapBaseParameter& other);

	};
	class Parameter : public BaseParameter // FESTWERT
	{
	public:
		double value;
		int dec_value;

		Parameter(std::string name, Element* prev = nullptr);
		Parameter(const Parameter& other);
		Parameter& operator=(const Parameter& other);
		void copy(const Parameter& other);
		void copyValue(const Parameter& other);
	};
	class Boolean : public BaseParameter // FESTWERT
	{
	public:
		std::string text;

		Boolean(std::string name, Element* prev = nullptr);
		Boolean(const Boolean& other);
		Boolean(const Parameter& other);
		Boolean& operator=(const Boolean& other);
		void copy(const Boolean& other);
		void copyValue(const Boolean& other);
	};
	class Array : public ArrayBaseParameter // FESTWERTEBLOCK
	{
	public:
		Array(std::string name, Element* prev = nullptr);
		Array(const Array& other);
		Array& operator=(const Array& other);
		void copy(const Array& other);
		void copyValue(const Array& other);
	};
	class Matrix : public ArrayBaseParameter // FESTWERTEBLOCK
	{
	public:
		Matrix(std::string name, Element* prev = nullptr);
		Matrix(const Matrix& other);
		Matrix& operator=(const Matrix& other);
		void copy(const Matrix& other);
		void copyValue(const Matrix& other);
	};
	class CharLine : public LineBaseParameter // KENNLINIE
	{
	public:
		CharLine(std::string name, Element* prev = nullptr);
		CharLine(const CharLine& other);
		CharLine& operator=(const CharLine& other);
		void copy(const CharLine& other);
		void copyValue(const CharLine& other);
	};
	class CharMap : public MapBaseParameter // KENNFELD
	{
	public:
		CharMap(std::string name, Element* prev = nullptr);
		CharMap(const CharMap& other);
		CharMap& operator=(const CharMap& other);
		void copy(const CharMap& other);
		void copyValue(const CharMap& other);
	};
	class FixedCharLine : public LineBaseParameter // FESTKENNLINIE
	{
	public:
		FixedCharLine(std::string name, Element* prev = nullptr);
		FixedCharLine(const FixedCharLine& other);
		FixedCharLine& operator=(const FixedCharLine& other);
		void copy(const FixedCharLine& other);
		void copyValue(const FixedCharLine& other);
	};
	class FixedCharMap : public MapBaseParameter // FESTKENNFELD
	{
	public:
		FixedCharMap(std::string name, Element* prev = nullptr);
		FixedCharMap(const FixedCharMap& other);
		FixedCharMap& operator=(const FixedCharMap& other);
		void copy(const FixedCharMap& other);
		void copyValue(const FixedCharMap& other);
	};
	class Distribution : public LineBaseParameter
	{
	public:
		Distribution(std::string name, Element* prev = nullptr);
		Distribution(const Distribution& other);
		Distribution& operator=(const Distribution& other);
		void copy(const Distribution& other);
		void copyValue(const Distribution& other);
	};
	class GroupCharLine : public LineBaseParameter
	{
	public:
		//Distrubution* dist_x = nullptr;
		std::string dist_x = "";

		GroupCharLine(std::string name, Element* prev = nullptr);
		GroupCharLine(const GroupCharLine& other);
		GroupCharLine& operator=(const GroupCharLine& other);
		void copy(const GroupCharLine& other);
		void copyValue(const GroupCharLine& other);

	};
	class GroupCharMap : public MapBaseParameter
	{
	public:
		//Distrubution* dist_x = nullptr;
		std::string dist_x = "";
		//Distrubution* dist_y = nullptr;
		std::string dist_y = "";

		GroupCharMap(std::string name, Element* prev = nullptr);
		GroupCharMap(const GroupCharMap& other);
		GroupCharMap& operator=(const GroupCharMap& other);
		void copy(const GroupCharMap& other);
		void copyValue(const GroupCharMap& other);

	};

}

