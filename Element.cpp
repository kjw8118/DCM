#include "Element.h"



DCM::Element::~Element() 
{
	delete lineIndex; 
};

DCM::Element::Element(int type, Element* prev)
	: type(type)
{
	if (prev != nullptr) lineIndex = new LineIndex(prev->lineIndex);
	else lineIndex = new LineIndex(nullptr);
};

DCM::Element::Element(const Element& other)
	: type(other.type), lineIndex(other.lineIndex) 
{

};

DCM::Element& DCM::Element::operator=(const Element& other)
{
	if (this != &other)
	{
		lineIndex = other.lineIndex;
		type = other.type;
	}
	return *this;
}

DCM::Unknown::Unknown(std::string text, Element* prev)
	: Element(TYPE::UNKNOWN, prev), text(text) 
{

};

DCM::Unknown::Unknown(const Unknown& other)
	: Element(other), text(other.text)
{

};

DCM::Unknown& DCM::Unknown::operator=(const Unknown& other)
{
	if (this != &other)
	{
		text = other.text;
	}
	return *this;
}

void DCM::Unknown::copy(const Unknown& other)
{
	if (this->type != other.type)
		return;
	this->text = other.text;
}


DCM::Comment::Comment(std::string text, Element* prev)
	: Element(TYPE::COMMENT, prev), text(text) 
{

}

DCM::Comment::Comment(const Comment& other)
	: Element(other), text(other.text) 
{

}

DCM::Comment& DCM::Comment::operator=(const Comment& other)
{
	if (this != &other)
	{
		text = other.text;
	}
	return *this;
}

void DCM::Comment::copy(const Comment& other)
{
	if (this->type != other.type)
		return;
	this->text = other.text;
}

DCM::Format::Format(std::string version, Element* prev)
	: Element(TYPE::FORMAT, prev), version(version)
{

}

DCM::Format::Format(const Format& other)
	: Element(other), version(other.version)
{

}

DCM::Format& DCM::Format::operator=(const Format& other)
{
	if (this != &other)
	{
		version = other.version;
	}
	return *this;
}

void DCM::Format::copy(const Format& other)
{
	if (this->type != other.type)
		return;
	this->version = other.version;
}

DCM::Function::Function(std::string name, std::string version, std::string longname)
	: name(name), version(version), longname(longname) 
{

};

DCM::Function::Function(const Function& other)
	: name(other.name), version(other.version), longname(other.longname)
{

}

DCM::Function& DCM::Function::operator=(const Function& other)
{
	if (this != &other)
	{
		name = other.name;
		version = other.version;
		longname = other.longname;
	}
	return *this;
}

void DCM::Function::copy(const Function& other)
{
	this->name = other.name;
	this->version = other.version;
	this->longname = other.longname;
}

DCM::Functions::Functions(Element* prev)
	: Element(TYPE::FUNCTIONS, prev)
{

}

DCM::Functions::Functions(const Functions& other)
	: Element(other), functions(other.functions)
{

}

DCM::Functions& DCM::Functions::operator=(const Functions& other)
{
	if (this != &other)
	{
		functions = other.functions;
	}
	return *this;
}

void DCM::Functions::copy(const Functions& other)
{
	if (this->type != other.type)
		return;
	this->functions = other.functions;
}

DCM::Variant::Variant(std::string name)
	: name(name)
{

}

DCM::Variant::Variant(const Variant& other)
	: name(other.name), values(other.values)
{

}

DCM::Variant& DCM::Variant::operator=(const Variant& other)
{
	if (this != &other)
	{
		name = other.name;
		values = other.values;
	}
	return *this;
}

void DCM::Variant::copy(const Variant& other)
{
	this->name = other.name;
	this->values = other.values;
}


DCM::VariantCoding::VariantCoding(Element* prev)
	: Element(TYPE::VARIANTCODING, prev)
{

}

DCM::VariantCoding::VariantCoding(const VariantCoding& other)
	: Element(other), variants(other.variants) 
{

}

DCM::VariantCoding& DCM::VariantCoding::operator=(const VariantCoding& other)
{
	if (this != &other)
	{
		variants = other.variants;
	}
	return *this;
}

void DCM::VariantCoding::copy(const VariantCoding& other)
{
	if (this->type != other.type)
		return;
	this->variants = other.variants;
}

DCM::ModuleHeader::ModuleHeader(std::string name, Element* prev)
	: Element(TYPE::MODULEHEADER, prev), name(name) 
{

}

DCM::ModuleHeader::ModuleHeader(const ModuleHeader& other)
	: Element(other), name(other.name), texts(other.texts) 
{

}

DCM::ModuleHeader& DCM::ModuleHeader::operator=(const ModuleHeader& other)
{
	if (this != &other)
	{
		name = other.name;
		texts = other.texts;
	}
	return *this;
}

void DCM::ModuleHeader::copy(const ModuleHeader& other)
{
	if (this->type != other.type)
		return;
	this->name = other.name;
	this->texts = other.texts;
}

void DCM::BaseParameter::copy(const BaseParameter& other)
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

DCM::BaseParameter::BaseParameter(int type, std::string name, Element* prev)
	: Element(type, prev), name(name) 
{

}

DCM::BaseParameter::BaseParameter(const BaseParameter& other)
	:Element(other), name(other.name), langname(other.langname), displayname(other.displayname),
	variant(other.variant), function(other.function), unit(other.unit)
{

}

DCM::BaseParameter& DCM::BaseParameter::operator=(const BaseParameter& other)
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

void DCM::ArrayBaseParameter::copy(const ArrayBaseParameter& other)
{
	if (this->type != other.type)
		return;
	BaseParameter::copy(other);
	this->size_x = other.size_x;
	this->size_y = other.size_y;
	this->values = other.values;
	this->dec_values = other.dec_values;
}

void DCM::ArrayBaseParameter::copyValue(const ArrayBaseParameter& other)
{
	if (this->type != other.type)
		return;
	if (this->values.size() != other.values.size())
		return;
	std::copy(other.values.begin(), other.values.end(), this->values.begin());
}

DCM::ArrayBaseParameter::ArrayBaseParameter(int type, std::string name, int size_x, int size_y, Element* prev)
	: BaseParameter(type, name, prev), size_x(size_x), size_y(size_y) 
{

}

DCM::ArrayBaseParameter::ArrayBaseParameter(const ArrayBaseParameter& other)
	: BaseParameter(other), size_x(other.size_x), size_y(other.size_y), values(other.values), dec_values(other.dec_values) 
{

}

DCM::ArrayBaseParameter& DCM::ArrayBaseParameter::operator=(const ArrayBaseParameter& other)
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

void DCM::LineBaseParameter::copy(const LineBaseParameter& other)
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

void DCM::LineBaseParameter::copyValue(const LineBaseParameter& other)
{
	if (this->type != other.type)
		return;
	if (this->values.size() != other.values.size() || this->point_x.size() != other.point_x.size())
		return;
	std::copy(other.point_x.begin(), other.point_x.end(), this->point_x.begin());
	std::copy(other.values.begin(), other.values.end(), this->values.begin());
}

DCM::LineBaseParameter::LineBaseParameter(int type, std::string name, int size_x, Element* prev)
	: BaseParameter(type, name, prev), size_x(size_x)
{

}

DCM::LineBaseParameter::LineBaseParameter(const LineBaseParameter& other)
	: BaseParameter(other), size_x(other.size_x), unit_x(other.unit_x), point_x(other.point_x), values(other.values),
	dec_point_x(other.dec_point_x), dec_values(other.dec_values) 
{

}

DCM::LineBaseParameter& DCM::LineBaseParameter::operator=(const LineBaseParameter& other)
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

void DCM::MapBaseParameter::copy(const MapBaseParameter& other)
{
	if (this->type != other.type)
		return;
	LineBaseParameter::copy(other);
	this->size_y = other.size_y;
	this->unit_y = other.unit_y;
	this->point_y = other.point_y;
	this->dec_point_y = other.dec_point_y;
}

void DCM::MapBaseParameter::copyValue(const MapBaseParameter& other)
{
	if (this->type != other.type)
		return;
	LineBaseParameter::copyValue(other);
	if (this->point_y.size() != other.point_y.size())
		return;
	std::copy(other.point_y.begin(), other.point_y.end(), this->point_y.begin());
}

DCM::MapBaseParameter::MapBaseParameter(int type, std::string name, int size_x, int size_y, Element* prev)
	: LineBaseParameter(type, name, size_x, prev), size_y(size_y) 
{

}

DCM::MapBaseParameter::MapBaseParameter(const MapBaseParameter& other)
	: LineBaseParameter(other), size_y(other.size_y), unit_y(other.unit_y), point_y(other.point_y), dec_point_y(other.dec_point_y) 
{

}

DCM::MapBaseParameter& DCM::MapBaseParameter::operator=(const MapBaseParameter& other)
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

DCM::Parameter::Parameter(std::string name, Element* prev)
	: BaseParameter(TYPE::PARAMETER, name, prev), value(0), dec_value(0) 
{

}

DCM::Parameter::Parameter(const Parameter& other)
	: BaseParameter(other), value(other.value), dec_value(other.dec_value) 
{

}
DCM::Parameter& DCM::Parameter::operator=(const Parameter& other)
{
	if (this != &other)
	{
		value = other.value;
		dec_value = other.dec_value;
	}
	return *this;
}

void DCM::Parameter::copy(const Parameter& other)
{
	if (this->type != other.type)
		return;
	BaseParameter::copy(other);
	this->value = other.value;
	this->dec_value = other.dec_value;
}

void DCM::Parameter::copyValue(const Parameter& other)
{
	if (this->type != other.type)
		return;
	this->value = other.value;
}

DCM::Boolean::Boolean(std::string name, Element* prev)
	: BaseParameter(TYPE::BOOLEAN, name, prev), text("")
{

}

DCM::Boolean::Boolean(const Boolean& other) 
	: BaseParameter(other), text(other.text)
{

}

DCM::Boolean::Boolean(const Parameter& other)
	: BaseParameter(other), text("") 
{
	type = TYPE::BOOLEAN; 
}

DCM::Boolean& DCM::Boolean::operator=(const Boolean& other)
{
	if (this != &other)
	{
		text = other.text;
	}
	return *this;
}

void DCM::Boolean::copy(const Boolean& other)
{
	if (this->type != other.type)
		return;
	BaseParameter::copy(other);
	this->text = other.text;
}

void DCM::Boolean::copyValue(const Boolean& other)
{
	if (this->type != other.type)
		return;
	this->text = other.text;
}

DCM::Array::Array(std::string name, Element* prev)
	: ArrayBaseParameter(TYPE::ARRAY, name, 0, 0, prev) 
{

}

DCM::Array::Array(const Array& other)
	: ArrayBaseParameter(other) 
{

}

DCM::Array& DCM::Array::operator=(const Array& other)
{
	if (this != &other)
	{
		;
	}
	return *this;
}

void DCM::Array::copy(const Array& other)
{
	if (this->type != other.type)
		return;
	ArrayBaseParameter::copy(other);
}

void DCM::Array::copyValue(const Array& other)
{
	ArrayBaseParameter::copy(other);
}

DCM::Matrix::Matrix(std::string name, Element* prev)
	: ArrayBaseParameter(TYPE::MATRIX, name, 0, 0, prev)
{

}

DCM::Matrix::Matrix(const Matrix& other) : ArrayBaseParameter(other) 
{

}

DCM::Matrix& DCM::Matrix::operator=(const Matrix& other)
{
	if (this != &other)
	{
		;
	}
	return *this;
}

void DCM::Matrix::copy(const Matrix& other)
{
	if (this->type != other.type)
		return;
	ArrayBaseParameter::copy(other);
}

void DCM::Matrix::copyValue(const Matrix& other)
{
	ArrayBaseParameter::copy(other);
}

DCM::CharLine::CharLine(std::string name, Element* prev)
	: LineBaseParameter(TYPE::CHARLINE, name, 0, prev) 
{

}

DCM::CharLine::CharLine(const CharLine& other) : LineBaseParameter(other) 
{

}

DCM::CharLine& DCM::CharLine::operator=(const CharLine& other)
{
	if (this != &other)
	{
		;
	}
	return *this;
}

void DCM::CharLine::copy(const CharLine& other)
{
	if (this->type != other.type)
		return;
	LineBaseParameter::copy(other);
}

void DCM::CharLine::copyValue(const CharLine& other)
{
	LineBaseParameter::copy(other);
}

DCM::CharMap::CharMap(std::string name, Element* prev)
	: MapBaseParameter(TYPE::CHARMAP, name, 0, 0, prev)
{

}

DCM::CharMap::CharMap(const CharMap& other) 
	: MapBaseParameter(other) 
{

}

DCM::CharMap& DCM::CharMap::operator=(const CharMap& other)
{
	if (this != &other)
	{
		;
	}
	return *this;
}

void DCM::CharMap::copy(const CharMap& other)
{
	if (this->type != other.type)
		return;
	MapBaseParameter::copy(other);
}

void DCM::CharMap::copyValue(const CharMap& other)
{
	MapBaseParameter::copy(other);
}

DCM::FixedCharLine::FixedCharLine(std::string name, Element* prev)
	: LineBaseParameter(TYPE::FIXEDCHARLINE, name, 0, prev) 
{

}
DCM::FixedCharLine::FixedCharLine(const FixedCharLine& other) 
	: LineBaseParameter(other) 
{

}

DCM::FixedCharLine& DCM::FixedCharLine::operator=(const FixedCharLine& other)
{
	if (this != &other)
	{
		;
	}
	return *this;
}

void DCM::FixedCharLine::copy(const FixedCharLine& other)
{
	if (this->type != other.type)
		return;
	LineBaseParameter::copy(other);
}

void DCM::FixedCharLine::copyValue(const FixedCharLine& other)
{
	LineBaseParameter::copy(other);
}

DCM::FixedCharMap::FixedCharMap(std::string name, Element* prev)
	: MapBaseParameter(TYPE::FIXEDCHARMAP, name, 0, 0, prev) 
{

}

DCM::FixedCharMap::FixedCharMap(const FixedCharMap& other) 
	: MapBaseParameter(other) 
{

}

DCM::FixedCharMap& DCM::FixedCharMap::operator=(const FixedCharMap& other)
{
	if (this != &other)
	{
		;
	}
	return *this;
}

void DCM::FixedCharMap::copy(const FixedCharMap& other)
{
	if (this->type != other.type)
		return;
	MapBaseParameter::copy(other);
}

void DCM::FixedCharMap::copyValue(const FixedCharMap& other)
{
	MapBaseParameter::copy(other);
}

DCM::Distribution::Distribution(std::string name, Element* prev)
	: LineBaseParameter(TYPE::DISTRIBUTION, name, 0, prev) 
{

}

DCM::Distribution::Distribution(const Distribution& other)
	: LineBaseParameter(other) 
{

}

DCM::Distribution& DCM::Distribution::operator=(const Distribution& other)
{
	if (this != &other)
	{
		;
	}
	return *this;
}

void DCM::Distribution::copy(const Distribution& other)
{
	if (this->type != other.type)
		return;
	LineBaseParameter::copy(other);
}

void DCM::Distribution::copyValue(const Distribution& other)
{
	LineBaseParameter::copy(other);
}

DCM::GroupCharLine::GroupCharLine(std::string name, Element* prev)
	: LineBaseParameter(TYPE::GROUPCHARLINE, name, 0, prev) 
{

};

DCM::GroupCharLine::GroupCharLine(const GroupCharLine& other)
	: LineBaseParameter(other), dist_x(other.dist_x) 
{

}

DCM::GroupCharLine& DCM::GroupCharLine::operator=(const GroupCharLine& other)
{
	if (this != &other)
	{
		dist_x = other.dist_x;
	}
	return *this;
}

void DCM::GroupCharLine::copy(const GroupCharLine& other)
{
	if (this->type != other.type)
		return;
	this->dist_x = other.dist_x;
	LineBaseParameter::copy(other);
}

void DCM::GroupCharLine::copyValue(const GroupCharLine& other)
{
	LineBaseParameter::copy(other);
}

DCM::GroupCharMap::GroupCharMap(std::string name, Element* prev)
	: MapBaseParameter(TYPE::GROUPCHARMAP, name, 0, 0, prev) 
{

}

DCM::GroupCharMap::GroupCharMap(const GroupCharMap& other) 
	: MapBaseParameter(other), dist_x(other.dist_x), dist_y(other.dist_y) 
{

}

DCM::GroupCharMap& DCM::GroupCharMap::operator=(const GroupCharMap& other)
{
	if (this != &other)
	{
		dist_x = other.dist_x;
		dist_y = other.dist_y;
	}
	return *this;
}

void DCM::GroupCharMap::copy(const GroupCharMap& other)
{
	if (this->type != other.type)
		return;
	this->dist_x = other.dist_x;
	this->dist_y = other.dist_y;
	MapBaseParameter::copy(other);
}

void DCM::GroupCharMap::copyValue(const GroupCharMap& other)
{
	MapBaseParameter::copy(other);
}
